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
*       Global transient setting using Publish & Subscribe. Implements the 
*       MPbkGlobalSetting interface.
*
*/


#include "CPbkPubSubSetting.h"
#include <PSVariables.h>
#include "PhonebookPrivatePSKeys.h"
#include <BTSapDomainPSKeys.h>

// unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicReason
    {
    EPanicPreCond_ConnectL,
    EPanicPreCond_Get,
    EPanicPreCond_Set,
    EPanicInvalidSetting
    };

void Panic(TPanicReason aReason)
    {
    _LIT(KPanicCategory, "CPbkPubSubSetting");
    User::Panic(KPanicCategory, aReason);
    }

#endif // _DEBUG

struct TCategoryToUidMapping
    {
    MPbkGlobalSetting::TPbkGlobalSettingCategory iCategory;
    TInt32 iUid;
    };

static const TCategoryToUidMapping categoryToUidMapping[] = 
    {
        {MPbkGlobalSetting::ESystemStatusCategory, KUidSystemCategoryValue},
        {MPbkGlobalSetting::EPbkSystemStateCategory, KPSUidPhonebook},
#ifdef _DEBUG
        {MPbkGlobalSetting::ETestCategory, KUidSystemCategoryValue},
#endif // _DEBUG
        // This marks the end of the array.
        {MPbkGlobalSetting::EUndefinedCategory, 0}
    };

TUid MapCategoryToUid(MPbkGlobalSetting::TPbkGlobalSettingCategory aCategory)
    {
    for (TInt i = 0; 
         categoryToUidMapping[i].iCategory != MPbkGlobalSetting::EUndefinedCategory;
         ++i)
        {
        if (aCategory == categoryToUidMapping[i].iCategory)
            {
            return TUid::Uid(categoryToUidMapping[i].iUid);
            }
        }

    if (aCategory == MPbkGlobalSetting::EBTSapCategory)
        {
        return KPSUidBluetoothSapConnectionState;
        }

    return KNullUid;
    }

struct TSettingToKeyDataMapping
    {
    MPbkGlobalSetting::TPbkGlobalSetting iSetting;
    TUint iKey;
    TStaticSecurityPolicy iReadPolicy;
    TStaticSecurityPolicy iWritePolicy;
    };

static const TSettingToKeyDataMapping settingToKeyDataMapping[] =
    {
        {MPbkGlobalSetting::ECurrentSimOwnedStatus, KPSUidCurrentSimOwnedSimStatusValue},
        {MPbkGlobalSetting::ESimInsertedStatus, KPSUidSimCStatusValue},
        {MPbkGlobalSetting::EBTSapEnabledStatus, KBTSapConnectionState },
        {MPbkGlobalSetting::ECopyFromSimAsked, KPhonebookSimCopyAsked, _INIT_SECURITY_POLICY_C1(ECapabilityReadUserData), _INIT_SECURITY_POLICY_S0(KPbkUID3)},
#ifdef _DEBUG        
        {MPbkGlobalSetting::ETestRealSetting, 0}, // not supported
        {MPbkGlobalSetting::ETestStringSetting, 0},
#endif //  _DEBUG
        // this marks the end of the array
        {MPbkGlobalSetting::EUndefinedSetting, 0}
    };

const TSettingToKeyDataMapping& MapSettingToKeyData(MPbkGlobalSetting::TPbkGlobalSetting aSetting)
    {
    for (TInt i = 0; 
         settingToKeyDataMapping[i].iSetting != MPbkGlobalSetting::EUndefinedSetting;
         ++i)
         {
         if (aSetting == settingToKeyDataMapping[i].iSetting)
             {
             return settingToKeyDataMapping[i];
             }
         }
           
    __ASSERT_DEBUG(EFalse, Panic(EPanicInvalidSetting));
    return settingToKeyDataMapping[MPbkGlobalSetting::EUndefinedSetting];
    }

} // namespace

inline CPbkPubSubSetting::CPbkPubSubSetting() :
    iCurrentCategory(KNullUid)
    {
    }


