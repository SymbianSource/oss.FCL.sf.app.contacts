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
* Description:  A command to delete a contact from the sim store
*
*/



#ifndef VPBKSIMSERVER_CDELETECOMMAND_H
#define VPBKSIMSERVER_CDELETECOMMAND_H

//  INCLUDES
#include "SimServerErrors.h"
#include <e32base.h>
#include <MVPbkSimCommand.h>
#include <MVPbkSimContactObserver.h>
#include <RVPbkStreamedIntArray.h>

// FORWARD DECLARATIONS
class CVPbkStoreSession;
class MVPbkSimStoreOperation;

namespace VPbkSimServer {
    
// CLASS DECLARATION

/**
*  A command to delete a contact from the sim store
*
*/
NONSHARABLE_CLASS(CDeleteCommand) : 
        public CBase,
        public MVPbkSimCommand,
        public MVPbkSimContactObserver
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        * @param aParentSession the session that created this command
        * @param aStreamedIntArray sim indexes in stream
        * @param aMessage the message that must completed after request is done
        */
        CDeleteCommand( CVPbkStoreSession& aParentSession, 
                        RVPbkStreamedIntArray& aSimIndexes,
                        const RMessage2& aMessage );

        /**
        * Destructor.
        */
        virtual ~CDeleteCommand();

    public:  // New functions
    
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
        void CompleteDeleteRequest( TInt aResult );

    private:    // Data
        /// the session that created this command
        CVPbkStoreSession& iParentSession;
        /// Ref: The sim indexes to be delete
        RVPbkStreamedIntArray& iSimIndexes;
        /// the message that must completed after request is done
        RMessage2 iMessage;
        /// the command store is the observer for this command
        MVPbkSimCommandObserver* iObserver;
        /// Own: a handle to the delete operation
        MVPbkSimStoreOperation* iDeleteOperation;
    };

} // namespace VPbkSimServer

#endif      // VPBKSIMSERVER_CDELETECOMMAND_H
            
// End of File
