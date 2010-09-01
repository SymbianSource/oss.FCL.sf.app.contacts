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
* Description:  A remote sim store implementation
*
*/



// INCLUDE FILES
#include "CRemoteStore.h"

// From Virtual Phonebook
#include "VPbkSimStoreError.h"
#include "CRemoteView.h"

#include <MVPbkSimStoreObserver.h>
#include <MVPbkSimContactObserver.h>
#include <MVPbkSimFindObserver.h>
#include <VPbkSimServerOpCodes.h>
#include <VPbkSimStoreTemplateFunctions.h>
#include <CVPbkSimContactBuf.h>
#include <MVPbkSimPhoneObserver.h>
#include <CVPbkETelCntConverter.h>
#include <CVPbkSimFieldTypeFilter.h>
#include <MVPbkSimStoreOperation.h>
#include <MVPbkSimCntView.h>
#include <VPbkSimStoreCommonUtil.h>
#include <CVPbkAsyncOperation.h>
#include <CVPbkAsyncCallback.h>

#include <VPbkDebug.h>

// System includes
#include <s32mem.h>

namespace VPbkSimStore {

// CONSTANTS
// The initial buffer size for number match results, 10 indexes
const TInt KInitSimIndexBufSize = 10 * sizeof(TInt);
// The maximum buffer size for number match results if server behaves badly
const TInt KMaxSimIndexBufSize = 100 * KInitSimIndexBufSize;

// ============================= LOCAL FUNCTIONS ============================

// --------------------------------------------------------------------------
// SendStoreEventMessage
// --------------------------------------------------------------------------
//
template<class Function, class Param>
inline void SendStoreEventMessage(
    RPointerArray<MVPbkSimStoreObserver>& aObservers, Function aFunction,
    Param& aParam )
    {
    const TInt count = aObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        ( aObservers[i]->*aFunction )( aParam );
        }
    }

// --------------------------------------------------------------------------
// SendStoreErrorMessage
// --------------------------------------------------------------------------
//
inline void SendStoreErrorMessage( 
    RPointerArray<MVPbkSimStoreObserver>& aObservers, MVPbkSimCntStore& aStore,
    TInt aError )
    {
    const TInt count = aObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        aObservers[i]->StoreError( aStore, aError );
        }
    }
    
// --------------------------------------------------------------------------
// SendStoreEventMessage
// --------------------------------------------------------------------------
//
inline void SendStoreEventMessage(
    RPointerArray<MVPbkSimStoreObserver>& aObservers,
    void (MVPbkSimStoreObserver::*aEvent)(MVPbkSimStoreObserver::TEvent, TInt),
    MVPbkSimStoreObserver::TEvent aEventType,
    TInt aParam1)
    {
    for (TInt i = aObservers.Count() - 1; i >= 0; --i)
        {
        (aObservers[i]->*aEvent)( aEventType, aParam1 );
        }    
    }    

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CAsyncSave::CAsyncSave
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CAsyncSave::CAsyncSave( RVPbkSimStore& aStore )
:   CActive( EPriorityStandard ),
    iStore( aStore )
    {
    CActiveScheduler::Add( this );
    }

// Destructor
CAsyncSave::~CAsyncSave()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CAsyncSave::DoRequestL
// --------------------------------------------------------------------------
//
void CAsyncSave::DoRequestL( const TDesC8& aData, 
    TInt& aSimIndex, MVPbkSimContactObserver& aObserver )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "CAsyncSave::DoRequestL: aSimIndex %d"),
        aSimIndex );
    
    iObserver = &aObserver;
    iStore.SaveL( iStatus, aData, aSimIndex );
    SetActive();
    }

// --------------------------------------------------------------------------
// CAsyncSave::RunL
// --------------------------------------------------------------------------
//
void CAsyncSave::RunL()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "CAsyncSave::RunL: iStatus %d"),
        iStatus.Int() );
    
    TInt result = iStatus.Int();
    if ( result == KErrNone )
        {
        iObserver->ContactEventComplete( MVPbkSimContactObserver::ESave, NULL );
        }
    else
        {
        iObserver->ContactEventError( MVPbkSimContactObserver::ESave, NULL, 
            result );
        }
    }

