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
#include    "CPbkFieldsInfo.h"
#include    <barsc.h>       // RResourceFile
#include    <barsread.h>    // RResourceReader
#include    <cntitem.h>
#include    <cntmodel.rsg>

#include    <PbkEng.rsg>
#include    "CPbkFieldInfo.h"
#include    "CPbkFieldInfoGroup.h"
#include    "CPbkUidMap.h"
#include    "TPbkContactItemFieldType.h"
#include    "TPbkVcardFieldType.h"
#include    "TPbkMatchPriorityLevel.h"
#include    <MPbkGlobalSetting.h>
#include    "CPbkCenRepSetting.h"

// MODULE DATA STRUCTURES


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    {
    EPanicIndexOutOfRange = 1,
    EPanicResourceArrayCountsMismatch,
    EPanicFieldInfoArrayCountMismatch,
    EPanicResourceLinkedGroupNotFound
    };


// LOCAL FUNCTIONS

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkFieldsInfo");
    User::Panic(KPanicText, aReason);
    }

TInt CompareOrdering(const CPbkFieldInfo& aLeft, const CPbkFieldInfo& aRight)
    {
    return aLeft.CompareOrdering(aRight);
    }

/**
 * Interface that matches a CPbkFieldInfo object against some criteria.
 */
class MPbkFieldInfoMatcher
    {
    public: // Interface
        /**
         * Returns true if aFieldInfo matches some criteria.
         */
        virtual TBool Match(const CPbkFieldInfo& aFieldInfo) const = 0;
    };

/**
 * Interface for an operation to be performed to a field info object.
 */
class MPbkFieldInfoOperation
    {
    public:  // Interface
        /**
         * Performs operation for aFieldInfo.
         */
        virtual void ProcessL(const CPbkFieldInfo& aFieldInfo) =0;
    };

/**
 * Implements MPbkFieldInfoMatcher to match a TPbkFieldId
 */
NONSHARABLE_CLASS(TMatchFieldId) : 
        public MPbkFieldInfoMatcher
    {
    public:  // Interface
		/**
         * Constructor.
		 * @param aFieldId contact item field id
         */
        inline TMatchFieldId(TPbkFieldId aFieldId) : 
            iFieldId(aFieldId) 
            { 
            }

    public:  // from MPbkFieldInfoMatcher
        TBool Match(const CPbkFieldInfo& aFieldInfo) const
            {
            return (aFieldInfo.FieldId()==iFieldId);
            }

    private:  // data
		/// Own: contact item field id
        const TPbkFieldId iFieldId;
    };

/**
 * Implements MPbkFieldInfoMatcher to match a <TPbkFieldId,TPbkFieldLocation> pair.
 */
NONSHARABLE_CLASS(TMatchFieldIdAndLocation) : 
        public MPbkFieldInfoMatcher
    {
    public:  // Interface
		/**
         * Constructor.
		 * @param aFieldId contact item field id
		 * @param aLocation field location
         */
        inline TMatchFieldIdAndLocation
                (TPbkFieldId aFieldId, TPbkFieldLocation aLocation) : 
            iFieldId(aFieldId), iLocation(aLocation)
            { 
            }

    public:  // from MPbkFieldInfoMatcher
        TBool Match(const CPbkFieldInfo& aFieldInfo) const
            {
            return (aFieldInfo.FieldId()==iFieldId && aFieldInfo.Location()==iLocation);
            }

    private:  // data
		/// Own: field id
        const TPbkFieldId iFieldId;
		/// Own: field location
        const TPbkFieldLocation iLocation;
    };

/**
 * Implements MPbkFieldInfoMatcher to match a CContactItemField.
 */
NONSHARABLE_CLASS(TMatchContactItemField) : 
        public MPbkFieldInfoMatcher
    {
    public:  // Interface
		/**
         * Constructor.
		 * @param aField contact item field
		 * @param aUidMap UID map
         */
        inline TMatchContactItemField
                (const CContactItemField& aField, const CPbkUidMap& aUidMap) : 
            iFieldType(aField,aUidMap)
            { 
            }

    public:  // from MPbkFieldInfoMatcher
        TBool Match(const CPbkFieldInfo& aFieldInfo) const
            {
            return aFieldInfo.Match(iFieldType);
            }

    private:  // data
		/// Own: field type
        const TPbkContactItemFieldType iFieldType;
    };

