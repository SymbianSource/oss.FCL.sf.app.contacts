/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A base class for contact name formatters
*
*/



// INCLUDE FILES
#include "CPbk2ContactNameFormatterBase.h"

#include <f32file.h>
#include <barsread.h>
#include <barsc.h>
#include <coemain.h>

#include <MVPbkFieldType.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkFieldTypeRefsList.h>

#include "CPbk2SortOrderManager.h"

#include "Pbk2PresentationUtils.h"
#include "Pbk2DataCaging.hrh"

namespace {

// CONSTANTS
_LIT(KSpace, " ");
const TText KReplaceChar = ' ';

// ============================= LOCAL FUNCTIONS ===============================

void TrimNameAppend( TUint32 aFormattingFlags, const TDesC& aName, 
    TDes& aTitle )
    {
    if ( aFormattingFlags & MPbk2ContactNameFormatter::EPreserveLeadingSpaces )
        {
        // In lists, the leading spaces are not formatted,
        // only the end of the string is
        Pbk2PresentationUtils::TrimRightAppend( aName, aTitle );
        }
    else if (aFormattingFlags & MPbk2ContactNameFormatter::EPreserveAllOriginalSpaces)
        {
         //Preserve original string with all spaces
        aTitle.Append(aName);
        }
        else
        {
        // String is trimmed from beginning and end
        Pbk2PresentationUtils::TrimAllAppend( aName, aTitle );
        }

    if ( aFormattingFlags & 
        MPbk2ContactNameFormatter::EReplaceNonGraphicChars )
        {
        // Non-graphical characters are replaced with space
        Pbk2PresentationUtils::ReplaceNonGraphicCharacters( aTitle, 
            KReplaceChar );
        }
    }
    
#ifdef _DEBUG
namespace {

    enum TPanic
        {
        EPanic_PreCond_UnnamedText = 1
        };
        
