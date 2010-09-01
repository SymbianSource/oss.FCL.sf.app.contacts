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
*     This class extends contact model CContactItemField class capabilities
*	  to better suit Phonebook's needs.
*
*/


// INCLUDE FILES
#include    "TPbkContactItemField.h"
#include    "CPbkFieldInfo.h"
#include    "PbkDefaults.h"
#include    <PbkEngUtils.h>

// Unnamed namespace for local defintions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
	EPanicPreCond_IconId = 1,
	EPanicPreCond_Text,
	EPanicPreCond_Time,
	EPanicPreCond_StorageType,
	EPanicPreCond_TextStorage,
	EPanicPreCond_DateTimeStorage,
	EPanicPreCond_Label,
	EPanicPreCond_SetLabel,
	EPanicPreCond_SetHidden,
	EPanicPreCond_IsHidden,
	EPanicPreCond_Compare,
	EPanicPreCond_IsEmpty,
	EPanicPreCond_IsEmptyOrAllSpaces,
	EPanicPreCond_DefaultPhoneNumberField,
    EPanicPreCond_DefaultVideoNumberField,
	EPanicPreCond_DefaultSmsField,
	EPanicPreCond_DefaultEmailField,
	EPanicPreCond_DefaultMmsField,
	EPanicPreCond_DefaultEmailOverSmsField,
	EPanicPreCond_SpeedDialIndexesL,
	EPanicPreCond_PbkFieldType,
	EPanicPreCond_PbkFieldId,
	EPanicPreCond_PrepareAfterLoadL,
	EPanicPreCond_HasInvalidDate,
    EPanicPostCond_Constructor,
    EPanicPostCond_Set,
    EPanicUnsupportedStorageType,
    EPanicPreCond_DefaultPocField,
    EPanicPreCond_DefaultVoipField
    };
#endif     

// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)   
    {
    _LIT(KPanicText, "TPbkContactItemField");
    User::Panic(KPanicText, aReason);
    }
#endif

inline const TDesC8& ToTDesC8(HBufC8* aBuf)
    {
    if (aBuf)
        {
        return *aBuf;
        }
    else
        {
        return KNullDesC8;
        }
    }

}  // namespace

// ================= MEMBER FUNCTIONS =======================

EXPORT_C TPbkContactItemField::TPbkContactItemField() :
    iField(NULL), iFieldInfo(NULL)
    {
    }

EXPORT_C TPbkContactItemField::TPbkContactItemField
        (CContactItemField* aField, CPbkFieldInfo* aFieldInfo)
    : iField(aField), iFieldInfo(aFieldInfo)
    {
    //PostCond:
    __ASSERT_DEBUG(iField && iFieldInfo, Panic(EPanicPostCond_Constructor));
    }

EXPORT_C void TPbkContactItemField::Set
        (CContactItemField* aField, CPbkFieldInfo* aFieldInfo)
    {
    iField = aField;
    iFieldInfo = aFieldInfo;
    
    //PostCond:
    __ASSERT_DEBUG(iField && iFieldInfo, Panic(EPanicPostCond_Set));
    }

EXPORT_C TPbkIconId TPbkContactItemField::IconId() const
    {
    //PreCond:
    __ASSERT_DEBUG(iFieldInfo, Panic(EPanicPreCond_IconId));

    return iFieldInfo->IconId();
    }

EXPORT_C CPbkFieldInfo& TPbkContactItemField::FieldInfo() const
    {
    return *iFieldInfo;
    }

EXPORT_C TPtrC TPbkContactItemField::Text() const
    {
    //PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_Text));

    // PreCond: iField->TextStorage() will __ASSERT_ALWAYS correct storage type
    return iField->TextStorage()->Text();
    }

EXPORT_C TTime TPbkContactItemField::Time() const
    {
    //PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_Time));

    // PreCond: iField->DateTimeStorage() will __ASSERT_ALWAYS correct
    // storage type
    return iField->DateTimeStorage()->Time();
    }

