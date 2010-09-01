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
*		Phonebook Contact item class
*
*/


// INCLUDE FILES
#include    "CPbkContactItem.h"
#include    <cntfield.h>    // CContactItemField
#include    <cntitem.h>     // CContactItem
#include    <featmgr.h>	    // Feature manager
#include    "CPbkFieldsInfo.h"
#include    "CPbkFieldInfo.h"
#include    "TPbkContactItemField.h"
#include    "PbkFields.hrh"
#include    <PbkEngUtils.h>
#include    <PbkDebug.h>
#include    "PbkDefaults.h"
#include    <MPbkContactNameFormat.h>
#include    "TPbkMatchPriorityLevel.h"
#include    "CPbkContactEngine.h"
#include    "CPbkSyncronizationConstants.h"

#ifdef _DEBUG
// Engine needed only for a few asserts
#include    "CPbkContactEngine.h"
#endif

// Unnamed namespace for local definitions

namespace {

// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ConstructL,
    EPanicInvariant_Pointer,
    EPanicInvariant_Count,
    EPanicInvariant_Field,
    EPanicInvariant_FieldInfo,
    EPanicInvariant_Sorting,
    EPanicInvariant_PrefCount,
    EPanicInvariant_SmsCount,
    EPanicInvariant_PrefSmsCount,
    EPanicInvariant_PhoneDefaultCount,
    EPanicInvariant_SmsDefaultCount,
    EPanicInvariant_EmailDefaultCount,
    EPanicInvariant_VideoTagCount,
    EPanicPostCond_FindFieldIndex,
    EPanicPreCond_RemoveField,
    EPanicLogic_RemoveField,
    EPanicPostCond_RemoveField,
    EPanicPostCond_AddFieldL,
    EPanicPreCond_GroupsJoinedLC,
    EPanicPostCond_SetDefaultPhoneNumberFieldL,
    EPanicPostCond_SetDefaultVideoNumberFieldL,
    EPanicPostCond_SetDefaultEmailFieldL,
    EPanicPreCond_UpdateFieldSetL,
    EPanicPostCond_InsertionPos,
    EPanicPostCond_SetVoiceTagFieldL,
    EPanicInvariant_VoiceTagCount,
    EPanicInvariant_VoiceTagFieldCount,
    EPanicPostCond_SetDefaultMmsFieldL,
    EPanicPostCond_SetDefaultEmailOverSmsFieldL,
    EPanicPostCond_SetDefaultPocFieldL,
    EPanicPostCond_SetDefaultVoipFieldL,
    EPanicPreCond_VoiceTagField1,
    EPanicPreCond_VoiceTagField2,
    EPanicPreCond_LoadSindHandlerImplL,
    EPanicDeprecatedFunction
    };

// ==================== LOCAL FUNCTIONS ====================

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactItem");
    User::Panic(KPanicText, aReason);
    }

/**
 * Searches a field set for the first field with type specified type.
 *
 * @param aFields   the field set to search.
 * @param aFieldInfo    the field type to search for.
 * @param aIndex        index where to start the search.
 * @return index of the matching field in aFields or KErrNotFound if not found.
 */
TInt FindFieldWithType
        (const CPbkFieldArray& aFields,
        const CPbkFieldInfo& aFieldInfo,
        TInt aIndex=0)
    {
    const TInt fieldCount = aFields.Count();
    for (TInt i=aIndex; i < fieldCount; ++i)
        {
        if (aFields[i].FieldInfo().IsSame(aFieldInfo))
            {
            return i;
            }
        }
    return KErrNotFound;
    }

}  // namespace


// ================= MEMBER FUNCTIONS =======================

/**
 * C++ constructor.
 */
inline CPbkContactItem::CPbkContactItem(MPbkContactNameFormat& aNameFormat) :
    iNameFormat(aNameFormat)
    {
    // new(ELeave) in NewL will reset members
    __ASSERT_DEBUG(!iItem, Panic(EPanicPostCond_Constructor));
    }

/**
 * Second phase constructor.
 */
inline void CPbkContactItem::ConstructL
        (CContactItem* aItem,
        const CPbkFieldsInfo& aFieldsInfo)
    {
    //PreCond:
    __ASSERT_DEBUG(aItem && !iItem, Panic(EPanicPreCond_ConstructL));

    CreateFieldArrayL(*aItem, aFieldsInfo);
    // construction was succesful, take ownership of aItem
    iItem = aItem;

    // Philosophical note: class invariant is effectively a postcondition
    // for a constructor.
    __TEST_INVARIANT;
    }

