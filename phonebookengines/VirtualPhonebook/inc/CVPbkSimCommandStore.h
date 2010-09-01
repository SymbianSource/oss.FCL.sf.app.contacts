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
* Description:  A command store for handling lifetime of the commands
*
*/



#ifndef CVPBKSIMCOMMANDSTORE_H
#define CVPBKSIMCOMMANDSTORE_H

//  INCLUDES
#include <e32base.h>
#include "MVPbkSimCommandObserver.h"

// FORWARD DECLARATIONS
class MVPbkSimCommand;

// CLASS DECLARATION

/**
*  A command store for handling lifetime of the commands
*
*/
class CVPbkSimCommandStore : public CBase,
                      protected MVPbkSimCommandObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aExecuteOneAtTime if this ETrue commands run one at a time.
        *           Otherwise commands are executed immediately which means
        *           multiple commands can run at a same time
        * @return a new instance of this class
        */
        IMPORT_C static CVPbkSimCommandStore* NewL( TBool aExecuteOneAtTime );
        
        /**
        * Destructor.
        */
        virtual ~CVPbkSimCommandStore();

    public: // New functions
        
        /**
        * Takes ownership of the command and executes it after that.
        * The store supports only one active command at the time
        * @param aCommand the command to execute
        */
        IMPORT_C void AddAndExecuteCommandL( MVPbkSimCommand* aCommand );

        /**
        * Cancels all commands.
        */
        IMPORT_C void CancelAll();

        /**
        * Cancels specified command
        */
        IMPORT_C void CancelCommand( MVPbkSimCommand& aCommand );

        /**
        * @return ETrue if there an executing command in the store
        */
        TBool Executing() const;

    protected: // Functions from base classes

        /**
        * From MVPbkSimCommandObserver
        */
        void CommandDone( MVPbkSimCommand& aCommand );

        /**
        * From MVPbkSimCommandObserver
        */
        void CommandError( MVPbkSimCommand& aCommand, TInt aError );
        
    private:    // Construction

        /**
        * C++ constructor.
        */
        CVPbkSimCommandStore( TBool aExecuteOneAtTime );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // New functions
        /// Runs next waiting command if none are running
        void RunNextCommand();
        /// Handles command completion/deletion
        void HandleCommandComplete( MVPbkSimCommand& aCommand );
        /// Start idle deletion of commands
        void StartAsyncDelete();
        /// Starts an idle object for running next command
        void StartAsyncCommandRunner();
        /// An idle destroyer callback function
        static TInt IdleDestroyerCallback( TAny* aSelf );
        /// Called from callback function to delete commands
        void DestroyCommands();
        /// A callback for async command execution
        static TInt CmdRunnerCallback( TAny* aSelf );
        /// Position or KErrNotFound
        TInt FindCommandPosition( MVPbkSimCommand& aCommand );
        
    private:    // Data
        class CCmd;
        /// An array of commands that will be deleted
        RPointerArray<CCmd> iCmdArray;
        /// Own: a destroyer for commands
        CIdle* iIdleDestroyer;
        /// Own: for async execute
        CIdle* iIdleCmdRunner;
        /// ETrue if commands run one at a time
        TBool iExecuteOneAtTime;
    };

#endif      // CVPBKSIMCOMMANDSTORE_H
            
// End of File
