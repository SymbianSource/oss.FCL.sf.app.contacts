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
* Description:  Phonebook 2 menu command observer interface.
*
*/


#ifndef MPBK2MENUCOMMANDOBSERVER_H
#define MPBK2MENUCOMMANDOBSERVER_H

// FORWARD DECLARATIONS
class MPbk2Command;

// CLASS DECLARATION

/**
 * Phonebook 2 menu command observer interface.
 */
class MPbk2MenuCommandObserver
    {
    public: // Interface

        /**
         * Pre command execution callback.
         *
         * @param aCommand      The command that is about to be executed.
         */
        virtual void PreCommandExecutionL(
                const MPbk2Command& aCommand ) = 0;

        /**
         * Post command execution callback.
         *
         * @param aCommand      The command that was just executed.
         */
        virtual void PostCommandExecutionL(
                const MPbk2Command& aCommand ) = 0;

    protected: // Disabled functions
        ~MPbk2MenuCommandObserver()
                {}
    };

#endif // MPBK2MENUCOMMANDOBSERVER_H

// End of File
