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
* Description:  The main session class of the server.
*
*/



// INCLUDE FILES
#include "CVPbkStoreSession.h"

#include "VPbkSimServerOpCodes.h"
#include "CVPbkSimServer.h"
#include "CVPbkViewSubSession.h"
#include "CSaveCommand.h"
#include "CDeleteCommand.h"
#include "CFindManager.h"
#include "SimServerErrors.h"
#include "SimServerInternal.h"
#include "SimServerDebug.h"
#include "CVPbkSimServerEventQueue.h"

#include <VPbkSimStoreTemplateFunctions.h>
#include <CVPbkSimStoreManager.h>
#include <MVPbkSimCntStore.h>
#include <MVPbkSimPhone.h>
#include <TVPbkSimStoreProperty.h>
#include <CVPbkSimContactBuf.h>
#include <RVPbkStreamedIntArray.h>
#include <VPbkDebug.h>

#include <s32mem.h>

// CONSTANTS
// This is some sensible limit for the checking the size of the
// sortarray.
const TInt KVPbkEventGranularity = 10;
const TInt KVPbkMaxEvents = 10 * KVPbkEventGranularity;

// A buffer size for the new contact buffer used in saving
const TInt KInitBufSizeForNewContact = 128;
// An initial buffer size for the find string and matched phone number
const TInt KInitBufSizeForFind = 50;
// An initial buffer size for sim index array
const TInt KInitBufSizeForSimIndexes = 10;
// This a maxium size of the contact that server accepts. It's not
// specified anywhere but server needs some limit to decide that is
// the size from client ok or not. Now set to 16kB that is a huge size
// for USIM contact.
const TInt KMaxContactSize = 16384;

// ============================= LOCAL FUNCTIONS ===============================

namespace {

#ifdef VPBK_ENABLE_DEBUG_PRINT
void PrintInfo( MVPbkSimCntStore* aStore, const RMessage2& aMessage,
        const TDesC& aOpCodeText )
    {
    TInt id = KErrUnknown;
    if ( aStore )
        {
        id = aStore->Identifier();
        }
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimServer: session %x, new message handle %d store id %d function %S(%d)"),
        aMessage.Session(), aMessage.Handle(), id, &aOpCodeText, 
        aMessage.Function() );
    }
#endif // VPBK_ENABLE_DEBUG_PRINT

/**
* A cleanup class for safely creating a subsession
*/
NONSHARABLE_CLASS( TSubSessionCleanup )
    {
public:
    TSubSessionCleanup() : iSubSession( NULL ), iIndex( NULL ) {}
    static void Cleanup( TAny* aSelf ) {
        (static_cast<TSubSessionCleanup*>( aSelf ) )->DoCleanup(); }
private:
    void DoCleanup()
        {
        if ( iIndex )
            {
            iIndex->Remove( iHandle );
            }
        else if ( iSubSession )
            {
            iSubSession->Close();
            }
        }
public:
    CObject* iSubSession;
    CObjectIx* iIndex;
    TInt iHandle;
    };

// -----------------------------------------------------------------------------
// LeaveIfPhoneNotOpennedL
// -----------------------------------------------------------------------------
//
inline void LeaveIfPhoneNotOpennedL( TBool aIsPhoneOpen )
    {
    if ( !aIsPhoneOpen )
        {
        User::Leave( KErrAbort );
        }
    }