EXPORT_C CPbkContactItem* CPbkContactItem::NewL
        (CContactItem* aItem,
        const CPbkFieldsInfo& aFieldsInfo,
        MPbkContactNameFormat& aNameFormat)
    {
    CPbkContactItem* self = new(ELeave) CPbkContactItem(aNameFormat);
    CleanupStack::PushL(self);
    self->ConstructL(aItem, aFieldsInfo);
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactItem::~CPbkContactItem()
    {
#ifdef _DEBUG
    // Test invariant only at the beginning of destructor and only if
    // construction was succesful.
    if (iItem)
        __TEST_INVARIANT;
#endif

    // Delete own data
    delete iItem;
    }

EXPORT_C CContactItem& CPbkContactItem::ContactItem()
    {
    return *iItem;
    }

EXPORT_C const CContactItem& CPbkContactItem::ContactItem() const
    {
    return *iItem;
    }

EXPORT_C HBufC* CPbkContactItem::GetContactTitleL() const
    {
    __TEST_INVARIANT;

    // Delegate call to iNameFormat
    return iNameFormat.GetContactTitleL(*this);
    }

EXPORT_C HBufC* CPbkContactItem::GetContactTitleOrNullL() const
    {
    __TEST_INVARIANT;

    // Delegate call to iNameFormat
    return iNameFormat.GetContactTitleOrNullL(*this);
    }

EXPORT_C TPbkIconId CPbkContactItem::ContactIconIdL() const
    {
    __TEST_INVARIANT;

    TPbkContactItemField* field = DefaultPhoneNumberField();
    if (field)
        {
        return field->IconId();
        }
    return EPbkNullIconId;
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::DefaultPhoneNumberField() const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    // scan the fields for a match in default field for field id.
    const TInt fieldCount = iFields.Count();
    for(TInt i=0; i < fieldCount; ++i)
        {
        if (iFields[i].DefaultPhoneNumberField())
            {
            return const_cast<TPbkContactItemField*>(&(iFields[i]));
            }
        }
    return NULL;
    }

EXPORT_C void CPbkContactItem::SetDefaultPhoneNumberFieldL
        (TPbkContactItemField* aField)
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    if (aField)
        {
        aField = FindSameField(*aField);
        if (!aField)
            {
            // Not this item's field
            User::Leave(KErrNotFound);
            }
        }

    // Test can we set the default for this field
    if (aField && !aField->FieldInfo().IsPhoneNumberField())
        {
        User::Leave(KErrNotSupported);
        }

    // 1. Fetch previous default
    TPbkContactItemField* prevDefault = DefaultPhoneNumberField();

    // 2. Set the new default, if specified
    if (aField)
        {
        if (prevDefault == aField)
            {
            // Default already set to aField
            return;
            }
        // If this leaves, this object's state remains unchanged
        aField->ItemField().AddFieldTypeL(KUidPbkDefaultFieldPref);
        }

    // 3. Unset any previous default (invariant tests that there is always just
    // one default.
    if (prevDefault)
        {
        // Unset previous default
        prevDefault->ItemField().RemoveFieldType(KUidPbkDefaultFieldPref);
        }

    __ASSERT_DEBUG(
        (aField && DefaultPhoneNumberField()==aField) ||
        (!aField && !DefaultPhoneNumberField()),
        Panic(EPanicPostCond_SetDefaultPhoneNumberFieldL));
    }

EXPORT_C void CPbkContactItem::RemoveDefaultPhoneNumberField()
    {
    // This is safe because SetDefaultPhoneNumberFieldL(NULL)
    // guarantees not to leave
    SetDefaultPhoneNumberFieldL(NULL);
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::DefaultVideoNumberField() const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    // scan the fields for a match in default field for field id.
    const TInt fieldCount = iFields.Count();
    for(TInt i=0; i < fieldCount; ++i)
        {
        if (iFields[i].DefaultVideoNumberField())
            {
            return const_cast<TPbkContactItemField*>(&(iFields[i]));
            }
        }
    return NULL;
    }

EXPORT_C void CPbkContactItem::SetDefaultVideoNumberFieldL
        (TPbkContactItemField* aField)
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    if (aField)
        {
        aField = FindSameField(*aField);
        if (!aField)
            {
            // Not this item's field
            User::Leave(KErrNotFound);
            }
        }

    // Test can we set the default for this field
    if (aField && !aField->FieldInfo().IsPhoneNumberField())
        {
        User::Leave(KErrNotSupported);
        }

    // 1. Fetch previous default
    TPbkContactItemField* prevDefault = DefaultVideoNumberField();

    // 2. Set the new default, if specified
    if (aField)
        {
        if (prevDefault == aField)
            {
            // Default already set to aField
            return;
            }
        // If this leaves, this object's state remains unchanged
        aField->ItemField().AddFieldTypeL(KUidPbkDefaultFieldVideo);
        }

    // 3. Unset any previous default (invariant tests that there is always just
    // one default.
    if (prevDefault)
        {
        // Unset previous default
        prevDefault->ItemField().RemoveFieldType(KUidPbkDefaultFieldVideo);
        }

    __ASSERT_DEBUG(
        (aField && DefaultVideoNumberField()==aField) ||
        (!aField && !DefaultVideoNumberField()),
        Panic(EPanicPostCond_SetDefaultVideoNumberFieldL));
    }

EXPORT_C void CPbkContactItem::RemoveDefaultVideoNumberField()
    {
    // This is safe because SetDefaultPhoneNumberFieldL(NULL)
    // guarantees not to leave
    SetDefaultVideoNumberFieldL(NULL);
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::DefaultSmsField() const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    const TInt fieldCount = iFields.Count();
    for (TInt i=0; i < fieldCount; ++i)
        {
        const TPbkContactItemField& field = iFields[i];
        if (field.DefaultSmsField())
            {
            return const_cast<TPbkContactItemField*>(&field);
            }
        }
    return NULL;
    }

