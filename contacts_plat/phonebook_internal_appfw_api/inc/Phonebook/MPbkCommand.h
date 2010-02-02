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
*     Interface class for an abstract command object.
*
*/

#ifndef __MPbkCommand_H__
#define __MPbkCommand_H__


// FORWARD DECLARATIONS
class MPbkCommandObserver;


// CLASS DECLARATION

/**
 * Abstract command object interface.
 */
class MPbkCommand
    {
    public: // Interface
        /**
         * Destructor.
         */
        virtual ~MPbkCommand() { }

        /**
         * Executes and destroys this command when done. The command is 
         * also destroyed if this function leaves.
         */
        virtual void ExecuteLD() =0;

        /**
         * Add a command observer to this command.
         * @param aObserver command observer.
         */
        virtual void AddObserver(MPbkCommandObserver& /*aObserver*/) { }

    };


#endif // __MPbkCommand_H__


// End of File
