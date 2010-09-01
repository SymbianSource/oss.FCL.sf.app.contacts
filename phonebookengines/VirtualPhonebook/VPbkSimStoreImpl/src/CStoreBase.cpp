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
* Description:  The base class for sim stores
*
*/



// INCLUDE FILES
#include "CStoreBase.h"

// From VPbkSimStoreImpl namespace
#include "CPhone.h"
#include "CContactArray.h"
#include "CETelStoreNotification.h"
#include "CSequentialCompositeCmd.h"
#include "CDeleteCmd.h"
#include "CGetInfoCmd.h"
#include "CServiceTableCmd.h"
#include "CNumberMatchOperation.h"
#include "CFindOperation.h"
#include "CSimPhoneNumberIndex.h"
#include "CSatRefreshNotifier.h"
#include "TStoreParams.h"
#include "CBtSapNotification.h"
#include "CSharedViewArray.h"

// From Virtual Phonebook
#include <CVPbkSimContactView.h>
#include <CVPbkSimCommandStore.h>
#include <CVPbkETelCntConverter.h>
#include <CVPbkSimContact.h>
#include <CVPbkSimCntField.h>
#include <CVPbkAsyncOperation.h>
#include <CVPbkAsyncCallback.h>
#include <CVPbkSimFieldTypeFilter.h>
#include <MVPbkSimCommand.h>
#include <MVPbkSimContactObserver.h>
#include <MVPbkSimStoreObserver.h>
#include <TVPbkSimStoreProperty.h>
#include <VPbkSimStoreTemplateFunctions.h>
#include <VPbkSimStoreImplError.h>

// System includes
#include <etelmm.h>

#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {

// ============================ LOCAL FUNCTIONS =============================

// --------------------------------------------------------------------------
// ConvertETelStoreCaps
// Converts the ETel store caps to own caps
// --------------------------------------------------------------------------
//
TUint32 ConvertETelStoreCaps( TUint32 aETelStoreCaps )
    {
    TUint32 caps = 0;

    if ( aETelStoreCaps & RMobilePhoneBookStore::KCapsReadAccess )
        {
        caps |= VPbkSimStoreImpl::KReadAccess;
        }
    if ( aETelStoreCaps & RMobilePhoneBookStore::KCapsWriteAccess )
        {
        caps |= VPbkSimStoreImpl::KWriteAccess;
        }
    if ( aETelStoreCaps & RMobilePhoneBookStore::KCapsDeleteAll )
        {
        caps |= VPbkSimStoreImpl::KDeleteAll;
        }
    if ( aETelStoreCaps & RMobilePhoneBookStore::KCapsNotifyEvent )
        {
        caps |= VPbkSimStoreImpl::KNotifyEvent;
        }
    if ( aETelStoreCaps & RMobilePhoneBookStore::KCapsRestrictedWriteAccess )
        {
        caps |= VPbkSimStoreImpl::KRestrictedWriteAccess;
        }
    if ( aETelStoreCaps & RMobilePhoneBookStore::KCapsSecondNameUsed )
        {
        caps |= VPbkSimStoreImpl::KSecondNameUsed;
        }
    if ( aETelStoreCaps & RMobilePhoneBookStore::KCapsAdditionalNumUsed )
        {
        caps |= VPbkSimStoreImpl::KAdditionalNumUsed;
        }
    if ( aETelStoreCaps & RMobilePhoneBookStore::KCapsGroupingUsed )
        {
        caps |= VPbkSimStoreImpl::KGroupingUsed;
        }
    if ( aETelStoreCaps & RMobilePhoneBookStore::KCapsEmailAddressUsed )
        {
        caps |= VPbkSimStoreImpl::KEmailAddressUsed;
        }
    if ( aETelStoreCaps & RMobilePhoneBookStore::KCapsEntryControlUsed )
        {
        caps |= VPbkSimStoreImpl::KEntryControlUsed;
        }
    return caps; 
    }

// ============================ MEMBER FUNCTIONS ===============================

