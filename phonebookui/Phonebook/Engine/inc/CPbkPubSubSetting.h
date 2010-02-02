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
*       Global persistent setting using Publish & Subscribe. Implements the 
*       MPbkGlobalSetting interface.
*
*/


#ifndef __CPbkPubSubSetting_H__
#define __CPbkPubSubSetting_H__

#include <MPbkGlobalSetting.h>
#include <e32base.h>
#include <e32property.h>

/**
 * Global persistent setting using Publish & Subscribe. Implements the 
 * MPbkGlobalSetting interface.
 */
NONSHARABLE_CLASS(CPbkPubSubSetting) : 
        public CBase, 
        public MPbkGlobalSetting
    {
    public:
        static CPbkPubSubSetting* NewL();
        ~CPbkPubSubSetting();

    private: // From MPbkGlobalSetting
        void ConnectL(TPbkGlobalSettingCategory aCategory);
        void Close();
        TInt Get(TPbkGlobalSetting aKey, TInt& aValue);
        TInt Get(TPbkGlobalSetting aKey, TDes& aValue);
        TInt Get(TPbkGlobalSetting aKey, TReal& aValue);
        TInt Set(TPbkGlobalSetting aKey, TInt aValue);
        TInt Set(TPbkGlobalSetting aKey, const TDesC& aValue);
        TInt Set(TPbkGlobalSetting aKey, TReal aValue);

    private: // Implementation
        CPbkPubSubSetting();        

    private: // Data
        RProperty iProperty;
        TUid iCurrentCategory;
    };

#endif // __CPbkPubSubSetting_H__

// End of File
