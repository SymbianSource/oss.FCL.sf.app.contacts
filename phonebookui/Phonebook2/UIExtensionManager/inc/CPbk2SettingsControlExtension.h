/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 settings control composite UI extension.
*
*/


#ifndef CPBK2SETTINGSCONTROLEXTENSION_H
#define CPBK2SETTINGSCONTROLEXTENSION_H

// INCLUDE FILES
#include <e32base.h>
#include <MPbk2SettingsControlExtension.h>

// FORWARD DECLARATIONS
class MPbk2SettingsViewExtension;

/**
 * Phonebook 2 settings control composite UI extension.
 */
NONSHARABLE_CLASS(CPbk2SettingsControlExtension) : 
        public CBase,
		public MPbk2SettingsControlExtension
    {
    public: // Construction and destruction

        /** 
         * Returns a new instance of this class.
         * 
         * @param aSettingsViewExtensions    Array of settings view extensions.
         * @return  A new instance of this class.
         */
        static CPbk2SettingsControlExtension* NewL(
                TArray<MPbk2SettingsViewExtension*> aSettingsViewExtensions );

        /**
         * Destructor.
         */
        ~CPbk2SettingsControlExtension();

    public: // From MPbk2SettingsControlExtension
        void DoRelease();
        void ModifySettingItemListL(
                CAknSettingItemList& aSettingItemList);
        CAknSettingItem* CreateSettingItemL(
                TInt aSettingId );
        
    private: // Construction
        CPbk2SettingsControlExtension();
        void ConstructL(
                TArray<MPbk2SettingsViewExtension*> aSettingViewExtensions );

    private: // Data
        /// Own: An array for setting extensions
        RPointerArray<MPbk2SettingsControlExtension> iSettingExtensions;
	};

#endif // CPBK2SETTINGSCONTROLEXTENSION_H

// End of File
