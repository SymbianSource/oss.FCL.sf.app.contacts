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
*       Phonebook contact Sort order manager.
*
*/


// INCLUDE FILES
#include "CPbkSortOrderManager.h"
#include <featmgr.h>
#include <bldvariant.hrh>
#include <CPbkContactEngine.h>
#include <StringLoader.h>
#include <pbkeng.rsg>
#include <barsread.h>    // TResourceReader
#include <PbkGlobalSettingFactory.h>
#include <MPbkGlobalSetting.h>
#include "PbkUID.h"
#include <PbkEngUtils.h>
#include "PbkEng.hrh"
/// Unnamed namespace for local definitions
namespace {

// CONSTANTS

// Name order setting value for First Name Last Name
// This value comes from phonebook specification (see
// qtn_phob_name_order) and it's opposite compared to
// TPbkNameOrder that is used in CenRep.
_LIT(KPbkFirstLastNameEnabled, "0");
// Value when name separator is used
_LIT(KPbkNameSeparatorUsed, "1");
// Dec value for space character
const TInt KPbkSpaceDecValue = 32;

// Separator string length is set to 4.
// Separator should be only one character long,
// but sometimes localized string or cenrep initial value
// may contain additional space characters at the end of string.
const TInt KPbkSeparatorMaxLength = 4;

#ifdef _DEBUG
// CONSTANTS
enum TPanicCode
    {
    EPanicPreCond_SetNameDisplayOrderL = 1
    };

// LOCAL FUNCTIONS
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkSortOrderManager");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

// Conversion function is used because old functions
// SetNameSeparatorL() and NameSeparator() are supported.
// In old format 0 means that space character is used and in
// new format 23 stands for space character
inline TChar ConvertSeparatorCharacterToOld(TChar aFrom)
    {
    // Convert space value to 0 value.
    return (TUint(aFrom)==KPbkSpaceDecValue)? TChar(0) : aFrom;
    }

inline TChar ConvertSeparatorCharacterToNew(TChar aFrom)
    {
    // Convert 0 value to space value.
    return TUint(aFrom)==0? TChar(KPbkSpaceDecValue) : aFrom;
    }

} // namespace

// ==================== MEMBER FUNCTIONS ====================

inline CPbkSortOrderManager::CPbkSortOrderManager(RFs& aFs, TBool aSettingsVisible):
                                        iFs(aFs), iSettingsVisible(aSettingsVisible)
    {
    }

inline void CPbkSortOrderManager::ConstructL()
    {
    iPersistentSetting = PbkGlobalSettingFactory::CreatePersistentSettingL();
    iPersistentSetting->ConnectL(MPbkGlobalSetting::EGeneralSettingCategory);

    // construct name sort order
    RContactViewSortOrder sortOrder =
        DoCreateSortOrderL(PersistentNameDisplayOrderL(), iSettingsVisible);
    iSortOrder = sortOrder;
    iNameSeparator =
        ConvertSeparatorCharacterToOld(PersistentNameSeparatorCharL());
    iPersistentSetting->RegisterObserverL(this);
    }

inline void CPbkSortOrderManager::SetPersistentNameDisplayOrderL
        (CPbkContactEngine::TPbkNameOrder aNameOrder)
    {
    __ASSERT_DEBUG(
            aNameOrder == CPbkContactEngine::EPbkNameOrderLastNameFirstName ||
            aNameOrder == CPbkContactEngine::EPbkNameOrderFirstNameLastName,
            Panic(EPanicPreCond_SetNameDisplayOrderL));

    User::LeaveIfError(iPersistentSetting->Set(
        MPbkGlobalSetting::ENameOrdering, TInt(aNameOrder)));
    }

inline void CPbkSortOrderManager::
        SetPersistentNameSeparatorCharL(TChar aNameSeparatorChar)
    {
    // 1 character is appended.
    TBuf<1> string;
    string.Append(aNameSeparatorChar);
    User::LeaveIfError(iPersistentSetting->Set(
        MPbkGlobalSetting::ENameSeparatorChar, string));
    }