// --------------------------------------------------------------------------
// CStoreBase::CStoreBase
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CStoreBase::CStoreBase( TStoreParams& aParams )
    : iIdentifier( aParams.iId ),
      iPhone( aParams.iPhone ),
      iSatNotifier( aParams.iSatNotifier ),
      iBtSapNotifier( aParams.iBtSapNotifier ),
      iSimStateInfo( aParams.iSimStateInfo ),
      iSystemMaxPhoneNumberLength( aParams.iSystemPhoneNumberMaxLength )
    {
    }

// --------------------------------------------------------------------------
// CStoreBase::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CStoreBase::ConstructL()
    {
    iCmdStore = CVPbkSimCommandStore::NewL( ETrue );
    iContactArray = new( ELeave ) CContactArray;
    iOpenOp = CVPbkAsyncObjectOperation<MVPbkSimStoreObserver>::NewL();
    iCntConverter = CVPbkETelCntConverter::NewL();
    // Leave shared views alive to offer an efficient view building
    // for S60 all contacts views.
    iSharedViewArray = CSharedViewArray::NewL( 
            CSharedViewArray::EDestroyViewWhenArrayIsDestroyed ); 
    // Initialize the BtSap status.
    iIsBtSapActivated = EFalse;
    }
    
// Destructor
CStoreBase::~CStoreBase()
    {
    delete iSharedViewArray;
    delete iCntConverter;
    delete iOpenOp;
    iStoreObservers.Close();
    delete iETelNotification;
    delete iPhoneNumberIndex;
    delete iCmdStore;
    delete iContactArray;
    }
// --------------------------------------------------------------------------
// CStoreBase::Identifier
// --------------------------------------------------------------------------
// 
TBool CStoreBase::AllowOpeningL()
    {
    TBool result = ETrue;

    // If BtSap is activated, not allow the sim server to open the sim store.
    if( iIsBtSapActivated )
       {
       result = EFalse;
       }
    
    return result;
    }    
// --------------------------------------------------------------------------
// CStoreBase::Identifier
// --------------------------------------------------------------------------
//
TVPbkSimStoreIdentifier CStoreBase::Identifier() const
    {
    return iIdentifier;
    }

// --------------------------------------------------------------------------
// CStoreBase::OpenL
// --------------------------------------------------------------------------
//
void CStoreBase::OpenL( MVPbkSimStoreObserver& aObserver )
    {
    TBool sendStoreUnavailableEvent = EFalse;
    switch ( iStoreState )
        {
        case EStoreNotOpen:
            {
            // Check if subclass doesn't allow to open store
            if ( AllowOpeningL() )
                {
                ConstructStoreL();    
                }
            else
                {
                // Send store not available
                sendStoreUnavailableEvent = ETrue;
                }
            break;
            }
        case EStoreOpen:
            {
            CVPbkAsyncObjectCallback<MVPbkSimStoreObserver>* callback =
                VPbkEngUtils::CreateAsyncObjectCallbackLC(
                    *this, 
                    &CStoreBase::DoStoreOpenCallbackL, 
                    &CStoreBase::DoStoreErrorCallback, 
                    aObserver);
            iOpenOp->CallbackL( callback );
            CleanupStack::Pop( callback );
            break;
            }
        case EStoreNotAvailable:
            {
            // Send store not available
            //sendStoreUnavailableEvent = ETrue;
            
            if ( AllowOpeningL() )
                {
                // If the store is not allowed to open, no need to reset the store.
                // So move the function into the if branch.
                ResetStore();
                ConstructStoreL();    
                }
            else
                {
                // Send store not available
                sendStoreUnavailableEvent = ETrue;
                }
            
            break;
            }
        case EStoreIsOpening: // FALLTHROUGH
        default:
            {
            // Do nothing
            break;
            }
        }

    if ( sendStoreUnavailableEvent )
        {
        CVPbkAsyncObjectCallback<MVPbkSimStoreObserver>* callback =
            VPbkEngUtils::CreateAsyncObjectCallbackLC(
                *this, 
                &CStoreBase::DoStoreUnavailableCallbackL, 
                &CStoreBase::DoStoreErrorCallback, 
                aObserver);
        iOpenOp->CallbackL( callback );
        CleanupStack::Pop( callback );    
        }
    
    iStoreObservers.AppendL( &aObserver );
    }