EXPORT_C void CPbkContactItem::SetDefaultSmsFieldL
        (TPbkContactItemField* aField)
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    if (aField)
        {
        aField = FindSameField(*aField);
        if (!aField)
            {
            // Not this item's field
            User::Leave(KErrNotFound);
            }
        }

    // Test can we set the default for this field
    if (aField && !aField->FieldInfo().IsPhoneNumberField())
        {
        User::Leave(KErrNotSupported);
        }

    // 1. Fetch previous default
    TPbkContactItemField* prevDefault = DefaultSmsField();

    // 2. Set the new default if specified
    if (aField)
        {
        if (aField == prevDefault)
            {
            // Already set
            return;
            }
        // If this leaves, this object's state remains unchanged
        aField->ItemField().AddFieldTypeL(KUidPbkDefaultFieldSms);
        }

    // 3. Unset any previous default (invariant tests that there is always just
    // one default).
    if (prevDefault)
        {
        prevDefault->ItemField().RemoveFieldType(KUidPbkDefaultFieldSms);
        }

    __ASSERT_DEBUG(
        (aField && DefaultSmsField()==aField) ||
        (!aField && !DefaultSmsField()),
        Panic(EPanicPostCond_SetDefaultPhoneNumberFieldL));
    }

EXPORT_C void CPbkContactItem::RemoveDefaultSmsField()
    {
    // This is safe because SetDefaultSmsFieldL(NULL) guarantees
    // not to leave
    SetDefaultSmsFieldL(NULL);
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::DefaultEmailField() const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    // scan the fields for a match in default field for field id.
    const TInt fieldCount = iFields.Count();
    for (TInt i = 0; i < fieldCount; ++i)
        {
        const TPbkContactItemField& field = iFields[i];
        if (field.DefaultEmailField())
            {
            return const_cast<TPbkContactItemField*>(&field);
            }
        }
    return NULL;
    }

EXPORT_C void CPbkContactItem::SetDefaultEmailFieldL
        (TPbkContactItemField* aField)
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    if (aField)
        {
        aField = FindSameField(*aField);
        if (!aField)
            {
            // Not this item's field
            User::Leave(KErrNotFound);
            }
        }

    if (aField && aField->FieldInfo().FieldId() != EPbkFieldIdEmailAddress)
        {
        User::Leave(KErrNotSupported);
        }

    // 1. Fetch previous default
    TPbkContactItemField* prevDefault = DefaultEmailField();

    // 2. Set the new default if specified
    if (aField)
        {
        if (aField == prevDefault)
            {
            // Already set
            return;
            }
        aField->ItemField().AddFieldTypeL(KUidPbkDefaultFieldPref);
        }

    // 3. Unset any previous default (invariant tests that there is always just
    // one default).
    if (prevDefault)
        {
        prevDefault->ItemField().RemoveFieldType(KUidPbkDefaultFieldPref);
        }

    __ASSERT_DEBUG(
        (aField && DefaultEmailField()==aField) ||
        (!aField && !DefaultEmailField()),
        Panic(EPanicPostCond_SetDefaultEmailFieldL));
    }

EXPORT_C void CPbkContactItem::RemoveDefaultEmailField()
    {
    // This is safe because SetDefaultEmailFieldL(NULL) guarantees
    // not to leave
    SetDefaultEmailFieldL(NULL);
    }


EXPORT_C TPbkContactItemField* CPbkContactItem::DefaultEmailOverSmsField() const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    // scan the fields for a match in default field for field id.
    const TInt fieldCount = iFields.Count();
    for (TInt i = 0; i < fieldCount; ++i)
        {
        const TPbkContactItemField& field = iFields[i];
        if (field.DefaultEmailOverSmsField())
            {
            return const_cast<TPbkContactItemField*>(&field);
            }
        }
    return NULL;
    }

EXPORT_C void CPbkContactItem::SetDefaultEmailOverSmsFieldL
        (TPbkContactItemField* aField)
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    if (aField)
        {
        aField = FindSameField(*aField);
        if (!aField)
            {
            // Not this item's field
            User::Leave(KErrNotFound);
            }
        }

    if (aField && !aField->FieldInfo().IsEmailOverSmsField())
        {
        User::Leave(KErrNotSupported);
        }

    // 1. Fetch previous default
    TPbkContactItemField* prevDefault = DefaultEmailOverSmsField();

    // 2. Set the new default if specified
    if (aField)
        {
        if (aField == prevDefault)
            {
            // Already set
            return;
            }
        aField->ItemField().AddFieldTypeL(KUidPbkDefaultFieldEmailOverSms);
        }

    // 3. Unset any previous default (invariant tests that there is always just
    // one default).
    if (prevDefault)
        {
        prevDefault->ItemField().RemoveFieldType(KUidPbkDefaultFieldEmailOverSms);
        }

    __ASSERT_DEBUG(
        (aField && DefaultEmailOverSmsField()==aField) ||
        (!aField && !DefaultEmailOverSmsField()),
        Panic(EPanicPostCond_SetDefaultEmailOverSmsFieldL));
    }

EXPORT_C void CPbkContactItem::RemoveDefaultEmailOverSmsField()
    {
    // This is safe because SetDefaultEmailOverSmsFieldL(NULL) guarantees
    // not to leave
    SetDefaultEmailOverSmsFieldL(NULL);
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::DefaultMmsField() const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    // scan the fields for a match in default field for field id.
    const TInt fieldCount = iFields.Count();
    for (TInt i = 0; i < fieldCount; ++i)
        {
        const TPbkContactItemField& field = iFields[i];
        if (field.DefaultMmsField())
            {
            return const_cast<TPbkContactItemField*>(&field);
            }
        }
    return NULL;
    }

