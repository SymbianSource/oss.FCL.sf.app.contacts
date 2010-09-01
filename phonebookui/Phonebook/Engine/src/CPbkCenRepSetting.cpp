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


#include "CPbkCenRepSetting.h"
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>
#include "PhonebookPrivateCRKeys.h"
#include "PhonebookExternalCRKeys.h"
#include "PbkUID.h"

// unnamed namespace for local definitions
namespace {



#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ConnectL,
    EPanicPreCond_Get,
    EPanicPreCond_Set,
    EPanicInvalidSetting
    };

void Panic(TPanicCode aPanic)
    {
    _LIT(KPanicCategory, "CPbkCenRepSetting");
    User::Panic(KPanicCategory, aPanic);
    }
#endif // _DEBUG

struct TCategoryToUidMapping
    {
    MPbkGlobalSetting::TPbkGlobalSettingCategory iCategory;
    TUid iUid;
    };

static const TCategoryToUidMapping categoryToUidMapping[] = 
    {
		{MPbkGlobalSetting::EGeneralSettingCategory, KCRUidPhonebook},
		{MPbkGlobalSetting::ELocalVariationCategory, KCRUidPhonebook},
        {MPbkGlobalSetting::ERichCallSettingsCategory, KPbkCRUidRichCallSettings},
        {MPbkGlobalSetting::EPbkConfigurationCategory, KCRUidPhonebookInternalConfig},
#ifdef _DEBUG
        {MPbkGlobalSetting::ETestCategory, KCRUidPhonebook},
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
            return categoryToUidMapping[i].iUid;
            }
        }

    return KNullUid;
    }

struct TSettingToKeyMapping
    {
    MPbkGlobalSetting::TPbkGlobalSetting iSetting;
    TUint32 iKey;
    };

static const TSettingToKeyMapping settingToKeyMapping[] =
    {
        {MPbkGlobalSetting::ENameOrdering, KPhonebookNameOrdering},
        {MPbkGlobalSetting::ELocalVariationFlags, KPhonebookLocalVariationFlags},
        {MPbkGlobalSetting::ENameSeparatorChar, KPhonebookNameSeparatorChar},
        {MPbkGlobalSetting::EPreferredTelephony, KRCSEPreferredTelephony},
        {MPbkGlobalSetting::EEditorMaxNumberLength, KPhonebookNumberEditorMaxLength},        
#ifdef _DEBUG
        {MPbkGlobalSetting::ETestIntSetting, 0},
        {MPbkGlobalSetting::ETestRealSetting, 0}, // not supported
        {MPbkGlobalSetting::ETestStringSetting, 0},
#endif //  _DEBUG
        // this marks the end of the array
        {MPbkGlobalSetting::EUndefinedSetting, 0}
    };

TUint32 MapSettingToKey(MPbkGlobalSetting::TPbkGlobalSetting aSetting)
    {
    for (TInt i = 0; 
         settingToKeyMapping[i].iSetting != MPbkGlobalSetting::EUndefinedSetting;
         ++i)
         {
         if (aSetting == settingToKeyMapping[i].iSetting)
             {
             return settingToKeyMapping[i].iKey;
             }
         }
   
    __ASSERT_DEBUG(EFalse, Panic(EPanicInvalidSetting));
    return 0;
    }
    
    
MPbkGlobalSetting::TPbkGlobalSetting MapKeyToSetting(TUint32 aKey)
    {
    for (TInt i = 0; 
         settingToKeyMapping[i].iSetting != MPbkGlobalSetting::EUndefinedSetting;
         ++i)
         {
         if (aKey == settingToKeyMapping[i].iKey)
             {
             return settingToKeyMapping[i].iSetting;
             }
         }
   
    // Unknown CenRep key. Ignore it as it's probably something that
    // has been introduced for a Phonebook2 feature, and does not
    // affect Phonebook1.
    return MPbkGlobalSetting::EUndefinedSetting;
    }

} // namespace 

inline CPbkCenRepSetting::CPbkCenRepSetting()
    {
    }

inline void CPbkCenRepSetting::ConstructL()
    {
    }

CPbkCenRepSetting* CPbkCenRepSetting::NewL()
    {
	CPbkCenRepSetting* self = new(ELeave) CPbkCenRepSetting();
	CleanupStack::PushL(self);
	self->ConstructL();
    CleanupStack::Pop(self);
	return self;
    }

CPbkCenRepSetting::~CPbkCenRepSetting()
    {
    }
    
    
