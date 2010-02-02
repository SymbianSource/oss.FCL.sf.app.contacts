/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Opens ETel RTelServer and RMobilePhone
*
*/



// INCLUDE FILES
#include "CPhone.h"

// From Virtual Phonebook
#include "CServiceTableCmd.h"
#include "CUsimAdnStore.h"
#include "CAdnStore.h"
#include "CFdnStore.h"
#include "CSdnStore.h"
#include "COwnNumberStore.h"
#include "CSatRefreshNotifier.h"
#include "CBtSapNotification.h"
#include "TStoreParams.h"
#include "VPbkSimStoreImplError.h"
#include "CSimStatusNotification.h"
#include <MVPbkSimCommand.h>
#include <CVPbkAsyncOperation.h>
#include <CVPbkSimStateInformation.h>
#include <VPbkSimStoreTemplateFunctions.h>
#include <VPbkSimStateDefinitions.h>
#include <CVPbkAsyncCallback.h>

// System includes
#include <mmtsy_names.h>

// DEBUG
#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {

// -----------------------------------------------------------------------------
// CSimPdUtilities::ConvertStatus
// Converts ETel status to SIMPD status
// -----------------------------------------------------------------------------
//
MVPbkSimPhone::TFDNStatus ConvertStatus(TInt aETelFDNStatus)
    {
    FUNC_ENTRY();
    MVPbkSimPhone::TFDNStatus status = MVPbkSimPhone::EFdnStatusUnknown;

    switch ( aETelFDNStatus )
        {
        case RMobilePhone::EFdnNotActive:
            {
            status = MVPbkSimPhone::EFdnIsNotActive;
            break;
            }
        case RMobilePhone::EFdnActive:
            {
            status = MVPbkSimPhone::EFdnIsActive;
            break;
            }
        case RMobilePhone::EFdnPermanentlyActive:
            {
            status = MVPbkSimPhone::EFdnIsPermanentlyActive;
            break;
            }
        case RMobilePhone::EFdnNotSupported:
            {
            status = MVPbkSimPhone::EFdnIsNotSupported;
            break;
            }
        case RMobilePhone::EFdnUnknown: // FALLTHROUGH
        default:
            {
            // Do nothing
            break;
            }
        }

    FUNC_EXIT();
    return status;
    }

// -----------------------------------------------------------------------------
// CPhone::CPhone
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhone::CPhone( CSatRefreshNotifier& aSatNotifier, 
        CVPbkSimStateInformation& aSimStateInfo, 
        CBtSapNotification& aBtSapNotifier )
        :   CActive( EPriorityStandard ),
            iSatNotifier( aSatNotifier ),
            iSimStateInfo( aSimStateInfo ),
            iBtSapNotifier( aBtSapNotifier )
    {
    FUNC_ENTRY();
    CActiveScheduler::Add(this);
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::~CPhone
// -----------------------------------------------------------------------------
//
CPhone::~CPhone()
    {
    FUNC_ENTRY();
    Cancel();
    delete iSimStatus;
    iSatNotifier.DeActivate( *this );
    iBtSapNotifier.RemoveObserver( *this );
    delete iAsyncOp;
    delete iSTCommand;
    iObservers.Close();
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhone Close handle %d"),
        iPhone.SubSessionHandle());
    iPhone.Close();
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RTelServer Close handle %d"),iTelServer.Handle());
    iTelServer.Close();
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::ETelPhone
// -----------------------------------------------------------------------------
//
RMobilePhone& CPhone::ETelPhone()
    {
    FUNC_ENTRY();
    return iPhone;
    }

// -----------------------------------------------------------------------------
// CPhone::CreateStoreLC
// -----------------------------------------------------------------------------
//
MVPbkSimCntStore* CPhone::CreateStoreLC( TStoreParams& aParams )
    {
    FUNC_ENTRY();
    OpenETelPhoneL();

    VPbkSimStoreImpl::CStoreBase* store = NULL;

    switch ( aParams.iId )
        {
        case EVPbkSimAdnGlobalStore:
            {
            if ( USimAccessSupported() )
                {
                store = VPbkSimStoreImpl::CUsimAdnStore::NewL( aParams );
                }
            else
                {
                store = VPbkSimStoreImpl::CAdnStore::NewL( aParams );
                }
            break;
            }
        case EVPbkSimFdnGlobalStore:
            {
            store = 
                VPbkSimStoreImpl::CFdnStore::NewL( aParams );
            break;
            }
        case EVPbkSimSdnGlobalStore:
            {
            store = 
                VPbkSimStoreImpl::CSdnStore::NewL( aParams );
            break;
            }
        case EVPbkSimONStore:
            {
            store = VPbkSimStoreImpl::COwnNumberStore::NewL( aParams );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, VPbkSimStoreImpl::Panic( 
                VPbkSimStoreImpl::ESimStoreNotSupportedYet ) );
            break;
            }
        }

    if ( store )
        {
        CleanupStack::PushL( store );
        }
    FUNC_EXIT();
    return store;
    }

