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
*		Concrete Phonebook contact name formatting interface for japanese 
*       region.
*
*/


// INCLUDE FILES
#include    "CPbkJapanContactNameFormatter.h"
#include    "MPbkFieldData.h"
#include    "TPbkJapanTitleFieldFilter.h"
#include    "TPbkJapanFindFieldFilter.h"
#include    <cntviewbase.h>
#include    <PbkEngUtils.h>
#include    "CPbkContactEngine.h"
#include    "CPbkSortOrderManager.h"

namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_GetContactTitle = 1,
    EPanicPostCond_GetContactListTitle,
    EPanicPreCond_PbkFieldAt,
    EPanicPreCond_PbkFieldTime
    };
#endif

enum TTitleType
    {
    ETitleTypeNormal = 1,
    ETitleTypeList
    };

enum TNameOrder
    {
    ETopContactOrderNumber = 0,
    EReadingFirstPart,
    EReadingSecondPart,
    ENameFirstPart,
    ENameSecondPart
    };

// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkJapanContactNameFormatter");
    User::Panic(KPanicText, aReason);
    }
#endif


// MODULE DATA STRUCTURES

}  // namespace


// ==================== MEMBER FUNCTIONS ====================

inline CPbkJapanContactNameFormatter::CPbkJapanContactNameFormatter
        (const CPbkSortOrderManager& aSortOrderManager) :
    iSortOrderManager(aSortOrderManager)
    {
    // CBase::operator new(TLeave) resets all members
    }

inline void CPbkJapanContactNameFormatter::ConstructL
        (const TDesC& aUnnamedText)
    {
    BaseConstructL(aUnnamedText);

    iTitleFieldFilter = new(ELeave) TTitleFieldFilter();
    iFindFieldFilter = new(ELeave) TFindFieldFilter();
    }

CPbkJapanContactNameFormatter* CPbkJapanContactNameFormatter::NewL
        (const TDesC& aUnnamedText, const CPbkSortOrderManager& aSortOrderManager)
    {
    CPbkJapanContactNameFormatter* self = 
        new (ELeave) CPbkJapanContactNameFormatter(aSortOrderManager);
    CleanupStack::PushL(self);
    self->ConstructL(aUnnamedText);
    CleanupStack::Pop(self);
    return self;
    }

CPbkJapanContactNameFormatter::~CPbkJapanContactNameFormatter()
    { 
    delete iTitleFieldFilter;
    delete iFindFieldFilter;
    }

inline TInt CPbkJapanContactNameFormatter::TrimNameLength
        (TInt aTitleType,
        const TDesC& aName) const
    {
    TInt result = 0;
    if (aTitleType == ETitleTypeList)
        {
        result += PbkEngUtils::TrimRightLength(aName);
        }
    else
        {
        result += PbkEngUtils::TrimAllLength(aName);
        }
    return result;
    }

TInt CPbkJapanContactNameFormatter::CalculateContactTitleLength
        (const MPbkFieldDataArray& aContactData,
        TInt aTitleType) const
    {
    TInt result = 0;

    // This algorithm is sort order independent,
    // the length is not dependent of the name ordering
    const TDesC& lastName = PbkContactNameFormattingUtil::FieldText(
            aContactData, EPbkFieldIdLastName);
    // The trimming depends on needed title type
    result += TrimNameLength(aTitleType, lastName);

    const TDesC& firstName = PbkContactNameFormattingUtil::FieldText(
        aContactData, EPbkFieldIdFirstName);
    if (RequiresSpaceBetweenNames(lastName,firstName))
        {
		TChar separator = iSortOrderManager.NameSeparator();
		if (!separator.Eos())
			{
		    ++result;
            }
        ++result;  // count space
        }

    result += TrimNameLength(aTitleType, firstName);

    // calculate reading fields length
    if (result == 0)
        {
        const TDesC& lastNameReading = PbkContactNameFormattingUtil::FieldText(
                aContactData, EPbkFieldIdLastNameReading);
        // The trimming depends on needed title type
        result += TrimNameLength(aTitleType, lastNameReading);

        const TDesC& firstNameReading = PbkContactNameFormattingUtil::FieldText(
                aContactData, EPbkFieldIdFirstNameReading);
        if (RequiresSpaceBetweenNames(lastNameReading, firstNameReading))
            {
			TChar separator = iSortOrderManager.NameSeparator();
			if (!separator.Eos())
				{
			    ++result;
            	}
            ++result;  // count space
            }

        result += TrimNameLength(aTitleType, firstNameReading);
        }

    if (result == 0)
        {
        const TDesC& companyName = PbkContactNameFormattingUtil::FieldText(
                aContactData, EPbkFieldIdCompanyName);
        result += TrimNameLength(aTitleType, companyName);
        }

    return result;
    }

TInt CPbkJapanContactNameFormatter::ContactTitleLength
        (const MPbkFieldDataArray& aContactData) const
    {
    return CalculateContactTitleLength(aContactData, ETitleTypeNormal);
    }

TInt CPbkJapanContactNameFormatter::ContactListTitleLength
        (const MPbkFieldDataArray& aContactData) const
    {
    return CalculateContactTitleLength(aContactData, ETitleTypeList);
    }