void CPbkCenRepSetting::HandleNotifyGeneric(TUint32 aId)
    {
    if (iObserver)
        {
        MPbkGlobalSetting::TPbkGlobalSetting setting = MapKeyToSetting(aId);
        TRAP_IGNORE(
            {
            // Note: If SettingChangedL leaves, no more notifications
            // will be requested
            iObserver->SettingChangedL(setting);
            iCenRepNotifyHandler->StartListeningL();
            });
        }
    }

void CPbkCenRepSetting::ConnectL(TPbkGlobalSettingCategory aCategory)
    {
    __ASSERT_DEBUG(!iRepository, Panic(EPanicPreCond_ConnectL));
	TUid uid = MapCategoryToUid(aCategory);
    __ASSERT_DEBUG(uid != KNullUid, Panic(EPanicPreCond_ConnectL));
	iRepository = CRepository::NewL(uid);
    }

void CPbkCenRepSetting::Close()
    {
    if (iCenRepNotifyHandler)
        {
        // Unregister
        iCenRepNotifyHandler->StopListening();
        delete iCenRepNotifyHandler;
        iCenRepNotifyHandler = NULL;
        }
	delete iRepository;
	iRepository = NULL;
    }

TInt CPbkCenRepSetting::Get(TPbkGlobalSetting aKey, TInt& aValue)
    {
    __ASSERT_DEBUG(iRepository, Panic(EPanicPreCond_Get));
    TUint32 id = MapSettingToKey(aKey);
    __ASSERT_DEBUG(id != 0, Panic(EPanicPreCond_Get));

    return iRepository->Get(id, aValue);
    }

TInt CPbkCenRepSetting::Get(TPbkGlobalSetting aKey, TDes& aValue)
    {
    __ASSERT_DEBUG(iRepository, Panic(EPanicPreCond_Get));
    TUint32 id = MapSettingToKey(aKey);
    __ASSERT_DEBUG(id != 0, Panic(EPanicPreCond_Get));

    return iRepository->Get(id, aValue);
    }

TInt CPbkCenRepSetting::Get(TPbkGlobalSetting aKey, TReal& aValue)
    {
    __ASSERT_DEBUG(iRepository, Panic(EPanicPreCond_Get));
    TUint32 id = MapSettingToKey(aKey);
    __ASSERT_DEBUG(id != 0, Panic(EPanicPreCond_Get));

    return iRepository->Get(id, aValue);
    }

TInt CPbkCenRepSetting::Set(TPbkGlobalSetting aKey, TInt aValue)
    {
    __ASSERT_DEBUG(iRepository, Panic(EPanicPreCond_Get));
    TUint32 id = MapSettingToKey(aKey);
    __ASSERT_DEBUG(id != 0, Panic(EPanicPreCond_Set));

    TInt err = iRepository->Set(id, aValue);
    if (err != KErrNotFound)
        {
        return err;
        }

    return iRepository->Create(id, aValue);
    }

TInt CPbkCenRepSetting::Set(TPbkGlobalSetting aKey, const TDesC& aValue)
    {
    __ASSERT_DEBUG(iRepository, Panic(EPanicPreCond_Get));
    TUint32 id = MapSettingToKey(aKey);
    __ASSERT_DEBUG(id != 0, Panic(EPanicPreCond_Set));

    TInt err = iRepository->Set(id, aValue);
    if (err != KErrNotFound)
        {
        return err;
        }

    return iRepository->Create(id, aValue);
    }

TInt CPbkCenRepSetting::Set(TPbkGlobalSetting aKey, TReal aValue)
    {
    __ASSERT_DEBUG(iRepository, Panic(EPanicPreCond_Get));
    TUint32 id = MapSettingToKey(aKey);
    __ASSERT_DEBUG(id != 0, Panic(EPanicPreCond_Set));

    TInt err = iRepository->Set(id, aValue);
    if (err != KErrNotFound)
        {
        return err;
        }

    return iRepository->Create(id, aValue);
    }
    
void CPbkCenRepSetting::RegisterObserverL(
                                MPbkGlobalSettingObserver* aObserver)
    {
    iObserver = aObserver;
    if (iObserver)
        {
    	iCenRepNotifyHandler = 
    	        CCenRepNotifyHandler::NewLC(*this, *iRepository);
    	CleanupStack::Pop(iCenRepNotifyHandler);
        iCenRepNotifyHandler->StartListeningL();
        }
    else
        {
        if (iCenRepNotifyHandler)
            {
            // Unregister
            iCenRepNotifyHandler->StopListening();
            delete iCenRepNotifyHandler;
            iCenRepNotifyHandler = NULL;
            }
        }
    }


// End of File