// --------------------------------------------------------------------------
// CStoreBase::Close
// --------------------------------------------------------------------------
//
void CStoreBase::Close( MVPbkSimStoreObserver& aObserver )
    {
    iOpenOp->CancelCallback( &aObserver );
    
    const TInt pos = iStoreObservers.Find( &aObserver );
    if ( pos != KErrNotFound )
        {
        iStoreObservers.Remove( pos );
        if ( iStoreObservers.Count() == 0 )
            {
            // Close handles to ETel etc.
            ResetStore();
            // Remove from SAT notifier list
            iSatNotifier.DeActivate( *this );
            // Remove from BT SAP notifier list
            iBtSapNotifier.RemoveObserver( *this );
            // Change state to not open
            iStoreState = EStoreNotOpen;
            }
        }    
    }

// --------------------------------------------------------------------------
// CStoreBase::CreateViewL
// --------------------------------------------------------------------------
//
MVPbkSimCntView* CStoreBase::CreateViewL( 
        const RVPbkSimFieldTypeArray& aSortOrder,
        TVPbkSimViewConstructionPolicy aConstructionPolicy,
        const TDesC& aViewName,
        CVPbkSimFieldTypeFilter* aFilter )
    {
    CleanupStack::PushL( aFilter );
    
    MVPbkSimCntView* view = NULL;
    // Filter is not used if it doesn't contain any filtering criteria.
   	TBool filterDefined = ( aFilter && 
   	    aFilter->FilteringCriteria() != 
   	        CVPbkSimFieldTypeFilter::ESimFilterCriteriaNoFiltering );
       	        
    if ( aViewName.Length() > 0 && !filterDefined )
        {
        view = iSharedViewArray->CreateNewHandleL( aSortOrder,
                aConstructionPolicy, *this, aViewName );
        CleanupStack::PopAndDestroy( aFilter );
        }
    else
        {
        CVPbkSimContactView* tmp = CVPbkSimContactView::NewLC( aSortOrder, 
                aConstructionPolicy, *this, KNullDesC, aFilter );
        CleanupStack::Pop( tmp );
        CleanupStack::Pop( aFilter );
        view = tmp;
        }
    
    return view;
    }

// --------------------------------------------------------------------------
// CStoreBase::AtL
// --------------------------------------------------------------------------
//
const TDesC8* CStoreBase::AtL( TInt aSimIndex )
    {
    const TDesC8* etelContact = NULL;
    const MVPbkSimContact* contact = ContactAtL( aSimIndex );
    if ( contact )
        {
        etelContact = &contact->ETelContactL();
        }
    return etelContact;
    }

// --------------------------------------------------------------------------
// CStoreBase::ContactAtL
// --------------------------------------------------------------------------
//
const MVPbkSimContact* CStoreBase::ContactAtL( TInt aSimIndex )
    {
    const MVPbkSimContact* contact = NULL;
    if ( aSimIndex > 0 && aSimIndex <= iContactArray->Size() )
        {
        contact = iContactArray->At( aSimIndex );
        }
    return contact;
    }
    
// --------------------------------------------------------------------------
// CStoreBase::ContactConverter
// --------------------------------------------------------------------------
//
const CVPbkETelCntConverter& CStoreBase::ContactConverter() const
    {
    return *iCntConverter;
    }

// --------------------------------------------------------------------------
// CStoreBase::CreateMatchPhoneNumberOperationL
// --------------------------------------------------------------------------
//
MVPbkSimStoreOperation* CStoreBase::CreateMatchPhoneNumberOperationL( 
    const TDesC& aPhoneNumber, TInt aMaxMatchDigits, 
    MVPbkSimFindObserver& aObserver )
    {
    CNumberMatchOperation* op = CNumberMatchOperation::NewL( aObserver, 
        *iPhoneNumberIndex, aPhoneNumber, aMaxMatchDigits, *this );
    op->Execute();
    return op;
    }

// --------------------------------------------------------------------------
// CStoreBase::CreateFindOperationL
// --------------------------------------------------------------------------
//
MVPbkSimStoreOperation* CStoreBase::CreateFindOperationL( 
    const TDesC& aStringToFind, RVPbkSimFieldTypeArray& aFieldTypes,
    MVPbkSimFindObserver& aObserver )
    {
    CFindOperation* op = 
        CFindOperation::NewL( aObserver, *this, aStringToFind, aFieldTypes );
    op->Execute();
    return op;
    }