    void Panic( TPanic aPanicCode )
        {
        _LIT( KPanicCat, "CPbk2ContactNameFormatterBase" );
        User::Panic( KPanicCat, aPanicCode );
        }
}
#endif // _DEBUG    

} // namespace


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::CPbk2ContactNameFormatterBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbk2ContactNameFormatterBase::CPbk2ContactNameFormatterBase(
        const MVPbkFieldTypeList& aMasterFieldTypeList,
        const CPbk2SortOrderManager& aSortOrderManager )
        :   iMasterFieldTypeList( aMasterFieldTypeList ),
            iSortOrderManager( aSortOrderManager )
    {
    iFieldMapper.SetSortOrder( iSortOrderManager.SortOrder() );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase:BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPbk2ContactNameFormatterBase::BaseConstructL( const TDesC& aUnnamedText,
        CVPbkFieldTypeSelector* aTitleFieldSelector )
    {
    iUnnamedText = aUnnamedText.AllocL();
    iTitleFieldSelector = aTitleFieldSelector;
    }

// Destructor
CPbk2ContactNameFormatterBase::~CPbk2ContactNameFormatterBase()
    {
    delete iUnnamedText;
    delete iTitleFieldSelector;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::GetContactTitleOrNullL
// -----------------------------------------------------------------------------
//
HBufC* CPbk2ContactNameFormatterBase::GetContactTitleOrNullL(
        const MVPbkBaseContactFieldCollection& aContactFields,
        TUint32 aFormattingFlags)
    {
    HBufC* result = HBufC::NewL
        (MaxTitleLength(aContactFields, aFormattingFlags));
    TPtr resultPtr = result->Des();
    GetContactTitle(aContactFields, resultPtr, aFormattingFlags);

    if (resultPtr.Length() == 0)
        {
        delete result;
        result = NULL;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::GetContactTitleL
// -----------------------------------------------------------------------------
//
HBufC* CPbk2ContactNameFormatterBase::GetContactTitleL(
       const MVPbkBaseContactFieldCollection& aContactFields,
       TUint32 aFormattingFlags )
    {
    HBufC* result = GetContactTitleOrNullL(aContactFields, aFormattingFlags);
    if (!result)
        {
        result = UnnamedText().AllocL();
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::UnnamedText
// -----------------------------------------------------------------------------
//
const TDesC& CPbk2ContactNameFormatterBase::UnnamedText() const
    {
    __ASSERT_DEBUG( iUnnamedText, Panic( EPanic_PreCond_UnnamedText ) );
    return *iUnnamedText;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::IsTitleField
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactNameFormatterBase::IsTitleField(
        const MVPbkBaseContactField& aContactField ) const
    {
    const TInt maxMatchPriority = iMasterFieldTypeList.MaxMatchPriority();
    for (TInt matchPriority = 0; matchPriority
            <= maxMatchPriority; ++matchPriority)
        {
        const MVPbkFieldType* fieldType =
            aContactField.MatchFieldType(matchPriority);
        if (fieldType)
            {
            return IsTitleFieldType(*fieldType);
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::IsTitleFieldType
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactNameFormatterBase::IsTitleFieldType(
        const MVPbkFieldType& aFieldType) const
    {
    return iTitleFieldSelector->IsFieldTypeIncluded( aFieldType );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::SetFieldMapper
// -----------------------------------------------------------------------------
//
void CPbk2ContactNameFormatterBase::SetFieldMapper(
        const MVPbkBaseContactFieldCollection& aContactFields )
    {
    iFieldMapper.SetSortOrder( iSortOrderManager.SortOrder() );
    iFieldMapper.SetContactFields( aContactFields );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::RequiresSeparatorBetweenNames
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactNameFormatterBase::RequiresSeparatorBetweenNames
        (const TDesC& aLhs, const TDesC& aRhs) const
    {
    return (aLhs.Length() > 0 && aRhs.Length() > 0);
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::DoGetContactTitle
// -----------------------------------------------------------------------------
//
void CPbk2ContactNameFormatterBase::DoGetContactTitle
            (TDes& aTitle, TUint32 aFormattingFlags,
            const TInt aStartingPosition, const TInt aEndingPosition)
    {
    // Find the fields in the sort order that have
    // data to build the title
    const TInt fieldCount = iFieldMapper.FieldCount();
    for (TInt i = aStartingPosition; i <= aEndingPosition && 
            i < fieldCount; ++i)
        {
        TakePartOfName(iFieldMapper.FieldAt(i),
            aTitle, aFormattingFlags, ETrue);
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::DoCalculateMaxTitleLength
// -----------------------------------------------------------------------------
//
TInt CPbk2ContactNameFormatterBase::DoCalculateMaxTitleLength
        (const TUint32 aFormattingFlags, const TInt aStartingPosition,
        const TInt aEndingPosition)
    {
    TInt result = 0;

    // Use all the fields in sort order to
    // build the title (ie. the company name)
    const TInt fieldCount = iFieldMapper.FieldCount();
    for (TInt i = aStartingPosition; i <= aEndingPosition &&
            i < fieldCount; ++i)
        {
        CalculatePartOfName(iFieldMapper.FieldAt(i),
            result, aFormattingFlags, ETrue);
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::DoAppendNonEmptyTitleFieldTypesL
// -----------------------------------------------------------------------------
//
void CPbk2ContactNameFormatterBase::DoAppendNonEmptyTitleFieldTypesL(
        CVPbkFieldTypeRefsList& aFieldTypeList,
        const TInt aStartingPosition,
        const TInt aEndingPosition )
    {
    // Find the fields in the sort order that have
    // data to build the title
    for ( TInt i = aStartingPosition; i <= aEndingPosition; ++i )
        {
        const MVPbkBaseContactField* field = iFieldMapper.FieldAt(i);
        if ( field && IsTitleField( *field ) && !field->FieldData().IsEmpty() )
            {
            // get the field type
            const TInt maxMatchPriority = iMasterFieldTypeList.MaxMatchPriority();
            for ( TInt matchPriority = 0; matchPriority
                    <= maxMatchPriority; ++matchPriority )
                {
                const MVPbkFieldType* fieldType =
                    field->MatchFieldType(matchPriority);
                // Do not add duplicates to aFieldTypeList
                if( fieldType && 
                    !aFieldTypeList.ContainsSame( *fieldType ) )
                    {
                    aFieldTypeList.AppendL( *fieldType );
                    }
                }
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::AreTitleFieldsEmpty
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactNameFormatterBase::AreTitleFieldsEmpty(
        const TInt aStartingPosition,
        const TInt aEndingPosition ) const
    {
    TBool result = ETrue;
    
    // Find the fields in the sort order that have
    // data to build the title
    const TInt fieldCount = iFieldMapper.FieldCount();
    for (TInt i = aStartingPosition; i <= aEndingPosition && 
            i < fieldCount; ++i)
        {
        const MVPbkBaseContactField* field = iFieldMapper.FieldAt(i);
        
        if ( field && IsTitleField(*field))
            {
            const MVPbkContactFieldData& fieldData = field->FieldData();
            if (fieldData.DataType() == EVPbkFieldStorageTypeText)
                {
                const TDesC& fieldText =
                    MVPbkContactFieldTextData::Cast(fieldData).Text();
                TInt length = fieldText.Length();
                
                if (length > 0)
                    {
                    // Define the firstNonSpaceChar to calculate the first 
                    // none space alph index in last name and first name, if the 
                    // firstNonSpaceChar is equal to the fieldText length
                    // then there are only spaces in first name and last name.
                    TInt firstNonSpaceChar = 0;
                    while( firstNonSpaceChar < length
                            && TChar(fieldText[firstNonSpaceChar]).IsSpace() )
                        {
                        ++firstNonSpaceChar;
                        }
                    if ( firstNonSpaceChar != length )
                        {
                        result = EFalse;
                        break;
                        }
                    }                
                }
            }
        }
    
    return result;
    }    

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::TakePartOfName
// Inlined for performance reasons
// -----------------------------------------------------------------------------
//
inline void CPbk2ContactNameFormatterBase::TakePartOfName
        (const MVPbkBaseContactField* aField,
        TDes& aTitle, const TUint32 aFormattingFlags,
        const TBool aUseSeparator) const
    {
    if (aField && IsTitleField(*aField))
        {
        const MVPbkContactFieldData& fieldData = aField->FieldData();
        if (fieldData.DataType() == EVPbkFieldStorageTypeText)
            {
            const TDesC& fieldText =
                MVPbkContactFieldTextData::Cast(fieldData).Text();

            if (aUseSeparator && RequiresSeparatorBetweenNames
                    (aTitle, fieldText))
                {
                const TDesC& separator = iSortOrderManager.CurrentSeparator();
                if (aFormattingFlags &
                    MPbk2ContactNameFormatter::EUseSeparator)
                    {
                    aTitle.Append(separator);
                    // If the separator is other than space, append a space to it
                    if (separator.Compare(KSpace) != 0)
                        {
                        aTitle.Append(KSpace);    
                        }
                    }
                else
                    {
                    // If separator is not used, add space then
                    aTitle.Append(KSpace);                        
                    }
                
                }
            TrimNameAppend(aFormattingFlags, fieldText, aTitle);
            }
        }
    }

// -----------------------------------------------------------------------------
// CPbk2ContactNameFormatterBase::CalculatePartOfName
// Inlined for performance reasons
// -----------------------------------------------------------------------------
//
inline void CPbk2ContactNameFormatterBase::CalculatePartOfName
        (const MVPbkBaseContactField* aField,
        TInt& aLength, const TUint32 aFormattingFlags,
        const TBool aUseSeparator) const
    {
    if (aField && IsTitleField(*aField))
        {
        const MVPbkContactFieldData& fieldData = aField->FieldData();
        if (fieldData.DataType() == EVPbkFieldStorageTypeText)
            {
            const TDesC& fieldText =
                MVPbkContactFieldTextData::Cast(fieldData).Text();

            const TInt length = fieldText.Length();
            aLength += length;

            if (aUseSeparator && length > 0)
                {
                if (aFormattingFlags &
                    MPbk2ContactNameFormatter::EUseSeparator)
                    {
                    aLength += iSortOrderManager.CurrentSeparator().Length();
                    }
                // There is also a space after separator
                aLength += KSpace().Length();
                }
            }
        }
    }

//  End of File
