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
* Description:  Takes a command and executes it if predefined
*                condition is met
*
*/


#ifndef CPBK2COMMANDACTIVATOR_H
#define CPBK2COMMANDACTIVATOR_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MPbk2CommandObserver.h>
#include <MPbk2StoreStateCheckerObserver.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class CPbk2StoreStateChecker;

// CLASS DECLARATIONS
/**
 * Phonebook 2 executes given command if predefined condition is met
 */
class CPbk2CommandActivator :  
        public CBase,
        public MPbk2Command,
        private MPbk2CommandObserver,
        private MPbk2StoreStateCheckerObserver
    {
    public: // Constructor and destructor
        IMPORT_C static CPbk2CommandActivator* NewL( MPbk2Command* aCommand, 
                     const MPbk2ContactUiControl& aUiControl );
        /**
         * Destructor.
         */
        IMPORT_C ~CPbk2CommandActivator();

    public: // From MPbk2Command
        void ExecuteLD();
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);
        void AddObserver( MPbk2CommandObserver& aObserver );
        TAny* CommandExtension(TUid /*aExtensionUid*/);                     
    private: // MPbk2CommandObserver
        void CommandFinished( const MPbk2Command& aCommand );

    private: // MPbk2StoreStateCheckerObserver
        void StoreState( 
                MPbk2StoreStateCheckerObserver::TState aState );

    private: // Implementation
        CPbk2CommandActivator( MPbk2Command* aCommand, 
                               const MPbk2ContactUiControl& aUiControl );
                               
    private: // Data
        /// Own: Commant to be executed
        MPbk2Command* iCommand;
        /// Not Owned: UiControl which has information about the environment
        ///            where that command was launched.
        const MPbk2ContactUiControl& iUiControl;    
        /// Not Own: Command Observer
        MPbk2CommandObserver* iObserver;
        /// Own: Store state checker
        CPbk2StoreStateChecker* iStoreStateChecker;
    };

#endif // CPBK2COMMANDACTIVATOR_H

// End of File
