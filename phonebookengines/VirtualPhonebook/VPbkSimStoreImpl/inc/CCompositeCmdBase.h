/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A composite command base class
*
*/



#ifndef VPBKSIMSTOREIMPL_CCOMPOSITECMDBASE_H
#define VPBKSIMSTOREIMPL_CCOMPOSITECMDBASE_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkSimCommand.h>
#include <MVPbkSimCommandObserver.h>

// FORWARD DECLARATIONS


namespace VPbkSimStoreImpl {

// CLASS DECLARATION

/**
*  A command base class
*
*/
NONSHARABLE_CLASS(CCompositeCmdBase) : public CBase,
                                       public MVPbkSimCommand,
                                       private MVPbkSimCommandObserver
    {
    public: // Destruction

        /**
        * Destructor.
        */
        ~CCompositeCmdBase();

    public: // New functions
        
        /**
        * Adds a new subcommand to this composite
        *
        * @param aSubCommand A new subcommand to the composite.
        */
        void AddSubCommandL( MVPbkSimCommand* aSubCommand );
        
    public: // Functions from base classes

        /**
        * Subclass must handle command execution logic
        */
        virtual void Execute() = 0;
                
        /**
        * From MVPbkSimCommand.
        * Only one observer supported.
        */
        void AddObserverL( MVPbkSimCommandObserver& aObserver );
    
        /**
        * From MVPbkSimCommand
        */
        void CancelCmd();

    private: // Functions from base classes
        
        /**
        * From MVPbkSimCommandObserver
        * Subclass must implement the logic for this
        */
        virtual void CommandDone( MVPbkSimCommand& aCommand ) = 0;

        /**
        * From MVPbkSimCommandObserver.
        * Default error handling is to forward error to composite's
        * observer.
        */
        void CommandError( MVPbkSimCommand& aCommand, TInt aError );

    protected:

        /**
        * C++ constructor.
        */
        CCompositeCmdBase();
        
        /**
        * Returns the subcommands for the subclass.
        *
        * @return An array of subcommands.
        */
        inline TArray<MVPbkSimCommand*> SubCommands();
        
        /**
        * Sends command done event to composite's observer
        */
        void SendCommandDone();
        
        /**
        * Sends command error event to composite's observer
        */
        void SendCommandError( TInt aError );
        
    private:    // Data
        /// Ref: the command that runs after this or NULL
        RPointerArray<MVPbkSimCommand> iSubCommands;
        /// Ref: The observer of this command
        RPointerArray<MVPbkSimCommandObserver> iObservers;
    };

// INLINE FUNCTIONS
inline TArray<MVPbkSimCommand*> CCompositeCmdBase::SubCommands()
    {
    return iSubCommands.Array();
    }
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CCOMPOSITECMDBASE_H
            
// End of File
