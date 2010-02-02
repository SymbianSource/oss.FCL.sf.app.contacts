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
* Description:  Phonebook 2 settings control UI extension interface.
*
*/


#ifndef MPBK2SETTINGSCONTROLEXTENSION_H
#define MPBK2SETTINGSCONTROLEXTENSION_H

// INCLUDES
#include <mpbk2uireleasable.h>

// FORWARD DECLARATIONS
class CAknSettingItemList;
class CAknSettingItem;

// CLASS DECLARATION

/**
 * Phonebook 2 settings control UI extension interface. 
 * Settings control extensions are created by settings view extension.
 * Settings control extensions can modify the settings list.
 */
class MPbk2SettingsControlExtension : public MPbk2UiReleasable
    {
    public: // Interface

        /**
         * Modifies the setting item list to contain settings
         * from extensions.
         *
         * @param aSettingItemList  The setting item list to modify.
         */
        virtual void ModifySettingItemListL(
                CAknSettingItemList& aSettingItemList ) = 0;

        /**
         * Creates a setting item from extension.
         *
         * @param aSettingId    Id of the setting item.
         * @return  Created setting item.
         */
        virtual CAknSettingItem* CreateSettingItemL(
                TInt aSettingId ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* SettingsControlExtensionExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2SETTINGSCONTROLEXTENSION_H

// End of File