TBool CPbkJapanContactNameFormatter::IsTitleField
		(TPbkFieldId aFieldId) const
    {
    switch (aFieldId)
        {
        case EPbkFieldIdLastName: // FALLTHROUGH
        case EPbkFieldIdFirstName: // FALLTHROUGH
        case EPbkFieldIdLastNameReading: // FALLTHROUGH
        case EPbkFieldIdFirstNameReading: // FALLTHROUGH
        case EPbkFieldIdCompanyName:
			{
            return ETrue;
			}
        default:
			{
            return EFalse;
			}
        }
    }

void CPbkJapanContactNameFormatter::GetContactTitle
         (const MPbkFieldDataArray& aContactData, 
		 TDes& aTitle) const
    {
    // Just delegate
    DoGetContactTitle(aContactData, aTitle, ETitleTypeNormal);

    __ASSERT_DEBUG(aTitle.Length() == ContactTitleLength(aContactData),
        Panic(EPanicPostCond_GetContactTitle));
    }

void CPbkJapanContactNameFormatter::GetContactListTitle
         (const MPbkFieldDataArray& aContactData, 
		 TDes& aTitle) const
    {
    // Just delegate
    DoGetContactTitle(aContactData, aTitle, ETitleTypeList);

    __ASSERT_DEBUG(aTitle.Length() == ContactListTitleLength(aContactData),
        Panic(EPanicPostCond_GetContactListTitle));
    }

inline void CPbkJapanContactNameFormatter::TrimName
        (TInt aTitleType,
        const TDesC& aName,
        TDes& aTitle) const
    {
    if (aTitleType == ETitleTypeList)
        {
        // In lists, the leading spaces are not formatted,
        // only the end of the string is
        PbkEngUtils::TrimRightAppend(aName, aTitle);
        }
    else
        {
        // Elsewhere, string is trimmed from beginning and end
        PbkEngUtils::TrimAllAppend(aName, aTitle);
        }
    }

// Inlined for speed
inline void CPbkJapanContactNameFormatter::DoGetContactTitle
         (const MPbkFieldDataArray& aContactData, 
		 TDes& aTitle,
         TInt aTitleType) const
    {
    aTitle.Zero();

    const RContactViewSortOrder& sortOrder = iSortOrderManager.SortOrder();

    // Get sort order - order fields according to the sort order
    TPbkFieldId firstFieldId = PbkContactNameFormattingUtil::GetPbkFieldIdAt(
            sortOrder, ENameFirstPart);
    const TDesC& nameFirstPart = PbkContactNameFormattingUtil::FieldText(
            aContactData, firstFieldId);
    
    // The trimming depends on needed title type
    TrimName(aTitleType, nameFirstPart, aTitle);
   
    TPbkFieldId secondFieldId = PbkContactNameFormattingUtil::GetPbkFieldIdAt(
            sortOrder, ENameSecondPart);
    const TDesC& nameSecondPart = PbkContactNameFormattingUtil::FieldText(
            aContactData, secondFieldId);
    if (RequiresSpaceBetweenNames(nameFirstPart,nameSecondPart))
        {
        TChar separator = iSortOrderManager.NameSeparator();
        if (!separator.Eos())
            {
            aTitle.Append(separator);
            }
        aTitle.Append(' ');
        }
    // The trimming depends on needed title type
    TrimName(aTitleType, nameSecondPart, aTitle);
    
    // if no real names, show reading names
    if (aTitle.Length() == 0)
        {
        TPbkFieldId firstReadingFieldId = 
            PbkContactNameFormattingUtil::GetPbkFieldIdAt
                (sortOrder, EReadingFirstPart);
        const TDesC& firstReadingPart = PbkContactNameFormattingUtil::FieldText(
                aContactData, firstReadingFieldId);
        // The trimming depends on needed title type
        TrimName(aTitleType, firstReadingPart, aTitle);
   
        TPbkFieldId secondReadingFieldId = 
            PbkContactNameFormattingUtil::GetPbkFieldIdAt
                (sortOrder, EReadingSecondPart);
        const TDesC& secondReadingPart = 
            PbkContactNameFormattingUtil::FieldText
                (aContactData, secondReadingFieldId);

        if (RequiresSpaceBetweenNames(firstReadingPart, secondReadingPart))
            {
			TChar separator = iSortOrderManager.NameSeparator();
				if (!separator.Eos())
				{
				aTitle.Append(separator);
				}
            aTitle.Append(' ');
            }
        // The trimming depends on needed title type
        TrimName(aTitleType, secondReadingPart, aTitle);
        }

    // no names or reading names, use company name
    if (aTitle.Length() == 0)
        {
        const TDesC& companyName = PbkContactNameFormattingUtil::FieldText(
                aContactData, EPbkFieldIdCompanyName);

        // The trimming depends on needed title type
        TrimName(aTitleType, companyName, aTitle);
        }
    }


const MPbkFieldDataArray& CPbkJapanContactNameFormatter::FilterContactTitleFields
        (const MPbkFieldDataArray& aContactData) const
    {
    iTitleFieldFilter->Set(aContactData);
    return *iTitleFieldFilter;
	}

const MPbkFieldDataArray& 
    CPbkJapanContactNameFormatter::FilterContactFieldsForFind
    (const MPbkFieldDataArray& aContactData) const
    {
    iFindFieldFilter->Set(aContactData);
    return *iFindFieldFilter;
    }

//  End of File 
