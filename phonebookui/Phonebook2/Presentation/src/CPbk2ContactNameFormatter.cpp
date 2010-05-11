/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Default contact name formatter.
*
*/


#include "CPbk2ContactNameFormatter.h"
#include <CVPbkContactFieldIterator.h>
#include <MVPbkFieldType.h>
#include <CVPbkFieldTypeRefsList.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>

#include <VPbkEng.rsg>




namespace {
_LIT(KSeparator, ",");
enum TNameOrder
    {
    ETopContactOrderNumber = 0,     //TC control data, not shown
    ENameFirstPart,                 //Contact name data
    ENameSecondPart,                //Contact name data
    ENameCompanyPart                //to support Company name
    };
} // namespace

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::CPbk2ContactNameFormatter
// -----------------------------------------------------------------------------
//
CPbk2ContactNameFormatter::CPbk2ContactNameFormatter(
    const MVPbkFieldTypeList& aMasterFieldTypeList,
    const CPbk2SortOrderManager& aSortOrderManager)
    :   CPbk2ContactNameFormatterBase( aMasterFieldTypeList,
            aSortOrderManager )
    {
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::NewL
// -----------------------------------------------------------------------------
//
CPbk2ContactNameFormatter* CPbk2ContactNameFormatter::NewL
    (
    const TDesC& aUnnamedText,
    const MVPbkFieldTypeList& aMasterFieldTypeList,
    const CPbk2SortOrderManager& aSortOrderManager,
    CVPbkFieldTypeSelector* aTitleFieldSelector
    )
    {
    CPbk2ContactNameFormatter* self = new(ELeave) CPbk2ContactNameFormatter
        (aMasterFieldTypeList, aSortOrderManager);
    CleanupStack::PushL(self);
    self->ConstructL( aUnnamedText, aTitleFieldSelector );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::ConstructL
// -----------------------------------------------------------------------------
//
void CPbk2ContactNameFormatter::ConstructL
        (const TDesC& aUnnamedText,
         CVPbkFieldTypeSelector* aTitleFieldSelector )
    {
    // Base class takes ownership of aTitleFieldSelector.
    // Must not leave after BaseConstructL
    BaseConstructL( aUnnamedText, aTitleFieldSelector );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::~CPbk2ContactNameFormatter
// -----------------------------------------------------------------------------
//
CPbk2ContactNameFormatter::~CPbk2ContactNameFormatter()
    {
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::GetContactTitle
// -----------------------------------------------------------------------------
//
void CPbk2ContactNameFormatter::GetContactTitle
        (const MVPbkBaseContactFieldCollection& aContactFields,
        TDes& aTitle, TUint32 aFormattingFlags)
    {    
    aTitle.Zero();
    SetFieldMapper( aContactFields );

    // Try to create title using first two fields of the sort order
    // If the field type in the sort order is not defined as a title field
    // then it's not used for title creation.
    if ( !AreTitleFieldsEmpty( ENameFirstPart, ENameSecondPart ) )
        {
        DoGetContactTitle(aTitle, aFormattingFlags,
            ENameFirstPart, ENameSecondPart);
        }
    else
        { 
        // If there was no data in the first two fields then
        // use the third and the rest of the fields in the sort order
        // for title creation.
        // Find the next field in the sort order that has
        // data to build the title
        const TInt count = iFieldMapper.FieldCount();
        DoGetContactTitle(aTitle, aFormattingFlags,
            ENameSecondPart+1 , count-1); // zero-based
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::MaxTitleLengthWithCompanyName
// -----------------------------------------------------------------------------
//
TInt CPbk2ContactNameFormatter::MaxTitleLengthWithCompanyName(
    const MVPbkBaseContactFieldCollection& aContactFields,
    const TUint32 aFormattingFlags )
    {
    TInt result = 0;
    SetFieldMapper( aContactFields );
     
     if ( !AreTitleFieldsEmpty( ENameFirstPart, ENameCompanyPart ) )
         {
         result = DoCalculateMaxTitleLength
             (aFormattingFlags, ENameFirstPart, ENameCompanyPart);
         }
     else
         {
         // Use the rest of the fields in sort order to
         // build the title (ie. the company name)
         const TInt count = iFieldMapper.FieldCount();
         result = DoCalculateMaxTitleLength
             (aFormattingFlags, ENameCompanyPart+1, count-1); // zero-based
         }
    
     if ( !result || aFormattingFlags & MPbk2ContactNameFormatter::EDisableCompanyNameSeparator  )
         {
         return result;
         }
     return result+1; // for seperator ","
    }
// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::GetContactTitleWithCompanyNameL
// -----------------------------------------------------------------------------
//
HBufC* CPbk2ContactNameFormatter::GetContactTitleWithCompanyNameL(
        const MVPbkBaseContactFieldCollection& aContactFields,
        TUint32 aFormattingFlags )
    {
    HBufC* result = HBufC::NewL
         (MaxTitleLengthWithCompanyName(aContactFields, aFormattingFlags));
    TPtr resultPtr = result->Des();
  
    GetContactTitleWithCompanyName(aContactFields, resultPtr, aFormattingFlags);

    if (resultPtr.Length() == 0)
        {
        delete result;
        result = NULL;
        }

    if (!result)
        {
        result = UnnamedText().AllocL();
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::ContactNameFormatterExtension
// -----------------------------------------------------------------------------
//
TAny* CPbk2ContactNameFormatter::ContactNameFormatterExtension( 
        TUid aExtensionUid )
    {
    if ( aExtensionUid == MPbk2ContactNameFormatterExtension2Uid )
           {
           return static_cast<MPbk2ContactNameFormatter2*>( this );
           }
    if ( aExtensionUid == MPbk2ContactNameFormatterExtension3Uid )
           {
           return static_cast<MPbk2ContactNameFormatter3*>( this );
           }
    return NULL;
    }
// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::GetContactTitleWithCompanyName
// Note: If changing this function, please consider also changing
// MaxTitleLengthForFind
// -----------------------------------------------------------------------------
//
void CPbk2ContactNameFormatter::GetContactTitleWithCompanyName(
        const MVPbkBaseContactFieldCollection& aContactFields,
        TDes& aTitle, TUint32 aFormattingFlags)
    {
    aTitle.Zero();
    SetFieldMapper( aContactFields );

    // Try to create title using first two fields of the sort order
    // If the field type in the sort order is not defined as a title field
    // then it's not used for title creation.
    if ( !AreTitleFieldsEmpty( ENameFirstPart, ENameSecondPart ) )
        {
        DoGetContactTitle(aTitle, aFormattingFlags,
            ENameFirstPart, ENameSecondPart);
        
        //try to add company name to the end of title
        //third field is expected to be company name
        if ( IsCompanyNameField() )
            {
            if ( !( aFormattingFlags & MPbk2ContactNameFormatter::EDisableCompanyNameSeparator)  )
                {
                aTitle.Append(KSeparator);
                }
            DoGetContactTitle(aTitle, EPreserveLeadingSpaces,
                    ENameCompanyPart, ENameCompanyPart);
            }
        }
    else
        { 
        // If there was no data in the first two fields then
        // use the third and the rest of the fields in the sort order
        // for title creation.
        // Find the next field in the sort order that has
        // data to build the title
        const TInt count = iFieldMapper.FieldCount();
        DoGetContactTitle(aTitle, aFormattingFlags,
            ENameSecondPart+1 , count-1); // zero-based
        }
    }
       
// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::GetContactTitleForFind
// Note: If changing this function, please consider also changing
// MaxTitleLengthForFind
// -----------------------------------------------------------------------------
//
void CPbk2ContactNameFormatter::GetContactTitleForFind
        (const MVPbkBaseContactFieldCollection& aContactFields,
        TDes& aTitle, TUint32 aFormattingFlags)
    {
    // Simply uses GetContactTitle as the defined functionality is the same
    GetContactTitle( aContactFields, aTitle, aFormattingFlags );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::TitleWithCompanyNameFieldsLC
// -----------------------------------------------------------------------------
//
CVPbkBaseContactFieldTypeListIterator* 
CPbk2ContactNameFormatter::TitleWithCompanyNameFieldsLC(
        CVPbkFieldTypeRefsList& aFieldTypeList,
        const MVPbkBaseContactFieldCollection& aContactFields )
    {
    
    TitleFieldsL( aFieldTypeList, aContactFields, ENameFirstPart, ENameCompanyPart);

    return CVPbkBaseContactFieldTypeListIterator::NewLC(
            aFieldTypeList,
            aContactFields);
    
    }
// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::ActualTitleFieldsLC
// See function CPbk2ContactNameFormatter::GetContactTitle.
// -----------------------------------------------------------------------------
//
CVPbkBaseContactFieldTypeListIterator*
CPbk2ContactNameFormatter::ActualTitleFieldsLC(
        CVPbkFieldTypeRefsList& aFieldTypeList,
        const MVPbkBaseContactFieldCollection& aContactFields)
    {

    TitleFieldsL( aFieldTypeList, aContactFields, ENameFirstPart, ENameSecondPart);
           
    return CVPbkBaseContactFieldTypeListIterator::NewLC(
            aFieldTypeList,
            aContactFields);
    }
// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::TitleFieldsL
// -----------------------------------------------------------------------------
//	
void CPbk2ContactNameFormatter::TitleFieldsL( CVPbkFieldTypeRefsList& aFieldTypeList,
        const MVPbkBaseContactFieldCollection& aContactFields, 
        const TInt aStartingPosition, const TInt aEndingPosition )
    {
    aFieldTypeList.Reset();

    SetFieldMapper( aContactFields );
    
    DoAppendNonEmptyTitleFieldTypesL(
            aFieldTypeList,
            aStartingPosition,
            aEndingPosition);

    if( aFieldTypeList.FieldTypeCount() == 0 )
        {
        const TInt count = iFieldMapper.FieldCount();
        DoAppendNonEmptyTitleFieldTypesL(
                aFieldTypeList,
                aEndingPosition+1,
                count-1);
        }
    }
// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::MaxTitleLength
// -----------------------------------------------------------------------------
//
TInt CPbk2ContactNameFormatter::MaxTitleLength
        (const MVPbkBaseContactFieldCollection& aContactFields,
        const TUint32 aFormattingFlags)
    {
    TInt result = 0;
    SetFieldMapper( aContactFields );
    
    if ( !AreTitleFieldsEmpty( ENameFirstPart, ENameSecondPart ) )
        {
        result = DoCalculateMaxTitleLength
            (aFormattingFlags, ENameFirstPart, ENameSecondPart);
        }
    else
        {
        // Use the rest of the fields in sort order to
        // build the title (ie. the company name)
        const TInt count = iFieldMapper.FieldCount();
        result = DoCalculateMaxTitleLength
            (aFormattingFlags, ENameSecondPart+1, count-1); // zero-based
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::MaxTitleLengthForFind
// -----------------------------------------------------------------------------
//
TInt CPbk2ContactNameFormatter::MaxTitleLengthForFind
        (const MVPbkBaseContactFieldCollection& aContactFields,
        const TUint32 aFormattingFlags)
    {
    // Simply uses MaxTitleLength as the defined functionality is the same
    return MaxTitleLength( aContactFields, aFormattingFlags );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::IsFindSeparatorChar
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactNameFormatter::IsFindSeparatorChar( TChar aCh )
    {
    return aCh.IsSpace();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatter::IsCompanyNameField
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactNameFormatter::IsCompanyNameField()
    {
    // This function uses field mapper of based classes and checks only
    // third field
    TBool ret = EFalse;
    const TInt count = iFieldMapper.FieldCount();
    if (count < ENameCompanyPart + 1)
        {
        return EFalse;
        }
    const MVPbkBaseContactField* field = iFieldMapper.FieldAt(ENameCompanyPart);
     
    if (field == NULL)
        {
        return EFalse;
        }
    const MVPbkFieldType* fieldType = field->BestMatchingFieldType();
    if ( fieldType && fieldType->FieldTypeResId() == R_VPBK_FIELD_TYPE_COMPANYNAME )
        {
        const MVPbkContactFieldData& fieldData = field->FieldData();
        if (fieldData.DataType() == EVPbkFieldStorageTypeText)
            {
            const TDesC& fieldText =
                MVPbkContactFieldTextData::Cast(fieldData).Text();
            if (fieldText.Size() > 0)
                ret = ETrue;
            }
        }
    return ret;
    }

// End of File