// --------------------------------------------------------------------------
// CStoreBase::SystemPhoneNumberMaxLength
// --------------------------------------------------------------------------
//
TInt CStoreBase::SystemPhoneNumberMaxLength() const
    {
    return iSystemMaxPhoneNumberLength;
    }

// --------------------------------------------------------------------------
// CStoreBase::ETelStoreChanged
// --------------------------------------------------------------------------
//
TBool CStoreBase::ETelStoreChanged( TInt aSimIndex, TUint32 /*aEvents*/ )
    {
    TRAPD( res, HandleStoreEventL( aSimIndex ) );
    if ( res != KErrNone )
        {
        // Store update failed -> all clients should close store
        SendStoreErrorEvent( res );
        }
    return ETrue;
    }

// --------------------------------------------------------------------------
// CStoreBase::ETelStoreChangeError
// --------------------------------------------------------------------------
//
TBool CStoreBase::ETelStoreChangeError( TInt aError )
    {
    SendStoreErrorEvent( aError );
    return ETrue;    
    }

// --------------------------------------------------------------------------
// CStoreBase::ContactAdded
// --------------------------------------------------------------------------
//
void CStoreBase::ContactAdded( MVPbkSimContact& aSimContact )
    {
    SendObserverMessageVV( iStoreObservers, 
        &MVPbkSimStoreObserver::StoreContactEvent, 
        MVPbkSimStoreObserver::EContactAdded, 
        aSimContact.SimIndex() );    
    }

// --------------------------------------------------------------------------
// CStoreBase::ContactRemoved
// --------------------------------------------------------------------------
//
void CStoreBase::ContactRemoved( MVPbkSimContact& aRemovedContact )
    {
    SendObserverMessageVV( iStoreObservers, 
        &MVPbkSimStoreObserver::StoreContactEvent,
        MVPbkSimStoreObserver::EContactDeleted, 
        aRemovedContact.SimIndex() );    
    }

// --------------------------------------------------------------------------
// CStoreBase::ContactChanged
// --------------------------------------------------------------------------
//
void CStoreBase::ContactChanged( MVPbkSimContact& /*aOldContact*/,
    MVPbkSimContact& aNewContact )
    {
    SendObserverMessageVV( iStoreObservers, 
        &MVPbkSimStoreObserver::StoreContactEvent,
        MVPbkSimStoreObserver::EContactChanged,
        aNewContact.SimIndex() );    
    }

// --------------------------------------------------------------------------
// CStoreBase::CancelRequests
// --------------------------------------------------------------------------
//
void CStoreBase::CancelRequests()
    {
    if ( iETelNotification )
        {
        iETelNotification->Cancel();
        }
    if ( iCmdStore ) 
        {
        iCmdStore->CancelAll();
        }
    }

// --------------------------------------------------------------------------
// CStoreBase::AddGetInfoCmdToChainL
// --------------------------------------------------------------------------
//
void CStoreBase::AddGetInfoCmdToChainL( CCompositeCmdBase& aCmdChain )
    {
    CGetInfoCmd* getInfoCmd = CGetInfoCmd::NewLC( *this );
    aCmdChain.AddSubCommandL( getInfoCmd );
    CleanupStack::Pop( getInfoCmd );
    }

// --------------------------------------------------------------------------
// CStoreBase::ResetStore
// --------------------------------------------------------------------------
//
void CStoreBase::ResetStore()
    {
    VPBK_DEBUG_PRINT(
        VPBK_DEBUG_STRING("CPbkSimStoreImpl:CStoreBase::ResetStore"));
    
    CancelRequests();
    CloseETelStore();
    if ( iPhoneNumberIndex )
        {
        iContactArray->RemoveObserver( *iPhoneNumberIndex );
        }
    iContactArray->RemoveObserver( *this );
    iContactArray->DeleteAll();
    iStoreState = EStoreNotOpen;
    }

// --------------------------------------------------------------------------
// CStoreBase::ConstructStoreL
// --------------------------------------------------------------------------
//
void CStoreBase::ConstructStoreL()
    {
    OpenETelStoreL();
    MVPbkSimCommand* initCmd = CreateInitCommandL( *this );
    // Store takes the ownership of the command
    iCmdStore->AddAndExecuteCommandL( initCmd );
    iStoreState = EStoreIsOpening;
    }

