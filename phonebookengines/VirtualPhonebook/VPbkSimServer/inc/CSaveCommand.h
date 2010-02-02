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
* Description:  A command to save a contact to sim store
*
*/



#ifndef VPBKSIMSERVER_CSAVECOMMAND_H
#define VPBKSIMSERVER_CSAVECOMMAND_H

//  INCLUDES
#include "SimServerErrors.h"
#include <e32base.h>
#include <MVPbkSimCommand.h>
#include <MVPbkSimContactObserver.h>

// FORWARD DECLARATIONS
class CVPbkStoreSession;
class MVPbkSimStoreOperation;

namespace VPbkSimServer {

// CLASS DECLARATION

/**
 *  A command to save a contact to sim store
 */
NONSHARABLE_CLASS(CSaveCommand) : 
        public CBase,
        public MVPbkSimCommand,
        public MVPbkSimContactObserver
    {
    public:  // Constructors and destructor
        /**
        * C++ constructor.
        * @param aParentSession the session that created this command
        * @param aETelContact the contact in ETel format
        * @param aSimIndex the sim index for the contact
        * @param aMessage the message that must completed after request is done
        */
        CSaveCommand( CVPbkStoreSession& aParentSession, 
            const TDesC8& aETelContact, TInt aSimIndex,
            const RMessage2& aMessage );

        /**
        * Destructor.
        */
        virtual ~CSaveCommand();

    public: // New functions
        
        /**
         * Panics the client process with aPanicCode
         *
         * @param aPanicCode the panic code to use
         */
        void PanicClient( TClientPanicCode aPanicCode );
        
    public: // Functions from base classes

        /**
        * From MVPbkSimCommand
        */
        void Execute();

        /**
        * From MVPbkSimCommand
        */
        void AddObserverL( MVPbkSimCommandObserver& aObserver );

        /**
        * From MVPbkSimCommand
        */
        void CancelCmd();

        /**
        * From MVPbkSimContactObserver
        */
        void ContactEventComplete( TEvent aEvent, CVPbkSimContact* aContact );

        /**
        * From MVPbkSimContactObserver
        */
        void ContactEventError( TEvent aEvent, CVPbkSimContact* aContact, 
            TInt aError );
        
    private:    // New functions

        /// Completes the message with given result
        void CompleteSaveRequest( TInt aResult );

    private:    // Data
        /// Ref: The session that created this command
        CVPbkStoreSession& iParentSession;
        /// Ref: The contact in ETel format
        const TDesC8& iETelContact;
        /// Ref: Sim index for the contact
        TInt iSimIndex;
        /// Ref: The message that must be completed after request is done
        RMessage2 iMessage;
        /// Ref: The command store is the observer for this command
        MVPbkSimCommandObserver* iObserver;
        /// Own: a handle to the save operation
        MVPbkSimStoreOperation* iSaveOperation;
    };

} // namespace VPbkSimServer

#endif      // VPBKSIMSERVER_CSAVECOMMAND_H
            
// End of File