/**
 * Implements MPbkFieldInfoMatcher to match a CContactItemField with priority.
 */
NONSHARABLE_CLASS(TMatchContactItemFieldWithLevel) : 
        public MPbkFieldInfoMatcher
    {
    public:  // Interface
		/**
         * Constructor.
		 * @param aField contact item field
		 * @param aUidMap UID map
		 * @param aLevel match priority level
         */
        inline TMatchContactItemFieldWithLevel
                (const CContactItemField& aField, 
                const CPbkUidMap& aUidMap,
                const TPbkMatchPriorityLevel& aLevel) : 
            iFieldType(aField,aUidMap), iLevel(aLevel)
            { 
            }

    public:  // from MPbkFieldInfoMatcher
        TBool Match(const CPbkFieldInfo& aFieldInfo) const
            {
            return aFieldInfo.Match(iFieldType,iLevel);
            }

    private:  // data
		/// Own: contact item field type
        const TPbkContactItemFieldType iFieldType;
		/// Own: match priority level
        const TPbkMatchPriorityLevel iLevel;
    };

/**
 * Implements MPbkFieldInfoMatcher to match a vCard property.
 */
NONSHARABLE_CLASS(TMatchVcard) : 
        public MPbkFieldInfoMatcher
    {
    public:  // Interface
		/**
         * Constructor.
		 * @param aVcardProperty
		 * @param aUidMap UID map
		 * @param aLevel match priority level
         */
        inline TMatchVcard
                (const MPbkVcardProperty& aVcardProperty,
                const CPbkUidMap& aUidMap,
                const TPbkMatchPriorityLevel& aLevel) : 
            iVcardType(aVcardProperty,aUidMap), iLevel(aLevel)
            { 
            }

    public:  // from MPbkFieldInfoMatcher
        TBool Match(const CPbkFieldInfo& aFieldInfo) const
            {
            return aFieldInfo.Match(iVcardType,iLevel);
            }

    private:  // data
		/// Own: vCard field type
        const TPbkVcardFieldType iVcardType;
		/// Own: match priority level
        const TPbkMatchPriorityLevel iLevel;
    };

/**
 * Implements MPbkFieldInfoOperation to call CPbkFieldInfo::AddToViewDefL.
 */
NONSHARABLE_CLASS(TAddToViewDef) : 
        public MPbkFieldInfoOperation
    {
    public:  // Interface
		/**
		 * Constructor.
		 * @param aViewDef view definition to contact item
		 */
        inline TAddToViewDef(CContactItemViewDef& aViewDef) :
            iViewDef(aViewDef)
            {
            }

    public:  // From MPbkFieldInfoOperation
        void ProcessL(const CPbkFieldInfo& aFieldInfo)
            {
            aFieldInfo.AddToViewDefL(iViewDef);
            }

    private:  // Data
		/// Ref: view definition
        CContactItemViewDef& iViewDef;
    };

/**
 * Implements MPbkFieldInfoOperation to call CPbkFieldInfo::AddToFieldDefL.
 */
NONSHARABLE_CLASS(TAddToFieldDef) : 
        public MPbkFieldInfoOperation
    {
    public:  // Interface
		/**
		 * Constructor.
		 * @param aFieldDef field definition for contact item
		 */
        inline TAddToFieldDef(CContactItemFieldDef& aFieldDef) :
            iFieldDef(aFieldDef)
            {
            }

    public:  // From MPbkFieldInfoOperation
        void ProcessL(const CPbkFieldInfo& aFieldInfo)
            {
            aFieldInfo.AddToFieldDefL(iFieldDef);
            }

    private:  // Data
		/// Ref: field definition
        CContactItemFieldDef& iFieldDef;
    };

/**
 * Finds a matching field info object in an array.
 * @param aFieldInfos   array to search.
 * @param aMatcher      the matching predicate.
 * @return the matching field info object or NULL if no match is found.
 */