// -----------------------------------------------------------------------------
// CPhone::AddObserverL
// -----------------------------------------------------------------------------
//    
void CPhone::AddObserverL( MVPbkSimPhoneObserver& aObserver )
    {
    FUNC_ENTRY();
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    FUNC_EXIT();
    }
        
// -----------------------------------------------------------------------------
// CPhone::RemoveObserver
// -----------------------------------------------------------------------------
//    
void CPhone::RemoveObserver( MVPbkSimPhoneObserver& aObserver )
    {
    FUNC_ENTRY();
    TInt pos = iObservers.Find( &aObserver );
    if ( pos != KErrNotFound )
        {
        iObservers.Remove( pos );
        }
    FUNC_EXIT();
    }
    
// -----------------------------------------------------------------------------
// CPhone::RunL
// -----------------------------------------------------------------------------
//    
void CPhone::RunL()
    {
    FUNC_ENTRY();
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl:CPhone::RunL: res: % d, ETel FDN status: %d"), 
        iStatus.Int(), iFdnStatus );
        
    if ( iStatus == KErrNone )
        {
        TInt conversion = ConvertStatus( iFdnStatus );
        
        SendObserverMessageR( iObservers, 
            &MVPbkSimPhoneObserver::FixedDiallingStatusChanged, 
            conversion );
        }
        
    ActivateFDNNotification();
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::BtSapDeactivated
// -----------------------------------------------------------------------------
//    
void CPhone::BtSapActivated()
    {
    FUNC_ENTRY();    
    SendObserverMessageRVV( iObservers, 
        &MVPbkSimPhoneObserver::PhoneError, *this,
        MVPbkSimPhoneObserver::EBtSapActive, KErrGeneral );
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::BtSapDeactivated
// -----------------------------------------------------------------------------
//    
void CPhone::BtSapDeactivated()
    {
    FUNC_ENTRY();    
    TRAPD( res, HandleBtSapDeactivationL() );
    if ( res != KErrNone )
        {
        SendObserverMessageRVV( iObservers, 
            &MVPbkSimPhoneObserver::PhoneError, *this,
            MVPbkSimPhoneObserver::ESystem, res );    
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::BtSapNotificationError
// -----------------------------------------------------------------------------
//    
void CPhone::BtSapNotificationError( TInt aError )
    {
    FUNC_ENTRY();
    SendObserverMessageRVV( iObservers, 
        &MVPbkSimPhoneObserver::PhoneError, *this,
        MVPbkSimPhoneObserver::ESystem, aError );
    FUNC_EXIT();
    }
    
// -----------------------------------------------------------------------------
// CPhone::DoCancel
// -----------------------------------------------------------------------------
//    
void CPhone::DoCancel()
    {
    FUNC_ENTRY();
    iPhone.CancelAsyncRequest( EMobilePhoneNotifyFdnStatusChange );
    FUNC_EXIT();
    }
    
// -----------------------------------------------------------------------------
// CPhone::OpenL
// -----------------------------------------------------------------------------
//
void CPhone::OpenL( MVPbkSimPhoneObserver& aObserver )
    {
    FUNC_ENTRY();
    MVPbkSimPhoneObserver::TErrorIdentifier errorId;
    if ( SimUsableL( errorId ) )
        {
        OpenETelPhoneL();
        
        if ( !StartInitL() && iIsOpen )
            {
            // store is open
            CVPbkAsyncObjectCallback<MVPbkSimPhoneObserver>* callback =
                VPbkEngUtils::CreateAsyncObjectCallbackLC(
                    *this, 
                    &CPhone::DoOpenL, 
                    &CPhone::DoOpenError, 
                    aObserver );
            AsyncOpL()->CallbackL( callback );
            CleanupStack::Pop( callback );
            }    
        }
    else
        {
        CVPbkAsyncObjectCallback<MVPbkSimPhoneObserver>* callback =
                VPbkEngUtils::CreateAsyncObjectCallbackLC(
                    *this, 
                    &CPhone::DoOpenWithErrorL,
                    errorId,
                    &CPhone::DoOpenError,
                    aObserver );
            AsyncOpL()->CallbackL( callback );
            CleanupStack::Pop( callback );
        }
    
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::Close
// -----------------------------------------------------------------------------
//
void CPhone::Close( MVPbkSimPhoneObserver& aObserver )
    {
    FUNC_ENTRY();
    if ( iAsyncOp )
        {
        iAsyncOp->CancelCallback( &aObserver );
        }

    TInt index = iObservers.Find( &aObserver );
    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }

    if ( iObservers.Count() == 0 )
        {
        if ( iAsyncOp )
            {
            iAsyncOp->Purge();
            }
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::USimAccessSupported
// -----------------------------------------------------------------------------
//
TBool CPhone::USimAccessSupported() const
    {
    FUNC_ENTRY();
    TBool supported = EFalse;
    TUint32 iccCaps;
    
    TInt result = iPhone.GetIccAccessCaps( iccCaps );
    if ( result == KErrNone && 
         iccCaps & RMobilePhone::KCapsUSimAccessSupported )
        {
        supported = ETrue;
        }
    FUNC_EXIT();
    return supported;
    }

// -----------------------------------------------------------------------------
// CPhone::ServiceTable
// -----------------------------------------------------------------------------
//
TUint32 CPhone::ServiceTable() const
    {
    FUNC_ENTRY();
    return iServiceTable;
    }

// -----------------------------------------------------------------------------
// CPhone::FixedDialingStatus
// -----------------------------------------------------------------------------
//
MVPbkSimPhone::TFDNStatus CPhone::FixedDialingStatus() const
    {
    FUNC_ENTRY();
    RMobilePhone::TMobilePhoneFdnStatus etelStatus = RMobilePhone::EFdnUnknown;
    TFDNStatus ownStatus = EFdnStatusUnknown;

    TInt result = iPhone.GetFdnStatus( etelStatus );
    if ( result == KErrNone )
        {
        ownStatus = ConvertStatus( etelStatus );
        }
    FUNC_EXIT();
    return ownStatus;
    }
    
// -----------------------------------------------------------------------------
// CPhone::CommandDone
// -----------------------------------------------------------------------------
//
void CPhone::CommandDone( MVPbkSimCommand& aCommand )
    {
    FUNC_ENTRY();
    if ( iSTCommand == &aCommand )
        {   
        // Delete the service table command
        delete iSTCommand;
        iSTCommand = NULL;
        
        // Activate observing BT SAP and SAT refresh.        
        TRAPD( res, ActivateBtSapAndSatRefreshL() );
        if ( res != KErrNone )
            {
            CommandError( aCommand, res );
            }
        else if ( !iIsOpen )
            {
            // The phone is open after service table have been read
            iIsOpen = ETrue;
            SendObserverMessageR( iObservers, 
                &MVPbkSimPhoneObserver::PhoneOpened, *this );
            }
        else
            {
            // It was already open so inform observers about updated table
            SendObserverMessageR( iObservers, 
                &MVPbkSimPhoneObserver::ServiceTableUpdated, iServiceTable );
            }
        
        // Service table is reloaded when SIM is refreshed and we
        // have to tell SAT server that we have updated ourselves.
        CompleteSatRefresh();
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::CommandError
// -----------------------------------------------------------------------------
//
void CPhone::CommandError( MVPbkSimCommand& aCommand, TInt aError )
    {
    FUNC_ENTRY();
    if ( iSTCommand == &aCommand )
        {
        delete iSTCommand;
        iSTCommand = NULL;
        SendObserverMessageRVV( iObservers, 
            &MVPbkSimPhoneObserver::PhoneError, *this,
            MVPbkSimPhoneObserver::ESystem, aError );
        // Service table is reloaded when SIM is refreshed. If this fails
        // SAT server is still waiting client notifying.
        CompleteSatRefresh();
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::SimRefreshed
// -----------------------------------------------------------------------------
//
void CPhone::SimRefreshed( TUint32 aSimRefreshFlags,
        MSimRefreshCompletion& aSimRefreshCompletion )
    {
    FUNC_ENTRY();
    // SAT notifier tells that SIM has been refreshed
    VPBK_DEBUG_PRINT(
        VPBK_DEBUG_STRING("CPbkSimStoreImpl:CPhone::SimRefreshed [%x]"), aSimRefreshFlags );
    // Check if there are elementary files that this class is interested in.
    // In practise service table file.
    TBool completeNow = ETrue;
    if ( aSimRefreshFlags & ElementaryFiles() )
        {
        // Service table has been updated -> load service table asynchronously.
        TRAPD( res, LoadServiceTableL() );
        VPBK_DEBUG_PRINT(
            VPBK_DEBUG_STRING("CPbkSimStoreImpl:CPhone LoadServiceTableL [%i]"), res );
        if ( res != KErrNone )
            {
            SendObserverMessageRVV( iObservers, 
                &MVPbkSimPhoneObserver::PhoneError, *this,
                MVPbkSimPhoneObserver::ESystem, res );
            }
        else
            {
            // Save completion to complete refresh later
            iSimRefreshCompletion = &aSimRefreshCompletion;
            completeNow = EFalse;
            }
        }
        
    // If we didn't start asyncrhonous update then complete refresh because
    // SAT server is waiting that.
    if ( completeNow )
        {
        aSimRefreshCompletion.SatRefreshCompleted( *this, EFalse );
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::AllowRefresh
// -----------------------------------------------------------------------------
//
TBool CPhone::AllowRefresh( TUint32 /*aSimRefreshFlags*/ )
    {
    FUNC_ENTRY();
    if ( iSTCommand )
        {
        // If service table request is active then don't allow refresh yet
        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CPhone::ElementaryFiles
// -----------------------------------------------------------------------------
//
TUint32 CPhone::ElementaryFiles()
    {
    FUNC_ENTRY();
    return KSTRefresh | KSimReset | KSimInit | 
    		KSimInitFullFileChangeNotification | KUsimApplicationReset |
    		K3GSessionReset;
    }
    
// -----------------------------------------------------------------------------
// CPhone::AsyncOpL
// -----------------------------------------------------------------------------
//
CVPbkAsyncObjectOperation<MVPbkSimPhoneObserver>* CPhone::AsyncOpL()
    {
    FUNC_ENTRY();
    if ( !iAsyncOp )
        {
        iAsyncOp = CVPbkAsyncObjectOperation<MVPbkSimPhoneObserver>::NewL();
        }
    FUNC_EXIT();
    return iAsyncOp;
    }

// -----------------------------------------------------------------------------
// CPhone::StartInitL
// -----------------------------------------------------------------------------
//
TBool CPhone::StartInitL()
    {
    FUNC_ENTRY();
    if ( !iIsOpen && !iSTCommand )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: CPhone::StartInitL"));    

        // phone is not open and service table is not currently loading
        LoadServiceTableL();
        FUNC_EXIT();
        return ETrue;
        }
    FUNC_EXIT();
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// CPhone::OpenETelPhoneL
// -----------------------------------------------------------------------------
//
void CPhone::OpenETelPhoneL()
    {
    FUNC_ENTRY();
    if ( !iTelServer.Handle() )
        {
        User::LeaveIfError( iTelServer.Connect() );
        User::LeaveIfError( iTelServer.LoadPhoneModule( KMmTsyModuleName ) );
        // This enables extended errors from low level which are needed
        // to get detailed information when saving contacts
        TInt res = iTelServer.SetExtendedErrorGranularity( 
            RTelServer::EErrorExtended );
        // If no memory then leave otherwise continue -> application works
        // with core errors too. Only some error notes are not shown.
        if ( res == KErrNoMemory )
            {
            User::Leave( res );
            }
        }

    if ( !iPhone.SubSessionHandle() )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: RTelServer Connected handle %d"),
            iTelServer.Handle());
        RTelServer::TPhoneInfo info;
        const TInt firstPhoneIndex = 0;
        User::LeaveIfError( iTelServer.GetPhoneInfo( 
            firstPhoneIndex , info ) );
        User::LeaveIfError( iPhone.Open( iTelServer, info.iName ) );
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: RMobilePhone Open h%d"),
            iPhone.SubSessionHandle());
        
        ActivateFDNNotification();
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::HandleBtSapDeactivationL
// -----------------------------------------------------------------------------
//
void CPhone::HandleBtSapDeactivationL()
    {
    FUNC_ENTRY();
    OpenETelPhoneL();
    /// If already open -> send only open message
    if ( !StartInitL() && iIsOpen )
        {
        SendObserverMessageR( iObservers, 
            &MVPbkSimPhoneObserver::PhoneOpened, *this );
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::ActivateBtSapAndSatRefreshL
// -----------------------------------------------------------------------------
//
void CPhone::ActivateBtSapAndSatRefreshL()
    {
    FUNC_ENTRY();
    iSatNotifier.ActivateL( *this );
    iBtSapNotifier.AddObserverL( *this );
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::DeActivateBtSapAndSatRefresh
// -----------------------------------------------------------------------------
//
void CPhone::DeActivateBtSapAndSatRefresh()
    {
    FUNC_ENTRY();
    iSatNotifier.DeActivate( *this );
    iBtSapNotifier.RemoveObserver( *this );
    FUNC_EXIT();
    }
    
// -----------------------------------------------------------------------------
// CPhone::LoadServiceTable
// -----------------------------------------------------------------------------
//
void CPhone::LoadServiceTableL()
    {
    FUNC_ENTRY();
    if ( !iSTCommand )
        {
        iSTCommand = CServiceTableCmd::NewLC( *this, iServiceTable );
        CleanupStack::Pop(); // iSTCommand
        iSTCommand->AddObserverL( *this );
        iSTCommand->Execute();
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::SimUsableL
// -----------------------------------------------------------------------------
//
TBool CPhone::SimUsableL( MVPbkSimPhoneObserver::TErrorIdentifier& aErrorId )
    {
    FUNC_ENTRY();
    if ( !iSimStatus )
        {
        iSimStatus = CSimStatusNotification::NewL();
        iSimStatus->AddObserverL( *this );
        }
    TBool result = EFalse;
    if ( iSimStateInfo.BTSapEnabledL() )
        {
        aErrorId = MVPbkSimPhoneObserver::EBtSapActive;
        }
    else if ( iSimStatus->CurrentStatus() != VPbkSimStoreImpl::SimOkStatus )
        {
        aErrorId = MVPbkSimPhoneObserver::ESimCardNotInserted;
        }
    else
        {
        result = ETrue;
        }
    FUNC_EXIT();
    return result;
    }

// -----------------------------------------------------------------------------
// CPhone::ActivateFDNNotification
// -----------------------------------------------------------------------------
//
void CPhone::ActivateFDNNotification()
    {
    FUNC_ENTRY();
    // Activate FDN activity notification
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CPhone::ActivateFDNNotification"));
    iPhone.NotifyFdnStatusChange(iStatus, iFdnStatus);
    SetActive();
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::CompleteSatRefresh
// -----------------------------------------------------------------------------
//
void CPhone::CompleteSatRefresh()
    {
    FUNC_ENTRY();
    // If iSimRefreshCompletion exists it means that service table has
    // been updated due to SAT refresh. We have to tell SAT server
    // that we have done the update. 
    if ( iSimRefreshCompletion )
        {
        iSimRefreshCompletion->SatRefreshCompleted( *this, EFalse );
        iSimRefreshCompletion = NULL;
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::SimStatusOk
// -----------------------------------------------------------------------------
//
void CPhone::SimStatusOk()
    {
    FUNC_ENTRY();
    OpenETelPhoneL();
    /// If already open -> send only open message
    if ( !StartInitL() && iIsOpen )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: CPhone::SimStatusOk Send phone opened"));    
        SendObserverMessageR( iObservers, 
            &MVPbkSimPhoneObserver::PhoneOpened, *this );
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::SimStatusNotOk
// -----------------------------------------------------------------------------
//
void CPhone::SimStatusNotOk()
    {
    FUNC_ENTRY();
    SendObserverMessageRVV( iObservers, 
        &MVPbkSimPhoneObserver::PhoneError, *this,
        MVPbkSimPhoneObserver::ESimCardNotInserted, KErrGeneral );
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::SimStatusUninitialized
// -----------------------------------------------------------------------------
//
void CPhone::SimStatusUninitialized()
    {
    FUNC_ENTRY();
    SendObserverMessageRVV( iObservers, 
        &MVPbkSimPhoneObserver::PhoneError, *this,
        MVPbkSimPhoneObserver::ESimCardNotInserted, KErrGeneral );
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::SimStatusNotificationError
// -----------------------------------------------------------------------------
//
void CPhone::SimStatusNotificationError( TInt /*aError*/ )
    {
    FUNC_ENTRY();
    SendObserverMessageRVV( iObservers, 
        &MVPbkSimPhoneObserver::PhoneError, *this,
        MVPbkSimPhoneObserver::ESimCardNotInserted, KErrGeneral );
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::DoOpenL
// -----------------------------------------------------------------------------
//
void CPhone::DoOpenL( MVPbkSimPhoneObserver& aObserver )
    {
    FUNC_ENTRY();
    if ( iObservers.Find( &aObserver ) != KErrNotFound )
        {
        aObserver.PhoneOpened( *this );
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::DoOpenError
// -----------------------------------------------------------------------------
//
void CPhone::DoOpenError( MVPbkSimPhoneObserver& aObserver, TInt aError )
    {
    FUNC_ENTRY();
    if ( iObservers.Find( &aObserver ) != KErrNotFound )
        {
        aObserver.PhoneError( *this, MVPbkSimPhoneObserver::ESystem, aError );
        }
    FUNC_EXIT();
    }

// -----------------------------------------------------------------------------
// CPhone::DoOpenWithErrorL
// -----------------------------------------------------------------------------
//
void CPhone::DoOpenWithErrorL
        ( MVPbkSimPhoneObserver& aObserver,
            MVPbkSimPhoneObserver::TErrorIdentifier aErrorId )
    {
    FUNC_ENTRY();
    if ( iObservers.Find( &aObserver ) != KErrNotFound )
        {
        aObserver.PhoneError( *this, aErrorId, KErrGeneral );
        }
    FUNC_EXIT();
    }
    
} // namespace VPbkSimStoreImpl


//  End of File  
