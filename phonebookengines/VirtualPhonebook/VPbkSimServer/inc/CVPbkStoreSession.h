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



#ifndef CVPBKSTORESESSION_H
#define CVPBKSTORESESSION_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkSimStoreObserver.h>
#include <MVPbkSimPhoneObserver.h>
#include <VPbkSimStoreCommon.h>
#include "VPbkSimServerCommon.h"
#include <RVPbkStreamedIntArray.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CVPbkSimServer;
class CVPbkViewSubSession;
class MVPbkSimCommand;
class CVPbkSimContactBuf;
class CVPbkSimServerEventQueue;

namespace VPbkSimServer {
// FORWARD DECLARATIONS
class CFindManager;
class CSaveCommand;
class CDeleteCommand;
} // namespace VPbkSimServer

// CLASS DECLARATION

/**
 *  The main session class of the server.
 *
 */
NONSHARABLE_CLASS(CVPbkStoreSession) :
        public CSession2,
        public MVPbkSimStoreObserver,
        public MVPbkSimPhoneObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param a the sim server of the session
        * @return a new instance of this class
        */
        static CVPbkStoreSession* NewL( const CVPbkSimServer& aServer );

        /**
        * Destructor.
        */
        virtual ~CVPbkStoreSession();

    public: // New functions

        /**
        * Returns the store that this session uses
        * @return the store that this session uses
        */
        inline MVPbkSimCntStore& Store();

        /**
        * Sets latest delete command to NULL. Called by the delete command.
        */
        void ResetLatestDeleteCommand();

        /**
        * Sets latest save command to NULL. Called by the save command.
        */
        void ResetLatestSaveCommand();
        
        /**
         * Access to security info
         */
        inline TSecurityInfo& SecurityInfo();

    public: // Functions from base classes

        /**
        * From CSession2
        */
        void ServiceL( const RMessage2& aMessage );

        /**
        * From CSession2
        */
        void ServiceError( const RMessage2 &aMessage, TInt aError );

        /**
        * From MVPbkSimStoreObserver
        */
        void StoreReady( MVPbkSimCntStore& aStore );

        /**
        * From MVPbkSimStoreObserver
        */
        void StoreError( MVPbkSimCntStore& aStore, TInt aError );

        /**
        * From MVPbkSimStoreObserver
        */
        void StoreNotAvailable( MVPbkSimCntStore& aStore );

        /**
         * From MVPbkSimStoreObserver
         */
        void StoreContactEvent( TEvent aEvent, TInt aSimIndex );

        /**
        * From MVPbkSimPhoneObserver
        */
        void PhoneOpened( MVPbkSimPhone& aPhone );

        /**
        * From MVPbkSimPhoneObserver
        */
        void PhoneError( MVPbkSimPhone& aPhone,
            TErrorIdentifier aIdentifier, TInt aError );

        /**
        * From MVPbkSimPhoneObserver
        */
        void ServiceTableUpdated( TUint32 aServiceTable );

        /**
        * From MVPbkSimPhoneObserver
        */
        void FixedDiallingStatusChanged(
            TInt aFDNStatus );

    private: // Construction

        /**
        * C++ constructor.
        */
        CVPbkStoreSession( const CVPbkSimServer& aServer );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:  // Functions from base classes

        /**
        * From CSession2
        */
        void CreateL();

    private:    // New functions

        // Sub service handlers called from ServiceL
        TBool BaseServiceL( const RMessage2& aMessage );
        TBool StoreServiceL( const RMessage2& aMessage );
        void ViewServiceL( const RMessage2& aMessage );

        // Service implementation functions

        /// Opens a store
        void OpenStoreL( const RMessage2& aMessage );
        /// Closes the store
        void CloseStore( const RMessage2& aMessage );
        /// Closes the store and the phone
        void DoCloseStore();
        /// Creates a new view subsession
        void CreateViewSubSessionL( const RMessage2& aMessage );
        /// Closes a view subsession
        void CloseViewSubSessionL( const RMessage2& aMessage );
        /// Saves the store event message
        void SaveStoreEventNotification( const RMessage2& aMessage );
        /// Cancels asynchronous message.
        void CancelRequestL( const RMessage2& aMessage );
        /// Reads the contact and returns the size of the contact
        void GetSizeAndLoadContactL( const RMessage2& aMessage );
        /// Reads a contact
        void GetContactL( const RMessage2& aMessage );
        /// Creates a new save command
        void SaveContactL( const RMessage2& aMessage );
        /// Creates a new delete command
        void DeleteContactL( const RMessage2& aMessage );
        /// Gets gsm store properties
        void GetGsmStorePropertiesL( const RMessage2& aMessage );
        /// Gets usim store properties
        void GetUSimStorePropertiesL( const RMessage2& aMessage );
        /// Creates a phone number match operation
        void MatchPhoneNumberL( const RMessage2& aMessage );
        /// Creates a find operation
        void FindL( const RMessage2& aMessage );
        /// Opens the phone
        void OpenPhoneL( const RMessage2& aMessage );
        /// Closes the phone
        void ClosePhoneL( const RMessage2& aMessage );
        /// Gets the usim access support state
        void GetUSimAccessSupportL( const RMessage2& aMessage );
        /// Gets the service table
        void GetServiceTableL( const RMessage2& aMessage );
        /// Gets the fixed dialling status
        void GetFDNStatusL( const RMessage2& aMessage );

        /// Returns the view corresponding to the handle.
        /// @exception KErrBadHandle if not found
        CVPbkViewSubSession& GetViewOrLeaveL( TInt aViewHandle );
        /// Handles the event, either completes it or add to queue
        void SendEvent( TVPbkSimEvent aEvent,
            TInt aEventResult );
        void SendEvent( TVPbkSimContactEventData& aEventData,
            TInt aCompletionResult );
        /// Completes the store notification message.
        void CompleteNotificationMessage( const RMessage2& aMsg,
            TVPbkSimContactEventData& aEventData, TInt aResult );
        /// Completes the store event notification using KErrCancel.
        void CancelStoreEventNotification();
        /// Cancels saving
        void CancelSavingContact();
        /// Cancels deleting
        void CancelDeletingContact();
        /// Cancels matching
        void CancelMatchPhoneNumber();
        /// Cancels find
        void CancelFind();
        /// Does the actions that are needed after an error has occured
        void HandleStoreError( TInt aError, TVPbkSimEvent aErrorEvent );
        /// Save error event
        void SetErrorEvent( TInt aError );
        /// Reset error event
        void ResetErrorEvent();
        /// Panics client and cancels requests if client is closing
        /// the session before canceling requests
        void AssertNoActiveAsyncRequests();

    private:    // Data
        /// A reference to the server instance
        CVPbkSimServer& iServer;
        /// Ref: a container for the subsessions
        CObjectCon* iSubSessionCon;
        /// Own: the subsession index that takes care of handles of sessions
        CObjectIx* iSubSessionIndex;
        /// Ref: the sim store
        MVPbkSimCntStore* iStore;
        /// Ref: the message that is completed after store event arrives
        RMessage2 iStoreEventMsg;
        /// Own: the contact that is recently loaded
        CVPbkSimContactBuf* iLatestReadContact;
        /// Own: the contact that is saved to (U)SIM
        HBufC8* iBufForNewContact;
        /// a pointer to the new contact buffer
        TPtr8 iNewContactBufPtr;
        /// Own: a buffer for phone number used in number match
        HBufC* iBufForNumberMatch;
        /// Ref: the lates save command for canceling command
        VPbkSimServer::CSaveCommand* iLatestSaveCommand;
        /// Ref: the lates delete command for canceling command
        VPbkSimServer::CDeleteCommand* iLatestDeleteCommand;
        /// Own: a manger for number matching operation
        VPbkSimServer::CFindManager* iNumberMatchManager;
        /// The store identifier
        TVPbkSimStoreIdentifier iStoreId;
        /// Own: a buffer for find string
        HBufC* iBufForFindString;
        /// Own: a manager for find operation
        VPbkSimServer::CFindManager* iFindManager;
        /// Own: The queue of view events
        CVPbkSimServerEventQueue* iEventQueue;
        /// Own: error event
        TVPbkSimContactEventData iErrorEvent;
        /// Own: flag indicating the phone's state
        TBool iIsPhoneOpen;
        /// Own: Buffer for Sim Indexes
        HBufC8* iBufForSimIndexes;
        /// a pointer to Sim Indexes
        TPtr8 iSimIndexesBufPtr;
        /// Own: The sim indexes to be delete
        RVPbkStreamedIntArray iSimIndexes;
        /// Own: Security information of client
        TSecurityInfo iSecurityInfo;
    };

// INLINE FUNCTIONS
inline MVPbkSimCntStore& CVPbkStoreSession::Store()
    {
    return *iStore;
    }
    
inline TSecurityInfo& CVPbkStoreSession::SecurityInfo()
    { 
    return iSecurityInfo;
    }

#endif      // CVPBKSTORESESSION_H

// End of File