CPbkFieldInfo* FindMatching
        (const RPointerArray<CPbkFieldInfo>& aFieldInfos,
        const MPbkFieldInfoMatcher& aMatcher)
    {
    const TInt count = aFieldInfos.Count();
    for (TInt i=0; i < count; ++i)
        {
        const CPbkFieldInfo& fieldInfo = *aFieldInfos[i];
        if (aMatcher.Match(fieldInfo))
            {
            return CONST_CAST(CPbkFieldInfo*,&fieldInfo);
            }
        }
    return NULL;
    }

/**
 * Processes matching field info objects in an array.
 *
 * @param aFieldInfos   array of field info objects to process.
 * @param aMatcher      the matching predicate.
 * @param aOperation    operation to perform to all matching field info 
 *                      objects.
 * @return number of field info objects processed
 */
void ProcessMatchingL
        (const RPointerArray<CPbkFieldInfo>& aFieldInfos,
        const MPbkFieldInfoMatcher& aMatcher,
        MPbkFieldInfoOperation& aOperation)
    {
    const TInt count = aFieldInfos.Count();
    for (TInt i=0; i < count; ++i)
        {
        const CPbkFieldInfo& fieldInfo = *aFieldInfos[i];
        if (aMatcher.Match(fieldInfo))
            {
            aOperation.ProcessL(fieldInfo);
            }
        }
    }

/**
 * Process all field info objects which match aFieldTypes.
 */
void ProcessMatchingL
        (const RPointerArray<CPbkFieldInfo>& aFieldInfos,
        const CPbkFieldIdArray& aFieldTypes,
        MPbkFieldInfoOperation& aOperation)
    {
    const TInt count = aFieldTypes.Count();
    for (TInt i=0; i<count; ++i)
        {
        TMatchFieldId fieldIdMatcher(aFieldTypes[i]);
        ProcessMatchingL(aFieldInfos,fieldIdMatcher,aOperation);
        }
    }

/**
 * Reads the needed central repository max number length value
 */
inline TInt ReadMaxNumberLengthL()
    {     
    MPbkGlobalSetting* globalSetting = CPbkCenRepSetting::NewL();
    // push to cleanupStack
    globalSetting->PushL();
    globalSetting->ConnectL(MPbkGlobalSetting::EPbkConfigurationCategory);
    TInt maxLength(0);
    TInt err = globalSetting->Get
        (MPbkGlobalSetting::EEditorMaxNumberLength, maxLength);
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( globalSetting );    
    return maxLength;
    }

}  // namespace


// ==================== MEMBER FUNCTIONS ====================

inline CPbkFieldsInfo::CPbkFieldsInfo()
    {
    // CBase::operator new(TLeave) resets members
    }

CPbkFieldsInfo* CPbkFieldsInfo::NewL
		(RResourceFile& aPbkResFile, RResourceFile& aCntModelResFile)
    {
    CPbkFieldsInfo* self = new(ELeave) CPbkFieldsInfo;
    CleanupStack::PushL(self);
    self->ConstructFromResourceL(aPbkResFile,aCntModelResFile);
    CleanupStack::Pop(self);
    return self;
    }

CPbkFieldsInfo::~CPbkFieldsInfo()
    {
    iGroups.ResetAndDestroy();
    delete iTypeUidMap;
    // Cannot use iEntries.ResetAndDestroy() because CPbkFieldInfo destructor 
    // is private
    for (TInt i=iEntries.Count()-1; i >= 0; --i)
        {
        delete iEntries[i];
        }
    iEntries.Close();
    delete iPbkFieldInfoParams;
    }

EXPORT_C CPbkFieldInfo* CPbkFieldsInfo::Find
		(TPbkFieldId aFieldId) const 
    {
    TMatchFieldId matcher(aFieldId);
    return FindMatching(iEntries,matcher);
    }

EXPORT_C CPbkFieldInfo* CPbkFieldsInfo::Find
		(TPbkFieldId aFieldId, TPbkFieldLocation aLocation) const 
    {
    TMatchFieldIdAndLocation matcher(aFieldId,aLocation);
    return FindMatching(iEntries,matcher);
    }

