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
* Description:  Phonebook 2 settings view UI extension interface.
*
*/


#ifndef MPBK2SETTINGSVIEWEXTENSION_H
#define MPBK2SETTINGSVIEWEXTENSION_H

// INCLUDES
#include <mpbk2uireleasable.h>

//  FORWARD DECLARATIONS
class MPbk2SettingsControlExtension;
class CEikMenuPane;

// CLASS DECLARATION

/**
 * Phonebook 2 settings view UI extension interface.
 * Settings view extension can modify settings menu pane and handle
 * settings view commands.
 */
class MPbk2SettingsViewExtension : public MPbk2UiReleasable
    {
    public: // Interface

        /**
         * Creates a Phonebook 2 settings control extension.
         *
         * @return  Settings control extension.
         */
        virtual MPbk2SettingsControlExtension*
                CreatePbk2SettingsControlExtensionL() = 0;

        /**
         * Filters the menu pane of the settings view.
         *
         * @param aResourceId   Menu resource id.
         * @param aMenuPane     Menu pane to filter.
         */
        virtual void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane ) = 0;

        /**
         * Processes settings commands.
         *
         * @param aCommandId    Id of the command to handle.
         * @return  ETrue if command was handled, EFalse otherwise.
         */
        virtual TBool HandleCommandL(
                TInt aCommandId ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* SettingViewExtensionExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2SETTINGSVIEWEXTENSION_H

// End of File