EXPORT_C void CPbkContactItem::SetDefaultMmsFieldL
        (TPbkContactItemField* aField)
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    if (aField)
        {
        aField = FindSameField(*aField);
        if (!aField)
            {
            // Not this item's field
            User::Leave(KErrNotFound);
            }
        }

    if (aField && !aField->FieldInfo().IsMmsField())
        {
        User::Leave(KErrNotSupported);
        }

    // 1. Fetch previous default
    TPbkContactItemField* prevDefault = DefaultMmsField();

    // 2. Set the new default if specified
    if (aField)
        {
        if (aField == prevDefault)
            {
            // Already set
            return;
            }
        aField->ItemField().AddFieldTypeL(KUidPbkDefaultFieldMms);
        }

    // 3. Unset any previous default (invariant tests that there is always just
    // one default).
    if (prevDefault)
        {
        prevDefault->ItemField().RemoveFieldType(KUidPbkDefaultFieldMms);
        }

    __ASSERT_DEBUG(
        (aField && DefaultMmsField()==aField) ||
        (!aField && !DefaultMmsField()),
        Panic(EPanicPostCond_SetDefaultMmsFieldL));
    }

EXPORT_C void CPbkContactItem::RemoveDefaultMmsField()
    {
    // This is safe because SetDefaultMmsFieldL(NULL) guarantees
    // not to leave
    SetDefaultMmsFieldL(NULL);
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::DefaultPocField() const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    // scan the fields for a match in default field for field id.
    const TInt fieldCount = iFields.Count();
    for (TInt i = 0; i < fieldCount; ++i)
        {
        const TPbkContactItemField& field = iFields[i];
        if (field.DefaultPocField())
            {
            return const_cast<TPbkContactItemField*>(&field);
            }
        }
    return NULL;
    }

EXPORT_C void CPbkContactItem::SetDefaultPocFieldL
        (TPbkContactItemField* aField)
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    if (aField)
        {
        aField = FindSameField(*aField);
        if (!aField)
            {
            // Not this item's field
            User::Leave(KErrNotFound);
            }
        }

    if (aField && !aField->FieldInfo().IsPocField())
        {
        User::Leave(KErrNotSupported);
        }

    // 1. Fetch previous default
    TPbkContactItemField* prevDefault = DefaultPocField();

    // 2. Set the new default if specified
    if (aField)
        {
        if (aField == prevDefault)
            {
            // Already set
            return;
            }
        aField->ItemField().AddFieldTypeL(KUidPbkDefaultFieldPoc);
        }

    // 3. Unset any previous default (invariant tests that there is always just
    // one default).
    if (prevDefault)
        {
        prevDefault->ItemField().RemoveFieldType(KUidPbkDefaultFieldPoc);
        }

    __ASSERT_DEBUG(
        (aField && DefaultPocField()==aField) ||
        (!aField && !DefaultPocField()),
        Panic(EPanicPostCond_SetDefaultPocFieldL));
    }

EXPORT_C void CPbkContactItem::RemoveDefaultPocField()
    {
    // This is safe because SetDefaultPocFieldL(NULL) guarantees
    // not to leave
    SetDefaultPocFieldL(NULL);
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::DefaultVoipField() const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    // scan the fields for a match in default field for field id.
    const TInt fieldCount = iFields.Count();
    for (TInt i = 0; i < fieldCount; ++i)
        {
        const TPbkContactItemField& field = iFields[i];
        if (field.DefaultVoipField())
            {
            return const_cast<TPbkContactItemField*>(&field);
            }
        }
    return NULL;
    }

EXPORT_C void CPbkContactItem::SetDefaultVoipFieldL
        (TPbkContactItemField* aField)
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    if (aField)
        {
        aField = FindSameField(*aField);
        if (!aField)
            {
            // Not this item's field
            User::Leave(KErrNotFound);
            }
        }

    if (aField && !aField->FieldInfo().IsVoipField())
        {
        User::Leave(KErrNotSupported);
        }

    // 1. Fetch previous default
    TPbkContactItemField* prevDefault = DefaultVoipField();

    // 2. Set the new default if specified
    if (aField)
        {
        if (aField == prevDefault)
            {
            // Already set
            return;
            }
        aField->ItemField().AddFieldTypeL(KUidPbkDefaultFieldVoip);
        }

    // 3. Unset any previous default (invariant tests that there is always just
    // one default).
    if (prevDefault)
        {
        prevDefault->ItemField().RemoveFieldType(KUidPbkDefaultFieldVoip);
        }

    __ASSERT_DEBUG(
        (aField && DefaultVoipField()==aField) ||
        (!aField && !DefaultVoipField()),
        Panic(EPanicPostCond_SetDefaultVoipFieldL));
    }

