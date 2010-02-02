/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The virtual phonebook field factory
*
*/


// INCLUDES

// From VirtualPhonebook
#include "CFieldFactory.h"
#include "CFieldTypeMap.h"
#include "CFieldInfo.h"
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkFieldType.h>
#include <RLocalizedResourceFile.h>
#include <VPbkDataCaging.hrh>
#include <VPbkCntModelRes.rsg>

// System includes
#include <cntdef.hrh>
#include <cntitem.h>
#include <barsread.h>

namespace {

// CONSTANTS
_LIT(KCntExtResFile, "VPbkCntModelRes.rsc");
}

namespace VPbkCntModel {
    
TBool ContainsField( CFieldsInfo& aFieldsInfo, 
        const CContactItemField& aField )
    {
    const TInt count = aFieldsInfo.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( aFieldsInfo[i]->IsEqualType( aField ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }    
    
/**
 * Maps a Versit type to a Contact Model field.
 */
NONSHARABLE_CLASS(CFieldFactory::TFieldAndTypeTuple)
    {
    public:
        TFieldAndTypeTuple(const MVPbkFieldType& aFieldType, 
                          const CContactItemField& aNativeField);
    public:
        const MVPbkFieldType& iFieldType;
        const CContactItemField& iNativeField;
    };

CFieldFactory::TFieldAndTypeTuple::TFieldAndTypeTuple
        (const MVPbkFieldType& aFieldType, 
         const CContactItemField& aNativeField) :
    iFieldType(aFieldType),
    iNativeField(aNativeField)
    {
    }

// CFieldFactory implementation
CFieldFactory::CFieldFactory()
    {
    }

inline void CFieldFactory::ConstructL(
        const CFieldTypeMap& aFieldTypeMap, 
        const CContactItem& aSystemTemplate,
        const MVPbkFieldTypeList& aMasterFieldTypeList,
        RFs& aFs )
    {
    // open the golden template extension resource file
    // for filtering some fields that are not allowed to create.
    VPbkEngUtils::RLocalizedResourceFile resFile;
    resFile.OpenLC(aFs, KVPbkRomFileDrive, KDC_RESOURCE_FILES_DIR, 
        KCntExtResFile );
    
    // Create fields infos that are not allowed to create from the factory
    TResourceReader reader;
    reader.SetBuffer( resFile.AllocReadLC( R_CNTUI_NON_CREATABLE_FIELD_DEFNS ));
    CFieldsInfo* fieldsInfo = CFieldsInfo::NewL( reader );
    CleanupStack::PopAndDestroy(); // reader buffer
    CleanupStack::PushL( fieldsInfo );
    
    const CContactItemFieldSet& fieldSet = aSystemTemplate.CardFields();
    const TInt fieldCount = fieldSet.Count();

    for (TInt i = 0; i < fieldCount; ++i)
        {
        const CContactItemField& nativeField = fieldSet[i];
        
        if ( !ContainsField( *fieldsInfo, nativeField ) )
            {
            DoInsertMappingTupleL(aMasterFieldTypeList, 
                    aFieldTypeMap, nativeField);
            }
        }
    CleanupStack::PopAndDestroy( fieldsInfo );
    iMaxMatchPriority = aMasterFieldTypeList.MaxMatchPriority();

    // add field and type mappings that are not in the system template
    reader.SetBuffer( resFile.AllocReadLC( 
            R_CNTUI_NON_SYSTEM_TEMPLATE_FIELD_DEFNS ));
    CFieldsInfo* nonSystemTemplatefieldsInfo = CFieldsInfo::NewL( reader );
    CleanupStack::PopAndDestroy(2); // reader buffer, resFile
    CleanupStack::PushL( nonSystemTemplatefieldsInfo );

    for (TInt j = 0; j < nonSystemTemplatefieldsInfo->Count() ; ++j )
        {
        const CFieldInfo* fieldInfo = (*nonSystemTemplatefieldsInfo)[j];
        CContactItemField* field = fieldInfo->CreateFieldL();
        CleanupStack::PushL(field);

        DoInsertMappingTupleL(aMasterFieldTypeList, 
                aFieldTypeMap, *field);
        // append field to array of non system template fields        
        iNonSystemTemplateFields.AppendL(field);
        CleanupStack::Pop(field);
        }
    CleanupStack::PopAndDestroy( nonSystemTemplatefieldsInfo );
    }

CFieldFactory* CFieldFactory::NewL(
        const CFieldTypeMap& aFieldTypeMap, 
        CContactItem* aSystemTemplate, 
        const MVPbkFieldTypeList& aMasterFieldTypeList,
        RFs& aFs )
    {
    CFieldFactory* self = new(ELeave) CFieldFactory;
    CleanupStack::PushL(self);
    self->ConstructL(aFieldTypeMap, *aSystemTemplate, aMasterFieldTypeList, 
        aFs );
    CleanupStack::Pop(self);
    // Take ownership after all initialisation that can leave is done
    self->iSystemTemplate = aSystemTemplate;
    return self;
    }

CFieldFactory::~CFieldFactory()
    {
    iFieldAndTypeMapping.Close();
    iNonSystemTemplateFields.ResetAndDestroy();
    delete iSystemTemplate;
    }

CContactItemField* CFieldFactory::CreateFieldLC
        (const MVPbkFieldType& aFieldType) const
    {
    CContactItemField* result = NULL;
    const CContactItemField* field = FindField(aFieldType);
    if (field)
        {
        result = CContactItemField::NewLC(*field);
        }
    return result;
    }

const CContactItemField* CFieldFactory::FindField(const MVPbkFieldType& aFieldType) const
    {
    const CContactItemField* contactItemField = NULL;

    const TInt count = iFieldAndTypeMapping.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldAndTypeMapping[i].iFieldType.IsSame(aFieldType))
            {
            contactItemField = &iFieldAndTypeMapping[i].iNativeField;
            break;
            }
        }

    return contactItemField;
    }

TInt CFieldFactory::FieldTypeCount() const
    {
    return iFieldAndTypeMapping.Count();
    }

const MVPbkFieldType& CFieldFactory::FieldTypeAt(TInt aIndex) const
    {
    return iFieldAndTypeMapping[aIndex].iFieldType;
    }

TBool CFieldFactory::ContainsSame(const MVPbkFieldType& aFieldType) const
    {
    TBool result = EFalse;
    const TInt count = iFieldAndTypeMapping.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldAndTypeMapping[i].iFieldType.IsSame(aFieldType))
            {
            result = ETrue;
            break;
            }
        }
    return result;
    }

