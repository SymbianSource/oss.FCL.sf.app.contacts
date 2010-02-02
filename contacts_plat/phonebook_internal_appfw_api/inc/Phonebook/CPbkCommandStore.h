/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       Component which owns all phonebook command objects.
*
*/


#ifndef __CPbkCommandStore_H__
#define __CPbkCommandStore_H__

// INCLUDES
#include <e32base.h>
#include <MPbkCommandObserver.h>
#include <MPbkCommandList.h>


// FORWARD DECLARATIONS
class MPbkCommand;


//  CLASS DECLARATION

/**
 * Phonebook Command storage object. This object owns all command objects. 
 */
class CPbkCommandStore : 
        public CBase,
        public MPbkCommandList,
        public MPbkCommandObserver
	{
	public: // Construction and destruction
		IMPORT_C static CPbkCommandStore* NewL();
        ~CPbkCommandStore();

    public: // from MPbkCommandList
        /**
         * Takes ownership of the command.
         * @param aCommand command object to be added to the storage.
         */
        IMPORT_C void AddAndExecuteCommandL(MPbkCommand* aCommand);

    private: // from MPbkCommandObserver
        /**
         * Notifies the command observer that the command has finished.
         * @param aCommand the command who finished.
         */
        void CommandFinished(const MPbkCommand& aCommand);
        
	private: // Implementation
		CPbkCommandStore();
		void ConstructL();	
        static TInt IdleDestructorCallback(TAny* aSelf);
        void IdleDestructor();

	private: // Data
        /// Own: array of executed commands
        RPointerArray<MPbkCommand> iCommandArray;
        /// Own: idle command destroyer
        CIdle* iIdleDestroyer;
        /// Own: array of executed commands
        RPointerArray<MPbkCommand> iIdleDestructableCommands;

	};

#endif // __CPbkCommandStore_H__

// End of File
