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
*       Interface for phonebook command objects.
*
*/


#ifndef __MPbkCommandObserver_H__
#define __MPbkCommandObserver_H__

// INCLUDES


// FORWARD DECLARATIONS
class MPbkCommand;


//  CLASS DECLARATION

/**
 * Interface for phonebook command objects.
 */
class MPbkCommandObserver
	{
    public: // interface
        /**
         * Notifies the command observer that the command has finished.
         * @param aCommand the command who finished.
         */
        virtual void CommandFinished(const MPbkCommand& aCommand) = 0;

    protected: 
        virtual ~MPbkCommandObserver() { }        

	};

#endif // __MPbkCommandObserver_H__

// End of File