TInt CFieldFactory::MaxMatchPriority() const
    {
    return iMaxMatchPriority;
    }

const MVPbkFieldType* CFieldFactory::FindMatch(
        const TVPbkFieldVersitProperty& aMatchProperty,
        TInt aMatchPriority) const
    {
    const MVPbkFieldType* result = NULL;

    const TInt count = iFieldAndTypeMapping.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldAndTypeMapping[i].iFieldType.Matches(aMatchProperty, aMatchPriority))
            {
            result = &iFieldAndTypeMapping[i].iFieldType;
            break;
            }
        }
    return result;
    }

const MVPbkFieldType* CFieldFactory::FindMatch
    (TVPbkNonVersitFieldType aNonVersitType) const
    {
    const MVPbkFieldType* result = NULL;

    const TInt count = iFieldAndTypeMapping.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldAndTypeMapping[i].iFieldType.NonVersitType() == aNonVersitType)
            {
            result = &iFieldAndTypeMapping[i].iFieldType;
            }
        }
    return result;
    }

const MVPbkFieldType* CFieldFactory::Find(
        TInt aFieldTypeResId) const
    {
    const MVPbkFieldType* result = NULL;

    const TInt count = iFieldAndTypeMapping.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldAndTypeMapping[i].iFieldType.FieldTypeResId() == aFieldTypeResId)
            {
            result = &iFieldAndTypeMapping[i].iFieldType;
            break;
            }
        }
    return result;
    }

void CFieldFactory::InsertInMasterFieldTypeOrderL( TFieldAndTypeTuple& aMapping,
        const MVPbkFieldTypeList& aMasterFieldTypeList )
    {
    // Insert field type mapping in the order defined in master field type
    // list.
    TInt newTypeIndex = IndexOfTypeInMasterList( aMapping.iFieldType, 
        aMasterFieldTypeList );
    const TInt count = iFieldAndTypeMapping.Count();
    TBool mappingInserted = EFalse;
    for ( TInt i = 0; i < count && !mappingInserted; ++i )
        {
        TInt curTypeIndex = IndexOfTypeInMasterList
            ( iFieldAndTypeMapping[i].iFieldType, aMasterFieldTypeList );
        if ( newTypeIndex < curTypeIndex )
            {
            iFieldAndTypeMapping.InsertL( aMapping, i );
            mappingInserted = ETrue;
            }
        }
    
    if ( !mappingInserted )
        {
        iFieldAndTypeMapping.AppendL( aMapping );
        }
    }

TInt CFieldFactory::IndexOfTypeInMasterList( const MVPbkFieldType& aType,
        const MVPbkFieldTypeList& aMasterFieldTypeList )
    {
    const TInt count = aMasterFieldTypeList.FieldTypeCount();
    TInt result = KErrNotFound;
    for ( TInt i = 0; i < count && ( result == KErrNotFound ); ++i )
        {
        if ( aMasterFieldTypeList.FieldTypeAt( i ).IsSame( aType ) )
            {
            result =  i;
            }
        }
    return result;
    }

void CFieldFactory::DoInsertMappingTupleL(
        const MVPbkFieldTypeList& aMasterFieldTypeList,
        const CFieldTypeMap& aFieldTypeMap,
        const CContactItemField& aField)
    {
    const TInt maxMatchPriority = aMasterFieldTypeList.MaxMatchPriority();
    for (TInt matchPriority = 0; matchPriority <= maxMatchPriority; 
            ++matchPriority)
        {
        // retrieves the VPbk field type for this contact model field
        const MVPbkFieldType* genericType = aFieldTypeMap.GenericFieldType(
            aField, matchPriority);
        if (genericType)
            {
            // Field mappings must be in the correct order because this
            // class is also the supported field types of the store.
            // Field types are inserted into same order as in the master
            // field type list. This way FindMatch works correctly.
            TFieldAndTypeTuple fieldAndTypeTuple(*genericType, aField);
            InsertInMasterFieldTypeOrderL( fieldAndTypeTuple, 
                    aMasterFieldTypeList );
            break;
            }
        }
    }

}  // namespace VPbkCntModel

// end of file
