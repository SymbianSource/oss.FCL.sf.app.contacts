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
* Description:  Phonebook 2 command observer interface.
*
*/


#ifndef MPBK2COMMANDOBSERVER_H
#define MPBK2COMMANDOBSERVER_H

// FORWARD DECLARATIONS
class MPbk2Command;

// CLASS DECLARATION

/**
 * Phonebook 2 command observer interface.
 */
class MPbk2CommandObserver
    {
    public: // Interface

        /**
         * Notifies the command observer that the command has finished.
         *
         * @param aCommand  The command that was finished.
         */
        virtual void CommandFinished(
                const MPbk2Command& aCommand ) = 0;

    protected: // Disabled functions
        virtual ~MPbk2CommandObserver()
                {}
    };

#endif // MPBK2COMMANDOBSERVER_H

// End of File
