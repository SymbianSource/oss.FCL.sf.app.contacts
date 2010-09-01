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
*		Phonebook field types collection class
*
*/


// INCLUDE FILES
#include    "CPbkFieldInfo.h"
#include    <cntdb.h>       // CContactItemViewDef
#include    <cntfield.h>    // CContentType, CContactItemField
#include    <barsc.h>       // RResourceFile
#include    <barsread.h>    // RResourceReader
#include    <bldvariant.hrh>    // Feature configuration
#include    <featmgr.h>         // FeatureManager
#include    "PbkDefaults.h"
#include    "CPbkVcardProperty.h"
#include    "CPbkVcardParameters.h"
#include    "CPbkFieldImportType.h"
#include    "CPbkUidMap.h"
#include    "TPbkMatchPriorityLevel.h"
#include    "CPbkCenRepSetting.h"


/// Unnamed namespace for local definitions
namespace {

// LOCAL FUNCTIONS
inline void UpdateUidMapL
        (CPbkUidMap& aUidMap, TStorageType aStorageType)
    {
    aUidMap.InsertL(TUid::Uid(aStorageType));
    }

inline void UpdateUidMapL
        (CPbkUidMap& aUidMap, const CContentType& aContentType)
    {
    aUidMap.InsertL(aContentType.Mapping());
    const TInt count = aContentType.FieldTypeCount();
    for (TInt i=0; i < count; ++i)
        {
        aUidMap.InsertL(aContentType.FieldType(i));
        }
    }
}


// ==================== MEMBER FUNCTIONS ====================

/**
 * C++ constructor
 */
inline CPbkFieldInfo::CPbkFieldInfo()
    {
    // CBase::operator new(TLeave) resets member data
    }

/**
 * Creates a new instance of this class. The created object is left on
 * the cleanup stack.
 *
 * @param aReaderStd resource reader initialised to the start of a 
 *                   STRUCT FIELD.
 * @param aReaderAdd resource reader initialised to the start of a 
 *                   STRUCT PHONEBOOK_FIELD.
 * @param aPbkFieldInfoParams parameters for initialisation
 * @return new instance of this class.
 */
CPbkFieldInfo* CPbkFieldInfo::NewLC(TResourceReader& aReaderStd, 
        TResourceReader& aReaderAdd, TPbkFieldInfoParams& aPbkFieldInfoParams)
    {
    CPbkFieldInfo* self = new(ELeave) CPbkFieldInfo;
    CleanupStack::PushL(self);
    self->ConstructFromResourceL(aReaderStd, aReaderAdd, aPbkFieldInfoParams);
    return self;
    }

/**
 * Destructor.
 */
CPbkFieldInfo::~CPbkFieldInfo()
    {
    delete iContentType;
    delete iFieldName;
    delete iAddItemText;
    delete iImportType;
    }    

EXPORT_C TPbkFieldId CPbkFieldInfo::FieldId() const 
    { 
    return iFieldId; 
    }

EXPORT_C TStorageType CPbkFieldInfo::FieldStorageType() const 
    { 
    return iFieldStorageType; 
    }

EXPORT_C const CContentType& CPbkFieldInfo::ContentType() const 
    { 
    return *iContentType; 
    }

EXPORT_C TInt CPbkFieldInfo::Category() const 
    { 
    return iCategory; 
    }

EXPORT_C const TDesC& CPbkFieldInfo::FieldName() const 
    {
    if (iFieldName)
        {
        return *iFieldName;
        }
    else 
        {
        return KNullDesC;
        }
    }

EXPORT_C TUint CPbkFieldInfo::Flags() const
    {
    return iFlags;
    }

EXPORT_C TPbkFieldMultiplicity CPbkFieldInfo::Multiplicity() const
    {
    return static_cast<TPbkFieldMultiplicity>(iMultiplicity);
    }

EXPORT_C TInt CPbkFieldInfo::MaxLength() const
    {
    return iMaxLength;
    }

EXPORT_C TPbkFieldEditMode CPbkFieldInfo::EditMode() const
    {
    return static_cast<TPbkFieldEditMode>(iEditMode);
    }

EXPORT_C TPbkFieldDefaultCase CPbkFieldInfo::DefaultCase() const
    {
    return static_cast<TPbkFieldDefaultCase>(iDefaultCase);
    }