CPbkSortOrderManager* CPbkSortOrderManager::NewL(RFs& aFs, TBool aSettingsVisible)
    {
    CPbkSortOrderManager* self = new(ELeave) CPbkSortOrderManager(aFs, aSettingsVisible);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkSortOrderManager::~CPbkSortOrderManager()
    {
    if (iPersistentSetting)
        {
        iPersistentSetting->Close();
        delete iPersistentSetting;
        }
    iSortOrder.Close();
    delete iNameOrderSetting;
    delete iNameSeparatorUsed;
    delete iInitialNameSeparatorChar;
    }

const RContactViewSortOrder& CPbkSortOrderManager::SortOrder() const
    {
    return iSortOrder;
    }

void CPbkSortOrderManager::SetNameDisplayOrderL
        (CPbkContactEngine::TPbkNameOrder aNameOrder)
    {
    // create new view sort order from aNameOrder
    RContactViewSortOrder newSortOrder = DoCreateSortOrderL(aNameOrder,
                                                            iSettingsVisible);
    CleanupClosePushL(newSortOrder);

    if (iContactView)
        {
        //retrieve the contact view sort order
        RContactViewSortOrder oldViewSortOrder;
        CleanupClosePushL(oldViewSortOrder);
        iContactView->GetSortOrderL(oldViewSortOrder);

        // try to change the contact view sort order
        iContactView->ChangeSortOrderL(newSortOrder);
        // Try to change the persistent key for name ordering
        TRAPD(err,SetPersistentNameDisplayOrderL(aNameOrder));
        if (err)
            {
            // error occurred, restore view sort order
            iContactView->ChangeSortOrderL(oldViewSortOrder);
            CleanupStack::Pop(); // oldViewSortOrder
            }
        else
            {
            CleanupStack::PopAndDestroy(); // oldViewSortOrder
            iSortOrder.Close();
            iSortOrder = newSortOrder;
            CleanupStack::Pop(); // newSortOrder
            }
        }
    else
        {
        CleanupStack::PopAndDestroy(); // newSortOrder
        }
    }

CPbkContactEngine::TPbkNameOrder CPbkSortOrderManager::NameDisplayOrder() const
    {
    // return current name display order
    for (TInt i = 0; i < iSortOrder.Count(); ++i)
        {
        if (iSortOrder[i] == KUidContactFieldFamilyName)
            {
            return CPbkContactEngine::EPbkNameOrderLastNameFirstName;
            }
        else if (iSortOrder[i] == KUidContactFieldGivenName)
            {
            return CPbkContactEngine::EPbkNameOrderFirstNameLastName;
            }
        }
    return CPbkContactEngine::EPbkNameOrderLastNameFirstName;
    }

void CPbkSortOrderManager::SetContactView
        (CContactNamedRemoteView& aContactView)
    {
    iContactView = &aContactView;
    }

void CPbkSortOrderManager::SettingChangedL(MPbkGlobalSetting::
                                                    TPbkGlobalSetting aKey)
    {
    if (aKey == MPbkGlobalSetting::ENameOrdering ||
        aKey == MPbkGlobalSetting::ENameSeparatorChar)
        {
        // Update sortorder and name separator char
        RContactViewSortOrder sortOrder =
                            DoCreateSortOrderL(PersistentNameDisplayOrderL(),
                                               iSettingsVisible);
        iSortOrder.Close();
        iSortOrder = sortOrder;

        iNameSeparator =
            ConvertSeparatorCharacterToOld(PersistentNameSeparatorCharL());
        }
    }

void CPbkSortOrderManager::SetNameSeparatorL(TChar aNameSeparator)
    {
    // 0 is converted to space character
    SetPersistentNameSeparatorCharL(
        ConvertSeparatorCharacterToNew(aNameSeparator));
    iNameSeparator = aNameSeparator;
    }

TChar CPbkSortOrderManager::NameSeparator() const
    {
    return iNameSeparator;
    }

RContactViewSortOrder CPbkSortOrderManager::DoCreateSortOrderL
        (CPbkContactEngine::TPbkNameOrder aNameOrder,
         TBool aSettingVisible)
    {
    RContactViewSortOrder sortOrder;
    CleanupClosePushL(sortOrder);
    // engine creation will assign it to the views
	
	sortOrder.AppendL(TUid::Uid(KUidContactFieldTopContactValue));
	
    if (FeatureManager::FeatureSupported(KFeatureIdChinese) &&
        !aSettingVisible)
        {
        // Chinese sort ordering
        sortOrder.AppendL(KUidContactFieldFamilyName);
        sortOrder.AppendL(KUidContactFieldGivenName);
        sortOrder.AppendL(KUidContactFieldCompanyName);
        }
    else if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
        {
        // japanese sort ordering
        if (aNameOrder == CPbkContactEngine::EPbkNameOrderLastNameFirstName)
            {
            sortOrder.AppendL(KUidContactFieldFamilyNamePronunciation);
            sortOrder.AppendL(KUidContactFieldGivenNamePronunciation);
            sortOrder.AppendL(KUidContactFieldFamilyName);
            sortOrder.AppendL(KUidContactFieldGivenName);
            }
        else
            {
            sortOrder.AppendL(KUidContactFieldGivenNamePronunciation);
            sortOrder.AppendL(KUidContactFieldFamilyNamePronunciation);
            sortOrder.AppendL(KUidContactFieldGivenName);
            sortOrder.AppendL(KUidContactFieldFamilyName);
            }
        sortOrder.AppendL(KUidContactFieldCompanyName);
        }
    else
        {
        // default sort ordering
        if (aNameOrder == CPbkContactEngine::EPbkNameOrderLastNameFirstName)
            {
            sortOrder.AppendL(KUidContactFieldFamilyName);
            sortOrder.AppendL(KUidContactFieldGivenName);
            }
        else
            {
            sortOrder.AppendL(KUidContactFieldGivenName);
            sortOrder.AppendL(KUidContactFieldFamilyName);
            }
        sortOrder.AppendL(KUidContactFieldCompanyName);
        }
    sortOrder.AppendL(KUidContactFieldTemplateLabel);

    CleanupStack::Pop(); // sortOrder
    return sortOrder;
    }

CPbkContactEngine::TPbkNameOrder
    CPbkSortOrderManager::PersistentNameDisplayOrderL()
    {
    TInt nameOrder = KErrNotFound;
    User::LeaveIfError(
        iPersistentSetting->Get(MPbkGlobalSetting::ENameOrdering, nameOrder));
    CPbkContactEngine::TPbkNameOrder pbkNameOrder =
        static_cast<CPbkContactEngine::TPbkNameOrder>(nameOrder);

    // Read phonebook contact name presentation order configuration
    // if order not defined.
    if (pbkNameOrder == CPbkContactEngine::EPbkNameOrderNotDefined)
        {
        RResourceFile pbkResFile;
        // Calls CleanupClosePushL for pbkResFile.
        PbkEngUtils::FindAndOpenDefaultResourceFileLC(iFs, pbkResFile);
        TResourceReader reader;

        if(!iNameOrderSetting)
            {
            // Value 0 is first name last name and value 1 is last name first name
            reader.SetBuffer(pbkResFile.AllocReadLC(R_PBK_NAME_ORDER));
            iNameOrderSetting = reader.ReadHBufCL();
            CleanupStack::PopAndDestroy();  // R_PBK_NAME_ORDER
            }

        if (!iNameOrderSetting->Compare(KPbkFirstLastNameEnabled))
            {
            // Compare return 0 if setting order value in loaded resource is "0"
            // then first name last name order is used.
            pbkNameOrder = CPbkContactEngine::EPbkNameOrderFirstNameLastName;
            }
        else
            {
            pbkNameOrder = CPbkContactEngine::EPbkNameOrderLastNameFirstName;
            }
        CleanupStack::PopAndDestroy(1); // pbkResFile
        }
    return pbkNameOrder;
    }

TChar CPbkSortOrderManager::PersistentNameSeparatorCharL()
    {
    TBuf<KPbkSeparatorMaxLength> string;
    string.SetLength(0);
    User::LeaveIfError(iPersistentSetting->Get(
        MPbkGlobalSetting::ENameSeparatorChar, string));
    // NULL string is for undefined
    if (!string.Length() || string[0] == 0)
        {
        RResourceFile pbkResFile;
        // Calls CleanupClosePushL for pbkResFile.
        PbkEngUtils::FindAndOpenDefaultResourceFileLC(iFs, pbkResFile);
        TResourceReader reader;

        if(!iNameSeparatorUsed)
            {
            reader.SetBuffer(pbkResFile.AllocReadLC(R_PBK_NAME_SEPARATOR_USED));
            iNameSeparatorUsed = reader.ReadHBufCL();
            CleanupStack::PopAndDestroy();  // R_PBK_NAME_SEPARATOR_USED
            }
        if(!iNameSeparatorUsed->Compare(KPbkNameSeparatorUsed))
            {
            // Separator is used, use initial value
            if(!iInitialNameSeparatorChar)
                {
                reader.SetBuffer(pbkResFile.AllocReadLC(R_PBK_NAME_SEPARATOR_CHAR));
                iInitialNameSeparatorChar = reader.ReadHBufCL();
                CleanupStack::PopAndDestroy();  // R_PBK_NAME_SEPARATOR_CHAR
                }
            string.SetLength(0);
            string.Append(*iInitialNameSeparatorChar);
            }
        else if (string.Length()<KPbkSeparatorMaxLength)
            {
            // Space is used if separator character is not defined by default.
            TChar space(KPbkSpaceDecValue);
            string.SetLength(0);
            string.Append(space);
            }
        CleanupStack::PopAndDestroy(1); // pbkResFile
        }

    return TChar(string[0]);
    }

// End of File