EXPORT_C CPbkFieldInfo* CPbkFieldsInfo::Find
		(const CContactItemField& aField) const
    {
    TMatchContactItemField matcher(aField,*iTypeUidMap);
    return FindMatching(iEntries,matcher);
    }

CPbkFieldInfo* CPbkFieldsInfo::Match
		(const CContactItemField& aField,
        const TPbkMatchPriorityLevel& aMatchPriority) const
    {
    TMatchContactItemFieldWithLevel matcher(aField,*iTypeUidMap,aMatchPriority);
    return FindMatching(iEntries,matcher);
    }

EXPORT_C CPbkFieldInfo* CPbkFieldsInfo::Match
        (const MPbkVcardProperty& aVcardProperty,
        const TPbkMatchPriorityLevel& aMatchPriority) const
    {
    TMatchVcard matcher(aVcardProperty, *iTypeUidMap, aMatchPriority);
    return FindMatching(iEntries,matcher);
    }

EXPORT_C TPbkMatchPriorityLevel CPbkFieldsInfo::CreateMatchPriority() const
    {
    return TPbkMatchPriorityLevel(iHighestMatchPriorityLevel);
    }

EXPORT_C TInt CPbkFieldsInfo::Count() const 
    { 
    return iEntries.Count(); 
    }

EXPORT_C CPbkFieldInfo* CPbkFieldsInfo::operator[](TInt aIndex) const
    {
    __ASSERT_ALWAYS(aIndex >= 0 && aIndex < iEntries.Count(), 
        Panic(EPanicIndexOutOfRange));
    return CONST_CAST(CPbkFieldInfo*,iEntries[aIndex]);
    }

EXPORT_C TInt CPbkFieldsInfo::GroupCount() const
    {
    return iGroups.Count();
    }

EXPORT_C const CPbkFieldInfoGroup& CPbkFieldsInfo::GroupAt(TInt aIndex) const
    {
    return *iGroups[aIndex];
    }

EXPORT_C CContactItemViewDef* CPbkFieldsInfo::CreateContactItemViewDefLC
        (const CPbkFieldIdArray& aFieldTypes) const
    {
    CContactItemViewDef* result = 
        CContactItemViewDef::NewLC(
            CContactItemViewDef::EIncludeFields, 
            CContactItemViewDef::EIncludeHiddenFields);
    TAddToViewDef viewDefAdder(*result);
    ProcessMatchingL(iEntries, aFieldTypes, viewDefAdder);
    return result;
    }

EXPORT_C CContactItemFieldDef* CPbkFieldsInfo::CreateContactItemFieldDefLC
        (const CPbkFieldIdArray* aFieldTypes) const
    {
    CContactItemFieldDef* result = new(ELeave) CContactItemFieldDef;
    CleanupStack::PushL(result);
    if (aFieldTypes)
        {
        TAddToFieldDef fieldDefAdder(*result);
        ProcessMatchingL(iEntries, *aFieldTypes, fieldDefAdder);
        }
    else
        {
        // No Phonebook fields specified, match all contact fields
        result->AppendL(KUidContactFieldMatchAll);
        }
    return result;
    }