EXPORT_C void TPbkContactItemField::GetTextL
        (TDes& aText) const
    {
    
    switch(StorageType())
        {
        case KStorageTypeText:
            {
            TPtrC text = TextStorage()->Text();
            if(text.Length() > aText.MaxLength())
                {
                aText.Copy(text.Left(aText.MaxLength()));
                // Leave with overflow error code
                User::Leave(KErrOverflow);
                }
            aText.Copy(text);
            }
            break;
        case KStorageTypeDateTime:
            {
            TTime datetime = DateTimeStorage()->Time();
            if(datetime != Time::NullTTime())
                {
                // Format date into aText using current locale settings
                datetime.FormatL(aText, _L("%D%M%Y%/0%1%/1%2%/2%3%/3"));
                }
            else
                {
                // time was null, reset aText
                aText.Zero();
                }

            }
            break;
        default:
            // Unsupported field type, reset aText
            aText.Zero();
            User::Leave(KErrNotSupported);
            break;
        }
    }

EXPORT_C TStorageType TPbkContactItemField::StorageType() const
    {
    //PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_StorageType));

    return iField->StorageType();
    }

EXPORT_C CContactItemField& TPbkContactItemField::ItemField() const
    {
    return *iField;
    }

EXPORT_C CContactTextField* TPbkContactItemField::TextStorage() const
    {
    //PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_TextStorage));

    return iField->TextStorage();
    }

EXPORT_C TPtrC TPbkContactItemField::Label() const
    {
    //PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_Label));

    return iField->Label();
    }

EXPORT_C void TPbkContactItemField::SetLabelL(const TDesC& aLabel)
    {
    //PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_SetLabel));

    iField->SetLabelL(aLabel);
    }

EXPORT_C void TPbkContactItemField::SetHidden(TBool aHidden)
    {
    //PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_SetHidden));

    iField->SetHidden(aHidden);
    }

EXPORT_C TBool TPbkContactItemField::IsHidden() const 
    { 
	//PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_IsHidden));

    return iField->IsHidden(); 
    }
    
EXPORT_C CContactDateField* TPbkContactItemField::DateTimeStorage() const
    {
	//PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_DateTimeStorage));

    return iField->DateTimeStorage();
    }

EXPORT_C TBool TPbkContactItemField::IsSame
        (const TPbkContactItemField& aField) const
    {
    // Fields are "same" if the embedded CContactItemField object
    // is the same by pointer comparison.
    return (iField == aField.iField);
    }

EXPORT_C CContactItemField& TPbkContactItemField::ContactItemField() 
    { 
    return *iField; 
    }

EXPORT_C const CContactItemField& TPbkContactItemField::ContactItemField() const 
    { 
    return *iField; 
    }

EXPORT_C TInt TPbkContactItemField::Compare
        (const TPbkContactItemField& aRhs) const
    {
	//PreCond:
    __ASSERT_DEBUG(iFieldInfo, Panic(EPanicPreCond_Compare));

    return iFieldInfo->CompareOrdering(*aRhs.iFieldInfo);
    }

EXPORT_C TInt TPbkContactItemField::UniqueFieldIdentity() const
    {
    // Unique identity is CContactItemField's this pointer
    return reinterpret_cast<TInt>(iField);
    }

EXPORT_C TBool TPbkContactItemField::IsEmpty() const
    {
	//PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_IsEmpty));

    return !iField->Storage()->IsFull();
    }

EXPORT_C TBool TPbkContactItemField::IsEmptyOrAllSpaces() const
    {
	//PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_IsEmptyOrAllSpaces));

    switch (iField->StorageType())
        {
        case KStorageTypeText:
            {
            TPtrC text = iField->TextStorage()->Text();
            return PbkEngUtils::IsEmptyOrAllSpaces(text);
            }

        default:
            {
            return !iField->Storage()->IsFull();
            }
        }
    }

EXPORT_C TBool TPbkContactItemField::DefaultPhoneNumberField() const
    {
	//PreCond:
    __ASSERT_DEBUG(iField && iFieldInfo, Panic(EPanicPreCond_DefaultPhoneNumberField));

    return (iFieldInfo->IsPhoneNumberField() &&
            iField->ContentType().ContainsFieldType(KUidPbkDefaultFieldPref));
    }

