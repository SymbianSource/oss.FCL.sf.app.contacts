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
*    Abstract interface for accessing global settings.
*
*/


#ifndef __MPbkGlobalSetting_H__
#define __MPbkGlobalSetting_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MPbkGlobalSettingObserver;

/**
 * Abstract interface for accessing global settings.
 */
class MPbkGlobalSetting
    {
    public: 
        enum TPbkGlobalSettingCategory
            {
            EUndefinedCategory,

            EGeneralSettingCategory,
            ESystemStatusCategory,
            EPbkSystemStateCategory,
            ELocalVariationCategory,
            EBTSapCategory,
            ERichCallSettingsCategory,
            EPbkConfigurationCategory,

            // used for testing implementations of this class
            ETestCategory
            };
        
        enum TPbkGlobalSetting
            {
            EUndefinedSetting,

            ENameOrdering,
            EAllowOpen1Touch,
            ECheckDuplicateEntry,
            ECopyFromSimAsked,
            ELocalVariationFlags,
            ENameSeparatorChar,
            
            ECurrentSimOwnedStatus,     // values may be ETrue/EFalse
            ESimInsertedStatus,         // values may be ETrue/EFalse
            EBTSapEnabledStatus,        // values may be ETrue/EFalse

            EPreferredTelephony,
            EEditorMaxNumberLength,

            // used for testing implementations of this class
            ETestIntSetting,            
            ETestRealSetting,
            ETestStringSetting
            };
    public:
        /**
         * Pushes the object to cleanup stack. When the object is destroyed
         * for example by calling CleanupStack::PopAndDestroy(), Close() will
         * be called prior to deleting.
         */
        void PushL();
        static void Cleanup(TAny* aObj);
    public:
        /**
         * Virtual destructor.
         */
        virtual ~MPbkGlobalSetting() {}

        /**
         * Connects to the setting with category aUid. If KNullUid
         * is used as the parameter, creates a temporary setting
         * if applicable to implementation.
         * @param aCategory Category of the setting to connect to.
         */
        virtual void ConnectL(TPbkGlobalSettingCategory aCategory) = 0;

        /**
         * Closes the connection.
         */
        virtual void Close() = 0;

        /**
         * Gets an integer value with key aKey.
         * Returns one of the system wide error codes.
         * @param aKey Subcategory of the value to get.
         */
        virtual TInt Get(TPbkGlobalSetting aKey, TInt& aValue) = 0;

        /**
         * Gets an integer value with key aKey.
         * Returns one of the system wide error codes.
         * @param aKey Subcategory of the value to get.
         */
        virtual TInt Get(TPbkGlobalSetting aKey, TDes& aValue) = 0;

        /**
         * Gets an integer value with key aKey.
         * Returns one of the system wide error codes.
         * @param aKey Subcategory of the value to get.
         */
        virtual TInt Get(TPbkGlobalSetting aKey, TReal& aValue) = 0;

        /**
         * Sets an integer value with key aKey.
         * Returns one of the system wide error codes.
         * @param aKey Subcategory of the value to set.
         */
        virtual TInt Set(TPbkGlobalSetting aKey, TInt aValue) = 0;

        /**
         * Sets an integer value with key aKey.
         * Returns one of the system wide error codes.
         * @param aKey Subcategory of the value to set.
         */
        virtual TInt Set(TPbkGlobalSetting aKey, const TDesC& aValue) = 0;

        /**
         * Sets an integer value with key aKey.
         * Returns one of the system wide error codes.
         * @param aKey Subcategory of the value to set.
         */
        virtual TInt Set(TPbkGlobalSetting aKey, TReal aValue) __SOFTFP = 0;
        
        /**
         * Registers an observer for setting changes. Only one setting can be
         * observed
         * @param aObserver Pointer to the observer. Use NULL to un-register
         * @param aKey The key to be observed
         */
        virtual void RegisterObserverL(
                        MPbkGlobalSettingObserver* /*aObserver*/) {};

    };

inline void MPbkGlobalSetting::PushL()
    {
    CleanupStack::PushL(TCleanupItem(Cleanup, this));
    }

inline void MPbkGlobalSetting::Cleanup(TAny* aObj)
    {
    MPbkGlobalSetting* setting = static_cast<MPbkGlobalSetting*>(aObj);
    if (setting)
        {
        setting->Close();
        delete setting;
        }
    }




/**
 * Observer class for getting notifications of setting changes
 */
class MPbkGlobalSettingObserver
    {
    public: 
        virtual void SettingChangedL(
                        MPbkGlobalSetting::TPbkGlobalSetting aKey) = 0;
    };


#endif // __MPbkGlobalSetting_H__

// End of File