// --------------------------------------------------------------------------
// CStoreBase::SendStoreReadyEvent
// --------------------------------------------------------------------------
//
void CStoreBase::SendStoreReadyEvent()
    {
    // Cancel all async open operations
    iOpenOp->Purge();
    iStoreState = EStoreOpen;
    SendObserverMessageR( iStoreObservers, 
        &MVPbkSimStoreObserver::StoreReady, *this );
    }
        
// --------------------------------------------------------------------------
// CStoreBase::SendStoreUnavailableEvent
// --------------------------------------------------------------------------
//
void CStoreBase::SendStoreUnavailableEvent()
    {
    // Cancel all async open operations
    iOpenOp->Purge();
    iStoreState = EStoreNotAvailable;
    SendObserverMessageR( iStoreObservers, 
        &MVPbkSimStoreObserver::StoreNotAvailable, *this );
    }

// --------------------------------------------------------------------------
// CStoreBase::SendStoreErrorEvent
// --------------------------------------------------------------------------
//
void CStoreBase::SendStoreErrorEvent( TInt aError )
    {
    // Cancel all async open operations
    iOpenOp->Purge();
    iStoreState = EStoreNotAvailable;
    SendObserverMessageRV( iStoreObservers, 
        &MVPbkSimStoreObserver::StoreError, *this, aError );
    }

// --------------------------------------------------------------------------
// CStoreBase::CommandDone
// --------------------------------------------------------------------------
//
void CStoreBase::CommandDone( MVPbkSimCommand& /*aCommand*/ )
    {
    if ( iSimRefreshCompletion )
        {
        CompleteSATRefresh( *iSimRefreshCompletion );
        iSimRefreshCompletion = NULL;
        }

    TRAPD( result, PostConstructStoreL() );
    if ( result != KErrNone )
        {
        ResetStore();
        SendStoreErrorEvent( result );
        }
    }

// --------------------------------------------------------------------------
// CStoreBase::CommandError
// --------------------------------------------------------------------------
//
void CStoreBase::CommandError( MVPbkSimCommand& /*aCommand*/, 
    TInt aError )
    {
    if ( iSimRefreshCompletion )
        {
        CompleteSATRefresh( *iSimRefreshCompletion );
        iSimRefreshCompletion = NULL;
        }

    ResetStore();
    SendStoreErrorEvent( aError );
    }