EXPORT_C TBool TPbkContactItemField::DefaultVideoNumberField() const
    {
	//PreCond:
    __ASSERT_DEBUG(iField && iFieldInfo, Panic(EPanicPreCond_DefaultVideoNumberField));

    return (iFieldInfo->IsPhoneNumberField() &&
            iField->ContentType().ContainsFieldType(KUidPbkDefaultFieldVideo));
    }

EXPORT_C TBool TPbkContactItemField::DefaultSmsField() const 
    {
	//PreCond:
    __ASSERT_DEBUG(iField && iFieldInfo, Panic(EPanicPreCond_DefaultSmsField));

    return (iFieldInfo->IsPhoneNumberField() && 
            iField->ContentType().ContainsFieldType(KUidPbkDefaultFieldSms));
    }

EXPORT_C TBool TPbkContactItemField::DefaultEmailField() const
    {
	//PreCond:
    __ASSERT_DEBUG(iField && iFieldInfo, Panic(EPanicPreCond_DefaultEmailField));

    return (iFieldInfo->FieldId() == EPbkFieldIdEmailAddress && 
            iField->ContentType().ContainsFieldType(KUidPbkDefaultFieldPref));
    }

EXPORT_C TBool TPbkContactItemField::DefaultMmsField() const
    {
	//PreCond:
    __ASSERT_DEBUG(iField && iFieldInfo, Panic(EPanicPreCond_DefaultMmsField));

    return (iFieldInfo->IsMmsField() && 
            iField->ContentType().ContainsFieldType(KUidPbkDefaultFieldMms));
    }

EXPORT_C TBool TPbkContactItemField::DefaultPocField() const
    {
	//PreCond:
    __ASSERT_DEBUG(iField && iFieldInfo, Panic(EPanicPreCond_DefaultPocField));

    return (iFieldInfo->IsPocField() && 
            iField->ContentType().ContainsFieldType(KUidPbkDefaultFieldPoc));
    }
   
EXPORT_C TBool TPbkContactItemField::DefaultVoipField() const
    {
	//PreCond:
    __ASSERT_DEBUG(iField && iFieldInfo, Panic(EPanicPreCond_DefaultVoipField));

    return (iFieldInfo->IsVoipField() && 
            iField->ContentType().ContainsFieldType(KUidPbkDefaultFieldVoip));
    }

EXPORT_C TBool TPbkContactItemField::DefaultEmailOverSmsField() const
    {
	//PreCond:
    __ASSERT_DEBUG(iField && iFieldInfo, Panic(EPanicPreCond_DefaultEmailOverSmsField));

    return (iFieldInfo->IsEmailOverSmsField() && 
            iField->ContentType().ContainsFieldType(KUidPbkDefaultFieldEmailOverSms));
    }

EXPORT_C CArrayFix<TInt>* TPbkContactItemField::SpeedDialIndexesL() const
    {
	//PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_SpeedDialIndexesL));

    CArrayFix<TInt>* speedDialIndexArray = new(ELeave) CArrayFixFlat<TInt>(2);
	CleanupStack::PushL(speedDialIndexArray);

    const TInt fieldtypeCount = iField->ContentType().FieldTypeCount();
    for (TInt i = 0; i < fieldtypeCount; ++i)
        {
        TFieldType fieldType = iField->ContentType().FieldType(i);
        switch (fieldType.iUid)
            {
            case KUidSpeedDialOneValue:
				{
                speedDialIndexArray->AppendL(1);
                break;
				}
            case KUidSpeedDialTwoValue:
				{
                speedDialIndexArray->AppendL(2);
                break;
				}
            case KUidSpeedDialThreeValue:
				{
                speedDialIndexArray->AppendL(3);
                break;
				}
            case KUidSpeedDialFourValue:
				{
                speedDialIndexArray->AppendL(4);
                break;
				}
            case KUidSpeedDialFiveValue:
				{
                speedDialIndexArray->AppendL(5);
                break;
				}
            case KUidSpeedDialSixValue:
				{
                speedDialIndexArray->AppendL(6);
                break;
				}
            case KUidSpeedDialSevenValue:
				{
                speedDialIndexArray->AppendL(7);
                break;
				}
            case KUidSpeedDialEightValue:
				{
                speedDialIndexArray->AppendL(8);
                break;
				}
            case KUidSpeedDialNineValue:
				{
                speedDialIndexArray->AppendL(9);
                break;
				}
            // NO DEFAULT CASE - search only for speed dial Uids, ignore others
            }
        }
    if (speedDialIndexArray->Count() == 0)
        {
    	CleanupStack::PopAndDestroy(speedDialIndexArray);
        return NULL;
        }
	CleanupStack::Pop(); // speedDialIndexArray
    return speedDialIndexArray;
    }