void CPbkFieldsInfo::AddFieldTypesFromResourceL
        (TResourceReader& aPbkResReader, 
        TResourceReader& aCntModelResReader,
        RArray<const CPbkFieldInfo*>* aAddedFieldTypes)
    {
    // get the counts of items in the arrays
    const TInt stdCount = aCntModelResReader.ReadInt16();
    const TInt addCount = aPbkResReader.ReadInt16();
    __ASSERT_ALWAYS(stdCount==addCount, 
        Panic(EPanicResourceArrayCountsMismatch));

    // Read central repository params before the loop.
    // Otherwise there can be performance problems during device boot up.
    // Params are read to iPbkFieldInfoParams when needed.
    ReadFieldInfoParamsL();    

    // Allocate the info array
    TInt i;
    for (i=0; i < stdCount; ++i)
        {
        // Create new field info object and initialise it from resources
        CPbkFieldInfo* fieldInfo = CPbkFieldInfo::NewLC
            (aCntModelResReader, aPbkResReader, *iPbkFieldInfoParams);
        fieldInfo->UpdateTypeUidMapL(*iTypeUidMap);
        
        // Set the group of the field info object
        const TPbkFieldGroupId groupId = fieldInfo->iGroupLink.iGroupId;
        if (groupId == EPbkFieldGroupIdNone)
            {
            fieldInfo->iGroupLink.iGroup = NULL;
            }
        else
            {
            CPbkFieldInfoGroup* group = FindGroup(groupId);
            __ASSERT_ALWAYS(group, Panic(EPanicResourceLinkedGroupNotFound));
            group->AddL(*fieldInfo);
            fieldInfo->iGroupLink.iGroup = group;
            }

        // Add field info object to the array
        User::LeaveIfError(iEntries.Append(fieldInfo));
        CleanupStack::Pop(fieldInfo);
        if (aAddedFieldTypes)
            {
            User::LeaveIfError( aAddedFieldTypes->Append(fieldInfo) );
            }
        }

    // Calculate type mapping signatures and maximum level of import priorities
    const TInt allCount = iEntries.Count();
    for (i=0; i < allCount; ++i)
        {
        iEntries[i]->CalculateTypeSignatures(*iTypeUidMap);
        const TInt importPropertyCount = iEntries[i]->ImportPropertyCount();
        if (importPropertyCount > iHighestMatchPriorityLevel)
            {
            iHighestMatchPriorityLevel = importPropertyCount;
            }
        }

    // Sort the info array to specified field ordering
    iEntries.Sort(CompareOrdering);
    }

inline CPbkFieldInfoGroup* CPbkFieldsInfo::FindGroup(TPbkFieldGroupId aGroupId)
    {
    const TInt count = iGroups.Count();
    for (TInt i=0; i < count; ++i)
        {
        CPbkFieldInfoGroup* group = iGroups[i];
        if (group->Id() == aGroupId)
            {
            return group;
            }
        }
    return NULL;
    }

inline void CPbkFieldsInfo::ReadGroupsInfoL(RResourceFile& aResFile)
    {
    TResourceReader resReader;
    resReader.SetBuffer(aResFile.AllocReadLC(R_PBK_FIELD_GROUPS));
    TInt count = resReader.ReadInt16();
    while (count-- > 0)
        {
        CPbkFieldInfoGroup* group = CPbkFieldInfoGroup::NewLC(resReader);
        User::LeaveIfError(iGroups.Append(group));
        CleanupStack::Pop(group);
        }
    CleanupStack::PopAndDestroy();  // R_PBK_FIELD_GROUPS
    }

void CPbkFieldsInfo::ConstructFromResourceL
		(RResourceFile& aPbkResFile, RResourceFile& aCntModelResFile)
    {
    // Read contact model field information
    TResourceReader cntModelReader;
    cntModelReader.SetBuffer(aCntModelResFile.AllocReadLC(R_CNTUI_NEW_FIELD_DEFNS));
    // Read additional phonebook information
    TResourceReader pbkReader;
	pbkReader.SetBuffer(aPbkResFile.AllocReadLC(R_PHONEBOOK_FIELD_PROPERTIES));

    // Allocate type UID map
    iTypeUidMap = new (ELeave) CPbkUidMap;

    ReadGroupsInfoL(aPbkResFile);

    AddFieldTypesFromResourceL(pbkReader, cntModelReader, NULL);    
    CleanupStack::PopAndDestroy(2);  // R_PHONEBOOK_FIELD_PROPERTIES, R_CNTUI_NEW_FIELD_DEFNS
    }

void CPbkFieldsInfo::ReadFieldInfoParamsL()
    {    
    if (!iPbkFieldInfoParams)
        {
        iPbkFieldInfoParams = new (ELeave) CPbkFieldInfo::TPbkFieldInfoParams();
        iPbkFieldInfoParams->iEditorMaxNumberLength = ReadMaxNumberLengthL();
        }
    }

//  End of File 
