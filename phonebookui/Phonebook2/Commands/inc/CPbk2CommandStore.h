/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 command store.
*
*/


#ifndef CPBK2COMMANDSTORE_H
#define CPBK2COMMANDSTORE_H

// INCLUDES
#include <e32base.h>
#include <MPbk2CommandObserver.h>
#include "MPbk2CommandList.h"

#include <akninputblock.h>

//  CLASS DECLARATION
class MPbk2MenuCommandObserver;
class CAknInputBlock;

/**
 * Phonebook 2 command store. This object owns all the command objects.
 */
class CPbk2CommandStore : public CBase,
                          public MPbk2CommandList,
                          public MPbk2CommandObserver,
                          public MAknInputBlockCancelHandler
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2CommandStore* NewL();

        /**
         * Destructor.
         */
        ~CPbk2CommandStore();

    public: // Interface

        /**
         * Adds a menu command observer.
         *
         * @param aObserver     Observer to add.
         */
        void AddMenuCommandObserver(
                MPbk2MenuCommandObserver& aObserver );

        /**
         * Removes a menu command observer.
         *
         * @param aObserver     Observer to remove.
         */
        void RemoveMenuCommandObserver(
                MPbk2MenuCommandObserver& aObserver );
        /**
         * Destroys all the commands in the iCommandArray.
         * by calling CommandFinished() on each one.
         */
        void DestroyAllCommands();

    public: // From MPbk2CommandList
        IMPORT_C void AddAndExecuteCommandL(
                MPbk2Command* aCommand );

    private: // From MPbk2CommandObserver
        void CommandFinished(
                const MPbk2Command& aCommand );
        
    public: // From MAknInputBlockCancelHandler
    	
    	void AknInputBlockCancel();


    private: // Implementation
        CPbk2CommandStore();
        void ConstructL();
        static TInt IdleDestructorCallback(
                TAny* aSelf );
        void IdleDestructor();

    private: // Data
        /// Own: Array of executed commands
        RPointerArray<MPbk2Command> iCommandArray;
        /// Own: Idle command destroyer
        CIdle* iIdleDestroyer;
        /// Own: Array of executed commands
        RPointerArray<MPbk2Command> iIdleDestructableCommands;
        /// Own: Array of menu command observers
        RPointerArray<MPbk2MenuCommandObserver> iCommandObservers;
        /// Own: User input blocker
        CAknInputBlock* iInputBlocker;
    };

#endif // CPBK2COMMANDSTORE_H

// End of File