EXPORT_C TPbkIconId CPbkFieldInfo::IconId() const
    {
    return static_cast<TPbkIconId>(iIconId);
    }

EXPORT_C TPbkFieldCtrlType CPbkFieldInfo::CtrlType() const
    {
    return static_cast<TPbkFieldCtrlType>(iCtrlType);
    }

EXPORT_C TUint CPbkFieldInfo::AddFlags() const
    {
    return iAddFlags;
    }

EXPORT_C TPbkAddItemOrdering CPbkFieldInfo::AddItemOrdering() const
    {
    return iAddItemOrdering;
    }

EXPORT_C const TDesC& CPbkFieldInfo::AddItemText() const
    {
    if (iAddItemText)
        {
        return *iAddItemText;
        }
    else
        {
        return FieldName();
        }
    }



EXPORT_C TBool CPbkFieldInfo::IsHidden() const
    {
    return (iFlags & EContactFieldFlagHidden);
    }

EXPORT_C TBool CPbkFieldInfo::IsReadOnly() const
    {
    return (iFlags & EContactFieldFlagReadOnly);
    }

EXPORT_C TBool CPbkFieldInfo::DoSynchronize() const
    {
    return (iFlags & EContactFieldFlagSynchronize);
    }

EXPORT_C TBool CPbkFieldInfo::IsDisabled() const
    {
    return (iFlags & EContactFieldFlagDisabled);
    }

EXPORT_C TBool CPbkFieldInfo::IsSame(const CPbkFieldInfo& aFieldInfo) const
    {
    return (iFieldId==aFieldInfo.iFieldId && iLocation==aFieldInfo.iLocation);
    }

EXPORT_C TBool CPbkFieldInfo::Match(TPbkFieldId aFieldId) const
    {
    return iFieldId==aFieldId;
    }

TBool CPbkFieldInfo::Match(const TPbkContactItemFieldType& aFieldType) const
    {
    return iImportType->Match(aFieldType);
    }

TBool CPbkFieldInfo::Match
        (const TPbkContactItemFieldType& aFieldType,
        const TPbkMatchPriorityLevel& aMatchPriority) const
    {
    return iImportType->Match(aFieldType,aMatchPriority);
    }

TBool CPbkFieldInfo::Match
        (const TPbkVcardFieldType& aVcardType,
        const TPbkMatchPriorityLevel& aMatchPriority) const
    {
    return iImportType->Match(aVcardType,aMatchPriority);
    }

// deprecated
EXPORT_C TBool CPbkFieldInfo::Match(const CContactItemField& aField) const
    {
    // Storage type and mapping must match exactly. 
    if (aField.StorageType() != iFieldStorageType ||
        aField.ContentType().Mapping() != iContentType->Mapping())
        {
        return EFalse;
        }

    // all types in iContentType must be found from aField.ContentType()
    const TInt count = iContentType->FieldTypeCount();
    for (TInt i = 0; i < count; ++i)
        {
        if (!aField.ContentType().ContainsFieldType(
                iContentType->FieldType(i)))
            {
            return EFalse;
            }
        }    
    
    return ETrue;
    }

EXPORT_C TInt CPbkFieldInfo::CompareOrdering(const CPbkFieldInfo& aOther) const
    {
    TInt diff = this->iOrderingItem - aOther.iOrderingItem;        
    return diff;
    }

EXPORT_C TBool CPbkFieldInfo::NameField() const
    {
    return iFieldId==EPbkFieldIdLastName || iFieldId==EPbkFieldIdFirstName;
    }

EXPORT_C TBool CPbkFieldInfo::TemplateField() const
    {
    return (iAddFlags & KPbkFieldFlagDefTemplate);
    }

EXPORT_C TBool CPbkFieldInfo::UserCanAddField() const
    {
    return (iAddFlags & KPbkFieldFlagUserCanAddField);
    }

EXPORT_C TBool CPbkFieldInfo::IsEditable() const
    {
    return !(iAddFlags & KPbkFieldFlagDisableEdit);
    }

EXPORT_C TBool CPbkFieldInfo::NumericField() const
    {
    return (iEditMode==EPbkFieldEditModeNumeric);
    }