CPbkPubSubSetting* CPbkPubSubSetting::NewL()
    {
	return new(ELeave) CPbkPubSubSetting;	
    }

CPbkPubSubSetting::~CPbkPubSubSetting()
    {
    }

void CPbkPubSubSetting::ConnectL(TPbkGlobalSettingCategory aCategory)
    {
    TUid uid = MapCategoryToUid(aCategory);
    __ASSERT_DEBUG(uid != KNullUid, Panic(EPanicPreCond_ConnectL));
    iCurrentCategory = uid;
    }

void CPbkPubSubSetting::Close()
    {
    iProperty.Close();
    }

TInt CPbkPubSubSetting::Get(TPbkGlobalSetting aKey, TInt& aValue)
    {
    __ASSERT_DEBUG(iCurrentCategory != KNullUid, Panic(EPanicPreCond_Get));
    TSettingToKeyDataMapping keyData = MapSettingToKeyData(aKey);
    __ASSERT_DEBUG(keyData.iKey != 0, Panic(EPanicPreCond_Get));
    return iProperty.Get(iCurrentCategory, keyData.iKey, aValue);
    }

TInt CPbkPubSubSetting::Get(TPbkGlobalSetting aKey, TDes& aValue)
    {
    __ASSERT_DEBUG(iCurrentCategory != KNullUid, Panic(EPanicPreCond_Get));
    TSettingToKeyDataMapping keyData = MapSettingToKeyData(aKey);
    __ASSERT_DEBUG(keyData.iKey != 0, Panic(EPanicPreCond_Get));
    return iProperty.Get(iCurrentCategory, keyData.iKey, aValue);
    }

TInt CPbkPubSubSetting::Get(TPbkGlobalSetting /* aKey */, TReal& /* aValue */)
    {
    __ASSERT_DEBUG(iCurrentCategory != KNullUid, Panic(EPanicPreCond_Get));
    return KErrNotSupported;
    }

TInt CPbkPubSubSetting::Set(TPbkGlobalSetting aKey, TInt aValue)
    {
    __ASSERT_DEBUG(iCurrentCategory != KNullUid, Panic(EPanicPreCond_Set));
    TSettingToKeyDataMapping keyData = MapSettingToKeyData(aKey);
    __ASSERT_DEBUG(keyData.iKey != 0, Panic(EPanicPreCond_Set));
    TInt err = iProperty.Set(iCurrentCategory, keyData.iKey, aValue);
    if (err != KErrNotFound)
        {
        return err;
        }

    err = iProperty.Define(keyData.iKey, RProperty::EInt, keyData.iReadPolicy, keyData.iWritePolicy);
    if (err != KErrNone)
        {
        return err;
        }
    
    return iProperty.Set(iCurrentCategory, keyData.iKey, aValue);
    }

TInt CPbkPubSubSetting::Set(TPbkGlobalSetting aKey, const TDesC& aValue)
    {
    __ASSERT_DEBUG(iCurrentCategory != KNullUid, Panic(EPanicPreCond_Set));
    TSettingToKeyDataMapping keyData = MapSettingToKeyData(aKey);
    __ASSERT_DEBUG(keyData.iKey != 0, Panic(EPanicPreCond_Set));
    TInt err = iProperty.Set(iCurrentCategory, keyData.iKey, aValue);
    if (err != KErrNotFound)
        {
        return err;
        }

    err = iProperty.Define(keyData.iKey, RProperty::EText, keyData.iReadPolicy, keyData.iWritePolicy);
    if (err != KErrNone)
        {
        return err;
        }
    
    return iProperty.Set(iCurrentCategory, keyData.iKey, aValue);
    }

TInt CPbkPubSubSetting::Set(TPbkGlobalSetting /* aKey */, TReal /* aValue */)
    {
    __ASSERT_DEBUG(iCurrentCategory != KNullUid, Panic(EPanicPreCond_Set));
    return KErrNotSupported;
    }

// End of File