// --------------------------------------------------------------------------
// CAsyncSave::DoCancel
// --------------------------------------------------------------------------
//
void CAsyncSave::DoCancel()
    {
    iStore.CancelAsyncRequest( EVPbkSimSrvSaveContact );
    }

// --------------------------------------------------------------------------
// CAsyncDelete::CAsyncDelete
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CAsyncDelete::CAsyncDelete( RVPbkSimStore& aStore )
:   CActive( EPriorityStandard ),
    iStore( aStore )
    {
    CActiveScheduler::Add( this );
    }

// Destructor
CAsyncDelete::~CAsyncDelete()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CAsyncDelete::DoRequestL
// --------------------------------------------------------------------------
//
void CAsyncDelete::DoRequestL( 
        RVPbkStreamedIntArray& aSimIndexes, 
        MVPbkSimContactObserver& aObserver )
    {
    iObserver = &aObserver;
    iStore.DeleteL( iStatus, aSimIndexes );
    SetActive();
    }

// --------------------------------------------------------------------------
// CAsyncDelete::RunL
// --------------------------------------------------------------------------
//
void CAsyncDelete::RunL()
    {
    TInt result = iStatus.Int();
    if ( result == KErrNone )
        {
        iObserver->ContactEventComplete( MVPbkSimContactObserver::EDelete, 
            NULL );
        }
    else
        {
        iObserver->ContactEventError( MVPbkSimContactObserver::EDelete, 
            NULL, result );
        }
    }

// --------------------------------------------------------------------------
// CAsyncDelete::DoCancel
// --------------------------------------------------------------------------
//
void CAsyncDelete::DoCancel()
    {
    iStore.CancelAsyncRequest( EVPbkSimSrvDeleteContact );
    }

// --------------------------------------------------------------------------
// CRemoteStore::CNumberMatchOperation::CNumberMatchOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CRemoteStore::CNumberMatchOperation::CNumberMatchOperation( 
    CRemoteStore& aStore, const TDesC& aPhoneNumber, 
    TInt aMaxMatchDigits, MVPbkSimFindObserver& aObserver )
    :   CActive( EPriorityStandard ),
        iStore( aStore ),
        iPhoneNumber( aPhoneNumber ),
        iMaxMatchDigits( aMaxMatchDigits ),
        iObserver( aObserver ),
        iSimIndexBufPtr( NULL, 0 )
    {
    }

// --------------------------------------------------------------------------
// CRemoteStore::CNumberMatchOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CRemoteStore::CNumberMatchOperation::ConstructL()
    {
    CActiveScheduler::Add( this );
    iSimIndexBuf = HBufC8::NewL( KInitSimIndexBufSize );
    iSimIndexBufPtr.Set( iSimIndexBuf->Des() );
    }

