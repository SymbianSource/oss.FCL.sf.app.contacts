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
* Description:  A stub version of the class CVPbkStoreSession for emulator.
*                The meaning of this stub is to make it possible for the client
*                to open a store and a create a view from it.
*
*/



// INCLUDE FILES
#include "CVPbkStoreSession.h"

#include <TVPbkSimStoreProperty.h>
#include "VPbkDebug.h"
#include "VPbkSimServerOpCodes.h"
#include "CVPbkSimServer.h"
#include "SimServerInternal.h"

// ============================ LOCAL FUNCTIONS ================================

/**
* A class for keeping view event notification. Stub doesn't have view
* subsession class so the event must be saved in session class
*/
struct TViewEventMessageHolder
    {
    RMessage2 iMessage;
    };

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
    iIsPhoneOpen(EFalse),
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
    iServer.IncreaseNumberOfSessions();
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
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CVPbkStoreSession::~CVPbkStoreSession()
    {
    // iLatestSaveCommand pointer is used for saving view event
    // notification in the stub. There is no view subsession in
    // the stub so it must be saved somewhere.
    TViewEventMessageHolder* holder =
        (TViewEventMessageHolder*) iLatestSaveCommand;
    delete holder;
    delete iSubSessionIndex;

    if ( iSubSessionCon )
        {
        iServer.RemoveContainer( *iSubSessionCon );
        }

    iServer.DecreaseNumberOfSessions();
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::ServiceL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::ServiceL( const RMessage2& aMessage )
    {
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
        case EVPbkSimSrvChangeViewSortOrder:
            {
            // Ignore the change sort order event, because there can't be views createad
            // from the stubbed store.
            VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
            break;
            }
        case EVPbkSimSrvViewEventNotification:
            {
            // iLatestSaveCommand pointer is used for saving view event
            // notification in the stub. There is no view subsession in
            // the stub so it must be saved somewhere.
            if ( !iLatestSaveCommand )
                {
                // When this is called first time notify client
                // that the view is not available.
                TVPbkSimContactEventData event;
                event.iEvent = EVPbkSimViewNotAvailable;
                TPckg<TVPbkSimContactEventData> pckg( event );
                aMessage.WriteL( KVPbkSlot0, pckg );
                VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
                // Create a message holder for next notification message
                TViewEventMessageHolder* holder =
                    new( ELeave ) TViewEventMessageHolder;
                iLatestSaveCommand = (VPbkSimServer::CSaveCommand*)holder;
                }
            else
                {
                // Client has received the first notification and activates
                // it again. Message is saved so that it can be cancelled
                // when the view is closed
                ((TViewEventMessageHolder*)(iLatestSaveCommand))->iMessage =
                    aMessage;
                }
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
            VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNotSupported );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::ServiceError
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::ServiceError( const RMessage2 &aMessage, TInt aError )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING("VPbkSimServer: ServiceError"));
    VPbkSimSrvUtility::CompleteRequest( aMessage, aError );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::StoreReady
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::StoreReady( MVPbkSimCntStore& /*aStore*/ )
    {
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::StoreError
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::StoreError( MVPbkSimCntStore& /*aStore*/,
    TInt /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::StoreNotAvailable
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::StoreNotAvailable( MVPbkSimCntStore& /*aStore*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// CVPbkStoreSession::StoreNotAvailable
// -----------------------------------------------------------------------------
//
void 
CVPbkStoreSession::StoreContactEvent( TEvent /*aEvent*/, TInt /*aSimIndex*/ )
    {    
    }    

// -----------------------------------------------------------------------------
// CVPbkStoreSession::PhoneOpened
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::PhoneOpened( MVPbkSimPhone& /*aPhone*/ )
    {

    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::PhoneError
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::PhoneError( MVPbkSimPhone& /*aPhone*/,
    TErrorIdentifier /*aIdentifier*/, TInt /*aError*/ )
    {

    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::ServiceTableUpdated
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::ServiceTableUpdated( TUint32 /*aServiceTable*/ )
    {

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
// CVPbkStoreSession::OpenStoreL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::OpenStoreL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CloseStore
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CloseStore( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CreateViewSubSessionL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CreateViewSubSessionL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::CloseViewSubSessionL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::CloseViewSubSessionL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::SaveStoreEventNotification
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::SaveStoreEventNotification( const RMessage2& aMessage )
    {
    iStoreEventMsg = aMessage;
    if (!iIsPhoneOpen)
        {
        iIsPhoneOpen = ETrue;
        TVPbkSimContactEventData event;
        event.iEvent = EVPbkSimPhoneOpen;
        event.iData = KErrNotFound;
        event.iOpData = KErrNone;        
        CompleteNotificationMessage( iStoreEventMsg, event, KErrNone );
        }
    // iStore pointer is used to check if this function has been called before.
    // This is necessary in the stub because client keeps listening to store
    // events and completing request every time would cause a loop
    else if ( !iStore )
        {
        iStore = (MVPbkSimCntStore*)0xffffffff;
        TVPbkSimContactEventData event;
        event.iEvent = EVPbkSimStoreNotAvailable;
        event.iData = KErrNotFound;
        event.iOpData = KErrNone;        
        CompleteNotificationMessage( iStoreEventMsg, event, KErrNone );
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

    if ( opCode == EVPbkSimSrvStoreEventNotification )
        {
        CancelStoreEventNotification();
        }
    else if ( opCode == EVPbkSimSrvViewEventNotification &&
              iLatestSaveCommand )
        {
        // iLatestSaveCommand is used to keep view event notification message
        // in the stub because there is no view subsession.
        TViewEventMessageHolder* holder =
            (TViewEventMessageHolder*) iLatestSaveCommand;
        if ( holder->iMessage.Handle() )
            {
            VPbkSimSrvUtility::CompleteRequest( holder->iMessage, KErrCancel );
            }
        }
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetSizeAndLoadContactL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::GetSizeAndLoadContactL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetContactL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::GetContactL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::SaveContactL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::SaveContactL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::DeleteContactL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::DeleteContactL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetGsmStorePropertiesL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::GetGsmStorePropertiesL( const RMessage2& aMessage )
    {
    TVPbkGsmStoreProperty gsmProp;
    gsmProp.iCaps = 0;
    gsmProp.iMaxNumLength = KErrNotSupported;
    gsmProp.iMaxTextLength = KErrNotSupported;
    gsmProp.iTotalEntries = KErrNotSupported;
    gsmProp.iUsedEntries = KErrNotSupported;
    TPckg<TVPbkGsmStoreProperty> pckg( gsmProp );
    aMessage.WriteL( KVPbkSlot0, pckg );
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetUSimStorePropertiesL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::GetUSimStorePropertiesL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::MatchPhoneNumberL
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::MatchPhoneNumberL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNotSupported );
    }
    
/// Opens the phone
void CVPbkStoreSession::OpenPhoneL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }
    
/// Closes the phone
void CVPbkStoreSession::ClosePhoneL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }
    
/// Gets the usim access support state
void CVPbkStoreSession::GetUSimAccessSupportL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNotSupported );
    }
    
/// Gets the service table
void CVPbkStoreSession::GetServiceTableL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNotSupported );
    }
    
/// Gets the fixed dialling status
void CVPbkStoreSession::GetFDNStatusL( const RMessage2& aMessage )
    {
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVPbkStoreSession::GetViewOrLeaveL
// -----------------------------------------------------------------------------
//
CVPbkViewSubSession& CVPbkStoreSession::GetViewOrLeaveL( TInt /*aViewHandle*/ )
    {
    // only make it compile
    CVPbkViewSubSession* view = NULL;
    return *view;
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
            "VPbkSimServer: complete store notification event %d data %d opdata %d"),
            aEventData.iEvent,aEventData.iData,aEventData.iOpData );
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
// CVPbkStoreSession::FixedDiallingStatusChanged
// -----------------------------------------------------------------------------
//
void CVPbkStoreSession::FixedDiallingStatusChanged(
        TInt /*aFDNStatus*/ )
    {
    }

//  End of File