EXPORT_C TBool CPbkFieldInfo::IsPhoneNumberField() const
    {
    switch (iFieldId)
        {
        case EPbkFieldIdPhoneNumberStandard:    // FALLTHROUGH
        case EPbkFieldIdPhoneNumberHome:        // FALLTHROUGH
        case EPbkFieldIdPhoneNumberWork:        // FALLTHROUGH
        case EPbkFieldIdPhoneNumberMobile:      // FALLTHROUGH
        case EPbkFieldIdPhoneNumberVideo:       // FALLTHROUGH
        case EPbkFieldIdFaxNumber:              // FALLTHROUGH
        case EPbkFieldIdPagerNumber:            // FALLTHROUGH
        case EPbkFieldIdAssistantNumber:        // FALLTHROUGH
        case EPbkFieldIdCarNumber:
            {
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
	}

EXPORT_C TBool CPbkFieldInfo::IsEmailOverSmsField() const
    {
    switch (iFieldId)
        {
        case EPbkFieldIdPhoneNumberStandard:    // FALLTHROUGH
        case EPbkFieldIdPhoneNumberHome:        // FALLTHROUGH
        case EPbkFieldIdPhoneNumberWork:        // FALLTHROUGH
        case EPbkFieldIdPhoneNumberMobile:      // FALLTHROUGH
        case EPbkFieldIdPhoneNumberVideo:       // FALLTHROUGH
        case EPbkFieldIdEmailAddress:           // FALLTHROUGH
        case EPbkFieldIdAssistantNumber:        // FALLTHROUGH
        case EPbkFieldIdCarNumber:
            {
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
    }

EXPORT_C TBool CPbkFieldInfo::IsEmailField() const
    {
    if (iFieldId == EPbkFieldIdEmailAddress)
        {
        return ETrue;
        }
    return EFalse;
    }

EXPORT_C TBool CPbkFieldInfo::IsMmsField() const
    {
    switch (iFieldId)
        {
        case EPbkFieldIdPhoneNumberStandard:    // FALLTHROUGH
        case EPbkFieldIdPhoneNumberHome:        // FALLTHROUGH
        case EPbkFieldIdPhoneNumberWork:        // FALLTHROUGH
        case EPbkFieldIdPhoneNumberMobile:      // FALLTHROUGH
        case EPbkFieldIdPhoneNumberVideo:       // FALLTHROUGH
        case EPbkFieldIdEmailAddress:           // FALLTHROUGH
        case EPbkFieldIdAssistantNumber:        // FALLTHROUGH
        case EPbkFieldIdCarNumber:
            {
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
    }


EXPORT_C TBool CPbkFieldInfo::IsImageField() const
    {
    switch (iFieldId)
        {
        case EPbkFieldIdPicture:    // FALLTHROUGH
        case EPbkFieldIdThumbnailImage:
            {
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
    }


EXPORT_C TBool CPbkFieldInfo::IsPocField() const
    {
    switch (iFieldId)
        {
        case EPbkFieldIdPhoneNumberStandard:    // FALLTHROUGH
        case EPbkFieldIdPhoneNumberHome:        // FALLTHROUGH
        case EPbkFieldIdPhoneNumberWork:        // FALLTHROUGH
        case EPbkFieldIdPhoneNumberMobile:      // FALLTHROUGH
        case EPbkFieldIdPhoneNumberVideo:       // FALLTHROUGH
        case EPbkFieldIdFaxNumber:              // FALLTHROUGH
        case EPbkFieldIdPagerNumber:            // FALLTHROUGH
		case EPbkFieldIdVOIP:                   // FALLTHROUGH
		case EPbkFieldIdPushToTalk:             // FALLTHROUGH
        case EPbkFieldIdShareView:              // FALLTHROUGH
        case EPbkFieldIdSIPID:                  // FALLTHROUGH
        case EPbkFieldIdAssistantNumber:        // FALLTHROUGH
        case EPbkFieldIdCarNumber:
            {
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
    }
    
EXPORT_C TBool CPbkFieldInfo::IsVoipField() const
    {
    switch (iFieldId)
        {        
        case EPbkFieldIdPhoneNumberStandard:    // FALLTHROUGH
        case EPbkFieldIdPhoneNumberHome:        // FALLTHROUGH
        case EPbkFieldIdPhoneNumberWork:        // FALLTHROUGH
        case EPbkFieldIdPhoneNumberMobile:      // FALLTHROUGH
        case EPbkFieldIdPhoneNumberVideo:       // FALLTHROUGH
        case EPbkFieldIdFaxNumber:              // FALLTHROUGH
        case EPbkFieldIdPagerNumber:            // FALLTHROUGH
		case EPbkFieldIdVOIP:                   // FALLTHROUGH
		case EPbkFieldIdPushToTalk:             // FALLTHROUGH
        case EPbkFieldIdShareView:              // FALLTHROUGH
        case EPbkFieldIdSIPID:                  // FALLTHROUGH
        case EPbkFieldIdAssistantNumber:        // FALLTHROUGH
        case EPbkFieldIdCarNumber:
            {
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
    }

EXPORT_C TBool CPbkFieldInfo::IsReadingField() const
    {
    switch (iFieldId)
        {
        case EPbkFieldIdLastNameReading:        // FALLTHROUGH
        case EPbkFieldIdFirstNameReading:
            {
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
    }


EXPORT_C CContactItemField* CPbkFieldInfo::CreateFieldL() const
    {
    CContactItemField* field = CContactItemField::NewL
        (iFieldStorageType, *iContentType);
    CleanupStack::PushL(field);
	field->SetUserFlags(iCategory);
    if (iFieldName)
        {
	    field->SetLabelL(*iFieldName);
        }
	if (iFlags&EContactFieldFlagHidden)
        {
		field->SetHidden(ETrue);
        }
	if (iFlags&EContactFieldFlagReadOnly)
        {
		field->SetReadOnly(ETrue);
        }
	if (iFlags&EContactFieldFlagSynchronize)
        {
		field->SetSynchronize(ETrue);
        }
	if (iFlags&EContactFieldFlagDisabled)
        {
		field->SetDisabled(ETrue);
        }
    CleanupStack::Pop(field);
    return field;
    }

/**
 * Returns ETrue if aField's flags are equal with this field info object.
 */
inline TBool CPbkFieldInfo::IsEqualFlags(const CContactItemField& aField) const
    {
    TUint flags = 0;
    if (aField.IsHidden())
		{
		flags |= EContactFieldFlagHidden;
		}
    if (aField.IsReadOnly())
		{
		flags |= EContactFieldFlagReadOnly;
		}
    // Do not check synchronize flag as ctnmodel modifies it
    if (aField.IsDisabled())
		{
		flags |= EContactFieldFlagDisabled;
		}
    return ((flags & iFlags) == flags);
    }

/**
 * Returns ETrue if aField's label is equal with this field info object.
 */
TBool CPbkFieldInfo::IsEqualLabel(const CContactItemField& aField) const
    {
    return
        ((!iFieldName && aField.Label().Length()==0) ||
        (iFieldName && aField.Label()==*iFieldName));
    }

/**
 * Replacement for Contact Model's buggy CContentType::operator==.
 */
/* original mods
TBool IsEqualContentType(const CContentType& aLhs, const CContentType& aRhs)
    {
    // Compare vCard name
    if (aLhs.Mapping() != aRhs.Mapping())
        {
        return EFalse;
        }

    // Check counts
    const TInt typeCount = aLhs.FieldTypeCount();
    if (typeCount != aRhs.FieldTypeCount())
        {
        return EFalse;
        }
    if (typeCount <= 0)
        {
        // Nothing more to compare
        return ETrue;
        }

    // Compare main Contact Model type
    if (aLhs.FieldType(0) != aRhs.FieldType(0))
        {
        return EFalse;
        }

    // Compare rest of the type params as a set
    for (TInt lhsIndex = 1; lhsIndex < typeCount; ++lhsIndex)
        {
        TInt rhsIndex = 1;
        for (rhsIndex = 1; rhsIndex < typeCount; ++rhsIndex)
            {
            if (aLhs.FieldType(lhsIndex) == aRhs.FieldType(rhsIndex))
                break;
            }
        if (rhsIndex == typeCount)
            {
            // No match found
            return EFalse;
            }
        }

    return ETrue;
    }
*/
TBool IsEqualContentType(const CContentType& aLhs, const CContentType& aRhs)
    {
    // vCard mapping must match excactly
    if (aLhs.Mapping() != aRhs.Mapping())
        {
        return EFalse;
        }

    // Check counts
    const TInt typeCount = aLhs.FieldTypeCount();
    if (typeCount != aRhs.FieldTypeCount())
        {
        // Type counts must match excactly
	  return EFalse;
        }

    // Compare rest of the type ids as a set
    for (TInt lhsIndex = 0; lhsIndex < typeCount; ++lhsIndex)
        {
        TInt rhsIndex = 0;
        for (; rhsIndex < typeCount; ++rhsIndex)
            {
            if (aLhs.FieldType(lhsIndex) == aRhs.FieldType(rhsIndex))
                break;
            }
        if (rhsIndex == typeCount)
            {
            // No match found
            return EFalse;
            }
        }

    return ETrue;
    }

TBool CPbkFieldInfo::IsEqualType(const CContactItemField& aField) const
    {
    return
        (iFieldStorageType == aField.StorageType() &&
        IsEqualContentType(*iContentType, aField.ContentType()) &&
        TUint(iCategory) == aField.UserFlags() &&
        IsEqualFlags(aField));
    }

EXPORT_C TBool CPbkFieldInfo::IsEqualTo(const CContactItemField& aField) const
    {
    return (IsEqualType(aField) && IsEqualLabel(aField));
    }

EXPORT_C const CPbkFieldInfoGroup* CPbkFieldInfo::Group() const
    {
    return iGroupLink.iGroup;
    }

EXPORT_C TPbkFieldLocation CPbkFieldInfo::Location() const
    {
    return static_cast<TPbkFieldLocation>(iLocation);
    }

EXPORT_C TPbkVersitStorageType CPbkFieldInfo::VersitStorageType() const
    {
    return static_cast<TPbkVersitStorageType>(iVersitStorageType);
    }

EXPORT_C void CPbkFieldInfo::AddToViewDefL(CContactItemViewDef& aViewDef) const
    {
    const TFieldType mapping = iContentType->Mapping();
    if (aViewDef.Find(mapping) == KErrNotFound)
        {
        aViewDef.AddL(mapping);
        }

    const TInt count = iContentType->FieldTypeCount();
    for (TInt i=0; i<count; ++i)
        {
        const TFieldType fieldType = iContentType->FieldType(i);
        if (aViewDef.Find(fieldType) == KErrNotFound)
            {
            aViewDef.AddL(fieldType);
            }
        }
    }

EXPORT_C void CPbkFieldInfo::AddToFieldDefL
        (CContactItemFieldDef& aFieldDef) const
    {
    TKeyArrayFix findKey(_FOFF(TUid,iUid), ECmpTInt32);
    TInt index;

    // Add all type UIDs in content type
    const TFieldType fieldType = iContentType->Mapping();
    if (aFieldDef.Find(fieldType,findKey,index) != 0)
        {
        aFieldDef.AppendL(fieldType);
        }

    const TInt count = iContentType->FieldTypeCount();
    for (TInt i=0; i<count; ++i)
        {
        const TFieldType fieldType = iContentType->FieldType(i);
        if (aFieldDef.Find(fieldType,findKey,index) != 0)
            {
            aFieldDef.AppendL(fieldType);
            }
        }
    }

inline void CPbkFieldInfo::ReadCntModelFieldsL(TResourceReader& aReaderStd)
    {
    iFieldStorageType = aReaderStd.ReadInt32();
    iContentType = CContentType::NewL();
	const TUid contactFieldUid = TUid::Uid(aReaderStd.ReadInt32());
	iContentType->SetMapping(TUid::Uid(aReaderStd.ReadInt32()));
	if (contactFieldUid != KUidContactFieldNone)
        {
	    iContentType->AddFieldTypeL(contactFieldUid);
        }
    TInt count = aReaderStd.ReadInt16();
	while (--count >= 0)
        {
		iContentType->AddFieldTypeL(TUid::Uid(aReaderStd.ReadInt32()));
        }
    iCategory = aReaderStd.ReadInt32();
    iFieldName = aReaderStd.ReadHBufCL();
    iFlags = aReaderStd.ReadInt32();
    }

inline void CPbkFieldInfo::ReadAdditionalFieldsL(TResourceReader& aReaderAdd,
    TPbkFieldInfoParams& aPbkFieldInfoParams)
    {
    iFieldId            = aReaderAdd.ReadInt8();
    iMultiplicity       = aReaderAdd.ReadInt8();
    iMaxLength          = aReaderAdd.ReadInt16();
    iEditMode           = aReaderAdd.ReadInt8();
    iDefaultCase        = aReaderAdd.ReadInt8();
    iIconId             = aReaderAdd.ReadInt8();
    iCtrlType           = aReaderAdd.ReadInt8();
    iAddFlags           = aReaderAdd.ReadUint32();
    iOrderingGroup      = aReaderAdd.ReadInt8();
    iOrderingItem       = aReaderAdd.ReadInt8();
    iAddItemOrdering    = aReaderAdd.ReadInt8();
    iAddItemText        = aReaderAdd.ReadHBufCL();
    iLocation           = aReaderAdd.ReadInt8();
    iVersitStorageType  = aReaderAdd.ReadInt8();
    
    if ( IsPhoneNumberField() )
        {
        // If needed use central repository value
        iMaxLength = aPbkFieldInfoParams.iEditorMaxNumberLength;
        }

    }

/**
 * Second phase constructor. Initializes this field info object
 * from resources.
 */
void CPbkFieldInfo::ConstructFromResourceL
        (TResourceReader& aReaderStd, TResourceReader& aReaderAdd, 
        TPbkFieldInfoParams& aPbkFieldInfoParams)
    {
    ReadCntModelFieldsL(aReaderStd);
    ReadAdditionalFieldsL(aReaderAdd, aPbkFieldInfoParams);
    iImportType = CPbkFieldImportType::NewL(aReaderAdd);
    iGroupLink.iGroupId = TPbkFieldGroupId(aReaderAdd.ReadInt8());
    }

TInt CPbkFieldInfo::ImportPropertyCount() const
    {
    return iImportType->ImportPropertyCount();
    }

void CPbkFieldInfo::UpdateTypeUidMapL(CPbkUidMap& aTypeUidMap) const
    {
    UpdateUidMapL(aTypeUidMap, iFieldStorageType);
    UpdateUidMapL(aTypeUidMap, *iContentType);
    iImportType->UpdateTypeUidMapL(aTypeUidMap);
    }

void CPbkFieldInfo::CalculateTypeSignatures(const CPbkUidMap& aTypeUidMap)
    {
    iImportType->CalculateSignatures(*this, aTypeUidMap);
    }

// Remove these exports from the EABI platform
#ifndef __EABI__

#ifdef NDEBUG
    // Export empty functions in release build to keep same exports in debug 
    //and release builds.
    EXPORT_C void __DebugPrint(const CPbkFieldInfo& /*aFi*/)        {/*empty*/}
    EXPORT_C void __DebugPrint(const CContactItemField& /*aCif*/)   {/*empty*/}
#else
    /**
     * Prints aFi content type to debugger log.
     */
    EXPORT_C void __DebugPrint(const CPbkFieldInfo& aFi)
        {
        _LIT(KFormat, "CPbkFieldInfo(0x%08x): name=\"%S\", \
            id=0x%x, storageType=0x%08x, category=%d, flags=%x, addFlags=%x");
        RDebug::Print(KFormat, &aFi, &aFi.FieldName(), 
            aFi.FieldId(), aFi.FieldStorageType(), aFi.Category(), aFi.Flags(),
            aFi.AddFlags());
        TBuf<256> buf;
        buf.Format(_L("    ContentType: mapping=0x%08x"), 
            aFi.ContentType().Mapping().iUid);
        for (TInt i=0; i<aFi.ContentType().FieldTypeCount(); ++i)
            {
            buf.AppendFormat(_L(", 0x%08x"), aFi.ContentType().FieldType(i));
            }
        RDebug::Print(buf);
        }

    /**
     * Prints aCif content type to debugger log
     */
    EXPORT_C void __DebugPrint(const CContactItemField& aCif)
        {
        _LIT(KFormat, "CContactItemField(0x%08x): name=\"%S\", \
            storageType=0x%08x, category=%d");
        TPtrC name = aCif.Label();
        RDebug::Print(KFormat, &aCif, &name, aCif.StorageType(),
            aCif.UserFlags());
        TBuf<256> buf;
        buf.Format(_L("    ContentType: mapping=0x%08x, types="), 
            aCif.ContentType().Mapping().iUid);
        for (TInt i=0; i<aCif.ContentType().FieldTypeCount(); ++i)
            {
            buf.AppendFormat(_L("0x%08x "), aCif.ContentType().FieldType(i));
            }
        RDebug::Print(buf);
        }
#endif  // #else NDEBUG
#endif  // #ifndef EABI

//  End of File
