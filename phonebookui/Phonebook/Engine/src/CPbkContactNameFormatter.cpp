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
*		Concrete Phonebook contact name formatting interface
*
*/


// INCLUDE FILES
#include    "CPbkContactNameFormatter.h"
#include    "MPbkFieldData.h"
#include    "TPbkTitleFieldFilter.h"
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
    ENameFirstPart,
    ENameSecondPart
    };

// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkContactNameFormatter");
    User::Panic(KPanicText, aReason);
    }
#endif

// MODULE DATA STRUCTURES


}  // namespace


// ==================== MEMBER FUNCTIONS ====================


inline CPbkContactNameFormatter::CPbkContactNameFormatter
        (const CPbkSortOrderManager& aSortOrderManager) :
    iSortOrderManager(aSortOrderManager)
    {
    // CBase::operator new(TLeave) resets all members
    }

inline void CPbkContactNameFormatter::ConstructL
        (const TDesC& aUnnamedText)
    {
    BaseConstructL(aUnnamedText);

    iTitleFieldFilter = new(ELeave) TTitleFieldFilter();
    }

CPbkContactNameFormatter* CPbkContactNameFormatter::NewL
        (const TDesC& aUnnamedText,
		const CPbkSortOrderManager& aSortOrderManager)
    {
    CPbkContactNameFormatter* self = new(ELeave) CPbkContactNameFormatter(aSortOrderManager);
    CleanupStack::PushL(self);
    self->ConstructL(aUnnamedText);
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactNameFormatter::~CPbkContactNameFormatter()
    {
    delete iTitleFieldFilter;
    }

inline void CPbkContactNameFormatter::TrimName
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

void CPbkContactNameFormatter::DoGetContactTitle
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

    if (aTitle.Length() == 0)
        {
        const TDesC& companyName = PbkContactNameFormattingUtil::FieldText(
                aContactData, EPbkFieldIdCompanyName);

        // The trimming depends on needed title type
        TrimName(aTitleType, companyName, aTitle);
        }
    }


void CPbkContactNameFormatter::GetContactTitle
         (const MPbkFieldDataArray& aContactData, 
		 TDes& aTitle) const
    {
    DoGetContactTitle(aContactData, aTitle, ETitleTypeNormal);

    __ASSERT_DEBUG(aTitle.Length() == ContactTitleLength(aContactData),
        Panic(EPanicPostCond_GetContactTitle));
    }

void CPbkContactNameFormatter::GetContactListTitle
         (const MPbkFieldDataArray& aContactData, 
		 TDes& aTitle) const
    {
    DoGetContactTitle(aContactData, aTitle, ETitleTypeList);

    __ASSERT_DEBUG(aTitle.Length() == ContactListTitleLength(aContactData),
        Panic(EPanicPostCond_GetContactListTitle));
    }

const MPbkFieldDataArray& CPbkContactNameFormatter::FilterContactTitleFields
        (const MPbkFieldDataArray& aContactData) const
    {
    iTitleFieldFilter->Set(aContactData, iSortOrderManager.SortOrder());
    return *iTitleFieldFilter;
	}

const MPbkFieldDataArray& CPbkContactNameFormatter::FilterContactFieldsForFind
    (const MPbkFieldDataArray& aContactData) const
    {
    // Delegate
    return FilterContactTitleFields(aContactData);
    }  
                
TInt CPbkContactNameFormatter::ContactTitleLength
    (const MPbkFieldDataArray& aContactData) const
    {
    TInt length = CPbkContactNameFormatterBase::ContactTitleLength(aContactData);

    const TDesC& lastName = PbkContactNameFormattingUtil::FieldText(
            aContactData, EPbkFieldIdLastName);
    const TDesC& firstName = PbkContactNameFormattingUtil::FieldText(
        aContactData, EPbkFieldIdFirstName);
    
    // If both names exist and a separator character is used, then add one
    if (RequiresSpaceBetweenNames(lastName,firstName) &&
        !iSortOrderManager.NameSeparator().Eos())
        {
        length++;
        }
    return length;
    }      
    
TInt CPbkContactNameFormatter::ContactListTitleLength
    (const MPbkFieldDataArray& aContactData) const
    {
    TInt length = 
        CPbkContactNameFormatterBase::ContactListTitleLength(aContactData);

    const TDesC& lastName = PbkContactNameFormattingUtil::FieldText(
            aContactData, EPbkFieldIdLastName);
    const TDesC& firstName = PbkContactNameFormattingUtil::FieldText(
        aContactData, EPbkFieldIdFirstName);
    
    // If both names exist and a separator character is used, then add one
    if (RequiresSpaceBetweenNames(lastName,firstName) &&
        !iSortOrderManager.NameSeparator().Eos())
        {
        length++;
        }
    return length;
    }      


//  End of File 
