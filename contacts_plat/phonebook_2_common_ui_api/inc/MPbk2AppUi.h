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
* Description:  Phonebook 2 application UI interface.
*
*/


#ifndef MPBK2APPUI_H
#define MPBK2APPUI_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CPbk2AppViewBase;
class MPbk2ApplicationServices;
class MPbk2ViewExplorer;
class MPbk2StartupMonitor;
class CPbk2TabGroupContainer;
class MPbk2KeyEventHandler;

// CLASS DECLARATION

/**
 * Phonebook 2 application UI interface.
 */
class MPbk2AppUi
    {
    public: // Interface

        /**
         * Returns the currently active view.
         *
         * @return  Active view.
         */
        virtual CPbk2AppViewBase* ActiveView() const = 0;
 
        /**
         * Returns the Phonebook 2 view explorer if it exists.
         *
         * @return  View explorer.
         */
        virtual MPbk2ViewExplorer* Pbk2ViewExplorer() const = 0;

        /**
         * Returns the Phonebook 2 startup monitor.
         *
         * @return  Startup monitor.
         */
        virtual MPbk2StartupMonitor* Pbk2StartupMonitor() const = 0;

        /**
         * Returns Phonebook 2 tab group container.
         *
         * @return  Tab group container.
         */
        virtual CPbk2TabGroupContainer* TabGroups() const = 0;

        /**
         * Returns Phonebook 2 key event handler.
         *
         * @return  Key event handler.
         */
        virtual MPbk2KeyEventHandler& KeyEventHandler() = 0;

        /**
         * Returns Phonebook 2 application services.
         *
         * @return  Application services.
         */
        virtual MPbk2ApplicationServices& ApplicationServices() const = 0;

        /**
         * Handles a command. If Phonebook 2 command handler does not
         * handle a command it can then be passed to application UI
         * command handler which ultimately handles it.
         * For example, exit command is handled by
         * application UI not by command handler.
         *
         * @return  Id of the command to handle.
         */
        virtual void HandleCommandL(
                TInt aCommand ) = 0;

        /**
         * Exits application.
         */
        virtual void ExitApplication() = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* AppUiExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Disabled functions
        ~MPbk2AppUi()
            {};
    };

namespace Phonebook2 {

/**
 * Returns Phonebook 2 application UI interface.
 * Usage: MPbk2AppUi* appUi = Phonebook2::Pbk2AppUi();
 *
 * @return  Phonebook 2 application UI interface.
 */
IMPORT_C MPbk2AppUi* Pbk2AppUi();

} /// namespace

#endif // MPBK2APPUI_H

// End of File