EXPORT_C void CPbkContactItem::RemoveDefaultVoipField()
    {
    // This is safe because SetDefaultVoipFieldL(NULL) guarantees
    // not to leave
    SetDefaultVoipFieldL(NULL);
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::VoiceTagField() const
    {
    TPbkContactItemField* ret = NULL;

    // Deprecated function
    if ( !FeatureManager::FeatureSupported( KFeatureIdSind ))
        {
        // scan the fields for a match in voice tag field for field id.
        const TInt fieldCount = iFields.Count();
        for (TInt i = 0; i < fieldCount; ++i)
            {
            const TPbkContactItemField& field = iFields[i];
            if (field.ItemField().ContentType().ContainsFieldType(
                KUidContactsVoiceDialField))
                {
                ret = const_cast<TPbkContactItemField*>(&field);
                break;
                }
            }
        }

    return ret;
    }

EXPORT_C void CPbkContactItem::SetVoiceTagFieldL
        (TPbkContactItemField* aField)
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;
    if ( !FeatureManager::FeatureSupported(KFeatureIdSind) )
        {

        if (aField)
            {
            aField = FindSameField(*aField);
            if (!aField)
                {
                // Not this item's field
                User::Leave(KErrNotFound);
                }
            }

        // Test can we set a voice tag for this field
        // The VoIP field completely includes the IsPhoneNumberField subset,
        // so its enough to test with IsVoipField method only.
        if (aField && !aField->FieldInfo().IsVoipField())
            {
            User::Leave(KErrNotSupported);
            }

        // 1. Fetch previous voice tag field
        TPbkContactItemField* prevTag = VoiceTagField();

        // 2. Set the new voice tag, if specified
        if (aField)
            {
            if (prevTag == aField)
                {
                // Voice tag already set to aField
                return;
                }
            // If this leaves, this object's state remains unchanged
            aField->ItemField().AddFieldTypeL(KUidContactsVoiceDialField);
            }

        // 3. Unset any previous voice tag (invariant tests that there is always
        // just one voice tag).
        if (prevTag)
            {
            // Unset previous voice tag
            prevTag->ItemField().RemoveFieldType(KUidContactsVoiceDialField);
            }

        __ASSERT_DEBUG(
            (aField && VoiceTagField()==aField) ||
            (!aField && !VoiceTagField()),
            Panic(EPanicPostCond_SetVoiceTagFieldL));
        }
    }

