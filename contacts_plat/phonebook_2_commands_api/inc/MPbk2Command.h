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
* Description:  Phonebook 2 command object interface.
*
*/


#ifndef MPBK2COMMAND_H
#define MPBK2COMMAND_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MPbk2CommandObserver;
class MPbk2ContactUiControl;

// CLASS DECLARATION

/**
 * Phonebook 2 command object interface. 
 *
 * Every Phonebook 2 command must implement this interface.
 * Phonebook 2 command handler can then create these command objects
 * and execute them. Commands can report about their execution via
 * MPbk2CommandObserver interface. Commands can also control the
 * UI behaviour via MPbk2ContactUiControl interface.
 */
class MPbk2Command
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2Command()
                {}

        /**
         * Executes the command and destroys this command object when done.
         * The command is also destroyed if this function leaves.
         */
        virtual void ExecuteLD() = 0;

        /**
         * Removes UI control reference from the command. After this call the 
         * command cannot reference the UI control.
         *
         * @param aUiControl    UI control whose reference is removed.
         */
        virtual void ResetUiControl(
                MPbk2ContactUiControl& aUiControl ) = 0;

        /**
         * Adds a command observer for this command.
         *
         * @param aObserver     Command observer to add.
         */
        virtual void AddObserver(
                MPbk2CommandObserver& aObserver ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* CommandExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2COMMAND_H

// End of File