// --------------------------------------------------------------------------
// CRemoteStore::CNumberMatchOperation::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CRemoteStore::CNumberMatchOperation* CRemoteStore::CNumberMatchOperation::NewL( 
    CRemoteStore& aStore, const TDesC& aPhoneNumber, 
    TInt aMaxMatchDigits, MVPbkSimFindObserver& aObserver )
    {
    CNumberMatchOperation* self = new( ELeave ) CNumberMatchOperation( aStore, 
        aPhoneNumber, aMaxMatchDigits, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CRemoteStore::CNumberMatchOperation::~CNumberMatchOperation()
    {
    Cancel();
    delete iSimIndexBuf;
    }

// --------------------------------------------------------------------------
// CRemoteStore::CNumberMatchOperation::Activate
// --------------------------------------------------------------------------
//
void CRemoteStore::CNumberMatchOperation::Activate()
    {
    iStore.StoreSession().MatchPhoneNumber( iStatus, iPhoneNumber, 
        iMaxMatchDigits, iSimIndexBufPtr );
    SetActive();
    }

// --------------------------------------------------------------------------
// CRemoteStore::CNumberMatchOperation::RunL
// --------------------------------------------------------------------------
//
void CRemoteStore::CNumberMatchOperation::RunL()
    {
    RVPbkStreamedIntArray matchedIndexes;
    CleanupClosePushL( matchedIndexes );

    TInt result = iStatus.Int();
    switch ( result )
        {
        case KErrNone:
            {
            RDesReadStream stream( iSimIndexBufPtr );
            CleanupClosePushL( stream );
            stream >> matchedIndexes;
            CleanupStack::PopAndDestroy(); // stream
            iObserver.FindCompleted( iStore, matchedIndexes );
            break;
            }
        case KErrNotFound:
            {
            iObserver.FindCompleted( iStore, matchedIndexes );
            break;
            }
        case KErrOverflow:
            {
            TInt newLength = 2 * iSimIndexBufPtr.MaxLength();
            if ( newLength > KMaxSimIndexBufSize )
                {
                iObserver.FindError( iStore, result );
                }
            else
                {
                VPbkSimStoreImpl::CheckAndUpdateBufferSizeL( iSimIndexBuf, 
                    iSimIndexBufPtr, newLength );
                Activate();
                }
            break;
            }
        default:
            {
            iObserver.FindError( iStore, result );
            break;
            }
        }
    CleanupStack::PopAndDestroy(); // matchedIndexes
    }
  
// --------------------------------------------------------------------------
// CRemoteStore::CNumberMatchOperation::DoCancel
// --------------------------------------------------------------------------
//
void CRemoteStore::CNumberMatchOperation::DoCancel()
    {
    iStore.StoreSession().CancelAsyncRequest( EVPbkSimSrvMatchPhoneNumber );
    }

// --------------------------------------------------------------------------
// CRemoteStore::CNumberMatchOperation::RunError
// --------------------------------------------------------------------------
//
TInt CRemoteStore::CNumberMatchOperation::RunError( TInt aError )
    {
    iObserver.FindError( iStore, aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CRemoteStore::CFindOperation::CFindOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CRemoteStore::CFindOperation::CFindOperation( CRemoteStore& aStore,
    const TDesC& aStringToFind, MVPbkSimFindObserver& aObserver )
:   CActive( EPriorityStandard ),
    iStore( aStore ),
    iStringToFind( aStringToFind ),
    iObserver( aObserver ),
    iSimIndexBufPtr( NULL, 0 )
    {
    }

// --------------------------------------------------------------------------
// CRemoteStore::CFindOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CRemoteStore::CFindOperation::ConstructL( 
    RVPbkSimFieldTypeArray& aFieldTypes )
    {
    CActiveScheduler::Add( this );
    
    iFieldTypes = ExternalizeArrayLC( aFieldTypes );
    CleanupStack::Pop( iFieldTypes );
    iSimIndexBuf = HBufC8::NewL( KInitSimIndexBufSize );
    iSimIndexBufPtr.Set( iSimIndexBuf->Des() );
    }

// --------------------------------------------------------------------------
// CRemoteStore::CFindOperation::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CRemoteStore::CFindOperation* CRemoteStore::CFindOperation::NewL( 
    CRemoteStore& aStore, const TDesC& aStringToFind, 
    MVPbkSimFindObserver& aObserver, RVPbkSimFieldTypeArray& aFieldTypes )
    {
    CFindOperation* self = 
        new( ELeave ) CFindOperation( aStore, aStringToFind, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aFieldTypes );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CRemoteStore::CFindOperation::~CFindOperation()
    {
    Cancel();
    delete iSimIndexBuf;
    delete iFieldTypes;
    }

// --------------------------------------------------------------------------
// CRemoteStore::CFindOperation::Activate
// --------------------------------------------------------------------------
//
void CRemoteStore::CFindOperation::Activate()
    {
    iStore.StoreSession().Find( iStatus, iStringToFind, 
        *iFieldTypes, iSimIndexBufPtr );
    SetActive();
    }

// --------------------------------------------------------------------------
// CRemoteStore::CFindOperation::RunL
// --------------------------------------------------------------------------
//
void CRemoteStore::CFindOperation::RunL()
    {
    RVPbkStreamedIntArray matchedIndexes;
    CleanupClosePushL( matchedIndexes );

    TInt result = iStatus.Int();
    switch ( result )
        {
        case KErrNone:
            {
            RDesReadStream stream( iSimIndexBufPtr );
            CleanupClosePushL( stream );
            stream >> matchedIndexes;
            CleanupStack::PopAndDestroy(); // stream
            iObserver.FindCompleted( iStore, matchedIndexes );
            break;
            }
        case KErrNotFound:
            {
            iObserver.FindCompleted( iStore, matchedIndexes );
            break;
            }
        case KErrOverflow:
            {
            TInt newLength = 2 * iSimIndexBufPtr.MaxLength();
            if ( newLength > KMaxSimIndexBufSize )
                {
                iObserver.FindError( iStore, result );
                }
            else
                {
                VPbkSimStoreImpl::CheckAndUpdateBufferSizeL( iSimIndexBuf, 
                    iSimIndexBufPtr, newLength );
                Activate();
                }
            break;
            }
        default:
            {
            iObserver.FindError( iStore, result );
            break;
            }
        }
    CleanupStack::PopAndDestroy(); // matchedIndexes
    }
  
// --------------------------------------------------------------------------
// CRemoteStore::CFindOperation::DoCancel
// --------------------------------------------------------------------------
//
void CRemoteStore::CFindOperation::DoCancel()
    {
    iStore.StoreSession().CancelAsyncRequest( EVPbkSimSrvFind );
    }

// --------------------------------------------------------------------------
// CRemoteStore::CFindOperation::RunError
// --------------------------------------------------------------------------
//
TInt CRemoteStore::CFindOperation::RunError( TInt aError )
    {
    iObserver.FindError( iStore, aError );
    return KErrNone;
    }
    
// --------------------------------------------------------------------------
// CRemoteStore::CRemoteStore
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CRemoteStore::CRemoteStore( TVPbkSimStoreIdentifier aIdentifier )
:   CActive( EPriorityStandard ),
    iIdentifier( aIdentifier ),
    iCurrentState( EVPbkSimUnknown )
    {
    }

// --------------------------------------------------------------------------
// CRemoteStore::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CRemoteStore::ConstructL()
    {
    CActiveScheduler::Add( this );
    iContactConverter = CVPbkETelCntConverter::NewL();
    iCurrentETelContact = CVPbkSimContactBuf::NewL( *this );
    iCurrentContact = CVPbkSimContactBuf::NewL( *this );
    iSystemPhoneNumberMaxLength = 
        VPbkSimStoreCommonUtil::SystemMaxPhoneNumberLengthL();
    iAsyncOpenOp = 
        CVPbkAsyncObjectOperation<MVPbkSimStoreObserver>::NewL();
    }

// --------------------------------------------------------------------------
// CRemoteStore::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CRemoteStore* CRemoteStore::NewL( TVPbkSimStoreIdentifier aIdentifier )
    {
    CRemoteStore* self = new( ELeave ) CRemoteStore( aIdentifier );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
CRemoteStore::~CRemoteStore()
    {
    delete iAsyncOpenOp;
    iObservers.Close();
    delete iCurrentContact;
    delete iCurrentETelContact;
    delete iContactConverter;
    ResetAndCloseStore();
    }

// --------------------------------------------------------------------------
// CRemoteStore::AddObserverL
// --------------------------------------------------------------------------
//
void CRemoteStore::AddObserverL( MVPbkSimStoreObserver& aObserver )
    {
    // adds aObserver to list of observers if it isn't there already, thus
    // avoids clients registering multiple times
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    }

// --------------------------------------------------------------------------
// CRemoteStore::RemoveObserver
// --------------------------------------------------------------------------
//
void CRemoteStore::RemoveObserver( MVPbkSimStoreObserver& aObserver )
    {
    TInt index = iObservers.Find( &aObserver );
    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }

// --------------------------------------------------------------------------
// CRemoteStore::IsOpen
// --------------------------------------------------------------------------
//
TBool CRemoteStore::IsOpen() const
    {
    return iCurrentState == EVPbkSimStoreOpen;
    }

// --------------------------------------------------------------------------
// CRemoteStore::RunL
// --------------------------------------------------------------------------
//
void CRemoteStore::RunL()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "CRemoteStore::RunL: result %d, handle %d, id %d"),
        iStatus.Int(), iSimStore.Handle(), iIdentifier );
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "CRemoteStore::RunL: event %d, eventData %d, eventOpData %d"),
        iStoreEvent.iEvent, iStoreEvent.iData, iStoreEvent.iOpData);
        
    if ( iStatus == KErrNone )
        {
        switch ( iStoreEvent.iEvent )
            {
            case EVPbkSimPhoneOpen:
                {
                // Open the store only first time. It's possible
                // that phone and store can change unavailable<->open
                // later.
                if ( iCurrentState == EVPbkSimUnknown )
                    {
                    if( iSecurityInfo )
                        {
                        iSimStore.OpenL( *iSecurityInfo, iIdentifier );
                        }
                    else
                        {
                        // OpenL invoked with MVPbkSimCntStore::OpenL
                        // CRemoteStore::OpenL( secInfo, observer ) should be used instead.
                        User::Leave( KErrPermissionDenied );
                        }
                    }
                break;
                }
            case EVPbkSimStoreOpen:
                {
                iCurrentState = iStoreEvent.iEvent;
                iAsyncOpenOp->Purge();
                SendStoreEventMessage( iObservers, 
                    &MVPbkSimStoreObserver::StoreReady, *this );
                break;
                }
            case EVPbkSimStoreNotAvailable:
                {
                iCurrentState = iStoreEvent.iEvent;
                iAsyncOpenOp->Purge();
                SendStoreEventMessage( iObservers, 
                    &MVPbkSimStoreObserver::StoreNotAvailable, *this );
                break;
                }
            case EVPbkSimContactAdded:
                {
                SendStoreEventMessage(iObservers, &MVPbkSimStoreObserver::StoreContactEvent, 
                          MVPbkSimStoreObserver::EContactAdded, 
                          iStoreEvent.iOpData );
                break;
                }
            case EVPbkSimContactDeleted:
                {
                SendStoreEventMessage(iObservers, &MVPbkSimStoreObserver::StoreContactEvent, 
                          MVPbkSimStoreObserver::EContactDeleted, 
                          iStoreEvent.iOpData );
                break;
                }
            case EVPbkSimContactChanged:
                {
                SendStoreEventMessage(iObservers, &MVPbkSimStoreObserver::StoreContactEvent, 
                          MVPbkSimStoreObserver::EContactChanged, 
                          iStoreEvent.iOpData );
                break;
                }
            // Store is not interested in these events
            case EVPbkSimPhoneServiceTableUpdated: // FALLTHROUGH
            case EVPbkSimPhoneFdnStatusChanged:
                {
                break;
                }
            case EVPbkSimPhoneError:
                {
                iAsyncOpenOp->Purge();
                TInt id = iStoreEvent.iData;
                
                if ( IsOpen() && id == MVPbkSimPhoneObserver::ESimCardNotInserted )
                    {
                    iCurrentState = iStoreEvent.iEvent;
                    }

                // If not SIM card or BT SAP is active then store is not available
                if ( id == MVPbkSimPhoneObserver::ESimCardNotInserted ||
                     id == MVPbkSimPhoneObserver::EBtSapActive )
                    {
                    SendStoreEventMessage( iObservers, 
                        &MVPbkSimStoreObserver::StoreNotAvailable, *this );
                    }
                else
                    {
                    SendStoreErrorMessage( iObservers, *this, iStoreEvent.iOpData );    
                    }
                break;
                }
            case EVPbkSimStoreError: // FALLTHROUGH
                {
                iCurrentState = iStoreEvent.iEvent;
                iAsyncOpenOp->Purge();
                SendStoreErrorMessage( iObservers, *this, iStoreEvent.iOpData );
                break;
                }
            default:
                {
                __ASSERT_DEBUG(EFalse, Panic(EUnknownSimStoreEvent));
                iAsyncOpenOp->Purge();
                SendStoreErrorMessage( iObservers, *this, iStoreEvent.iOpData );
                break;
                }
            }
        }
    else
        {
        // Error case.
        iCurrentState = EVPbkSimStoreNotAvailable;
        iAsyncOpenOp->Purge();
        SendStoreErrorMessage( iObservers, *this, iStatus.Int() );
        }
        
    // check session, it may have been closed during message send
    if ( iSimStore.Handle() )
        {
        // Listen to store event always.
        iSimStore.ListenToStoreEvents( iStatus, iStoreEvent );
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CRemoteStore::DoCancel
// --------------------------------------------------------------------------
//
void CRemoteStore::DoCancel()
    {
    iSimStore.CancelAsyncRequest( EVPbkSimSrvStoreEventNotification );
    }

// --------------------------------------------------------------------------
// CRemoteStore::RunError
// --------------------------------------------------------------------------
//
TInt CRemoteStore::RunError( TInt aError )
    {
    SendStoreErrorMessage( iObservers, *this, aError );
    return KErrNone;
    }
    
// --------------------------------------------------------------------------
// CRemoteStore::Identifier
// --------------------------------------------------------------------------
//
TVPbkSimStoreIdentifier CRemoteStore::Identifier() const
    {
    return iIdentifier;
    }

// --------------------------------------------------------------------------
// CRemoteStore::OpenL
// --------------------------------------------------------------------------
//
void CRemoteStore::OpenL( MVPbkSimStoreObserver& aObserver )
    {
    if ( iSimStore.Handle() )
        {
        CVPbkAsyncObjectCallback<MVPbkSimStoreObserver>* callback =
                VPbkEngUtils::CreateAsyncObjectCallbackLC(
                    *this, 
                    &CRemoteStore::DoOpenL, 
                    &CRemoteStore::DoOpenError, 
                    aObserver);
        iAsyncOpenOp->CallbackL( callback );
        CleanupStack::Pop( callback );
        }
    else
        {
        // Connect to sim store server
        iSimStore.ConnectToServerL();
        // Start listening to store events
        iSimStore.ListenToStoreEvents( iStatus, iStoreEvent );
        SetActive();
        // Open the phone, after phone is open we can open store
        iSimStore.OpenPhoneL();
        }
    
    AddObserverL( aObserver );
    }

// --------------------------------------------------------------------------
// CRemoteStore::OpenL
// --------------------------------------------------------------------------
//
void CRemoteStore::OpenL( const TSecurityInfo& aSecurityInfo, 
                          MVPbkSimStoreObserver& aObserver )
    {
    OpenL( aObserver );
    iSecurityInfo = &aSecurityInfo;
    }

// --------------------------------------------------------------------------
// CRemoteStore::Close
// --------------------------------------------------------------------------
//
void CRemoteStore::Close( MVPbkSimStoreObserver& aObserver )
    {
    iAsyncOpenOp->CancelCallback( &aObserver );
    RemoveObserver( aObserver );
    ResetAndCloseStore();
    }

// --------------------------------------------------------------------------
// CRemoteStore::CreateViewL
// --------------------------------------------------------------------------
//
MVPbkSimCntView* CRemoteStore::CreateViewL(
        const RVPbkSimFieldTypeArray& aSortOrder, 
        TVPbkSimViewConstructionPolicy aConstructionPolicy,
        const TDesC& aViewName,
        CVPbkSimFieldTypeFilter* aFilter )
    {
    // Create always a remote view which is shared in the server
    // side.
    return CRemoteView::NewL( *this, aSortOrder, aConstructionPolicy, 
            aViewName, aFilter );
    }
      
// --------------------------------------------------------------------------
// CRemoteStore::ReadLC
// --------------------------------------------------------------------------
//
const TDesC8* CRemoteStore::AtL( TInt aSimIndex )
    {
    HBufC8* cnt = iSimStore.ReadLC( aSimIndex );
    if ( cnt )
        {
        iCurrentETelContact->SetL( *cnt );
        CleanupStack::PopAndDestroy( cnt );
        return &iCurrentETelContact->ETelContactL();
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CRemoteStore::ContactAtL
// --------------------------------------------------------------------------
//
const MVPbkSimContact* CRemoteStore::ContactAtL( TInt aSimIndex )
    {
    HBufC8* cnt = iSimStore.ReadLC( aSimIndex );
    if ( cnt )
        {
        iCurrentContact->SetL( *cnt );
        CleanupStack::PopAndDestroy( cnt );
        return iCurrentContact;
        }
    return NULL;
    }
    
// --------------------------------------------------------------------------
// CRemoteStore::SaveL
// --------------------------------------------------------------------------
//
MVPbkSimStoreOperation* CRemoteStore::SaveL( const TDesC8& aData, 
        TInt& aSimIndex, MVPbkSimContactObserver& aObserver )
    {
    CAsyncSave* op = new( ELeave ) CAsyncSave( iSimStore );
    CleanupStack::PushL( op );
    op->DoRequestL( aData, aSimIndex, aObserver );
    CleanupStack::Pop( op );
    return op;
    }

// --------------------------------------------------------------------------
// CRemoteStore::DeleteL
// --------------------------------------------------------------------------
//
MVPbkSimStoreOperation* CRemoteStore::DeleteL( 
        RVPbkStreamedIntArray& aSimIndexes, 
        MVPbkSimContactObserver& aObserver )
    {    
    CAsyncDelete* op = new( ELeave ) CAsyncDelete( iSimStore );
    CleanupStack::PushL( op );
    op->DoRequestL( aSimIndexes, aObserver );
    CleanupStack::Pop( op );
    return op;
    }
    
// --------------------------------------------------------------------------
// CRemoteStore::GetGsmStoreProperties
// --------------------------------------------------------------------------
//
TInt CRemoteStore::GetGsmStoreProperties( 
    TVPbkGsmStoreProperty& aGsmProperties ) const
    {
    return iSimStore.GetGsmStoreProperties( aGsmProperties );
    }

// --------------------------------------------------------------------------
// CRemoteStore::GetUSimStoreProperties
// --------------------------------------------------------------------------
//
TInt CRemoteStore::GetUSimStoreProperties( 
    TVPbkUSimStoreProperty& aUSimProperties ) const
    {
    return iSimStore.GetUSimStoreProperties( aUSimProperties );
    }

// --------------------------------------------------------------------------
// CRemoteStore::ContactConverter
// --------------------------------------------------------------------------
//
const CVPbkETelCntConverter& CRemoteStore::ContactConverter() const
    {
    return *iContactConverter;
    }

// --------------------------------------------------------------------------
// CRemoteStore::CreateMatchPhoneNumberOperationL
// --------------------------------------------------------------------------
//
MVPbkSimStoreOperation* CRemoteStore::CreateMatchPhoneNumberOperationL( 
    const TDesC& aPhoneNumber, TInt aMaxMatchDigits, 
    MVPbkSimFindObserver& aObserver )
    {
    CNumberMatchOperation* op = CNumberMatchOperation::NewL( *this, 
        aPhoneNumber, aMaxMatchDigits, aObserver );
    op->Activate();
    return op;
    }

// --------------------------------------------------------------------------
// CRemoteStore::CreateFindOperationL
// --------------------------------------------------------------------------
//
MVPbkSimStoreOperation* CRemoteStore::CreateFindOperationL( 
    const TDesC& aStringToFind, RVPbkSimFieldTypeArray& aFieldTypes,
    MVPbkSimFindObserver& aObserver )
    {
    CFindOperation* op = 
        CFindOperation::NewL( *this, aStringToFind, aObserver, aFieldTypes );
    op->Activate();
    return op;
    }

// --------------------------------------------------------------------------
// CRemoteStore::SystemPhoneNumberMaxLength
// --------------------------------------------------------------------------
//
TInt CRemoteStore::SystemPhoneNumberMaxLength() const
    {
    return iSystemPhoneNumberMaxLength;
    }
    
// --------------------------------------------------------------------------
// CRemoteStore::ResetAndCloseStore
// --------------------------------------------------------------------------
//
void CRemoteStore::ResetAndCloseStore()
    {
    if ( iObservers.Count() == 0 )
        {
        // Close session if no observers
        Cancel();
        iSimStore.Close();
        iCurrentState = EVPbkSimUnknown;
        }
    }

// --------------------------------------------------------------------------
// CRemoteStore::DoOpenL
// --------------------------------------------------------------------------
//
void CRemoteStore::DoOpenL( MVPbkSimStoreObserver& aObserver )
    {
    switch ( iCurrentState )
        {
        case EVPbkSimStoreOpen:
            {
            aObserver.StoreReady( *this );
            break;
            }
        case EVPbkSimUnknown:
            {
            // Wait that store receives its state from server.
            break;
            }
        default:
            {
            aObserver.StoreNotAvailable( *this );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CRemoteStore::DoOpenError
// --------------------------------------------------------------------------
//
void CRemoteStore::DoOpenError( MVPbkSimStoreObserver& aObserver, 
        TInt aError )
    {
    aObserver.StoreError( *this, aError );
    }
} // namespace VPbkSimStore
//  End of File  