// --------------------------------------------------------------------------
// CStoreBase::SimRefreshed
// --------------------------------------------------------------------------
//
void CStoreBase::SimRefreshed( TUint32 aSimRefreshFlags, 
        MSimRefreshCompletion& aSimRefreshCompletion )
    {
    VPBK_DEBUG_PRINT(
        VPBK_DEBUG_STRING("CPbkSimStoreImpl:CStoreBase::SimRefreshed"));
    
    if ( aSimRefreshFlags & ElementaryFiles() )
        {
        ResetStore();
        TRAPD( res, ConstructStoreL() );
        if ( res == KErrNone )
            {
            SendStoreUnavailableEvent();
            }
        else
            {
            SendStoreErrorEvent( res );
            }
        iSimRefreshCompletion = &aSimRefreshCompletion;
        }
    else
        {
        // Must always complete.
        aSimRefreshCompletion.SatRefreshCompleted( *this, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CStoreBase::AllowRefresh
// --------------------------------------------------------------------------
//
TBool CStoreBase::AllowRefresh( TUint32 aSimRefreshFlags )
    {
    if ( iCmdStore->Executing() && aSimRefreshFlags & ElementaryFiles() )
        {
        // Don't allow refresh if there is a command running
        return EFalse;
        }
    return ETrue;
    }

// --------------------------------------------------------------------------
// CStoreBase::ElementaryFiles
// --------------------------------------------------------------------------
//
TUint32 CStoreBase::ElementaryFiles()
    {
    // base class returns 0, sub class overwrites and returns files it uses
    return 0;
    }
    
// --------------------------------------------------------------------------
// CStoreBase::BtSapActivated
// --------------------------------------------------------------------------
//
void CStoreBase::BtSapActivated()
    {
    iIsBtSapActivated = ETrue;
    if ( iStoreState == EStoreOpen || iStoreState == EStoreIsOpening )
        {
        SendStoreUnavailableEvent();
        ResetStore();
        }
    }

// --------------------------------------------------------------------------
// CStoreBase::BtSapDeactivated
// --------------------------------------------------------------------------
//
void CStoreBase::BtSapDeactivated()
    {
    iIsBtSapActivated = EFalse;
    TRAPD( res, HandleBtSapDeactivationL() );
    if ( res != KErrNone )
        {
        SendStoreErrorEvent( res );
        }
    }

// --------------------------------------------------------------------------
// CStoreBase::BtSapNotificationError
// --------------------------------------------------------------------------
//
void CStoreBase::BtSapNotificationError( TInt aError )
    {
    SendStoreErrorEvent( aError );
    }
        
// --------------------------------------------------------------------------
// CStoreBase::PostConstructStoreL
// --------------------------------------------------------------------------
//
void CStoreBase::PostConstructStoreL()
    {
    // Reserve memory for contacts
    iContactArray->ReserveL( ETelStoreInfo().iTotalEntries );
    // Start listen to contact array events
    iContactArray->AddObserverL( *this );
    
    // Create or rebuild the phone number index that is used for 
    // number matching
    if ( !iPhoneNumberIndex )
        {
        iPhoneNumberIndex = CSimPhoneNumberIndex::NewL( *this );
        }
    else
        {
        iPhoneNumberIndex->ReBuildL();
        }
    // Set number index to listen contact array events
    iContactArray->AddObserverL( *iPhoneNumberIndex );
    
    // Create an ETel store notifier if the ETel store supports it.
    if ( !iETelNotification )
        {
        TVPbkGsmStoreProperty prop;
        GetGsmStoreProperties( prop );
        if ( prop.iCaps & KNotifyEvent )
            {
            iETelNotification = new( ELeave ) CETelStoreNotification( *this );
            iETelNotification->Activate();
            }
        }
    else
        {
        iETelNotification->Activate();
        }
    
    // Start listen to SIM application toolkit (SAT) refresh events
    iSatNotifier.ActivateL( *this );
    // Start listen to Bluetoot SIM Access profile (BT SAP) events
    iBtSapNotifier.AddObserverL( *this );
    
    // Postconstruction done -> store is ready for use
    SendStoreReadyEvent();
    }
    
// --------------------------------------------------------------------------
// CStoreBase::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CStoreBase::HandleStoreEventL( TInt aSimIndex )
    {
    MVPbkSimCommand* updateCmd = CreateUpdateCommandL( aSimIndex );
    // Store takes the ownership of the command and executes it.
    iCmdStore->AddAndExecuteCommandL( updateCmd );
    }

// --------------------------------------------------------------------------
// CStoreBase::HandleBtSapDeactivationL
// --------------------------------------------------------------------------
//        
void CStoreBase::HandleBtSapDeactivationL()
    {
    if ( iStoreState == EStoreNotOpen ||
         iStoreState == EStoreNotAvailable  )
        {
        ConstructStoreL();
        }
    }

// --------------------------------------------------------------------------
// CStoreBase::DoStoreOpenCallbackL
// --------------------------------------------------------------------------
//        
void CStoreBase::DoStoreOpenCallbackL( MVPbkSimStoreObserver& aObserver )
    {
    aObserver.StoreReady( *this );
    }

// --------------------------------------------------------------------------
// CStoreBase::DoStoreUnavailableCallbackL
// --------------------------------------------------------------------------
//        
void CStoreBase::DoStoreUnavailableCallbackL( 
        MVPbkSimStoreObserver& aObserver )
    {
    aObserver.StoreNotAvailable( *this );
    }

// --------------------------------------------------------------------------
// CStoreBase::DoStoreErrorCallback
// --------------------------------------------------------------------------
//            
void CStoreBase::DoStoreErrorCallback( MVPbkSimStoreObserver& aObserver, 
        TInt aError )
    {
    aObserver.StoreError( *this, aError );
    }
} // namespace VPbkSimStoreImpl
//  End of File  
