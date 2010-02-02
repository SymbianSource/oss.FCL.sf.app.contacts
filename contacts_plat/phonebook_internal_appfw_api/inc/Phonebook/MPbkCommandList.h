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
*       Interface to component which handles all command objects in phonebook.
*
*/


#ifndef __MPbkCommandList_H__
#define __MPbkCommandList_H__

// INCLUDES


// FORWARD DECLARATIONS
class MPbkCommand;


//  CLASS DECLARATION

/**
 * Interface to component which handles all command objects in phonebook.
 */
class MPbkCommandList
	{
    public: // interface
        /**
         * Takes ownership of the command.
         * @param aCommand command object to be added to the storage.
         */
        virtual void AddAndExecuteCommandL(MPbkCommand* aCommand) = 0;

    public: 
        virtual ~MPbkCommandList() { }        
	};

#endif // __MPbkCommandList_H__

// End of File
