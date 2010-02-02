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
*     Japanese contact name formatter.
*
*/


#include "CPbk2JapaneseContactNameFormatter.h"
#include <CVPbkContactFieldIterator.h>
#include <MVPbkFieldType.h>
#include <CVPbkFieldTypeRefsList.h>

namespace {

enum TNameOrder
    {
    ETopContactOrderNumber = 0,
    EReadingFirstPart,
    EReadingSecondPart,
    ENameFirstPart,
    ENameSecondPart
    };
} // namespace

// -----------------------------------------------------------------------------
// CPbk2JapaneseContactNameFormatter::CPbk2JapaneseContactNameFormatter
// -----------------------------------------------------------------------------
//
CPbk2JapaneseContactNameFormatter::CPbk2JapaneseContactNameFormatter(
    const MVPbkFieldTypeList& aMasterFieldTypeList,
    const CPbk2SortOrderManager& aSortOrderManager)
    :   CPbk2ContactNameFormatterBase( aMasterFieldTypeList,
            aSortOrderManager )
    {
    }

// -----------------------------------------------------------------------------
// CPbk2JapaneseContactNameFormatter::NewL
// -----------------------------------------------------------------------------
//
CPbk2JapaneseContactNameFormatter* CPbk2JapaneseContactNameFormatter::NewL
    (
    const MVPbkFieldTypeList& aMasterFieldTypeList,
    const CPbk2SortOrderManager& aSortOrderManager,
    CVPbkFieldTypeSelector* aTitleFieldSelector,
    const TDesC& aUnnamedText
    )
    {
    CPbk2JapaneseContactNameFormatter* self = 
        new(ELeave) CPbk2JapaneseContactNameFormatter(
            aMasterFieldTypeList, aSortOrderManager );
    CleanupStack::PushL(self);
    self->ConstructL( aUnnamedText, aTitleFieldSelector );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CPbk2JapaneseContactNameFormatter::ConstructL
// -----------------------------------------------------------------------------
//
inline void CPbk2JapaneseContactNameFormatter::ConstructL
        (const TDesC& aUnnamedText,
         CVPbkFieldTypeSelector* aTitleFieldSelector )
    {
    // Base class takes ownership of aTitleFieldSelector.
    // Must not leave after BaseConstructL
    BaseConstructL( aUnnamedText, aTitleFieldSelector );
    }

// -----------------------------------------------------------------------------
// CPbk2JapaneseContactNameFormatter::~CPbk2JapaneseContactNameFormatter
// -----------------------------------------------------------------------------
//
CPbk2JapaneseContactNameFormatter::~CPbk2JapaneseContactNameFormatter()
    {
    }

// -----------------------------------------------------------------------------
// CPbk2JapaneseContactNameFormatter::GetContactTitle
// -----------------------------------------------------------------------------
//
void CPbk2JapaneseContactNameFormatter::GetContactTitle
        (const MVPbkBaseContactFieldCollection& aContactFields,
        TDes& aTitle, TUint32 aFormattingFlags)
    {
    aTitle.Zero();
    SetFieldMapper( aContactFields );

    // Try to create title using last/first name.
    DoGetContactTitle(aTitle, aFormattingFlags,
        ENameFirstPart, ENameSecondPart );

    // If no last/first name, use reading fields
    if ( aTitle.Length() == 0 && 
         AreTitleFieldsEmpty( ENameFirstPart, ENameSecondPart ) )
        {
        DoGetContactTitle(aTitle, aFormattingFlags,
            EReadingFirstPart, EReadingSecondPart );
        }

    // If there there was no data use rest of the fields
    if ( aTitle.Length() == 0 &&
         AreTitleFieldsEmpty( EReadingFirstPart, EReadingSecondPart ) )
        {
        // Find the next field in the sort order that has
        // data to build the title
        const TInt count = iFieldMapper.FieldCount();
        DoGetContactTitle( aTitle, aFormattingFlags,
            ENameSecondPart+1 , count-1 ); // zero-based
        }
    }

// -----------------------------------------------------------------------------
// CPbk2JapaneseContactNameFormatter::GetContactTitleForFind
// -----------------------------------------------------------------------------
//
void CPbk2JapaneseContactNameFormatter::GetContactTitleForFind
        (const MVPbkBaseContactFieldCollection& aContactFields,
        TDes& aTitle, TUint32 aFormattingFlags)
    {
    aTitle.Zero();
    SetFieldMapper( aContactFields );

    // Try to use the name fields "reading first part", "reading second part", 
    // "first part" and "second part"
    DoGetContactTitle( aTitle, aFormattingFlags,
        EReadingFirstPart, ENameSecondPart );

    // If there there was no data in the name fields then
    // use the rest of the fields in the sort order for the find
    if ( aTitle.Length() == 0 &&
         AreTitleFieldsEmpty( EReadingFirstPart, ENameSecondPart ) )
        {
        // Find the next title field in the sort order that has
        // data to build the title
        const TInt count = iFieldMapper.FieldCount();
        DoGetContactTitle( aTitle, aFormattingFlags,
            ENameSecondPart+1 , count-1 ); // zero-based
        }
    }

// -----------------------------------------------------------------------------
// CPbk2JapaneseContactNameFormatter::ActualTitleFieldsLC
// See CPbk2JapaneseContactNameFormatter::GetContactTitle.
// -----------------------------------------------------------------------------
//
CVPbkBaseContactFieldTypeListIterator*
CPbk2JapaneseContactNameFormatter::ActualTitleFieldsLC(
        CVPbkFieldTypeRefsList& aFieldTypeList,
        const MVPbkBaseContactFieldCollection& aContactFields)
    {
    aFieldTypeList.Reset();

    SetFieldMapper( aContactFields );

    DoAppendNonEmptyTitleFieldTypesL(
            aFieldTypeList,
            ENameFirstPart,
            ENameSecondPart);

    if( aFieldTypeList.FieldTypeCount() == 0 )
        {
        DoAppendNonEmptyTitleFieldTypesL(
                aFieldTypeList,
                EReadingFirstPart,
                EReadingSecondPart);
        }

    if( aFieldTypeList.FieldTypeCount() == 0 )
        {
        const TInt count = iFieldMapper.FieldCount();
        DoAppendNonEmptyTitleFieldTypesL(
                aFieldTypeList,
                ENameSecondPart+1,
                count-1);
        }

    return CVPbkBaseContactFieldTypeListIterator::NewLC(
            aFieldTypeList,
            aContactFields);
    }

// -----------------------------------------------------------------------------
// CPbk2JapaneseContactNameFormatter::MaxTitleLength
// -----------------------------------------------------------------------------
//
TInt CPbk2JapaneseContactNameFormatter::MaxTitleLength
        (const MVPbkBaseContactFieldCollection& aContactFields,
        const TUint32 aFormattingFlags)
    {
    TInt result = 0;
    SetFieldMapper( aContactFields );

    // Last/First name
    result = DoCalculateMaxTitleLength
        (aFormattingFlags, ENameFirstPart, ENameSecondPart);

    // Last/First name reading
    if (result == 0)
        {
        result = DoCalculateMaxTitleLength
            ( aFormattingFlags, EReadingFirstPart, EReadingSecondPart );
        }

    if (result == 0)
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
// CPbk2JapaneseContactNameFormatter::MaxTitleLengthForFind
// -----------------------------------------------------------------------------
//
TInt CPbk2JapaneseContactNameFormatter::MaxTitleLengthForFind
        (const MVPbkBaseContactFieldCollection& aContactFields,
        const TUint32 aFormattingFlags)
    {
    TInt result = 0;
    SetFieldMapper( aContactFields );

    const TInt count = iFieldMapper.FieldCount();
    result = DoCalculateMaxTitleLength
        ( aFormattingFlags, EReadingFirstPart, ENameSecondPart );

    // Rest of the title fields
    if( result == 0 )
        {
        result = DoCalculateMaxTitleLength
            ( aFormattingFlags, ENameSecondPart + 1, count - 1 );
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CPbk2JapaneseContactNameFormatter::IsFindSeparatorChar
// -----------------------------------------------------------------------------
//
TBool CPbk2JapaneseContactNameFormatter::IsFindSeparatorChar( TChar aCh )
    {
    return aCh.IsSpace();
    }


// End of File
