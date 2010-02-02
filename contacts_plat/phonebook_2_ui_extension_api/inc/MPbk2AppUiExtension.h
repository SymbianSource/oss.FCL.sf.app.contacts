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
* Description:  Phonebook 2 application UI UI extension interface.
*
*/


#ifndef MPBK2APPUIEXTENSION_H
#define MPBK2APPUIEXTENSION_H

// INCLUDES
#include <mpbk2uireleasable.h>

// FORWARD DECLARATIONS
class MPbk2StartupMonitor;
class CAknTabGroup;
class CPbk2IconInfoContainer;
class CPbk2ViewNode;

// CLASS DECLARATION

/**
 * Phonebook 2 application UI UI extension interface.
 * Application UI extensions can be used to monitor startup
 * procedure and to maintain tab groups.
 */
class MPbk2AppUiExtension :  public MPbk2UiReleasable
    {
    public: // Interface

        /**
         * Called by the Phonebook 2 startup monitor when
         * the application startup begins.
         *
         * NOTE: The extension MUST always call either HandleStartupComplete
         *       or HandleStartupFailed. If the extension don't need to monitor
         *       events it can do the call back in it's implementation of 
         *       ExtensionStartupL.
         *
         * @param aStartupMonitor   Startup monitor.
         */
        virtual void ExtensionStartupL(
                MPbk2StartupMonitor& aStartupMonitor ) = 0;

        /**
         * Allows extensions to add tab groups dynamically.
         *
         * @param aNode         View graph node.
         * @param aAknTabGroup  Avkon tab group.
         * @param aTabIcons     Tab icon info container.
         * @param aViewCount    Amount of views in the tab.
         */
        virtual void ApplyDynamicPluginTabGroupContainerChangesL(
                const CPbk2ViewNode& aNode, 
                CAknTabGroup& aAknTabGroup,
                CPbk2IconInfoContainer& aTabIcons,
                TInt aViewCount ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* AppUiExtensionExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2APPUIEXTENSION_H

// End of File