// -----------------------------------------------------------------------------
// IsStoreSupported
// -----------------------------------------------------------------------------
//
inline TBool IsStoreSupported( TVPbkSimStoreIdentifier aStoreId,
    TUint32 aServiceTable )
    {
    TBool result = EFalse;
    if ( ( aStoreId == EVPbkSimAdnGlobalStore &&
            aServiceTable & MVPbkSimPhone::KVPbkCapsADNSupported ) ||
         ( aStoreId == EVPbkSimFdnGlobalStore &&
            aServiceTable & MVPbkSimPhone::KVPbkCapsFDNSupported ) ||
         ( aStoreId == EVPbkSimSdnGlobalStore &&
            aServiceTable & MVPbkSimPhone::KVPbkCapsSDNSupported ) ||
         ( aStoreId == EVPbkSimONStore &&
            aServiceTable & MVPbkSimPhone::KVPbkCapsONSupported ) )
        {
        result = ETrue;
        }
    return result;
    }
} // unnamed namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CVPbkStoreSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVPbkStoreSession::CVPbkStoreSession( const CVPbkSimServer& aServer )
:   iServer( const_cast<CVPbkSimServer&>( aServer ) ),
    iNewContactBufPtr( NULL, 0 ),
    iIsPhoneOpen     ( EFalse  ),
    iSimIndexesBufPtr( NULL, 0 )
    {
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::ConstructL()
    {
    iBufForNewContact = HBufC8::NewL( KInitBufSizeForNewContact );
    iNewContactBufPtr.Set( iBufForNewContact->Des() );
    iBufForSimIndexes = HBufC8::NewL( KInitBufSizeForSimIndexes );
    iSimIndexesBufPtr.Set( iBufForSimIndexes->Des() );
    iBufForNumberMatch = HBufC::NewL( KInitBufSizeForFind );
    iNumberMatchManager = VPbkSimServer::CFindManager::NewL();
    iBufForFindString = HBufC::NewL( KInitBufSizeForFind );
    iFindManager = VPbkSimServer::CFindManager::NewL();
    iServer.IncreaseNumberOfSessions();
    iEventQueue = CVPbkSimServerEventQueue::NewL(
        KVPbkEventGranularity, KVPbkMaxEvents);
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVPbkStoreSession* CVPbkStoreSession::NewL(
    const CVPbkSimServer& aServer )
    {
    CVPbkStoreSession* self =
        new( ELeave ) CVPbkStoreSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// Destructor
CVPbkStoreSession::~CVPbkStoreSession()
    {
    AssertNoActiveAsyncRequests();
    
    delete iNumberMatchManager;
    delete iFindManager;
    delete iBufForNewContact;
    delete iBufForNumberMatch;
    delete iBufForFindString;
    delete iLatestReadContact;
    delete iSubSessionIndex;
    delete iEventQueue;
    delete iBufForSimIndexes;
    iSimIndexes.Close();

    if ( iSubSessionCon )
        {
        iServer.RemoveContainer( *iSubSessionCon );
        }

    DoCloseStore();
    iServer.DecreaseNumberOfSessions();
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::ResetLatestDeleteCommand
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::ResetLatestDeleteCommand()
    {
    iLatestDeleteCommand = NULL;
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::ResetLatestSaveCommand
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::ResetLatestSaveCommand()
    {
    iLatestSaveCommand = NULL;
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::ServiceL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::ServiceL( const RMessage2& aMessage )
    {
#if defined(_DEBUG)
    // Check this always in the debug build to make sure that
    // all messages have the related debug text. MessageInTextFormat
    // will panic if the text is not found
    /*const TDesC& text = */VPbkSimServer::MessageInTextFormat( 
        aMessage.Function() );
#endif // _DEBUG

#ifdef VPBK_ENABLE_DEBUG_PRINT
    // Prints message information
    PrintInfo( iStore, aMessage, VPbkSimServer::MessageInTextFormat( 
        aMessage.Function() ) );
#endif // VPBK_ENABLE_DEBUG_PRINT

    // Three step service handling
    // 1) Base services that doesn't need an existing store.
    // 2) Store services that can not succeed if store is not opened.
    // 3) View services that needs a view subsession
    if ( !BaseServiceL( aMessage ) )
        {
        if ( !StoreServiceL( aMessage ) )
            {
            ViewServiceL( aMessage );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::ServiceError
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::ServiceError( const RMessage2 &aMessage, TInt aError )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimServer: session %x, ServiceError"), aMessage.Session() );
    VPbkSimSrvUtility::CompleteRequest( aMessage, aError );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::StoreReady
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::StoreReady( MVPbkSimCntStore& /*aStore*/ )
    {
    SendEvent( EVPbkSimStoreOpen, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::StoreError
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::StoreError( MVPbkSimCntStore& /*aStore*/, TInt aError )
    {
    // There is an error in the store. It can not be used anymore.
    HandleStoreError( aError, EVPbkSimStoreError );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::StoreNotAvailable
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::StoreNotAvailable( MVPbkSimCntStore& /*aStore*/ )
    {
    // Store is not available. Cancel all asynchronous requests but
    // stay observing the store.
    
    // Cancel save command
    CancelSavingContact();
    // Cancel delete command
    CancelDeletingContact();
    // Cancel find request
    CancelFind();
    // Cancel number match
    CancelMatchPhoneNumber();
    
    SendEvent( EVPbkSimStoreNotAvailable, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::StoreContactEvent
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::StoreContactEvent( TEvent aEvent, TInt aSimIndex )
    {
    TVPbkSimEvent storeEvent = EVPbkSimUnknown;
    switch(aEvent)
        {
        case EContactAdded:
            {
            storeEvent = EVPbkSimContactAdded;
            break;
            }
        case EContactDeleted:
            {
            storeEvent = EVPbkSimContactDeleted;
            break;
            }
        case EContactChanged:
            {
            storeEvent = EVPbkSimContactChanged;
            break;
            }
        }

    TVPbkSimContactEventData event;
    event.iEvent = storeEvent;
    event.iData = KErrNone; // This data is not used
    event.iOpData = (TInt16) aSimIndex;

    SendEvent( event, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::PhoneOpened
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::PhoneOpened( MVPbkSimPhone& /*aPhone*/ )
    {
    iIsPhoneOpen = ETrue;

    TVPbkSimContactEventData event;
    event.iEvent = EVPbkSimPhoneOpen;
    event.iData = KErrNone; // This data is not used
    event.iOpData = KErrNone; // This is not used

    SendEvent( event, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::PhoneError
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::PhoneError( MVPbkSimPhone& /*aPhone*/,
    TErrorIdentifier aIdentifier, TInt aError )
    {
    TVPbkSimContactEventData event;
    event.iEvent = EVPbkSimPhoneError;
    event.iData = aIdentifier;
    event.iOpData = aError;

    if ( aIdentifier == MVPbkSimPhoneObserver::ESystem )
        {
        // If it's a system error then this session is unusable.
        HandleStoreError( aError, EVPbkSimPhoneError );
        }
    else
        {
        SendEvent( event, KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::ServiceTableUpdated
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::ServiceTableUpdated( TUint32 aServiceTable )
    {
    if ( !IsStoreSupported( iStoreId, aServiceTable ) )
        {
        // Store is not supported by the (U)SIM
        SendEvent( EVPbkSimStoreNotAvailable, KErrNone );
        }

    // Send the actual service table updated event
    TVPbkSimContactEventData event;
    event.iEvent = EVPbkSimPhoneServiceTableUpdated;
    event.iData = aServiceTable;
    event.iOpData = KErrNotFound; // This is not used
    SendEvent( event, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::FixedDiallingStatusChanged
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::FixedDiallingStatusChanged(
        TInt aFDNStatus )
    {
    TVPbkSimContactEventData event;
    event.iEvent = EVPbkSimPhoneFdnStatusChanged;
    event.iData = aFDNStatus;
    event.iOpData = KErrNotFound; // This is not used
    SendEvent( event, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CreateL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CreateL()
    {
    iSubSessionCon = iServer.NewContainerL();
    iSubSessionIndex = CObjectIx::NewL();
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::BaseServiceL
// -----------------------------------------------------------------------------
//        
TBool CVPbkStoreSession::BaseServiceL( const RMessage2& aMessage )
    {
    TBool result = ETrue;
    switch ( aMessage.Function() )
        {
        case EVPbkSimSrvOpenStore:
            {
            OpenStoreL( aMessage );
            break;
            }
        case EVPbkSimSrvCloseStore:
            {
            CloseStore( aMessage );
            break;
            }
        case EVPbkSimSrvStoreEventNotification:
            {
            SaveStoreEventNotification( aMessage );
            break;
            }
        case EVPbkSimSrvCancelAsyncRequest:
            {
            CancelRequestL( aMessage );
            break;
            }
        case EVPbkSimSrvOpenPhone:
            {
            OpenPhoneL(aMessage);
            break;
            }
        case EVPbkSimSrvClosePhone:
            {
            ClosePhoneL(aMessage);
            break;
            }
        case EVPbkSimSrvUSimAccessSupported:
            {
            GetUSimAccessSupportL(aMessage);
            break;
            }
        case EVPbkSimSrvServiceTable:
            {
            GetServiceTableL(aMessage);
            break;
            }
        case EVPbkSimSrvFixedDiallingStatus:
            {
            GetFDNStatusL(aMessage);
            break;
            }
        default:
            {
            result = EFalse;
            break;
            }
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::StoreServiceL
// -----------------------------------------------------------------------------
//        
TBool CVPbkStoreSession::StoreServiceL( const RMessage2& aMessage )
    {
    TBool result = ETrue;
    if ( !iStore )
        {
        VPbkSimServer::PanicClient( aMessage, 
            VPbkSimServer::ECVPbkStoreSession_StoreServiceL );
        }
    else
        {
        switch ( aMessage.Function() )
            {
            case EVPbkSimSrvGetSizeAndLoadContact:
                {
                GetSizeAndLoadContactL( aMessage );
                break;
                }
            case EVPbkSimSrvGetContact:
                {
                GetContactL( aMessage );
                break;
                }
            case EVPbkSimSrvSaveContact:
                {
                SaveContactL( aMessage );
                break;
                }
            case EVPbkSimSrvDeleteContact:
                {
                DeleteContactL( aMessage );
                break;
                }
            case EVPbkSimSrvGsmStoreProperties:
                {
                GetGsmStorePropertiesL( aMessage );
                break;
                }
            case EVPbkSimSrvUSimStoreProperties:
                {
                GetUSimStorePropertiesL( aMessage );
                break;
                }
            case EVPbkSimSrvMatchPhoneNumber:
                {
                MatchPhoneNumberL( aMessage );
                break;
                }
            case EVPbkSimSrvFind:
                {
                FindL( aMessage );
                break;
                }
            case EVPbkSimSrvOpenView:
                {
                CreateViewSubSessionL( aMessage );
                break;
                }
            case EVPbkSimSrvCloseView:
                {
                CloseViewSubSessionL( aMessage );
                break;
                }
            default:
                {
                result = EFalse;
                break;
                }
            }
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::ViewServiceL
// -----------------------------------------------------------------------------
//    
void CVPbkStoreSession::ViewServiceL( const RMessage2& aMessage )
    {
    CVPbkViewSubSession& view = GetViewOrLeaveL( aMessage.Int3() );
    // View panics the client if it doesn't regonize the opcode
    view.ServiceL( aMessage );
    }
    
// -----------------------------------------------------------------------------
// CVPbkStoreSession::OpenStoreL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::OpenStoreL( const RMessage2& aMessage )
    {
    TInt result = KErrNone;
    if ( iStore )
        {
        result = KErrAlreadyExists;
        }
    else if (!iIsPhoneOpen)
        {
        // phone should be open before the store is openned
        result = KErrNotReady;
        }
    else
        {
        // Read the store identiefier
        TPckg<TVPbkSimStoreIdentifier> pckg( iStoreId );
        aMessage.ReadL( KVPbkSlot0, pckg );

        if ( IsStoreSupported( iStoreId,
            iServer.StoreManager().Phone().ServiceTable() ) )
            {
            TRAPD(res,
                {
                // Get the store
                iStore = &iServer.StoreManager().StoreL( iStoreId );
                iStore->OpenL( *this );
                });
            if ( res != KErrNone )
                {
                HandleStoreError( res, EVPbkSimStoreError );
                }
            }
        else
            {
            // Store is not supported by the (U)SIM
            SendEvent( EVPbkSimStoreNotAvailable, KErrNone );
            }
        }
    VPbkSimSrvUtility::CompleteRequest( aMessage, result );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CloseStore
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CloseStore( const RMessage2& aMessage )
    {
    // Check that client implementation has been done properly
    AssertNoActiveAsyncRequests();
    // Then close the VPbkSimStoreImpl store 
    DoCloseStore();
    // And complete the close message
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::DoCloseStore
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::DoCloseStore()
    {
    // Note: this function must not complete any message.
    
    // Close store
    if ( iStore )
        {
        iStore->Close( *this );
        iStore = NULL;
        }
    // Close phone
    iServer.StoreManager().Phone().Close( *this );
    // Update phone state
    iIsPhoneOpen = EFalse;
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CreateViewSubSessionL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CreateViewSubSessionL( const RMessage2& aMessage )
    {
    TSubSessionCleanup cleanUp;
    CleanupStack::PushL(
        TCleanupItem( TSubSessionCleanup::Cleanup, &cleanUp ) );

    CVPbkViewSubSession* view = CVPbkViewSubSession::NewL( aMessage, *iStore );
    cleanUp.iSubSession = view;
    iSubSessionCon->AddL( view );
    TInt handle = iSubSessionIndex->AddL( view );
    cleanUp.iIndex = iSubSessionIndex;
    cleanUp.iHandle = handle;
    TPckg<TInt> handlePckg( handle );
    aMessage.WriteL( KVPbkSlot3, handlePckg );
    CleanupStack::Pop(); // TCleanupItem

    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CloseViewSubSessionL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CloseViewSubSessionL( const RMessage2& aMessage )
    {
    TInt viewHandle = aMessage.Int3();
    // Check that view exists
    GetViewOrLeaveL( viewHandle );
    // Remove from the index
    iSubSessionIndex->Remove( viewHandle );
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::SaveStoreEventNotification
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::SaveStoreEventNotification( const RMessage2& aMessage )
    {
    // First check if the notification is already set
    if ( iStoreEventMsg.Handle() )
        {
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrAlreadyExists );
        }
    // Second: check if there is an error waiting
    else if ( iErrorEvent.iEvent == EVPbkSimStoreError )
        {
        CompleteNotificationMessage( aMessage, iErrorEvent, KErrNone );
        ResetErrorEvent();
        }
    // Third: check if there is an event waiting
    else if ( !iEventQueue->IsEmpty() )
        {
        CompleteNotificationMessage( aMessage, iEventQueue->FirstEvent(),
            KErrNone );
        iEventQueue->Pop();
        }
    // Save notification message
    else
        {
        iStoreEventMsg = aMessage;
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CancelRequestL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CancelRequestL( const RMessage2& aMessage )
    {
    TInt opCode = KErrUnknown;
    TPckg<TInt> pckg( opCode );
    aMessage.ReadL( KVPbkSlot0, pckg );

    switch ( opCode )
        {
        case EVPbkSimSrvStoreEventNotification:
            {
            CancelStoreEventNotification();
            VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
            break;
            }
        case EVPbkSimSrvSaveContact:
            {
            CancelSavingContact();
            VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
            break;
            }
        case EVPbkSimSrvDeleteContact:
            {
            CancelDeletingContact();
            VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
            break;
            }
        case EVPbkSimSrvMatchPhoneNumber:
            {
            CancelMatchPhoneNumber();
            VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
            break;
            }
        case EVPbkSimSrvFind:
            {
            CancelFind();
            VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
            break;
            }
        default:
            {
            CVPbkViewSubSession& view = GetViewOrLeaveL( aMessage.Int3() );
            // View panics the client if it doesn't regonize the opcode
            view.CancelRequestL( aMessage );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetSizeAndLoadContactL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::GetSizeAndLoadContactL( const RMessage2& aMessage )
    {
    TInt simIndex = aMessage.Int0();
    if ( !iLatestReadContact )
        {
        iLatestReadContact = CVPbkSimContactBuf::NewL( *iStore );
        }
    const TDesC8* cnt = iStore->AtL( simIndex );
    if ( cnt )
        {
        iLatestReadContact->SetL( *cnt );
        TInt length = iLatestReadContact->ETelContactL().Length();
        TPckg<TInt> lengthPckg( length );
        aMessage.WriteL( KVPbkSlot1, lengthPckg );
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
        }
    else
        {
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetContactL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::GetContactL( const RMessage2& aMessage )
    {
    if ( !iLatestReadContact )
        {
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNotFound );
        }
    else
        {
        aMessage.WriteL( KVPbkSlot0, iLatestReadContact->ETelContactL() );
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::SaveContactL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::SaveContactL( const RMessage2& aMessage )
    {
    if ( iLatestSaveCommand )
        {
        VPbkSimServer::PanicClient( aMessage, 
            VPbkSimServer::ECVPbkStoreSession_SaveContactL );
        }
    else
        {
        TInt length = aMessage.GetDesLengthL( KVPbkSlot0 );
        if ( length > 0 && length <= KMaxContactSize )
            {
            VPbkSimStoreImpl::CheckAndUpdateBufferSizeL( iBufForNewContact,
                iNewContactBufPtr, length );
            aMessage.ReadL( KVPbkSlot0, iNewContactBufPtr );
            TInt simIndex = KErrUnknown;
            TPckg<TInt> indexPckg( simIndex );
            aMessage.ReadL( KVPbkSlot1, indexPckg );

            VPbkSimServer::CSaveCommand* cmd = 
                new( ELeave ) VPbkSimServer::CSaveCommand( *this, 
                    iNewContactBufPtr, simIndex, aMessage );
            // Takes owner ship. Not needed to put into cleanup stack
            iServer.AppendModificationCmdL( cmd );
            iLatestSaveCommand = cmd;
            }
        else
            {
            VPbkSimSrvUtility::CompleteRequest( aMessage, KErrArgument );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::DeleteContactL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::DeleteContactL( const RMessage2& aMessage )
    {
    if ( iLatestDeleteCommand )
        {
        VPbkSimServer::PanicClient( aMessage, 
            VPbkSimServer::ECVPbkStoreSession_DeleteContactL );
        }
    else
        {
        TInt length = aMessage.GetDesLengthL( KVPbkSlot0 );
        // Define some max value for indexes: 50000 indexes takes 200000 bytes.
        const TInt maxSizeForIndexes = 200000;
        if ( length > 0 && length <= maxSizeForIndexes )
            {
            VPbkSimStoreImpl::CheckAndUpdateBufferSizeL( iBufForSimIndexes,
                iSimIndexesBufPtr, length );
            aMessage.ReadL( KVPbkSlot0, iSimIndexesBufPtr );
            iSimIndexes.RemoveAll();
            RDesReadStream stream( iSimIndexesBufPtr );
            CleanupClosePushL( stream );
            stream >> iSimIndexes;
            CleanupStack::PopAndDestroy(); // stream
            VPbkSimServer::CDeleteCommand* cmd = 
                new( ELeave ) VPbkSimServer::CDeleteCommand( *this,
                    iSimIndexes, aMessage );
            // Takes owner ship. Not needed to put into cleanup stack
            iServer.AppendModificationCmdL( cmd );
            iLatestDeleteCommand = cmd;
            }
        else
            {
            // Client gave an invalid index array.
            VPbkSimSrvUtility::CompleteRequest( aMessage, KErrArgument );
            }
        }    
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetGsmStorePropertiesL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::GetGsmStorePropertiesL( const RMessage2& aMessage )
    {
    TVPbkGsmStoreProperty gsmProp;
    TInt result = iStore->GetGsmStoreProperties( gsmProp );
    TPckg<TVPbkGsmStoreProperty> pckg( gsmProp );
    aMessage.WriteL( KVPbkSlot0, pckg );
    VPbkSimSrvUtility::CompleteRequest( aMessage, result );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetUSimStorePropertiesL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::GetUSimStorePropertiesL( const RMessage2& aMessage )
    {
    TVPbkUSimStoreProperty usimProp;
    TInt result = iStore->GetUSimStoreProperties( usimProp );
    TPckg<TVPbkUSimStoreProperty> pckg( usimProp );
    aMessage.WriteL( KVPbkSlot0, pckg );
    VPbkSimSrvUtility::CompleteRequest( aMessage, result );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::MatchPhoneNumberL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::MatchPhoneNumberL( const RMessage2& aMessage )
    {
    TInt length = aMessage.GetDesLengthL( KVPbkSlot0 );
    if ( length > 0 )
        {
        TPtr ptr( iBufForNumberMatch->Des() );
        VPbkSimStoreImpl::CheckAndUpdateBufferSizeL( iBufForNumberMatch,
            ptr, length );
        aMessage.ReadL( KVPbkSlot0, ptr );
        MVPbkSimStoreOperation* op =
            iStore->CreateMatchPhoneNumberOperationL( *iBufForNumberMatch,
            aMessage.Int1(), *iNumberMatchManager );
        iNumberMatchManager->SetL( aMessage, op );
        }
    else
        {
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::FindL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::FindL( const RMessage2& aMessage )
    {
    TInt length = aMessage.GetDesLengthL( KVPbkSlot0 );
    if ( length > 0 )
        {
        // Read the find string
        TPtr ptr( iBufForFindString->Des() );
        VPbkSimStoreImpl::CheckAndUpdateBufferSizeL( iBufForFindString,
            ptr, length );
        aMessage.ReadL( KVPbkSlot0, ptr );

        // Read field types
        RVPbkSimFieldTypeArray fieldTypes;
        CleanupClosePushL( fieldTypes );
        VPbkSimSrvUtility::ReadFieldTypesL(fieldTypes, aMessage, KVPbkSlot1 );

        MVPbkSimStoreOperation* op = iStore->CreateFindOperationL(
            *iBufForFindString, fieldTypes, *iFindManager );
        // Takes owner ship. Not needed to put into cleanup stack
        iFindManager->SetL( aMessage, op );
        CleanupStack::PopAndDestroy(); // fieldTypes
        }
    else
        {
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::OpenPhoneL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::OpenPhoneL(const RMessage2& aMessage)
    {
    if ( iIsPhoneOpen )
        {
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrAlreadyExists );
        }
    else
        {
        // The phone must be opened before store to check if the store
        // is supported
        // PhoneOpened()/PhoneError() will be called
        iServer.StoreManager().Phone().OpenL( *this );
        // Complete opening. Client must use store event notification to get
        // open event
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::ClosePhoneL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::ClosePhoneL(const RMessage2& aMessage)
    {
    iIsPhoneOpen = EFalse;
    iServer.StoreManager().Phone().Close( *this );
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetUSimAccessSupportL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::GetUSimAccessSupportL(const RMessage2& aMessage)
    {
    LeaveIfPhoneNotOpennedL(iIsPhoneOpen);
    TBool isSupported(iServer.StoreManager().Phone().USimAccessSupported());
    TPckg<TBool> pckg( isSupported );
    aMessage.WriteL( KVPbkSlot0, pckg );
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetServiceTableL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::GetServiceTableL(const RMessage2& aMessage)
    {
    LeaveIfPhoneNotOpennedL(iIsPhoneOpen);
    TUint32 serviceTable(iServer.StoreManager().Phone().ServiceTable());
    TPckg<TUint32> pckg( serviceTable );
    aMessage.WriteL( KVPbkSlot0, pckg );
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetFDNStatusL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::GetFDNStatusL(const RMessage2& aMessage)
    {
    LeaveIfPhoneNotOpennedL(iIsPhoneOpen);
    MVPbkSimPhone::TFDNStatus status(
        iServer.StoreManager().Phone().FixedDialingStatus());
    TPckg<MVPbkSimPhone::TFDNStatus> pckg(status);
    aMessage.WriteL( KVPbkSlot0, pckg );
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetViewOrLeaveL
// -----------------------------------------------------------------------------
//
CVPbkViewSubSession& CVPbkStoreSession::GetViewOrLeaveL( TInt aViewHandle )
    {
    CObject* view = iSubSessionIndex->At( aViewHandle );
    if ( !view )
        {
        User::Leave( KErrBadHandle );
        }
    return static_cast<CVPbkViewSubSession&>( *view );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CompleteNotificationMessage
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CompleteNotificationMessage( const RMessage2& aMsg,
    TVPbkSimContactEventData& aEventData, TInt aResult )
    {
    if (aMsg.Handle())
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimServer: session %x, complete event %d data %d opdata %d"),
            aMsg.Session(), aEventData.iEvent,aEventData.iData,
            aEventData.iOpData );
        TPckg<TVPbkSimContactEventData> pckg( aEventData );
        TInt result = aMsg.Write( KVPbkSlot0, pckg );
        if ( result != KErrNone )
            {
            VPbkSimSrvUtility::CompleteRequest( aMsg, result );
            }
        else
            {
            VPbkSimSrvUtility::CompleteRequest( aMsg, aResult );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::HandleEvent
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::SendEvent( TVPbkSimEvent aEvent,
    TInt aEventResult )
    {
    TVPbkSimContactEventData event;
    event.iEvent = aEvent;
    event.iData = KErrNone; // Not used
    event.iOpData = aEventResult;
    // This will always complete the event message itself with KErrNone.
    SendEvent( event, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkViewSubSession::SendEvent
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::SendEvent( TVPbkSimContactEventData& aEventData, 
        TInt aCompletionResult )
    {
    if ( iStoreEventMsg.Handle() )
        {
        CompleteNotificationMessage( iStoreEventMsg, aEventData, 
            aCompletionResult );
        }
    else
        {
        if ( iEventQueue->IsFull() )
            {
            SetErrorEvent( KErrOverflow );
            }
        else
            {
            TInt result = iEventQueue->Push( aEventData );
            if ( result != KErrNone )
                {
                SetErrorEvent( result );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CancelStoreEventNotification
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CancelStoreEventNotification()
    {
    if ( iStoreEventMsg.Handle() )
        {
        VPbkSimSrvUtility::CompleteRequest( iStoreEventMsg, KErrCancel );
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CancelSavingContact
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CancelSavingContact()
    {
    if ( iLatestSaveCommand )
        {
        iServer.CancelCommand( *iLatestSaveCommand );
        iLatestSaveCommand = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CancelDeletingContact
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CancelDeletingContact()
    {
    if ( iLatestDeleteCommand )
        {
        iServer.CancelCommand( *iLatestDeleteCommand );
        iLatestDeleteCommand = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CancelMatchPhoneNumber
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CancelMatchPhoneNumber()
    {
    iNumberMatchManager->CancelMessage();
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CancelFind
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CancelFind()
    {
    iFindManager->CancelMessage();
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::HandleStoreError
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::HandleStoreError( TInt aError, 
        TVPbkSimEvent aErrorEvent )
    {
    // Cancel save command
    CancelSavingContact();
    
    // Cancel delete command
    CancelDeletingContact();
    
    // Cancel find request
    CancelFind();
    
    // Cancel number match
    CancelMatchPhoneNumber();
    
    // Cancel store event message
    TVPbkSimContactEventData event;
    event.iEvent = aErrorEvent;
    event.iData = KErrNone; // Not used
    event.iOpData = aError;
    SendEvent( event, aError );
    }
    
// -----------------------------------------------------------------------------
// CVPbkStoreSession::SetErrorEvent
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::SetErrorEvent( TInt aError )
    {
    iErrorEvent.iEvent = EVPbkSimViewError;
    iErrorEvent.iData = aError;
    }

// -----------------------------------------------------------------------------
// CVPbkViewSubSession::ResetErrorEvent
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::ResetErrorEvent()
    {
    iErrorEvent.iEvent = EVPbkSimUnknown;
    iErrorEvent.iData = 0;
    iErrorEvent.iOpData = 0;
    }

// -----------------------------------------------------------------------------
// CVPbkViewSubSession::AssertNoActiveAsyncRequests
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::AssertNoActiveAsyncRequests()
    {
    // Check that client has canceled the event message.
    if ( iStoreEventMsg.Handle() )
        {
        VPbkSimServer::PanicClient( iStoreEventMsg,
            VPbkSimServer::ECVPbkStoreSession_CloseStore_EventMessage );
        }
    
    // Check that client has cancelled the save command
    if ( iLatestSaveCommand )
        {
        iLatestSaveCommand->PanicClient( 
            VPbkSimServer::ECVPbkStoreSession_CloseStore_Save );
        }
    
    // Check that client has cancelled the delete command
    if ( iLatestDeleteCommand )
        {
        iLatestDeleteCommand->PanicClient( 
            VPbkSimServer::ECVPbkStoreSession_CloseStore_Delete );
        }
    
    // Check that client has cancelled the number match request
    if ( iNumberMatchManager->IsActive() )
        {
        iNumberMatchManager->PanicClient( 
            VPbkSimServer::ECVPbkStoreSession_CloseStore_MatchNumber );
        }
    
    // Check that client has cancelled the find request
    if ( iFindManager->IsActive() )
        {
        iFindManager->PanicClient( 
            VPbkSimServer::ECVPbkStoreSession_CloseStore_Find );
        }
    }
//  End of File
