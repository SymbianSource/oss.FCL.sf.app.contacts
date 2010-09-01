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
*    Factory to create instances of MPbkGlobalSetting.
*
*/


#ifndef __PbkGlobalSettingFactory_H__
#define __PbkGlobalSettingFactory_H__

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MPbkGlobalSetting;

/**
 * Factory to create instances of MPbkGlobalSetting.
 */
class PbkGlobalSettingFactory
    {
    public: // Factory interface
        /**
         * Creates an instance of MPbkGlobalSetting that can be used
         * to store and retrieve transient settings.
         *
         * @param aWritableSetting If ETrue the setting will be also 
         *                         writable, otherwise only readable.
         * @return Newly created instance of a class that implements 
         *         MPbkGlobalSetting and can be used to store and retrieve
         *         transient settings.
         */
        IMPORT_C static MPbkGlobalSetting* CreateTransientSettingL
            (TBool aWritableSetting = EFalse);

        /**
         * Creates an instance of MPbkGlobalSetting that can be used
         * to store and retrieve persistent settings.
         * @return Newly created instance of a class that implements 
         *         MPbkGlobalSetting and can be used to store and retrieve
         *         persistent settings.
         */
        IMPORT_C static MPbkGlobalSetting* CreatePersistentSettingL();

    private: // Hidden constructor and destructor
        PbkGlobalSettingFactory();
        ~PbkGlobalSettingFactory();
    };

#endif // __PbkGlobalSettingFactory_H__

// End of File