EXPORT_C void CPbkContactItem::RemoveVoiceTagField()
    {
    if ( !FeatureManager::FeatureSupported(KFeatureIdSind) )
        {
        // This is safe because SetVoiceTagFieldL(NULL) guarantees
        // not to leave
        SetVoiceTagFieldL(NULL);
        }
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::FindField
        (TPbkFieldId aFieldId) const
    {
    TInt index = 0;
    return FindField(aFieldId, index);
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::FindField
        (TPbkFieldId aFieldId, TInt& aIndex) const
    {
    __TEST_INVARIANT;

    const TInt fieldCount = iFields.Count();
    for (TInt i = Max(aIndex,0) ; i < fieldCount; ++i)
        {
        const TPbkContactItemField& field = iFields[i];
        if (field.FieldInfo().Match(aFieldId))
            {
            aIndex = i;
            return const_cast<TPbkContactItemField*>(&field);
            }
        }

    aIndex = KErrNotFound;
    return NULL;
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::FindField
        (const CPbkFieldInfo& aFieldInfo) const
    {
    TInt index = 0;
    return FindField(aFieldInfo, index);
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::FindField
        (const CPbkFieldInfo& aFieldInfo, TInt& aIndex) const
    {
    __TEST_INVARIANT;

    aIndex = FindFieldWithType(iFields, aFieldInfo, Max(aIndex,0));
    if (aIndex != KErrNotFound)
        {
        return const_cast<TPbkContactItemField*>(&iFields[aIndex]);
        }
    else
        {
        return NULL;
        }
    }

EXPORT_C TInt CPbkContactItem::FindFieldIndex
        (const TPbkContactItemField& aField) const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    const TInt fieldCount = iFields.Count();
    for (TInt i = 0; i < fieldCount; ++i)
        {
        if(iFields[i].IsSame(aField))
            {
            __ASSERT_DEBUG(CardFields()[i].IsSame(aField),
                Panic(EPanicPostCond_FindFieldIndex));
            return i;
            }
        }

    return KErrNotFound;
    }

EXPORT_C TInt CPbkContactItem::FindContactItemFieldIndex
        (const CContactItemField& aField) const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    const CContactItemFieldSet& fields = iItem->CardFields();
    const TInt fieldCount = fields.Count();
    for (TInt i = 0; i < fieldCount; ++i)
        {
        if (&fields[i] == &aField)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::FindNextFieldWithPhoneNumber(
        const TDesC& aNumber,
        TInt aNumberOfDigits,
        TInt& aIndex) const
    {
    __TEST_INVARIANT;

    const TInt fieldCount = iFields.Count();
    for (TInt i = Max(aIndex,0); i < fieldCount; ++i)
        {
        const TPbkContactItemField& field = iFields[i];
        if (field.FieldInfo().IsPhoneNumberField())
            {
            if (PbkEngUtils::ContainSameDigits(field.Text(), aNumber,
                aNumberOfDigits))
                {
                aIndex = i;
                return const_cast<TPbkContactItemField*>(&field);
                }
            }
        }

    aIndex = KErrNotFound;
    return NULL;
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::FindNextFieldWithText
        (const TDesC& aText,
        TInt& aIndex) const
    {
    __TEST_INVARIANT;

    const TInt fieldCount = iFields.Count();
    for (TInt i = Max(aIndex,0); i < fieldCount; ++i)
        {
        const TPbkContactItemField& field = iFields[i];
        if (field.StorageType() == KStorageTypeText)
            {
            if (field.Text().FindF(aText) >= 0)
                {
                aIndex = i;
                return const_cast<TPbkContactItemField*>(&field);
                }
            }
        }

    aIndex = KErrNotFound;
    return NULL;
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::FindSameField
        (const TPbkContactItemField& aField) const
    {
    const TInt fieldCount = iFields.Count();
    for (TInt i=0; i < fieldCount; ++i)
        {
        if (iFields[i].IsSame(aField))
            {
            return const_cast<TPbkContactItemField*>(&iFields[i]);
            }
        }
    return NULL;
    }

EXPORT_C void CPbkContactItem::RemoveField
        (TInt aIndex)
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;
    __ASSERT_DEBUG(aIndex >= 0 && aIndex < CardFields().Count(),
        Panic(EPanicPreCond_RemoveField));
    PBK_DEBUG_ONLY(TInt old_myCount = CardFields().Count());
    PBK_DEBUG_ONLY(TInt old_otherCount = iItem->CardFields().Count());

    CContactItemField* field = &(CardFields()[aIndex].ItemField());
    TInt i;
    const TInt fieldCount = iItem->CardFields().Count();
    for (i=0; i < fieldCount; ++i)
        {
        if (&iItem->CardFields()[i] == field)
            {
            break;
            }
        }
    // Field must be found
    __ASSERT_DEBUG(i < iItem->CardFields().Count(), Panic(EPanicLogic_RemoveField));
    iItem->RemoveField(i);

    iFields.Delete(aIndex);

    __ASSERT_DEBUG(CardFields().Count() == old_myCount-1,
        Panic(EPanicPostCond_RemoveField));
    __ASSERT_DEBUG(iItem->CardFields().Count() == old_otherCount-1,
        Panic(EPanicPostCond_RemoveField));
    }


EXPORT_C TPbkContactItemField& CPbkContactItem::AddFieldL
        (CPbkFieldInfo& aFieldInfo)
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;
    PBK_DEBUG_ONLY(TInt old_Count = CardFields().Count());

    // Create new contact model field with help from aFieldInfo
    CContactItemField* itemField = aFieldInfo.CreateFieldL();
    CleanupStack::PushL(itemField);

    // Create and initialize a new Phonebook field object.
    TPbkContactItemField pbkField(itemField,&aFieldInfo);

    // Try to make room to iFields for the new Phonebook field. This call makes
    // this function leave-safe, see *)
    iFields.SetReserveL(iFields.Count()+1);

    // Check that calling SetReserveL doesn't break the class invariant.
    // It shouldn't because it doesnt change iFields.Count().
    __TEST_INVARIANT;

    // Add the contact model field to iItem
    iItem->AddFieldL(*itemField);
    CleanupStack::Pop(itemField);  // itemField now owned by iItem

    // Here the invariant doesn't hold

    // Insert the new Phonebook field.
    // *) This call is guaranteed NOT to leave because we have succesfully
    // reserved space to iFields with the SetReserveL call above.
    TInt index = InsertionPos(pbkField);
    iFields.InsertL(index, pbkField);

    TPbkContactItemField& newField = iFields[index];

    //PostCond:
    __ASSERT_DEBUG(&newField.ItemField() == itemField,
        Panic(EPanicPostCond_AddFieldL));
    __ASSERT_DEBUG(newField.FieldInfo().IsSame(aFieldInfo),
        Panic(EPanicPostCond_AddFieldL));
    __ASSERT_DEBUG(iFields.Count() == old_Count+1,
        Panic(EPanicPostCond_AddFieldL));
    __ASSERT_DEBUG(index == iFields.Count()-1 ||
        iFields[index+1].Compare(iFields[index]) > 0,
        Panic(EPanicPostCond_AddFieldL));

    return newField;
    }

EXPORT_C TBool CPbkContactItem::CanAcceptDataOfType
        (CPbkFieldInfo& aFieldInfo) const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    // Search for an empty field of type aFieldInfo
    const TInt fieldCount = CardFields().Count();
    TInt countOfFieldsOfSameType = 0;
    for (TInt i=0; i < fieldCount; ++i)
        {
        const TPbkContactItemField& f = iFields[i];
        if (f.FieldInfo().IsSame(aFieldInfo))
            {
            ++countOfFieldsOfSameType;
            if (f.IsEmpty())
                {
                // Empty field of same type found
                return ETrue;
                }
            }
        }

    // Check if a new field can be added
    if (countOfFieldsOfSameType == 0 ||
         aFieldInfo.Multiplicity()==EPbkFieldMultiplicityMany)
        {
        // New field can be added
        return ETrue;
        }

    // Out of luck
    return EFalse;
    }

EXPORT_C TPbkContactItemField* CPbkContactItem::AddOrReturnUnusedFieldL
        (CPbkFieldInfo& aFieldInfo)
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    TPbkContactItemField* field = NULL;

    // Search for an empty field of type aFieldInfo
    const TInt fieldCount = CardFields().Count();
    TInt countOfFieldsOfSameType = 0;
    for (TInt i=0; i < fieldCount; ++i)
        {
        TPbkContactItemField& f = iFields[i];
        if (f.FieldInfo().IsSame(aFieldInfo))
            {
            ++countOfFieldsOfSameType;
            if (f.IsEmpty())
                {
                // Empty field of same type found
                field = &f;
                break;
                }
            }
        }

    if (!field)
        {
        // Empty field of same type was not found
        if (countOfFieldsOfSameType == 0 ||
             aFieldInfo.Multiplicity()==EPbkFieldMultiplicityMany)
            {
            // Add a new field
            field = &AddFieldL(aFieldInfo);
            }
        }

    return field;
    }

EXPORT_C void CPbkContactItem::UpdateFieldSetL
        (const CPbkFieldsInfo& aFieldsInfo)
    {
    //PreCond:
    __ASSERT_DEBUG(iItem, Panic(EPanicPreCond_UpdateFieldSetL));
    // Don't test invariant, because this function needs to be called when
    // the invariant is broken!

    // Recreate field array like in ConstructL
    CreateFieldArrayL(*iItem, aFieldsInfo);

    __TEST_INVARIANT;
    }

EXPORT_C CPbkFieldArray& CPbkContactItem::CardFields() const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;
    return const_cast<CPbkFieldArray&>(iFields);
    }


EXPORT_C TUid CPbkContactItem::Type() const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;
    return iItem->Type();
    }


