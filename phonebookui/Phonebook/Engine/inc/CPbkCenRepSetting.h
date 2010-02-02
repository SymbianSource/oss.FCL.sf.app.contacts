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
*       Global persistent setting using Central Repository. Implements the 
*       MPbkGlobalSetting interface.
*
*/


#ifndef __CPBKCENREPSETTING_H__
#define __CPBKCENREPSETTING_H__

// INCLUDES
#include <MPbkGlobalSetting.h>
#include <e32base.h>
#include <cenrepnotifyhandler.h>

// FORWARD DECLARATIONS
class CRepository;

/**
 * Global persistent setting using Central Repository. Implements the 
 * MPbkGlobalSetting interface.
 */
NONSHARABLE_CLASS(CPbkCenRepSetting) : 
        public CBase, 
        public MPbkGlobalSetting,
        public MCenRepNotifyHandlerCallback
    {
    public:
        static CPbkCenRepSetting* NewL();
        ~CPbkCenRepSetting();
        
    public:  // From MCenRepNotifyHandlerCallback
        void HandleNotifyGeneric(TUint32 aId);

    private: // From MPbkGlobalSetting
        void ConnectL(TPbkGlobalSettingCategory aCategory);
        void Close();
        TInt Get(TPbkGlobalSetting aKey, TInt& aValue);
        TInt Get(TPbkGlobalSetting aKey, TDes& aValue);
        TInt Get(TPbkGlobalSetting aKey, TReal& aValue);
        TInt Set(TPbkGlobalSetting aKey, TInt aValue);
        TInt Set(TPbkGlobalSetting aKey, const TDesC& aValue);
        TInt Set(TPbkGlobalSetting aKey, TReal aValue);
        void RegisterObserverL(MPbkGlobalSettingObserver* aObserver);
       
    private: // Implementation
        CPbkCenRepSetting();
        void ConstructL();

    private: // Data
        /// Own: Central Repository connection
        CRepository* iRepository;
        /// Ref: The observer that wants notifications of settings changes
        MPbkGlobalSettingObserver* iObserver;
        /// Own: AO wrapper for receiving notifications of CenRep-changes
        CCenRepNotifyHandler*      iCenRepNotifyHandler;   
    };

#endif // __CPBKCENREPSETTING_H__

// End of File