EXPORT_C TStorageType TPbkContactItemField::PbkFieldType() const
    {
	//PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_PbkFieldType));

    return iField->StorageType();
    }

EXPORT_C TPbkFieldId TPbkContactItemField::PbkFieldId() const
    {
	//PreCond:
    __ASSERT_DEBUG(iFieldInfo, Panic(EPanicPreCond_PbkFieldId));

    return iFieldInfo->FieldId();
    }

EXPORT_C TPtrC TPbkContactItemField::PbkFieldText() const
    {
    return Text();
    }

EXPORT_C TTime TPbkContactItemField::PbkFieldTime() const
    {
    return Time();
    }

void TPbkContactItemField::PrepareForSaveL()
    {
	// Intentionally left blank
    }

void TPbkContactItemField::PrepareAfterLoadL()
    {
	//PreCond:
    __ASSERT_DEBUG(iField && iFieldInfo, Panic(EPanicPreCond_PrepareAfterLoadL));

    // Field was hidden for some reason -> unhide it.
    // This fixes a problem with contact model related to inheriting
    // field attributes from matching golden template fields.
    // This is safe because Phonebook never hides any fields it recognizes.
	if ((iField->IsHidden()) && (!iFieldInfo->IsHidden()))
        {
        iField->SetHidden(EFalse);
        }
    }

TBool TPbkContactItemField::HasInvalidDate()
    {
	//PreCond:
    __ASSERT_DEBUG(iField, Panic(EPanicPreCond_HasInvalidDate));

    if (iField->StorageType() == KStorageTypeDateTime)
        {
        const TDateTime& date = iField->DateTimeStorage()->Time().DateTime();
        
        // check that values are correct
        if ((date.Year() < KPbkDateMinYear) || (date.Year() > KPbkDateMaxYear))
            {
            return ETrue;
            }

        if ((date.Month() < EJanuary) || (date.Month() > EDecember))
            {
            return ETrue;
            }

        if ((date.Day() < KPbkDateMinDay ) || (date.Day() > KPbkDateMaxDay))
            {
            return ETrue;
            }
        }
    // either no datetime field or date is valid, return false
    return EFalse;
    }


// ================= OTHER EXPORTED FUNCTIONS ==============

EXPORT_C TBool operator==
        (const TPbkContactItemField& aLeft, 
        const TPbkContactItemField& aRight)
    {
    if (aLeft.iField == aRight.iField)
        {
        // Trivially equal if both wrappers of the same contact model 
        // field object
        return ETrue;
        }

    if (aLeft.iFieldInfo != aRight.iFieldInfo ||
        aLeft.iField->StorageType() != aRight.iField->StorageType())
        {
        // Trivially not equal if different types
        return EFalse;
        }

    // Compare field content types
    if (!(aLeft.iField->ContentType() == aRight.iField->ContentType()))
        {
        return EFalse;
        }

    // Compare relevant flags
    if (aLeft.iField->IsHidden() != aRight.iField->IsHidden())
        {
        return EFalse;
        }

    // Compare field labels
    if (aLeft.iField->Label() != aRight.iField->Label())
        {
        return EFalse;
        }

    // Compare field data
    switch (aLeft.iField->StorageType())
        {
        case KStorageTypeText:
            {
            return (aLeft.iField->TextStorage()->Text() == aRight.iField->TextStorage()->Text());
            }
        case KStorageTypeDateTime:
            {
            return (aLeft.iField->DateTimeStorage()->Time() == aRight.iField->DateTimeStorage()->Time());
            }
        case KStorageTypeStore:
            {
            return (ToTDesC8(aLeft.iField->StoreStorage()->Thing()) == ToTDesC8(aRight.iField->StoreStorage()->Thing()));
            }
        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(EPanicUnsupportedStorageType));
            break;
            }
        }

    return EFalse;
    }


//  End of File
