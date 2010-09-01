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
* Description:  Interface to component which handles all command objects
*                in Phonebook 2.
*
*/


#ifndef MPBK2COMMANDLIST_H
#define MPBK2COMMANDLIST_H

// FORWARD DECLARATIONS
class MPbk2Command;

//  CLASS DECLARATION

/**
 * Interface to component which handles all command objects in Phonebook 2.
 */
class MPbk2CommandList
	{
    public: // interface
        /**
         * Takes ownership of the command.
         * @param aCommand command object to be added to the storage.
         */
        virtual void AddAndExecuteCommandL(MPbk2Command* aCommand) = 0;

        virtual ~MPbk2CommandList() { }
	};

#endif // MPBK2COMMANDLIST_H

// End of File