EXPORT_C TContactItemId CPbkContactItem::Id() const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;
    return iItem->Id();
    }

EXPORT_C CContactIdArray* CPbkContactItem::GroupsJoinedLC() const
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;
    __ASSERT_ALWAYS(iItem->Type() == KUidContactCard,
        Panic(EPanicPreCond_GroupsJoinedLC));
    return static_cast<const CContactCard*>(iItem)->GroupsJoinedLC();
    }

TInt CPbkContactItem::PbkFieldCount() const
    {
    return iFields.Count();
    }

MPbkFieldData& CPbkContactItem::PbkFieldAt(TInt aIndex)
    {
    // PreCond: iFields will __ASSERT_ALWAYS aIndex validity
    return iFields[aIndex];
    }

const MPbkFieldData& CPbkContactItem::PbkFieldAt(TInt aIndex) const
    {
    // PreCond: iFields will __ASSERT_ALWAYS aIndex validity
    return iFields[aIndex];
    }

void CPbkContactItem::PrepareForSaveL()
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;

    // check that there is the syncronization field
    // otherwise add the syncronization field
    TPbkContactItemField* syncField = FindField(EPbkFieldIdSyncronization);
    if (!syncField)
        {
        // add the syncronization field to the contact
        CPbkContactEngine* engine = CPbkContactEngine::Static();
        if (engine)
            {
            CPbkFieldInfo* syncFieldInfo = engine->FieldsInfo().Find(
                EPbkFieldIdSyncronization);
            if (syncFieldInfo)
                {
                // add syncronization field to contact - compulsory
                AddFieldL(*syncFieldInfo);
                }
            }
        }

    for (TInt i=iFields.Count()-1; i >= 0; --i)
        {
        TPbkContactItemField& field = iFields[i];

        // check that syncronization field has correct value
        if (field.FieldInfo().FieldId() == EPbkFieldIdSyncronization)
            {
            // check that theres private, public or none text in the field
            // use the default if incorrect or no value
            const TDesC& fieldtext = field.TextStorage()->Text();
            // safely detect whether theres syncronization setting in the field
            if (!(!fieldtext.CompareF(KPbkContactSyncPrivate) ||
                !fieldtext.CompareF(KPbkContactSyncPublic) ||
                !fieldtext.CompareF(KPbkContactSyncNoSync)))
                {
                // set default sync setting to field
                field.TextStorage()->SetTextL(KPbkContactSyncPrivate);
                }
            }

        field.PrepareForSaveL();
        if (field.HasInvalidDate())
            {
            RemoveField(i);
            }
        }
    }

void CPbkContactItem::PrepareAfterLoadL()
    {
    PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT;
    for (TInt i=iFields.Count()-1; i >= 0; --i)
        {
        TPbkContactItemField& field = iFields[i];
        field.PrepareAfterLoadL();
        if (field.HasInvalidDate())
            {
            RemoveField(i);
            }
        }
    }

/**
 * Class invariant.
 */
EXPORT_C void CPbkContactItem::__DbgTestInvariant() const
    {
#ifdef _DEBUG
    // Check proper construction
    __ASSERT_DEBUG(iItem, Panic(EPanicInvariant_Pointer));

    // Check field counts
    __ASSERT_DEBUG(iItem->CardFields().Count() >= iFields.Count(),
        Panic(EPanicInvariant_Count));

    TInt phoneDefaultCount = 0;
    TInt smsDefaultCount = 0;
    TInt emailDefaultCount = 0;
    TInt voiceTagFieldCount = 0;

    CPbkContactEngine* engine = CPbkContactEngine::Static();

    // Scan all fields
    TInt i;
    const TInt fieldCount = iFields.Count();
    for (i=0; i < fieldCount; ++i)
        {
        const TPbkContactItemField& pbkField = iFields[i];

        // Check field info for all fields
        if (engine)
            {
            const CPbkFieldInfo* fieldInfo =
                engine->FieldsInfo().Find(pbkField.ItemField());
            __ASSERT_DEBUG(fieldInfo && fieldInfo->IsSame(pbkField.FieldInfo()),
                           Panic(EPanicInvariant_FieldInfo));
            }

        // Search field from iItem's fieldset
        TInt j;
        const TInt itemFieldCount = iItem->CardFields().Count();
        for (j = 0; j < itemFieldCount; ++j)
            {
            CContactItemField& field = iItem->CardFields()[j];
            // Check if field found
            if(&pbkField.ItemField() == &field)
                {
                break;
                }
            }
        // field must be found
        __ASSERT_DEBUG(j < iItem->CardFields().Count(),
            Panic(EPanicInvariant_Field));

        // Update default field counts
        TInt prefCount = 0;
        TInt smsCount = 0;
        TInt voiceTagCount = 0;
        TInt videoTagCount = 0;
        const CContentType& contentType = pbkField.ItemField().ContentType();
        for (TInt t = 0; t < contentType.FieldTypeCount(); ++t)
            {
            if (contentType.FieldType(t) == KUidPbkDefaultFieldPref)
                ++prefCount;
            else if (contentType.FieldType(t) == KUidPbkDefaultFieldSms)
                ++smsCount;
            else if (contentType.FieldType(t) == KUidContactsVoiceDialField)
                ++voiceTagCount;
            else if (contentType.FieldType(t) == KUidPbkDefaultFieldVideo)
                ++videoTagCount;
            }
        __ASSERT_DEBUG(prefCount <= 2, Panic(EPanicInvariant_PrefCount));
        __ASSERT_DEBUG(smsCount <= 1, Panic(EPanicInvariant_SmsCount));
        __ASSERT_DEBUG(prefCount < 2 || smsCount == 1,
            Panic(EPanicInvariant_PrefSmsCount));
        __ASSERT_DEBUG(voiceTagCount <= 1,
            Panic(EPanicInvariant_VoiceTagCount));
        __ASSERT_DEBUG(videoTagCount <= 1,
            Panic(EPanicInvariant_VideoTagCount));

        if (pbkField.FieldInfo().IsPhoneNumberField() && prefCount >= 1)
            {
            if (smsCount == 1)
                {
                ++smsDefaultCount;
                }
            if ((prefCount==1 && smsCount==0) || (prefCount==2 && smsCount==1))
                {
                ++phoneDefaultCount;
                }
            }
        if (pbkField.FieldInfo().FieldId() == EPbkFieldIdEmailAddress &&
            prefCount==1)
            {
            ++emailDefaultCount;
            }
        if (voiceTagCount > 0)
            {
            ++voiceTagFieldCount;
            }
        }

    // Check default counts: only one default per category allowed
    __ASSERT_DEBUG(phoneDefaultCount <= 1,
        Panic(EPanicInvariant_PhoneDefaultCount));
    __ASSERT_DEBUG(smsDefaultCount <= 1,
        Panic(EPanicInvariant_SmsDefaultCount));
    __ASSERT_DEBUG(emailDefaultCount <= 1,
        Panic(EPanicInvariant_EmailDefaultCount));
    __ASSERT_DEBUG(voiceTagFieldCount <= 1,
        Panic(EPanicInvariant_VoiceTagFieldCount));

    // Test field sorting
    for (i = 0; i < iFields.Count()-1; ++i)
        {
        __ASSERT_DEBUG(iFields[i].Compare(iFields[i+1]) <= 0,
            Panic(EPanicInvariant_Sorting));
        }
#endif
    }

void CPbkContactItem::CreateFieldArrayL
        (CContactItem& aContactItem,
        const CPbkFieldsInfo& aFieldsInfo)
    {
    const TInt fieldCount = aContactItem.CardFields().Count();
    iFields.SetReserveL(fieldCount);
    // Use Delete instead of Reset to keep the array buffer allocated above
    iFields.Delete(0, iFields.Count());
    CContactItemFieldSet& fieldSet = aContactItem.CardFields();

    // Loop through all import priority levels
    for (TPbkMatchPriorityLevel priorityLevel(aFieldsInfo.CreateMatchPriority());
        !priorityLevel.End();
        priorityLevel.Next())
        {
        // Loop through all the fields
        for (TInt i=0; i < fieldCount; ++i)
            {
            CContactItemField& field = fieldSet[i];
            CPbkFieldInfo* fieldType = aFieldsInfo.Match(field, priorityLevel);
            if (fieldType)
                {
                // Check that the field has not already been added
                const TInt fieldCount = iFields.Count();
                TInt i;
                for (i=0; i < fieldCount; ++i)
                    {
                    const TPbkContactItemField& pbkField = iFields[i];
                    if (&pbkField.ItemField()==&field ||
                        (fieldType->Multiplicity()==EPbkFieldMultiplicityOne &&
                        pbkField.FieldInfo().IsSame(*fieldType)))
                        {
                        break;
                        }
                    }
                if (i == fieldCount)
                    {
                    // Insert the field directly into presentation order
                    TPbkContactItemField pbkField(&field, fieldType);
                    iFields.InsertL(InsertionPos(pbkField), pbkField);
                    }
                }
            }
        }
    }

/**
 * Returns insertion position for aField.
 */
TInt CPbkContactItem::InsertionPos
        (const TPbkContactItemField& aField) const
    {
    // Search for an insertion pos using the "upper bound" algorithm.
    TInt first = 0;
    TInt len = iFields.Count();
    while (len > 0)
        {
        const TInt half = len / 2;
        const TInt middle = first + half;
        if (aField.Compare(iFields[middle]) < 0)
            {
            len = half;
            }
        else
            {
            first = middle + 1;
            len = len - half - 1;
            }
        }

    // If upper bound found a match, insertion pos is after that.
    if (first < iFields.Count() && aField.Compare(iFields[first]) == 0)
        {
        ++first;
        }

    // PostCond
    __ASSERT_DEBUG(first >= 0 && first <= iFields.Count(),
        Panic(EPanicPostCond_InsertionPos));
    return first;
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

EXPORT_C TBool operator==
        (const CPbkContactItem& aLeft, const CPbkContactItem& aRight)
    {
    const TInt fieldCount = aLeft.iFields.Count();
    if (aRight.iFields.Count() != fieldCount)
        {
        return EFalse;
        }

    for (TInt i=0; i < fieldCount; ++i)
        {
        if (aLeft.iFields[i] != aRight.iFields[i])
            {
            return EFalse;
            }
        }

    return ETrue;
    }

//  End of File
