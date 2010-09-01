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
*		Phonebook contact name formatting base
*
*/


// INCLUDE FILES
#include "CPbkContactNameFormatterBase.h"
#include <cntviewbase.h>
#include <PbkEngUtils.h>
#include "MPbkFieldData.h"
#include "PbkContactNameFormattingUtil.h"


namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
enum TPanicCode
    {
    EPanic_InvalidNameFormat = 1,
    EPanicPostCond_GetContactTitleOrNullL,
    EPanicPostCond_GetContactTitle,
    EPanicPreCond_PbkFieldAt,
    EPanicPreCond_PbkFieldTime
    };
#endif

enum TTitleType
    {
    ETitleTypeNormal = 1,
    ETitleTypeList
    };


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkContactNameFormatterBase");
    User::Panic(KPanicText, aReason);
    }
#endif

// MODULE DATA STRUCTURES

}  // namespace


// ==================== MEMBER FUNCTIONS ====================

CPbkContactNameFormatterBase::CPbkContactNameFormatterBase()
    {
    // CBase::operator new(TLeave) resets all members
    }

void CPbkContactNameFormatterBase::BaseConstructL
        (const TDesC& aUnnamedText)
    {
    iUnnamedText = aUnnamedText.AllocL();
    }

CPbkContactNameFormatterBase::~CPbkContactNameFormatterBase()
    {
    delete iUnnamedText;
    }

HBufC* CPbkContactNameFormatterBase::GetContactTitleOrNullL
        (const MPbkFieldDataArray& aContactData) const
    {
    // Calculate needed buffer length 
    const TInt titleLength = ContactTitleLength(aContactData);
    if (titleLength > 0)
        {
        // Allocate buffer
        HBufC* titleHeap =  HBufC::NewL(titleLength);
        TPtr title(titleHeap->Des());
        // Format title to buffer
        GetContactTitle(aContactData, title);
          
        __ASSERT_DEBUG
            (titleHeap && 
            titleHeap->Length() == titleLength &&
            !PbkEngUtils::IsEmptyOrAllSpaces(*titleHeap), 
            Panic(EPanicPostCond_GetContactTitleOrNullL));
        return titleHeap;
        }

    return NULL;
    }

HBufC* CPbkContactNameFormatterBase::GetContactTitleL
        (const MPbkFieldDataArray& aContactData) const
    {
    HBufC* title = GetContactTitleOrNullL(aContactData);
    if (!title)
        {
        title = iUnnamedText->AllocL();
        }
    __ASSERT_DEBUG
        (title && 
        (!PbkEngUtils::IsEmptyOrAllSpaces(*title) || *title==*iUnnamedText),
        Panic(EPanicPostCond_GetContactTitle));
    return title;
    }

const TDesC& CPbkContactNameFormatterBase::UnnamedText() const
    {
    return (*iUnnamedText);
    }

inline TInt CPbkContactNameFormatterBase::TrimNameLength
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

inline TInt CPbkContactNameFormatterBase::CalculateContactTitleLength
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
        ++result;  // count space
        }

    result += TrimNameLength(aTitleType, firstName);

    if (result == 0)
        {
        const TDesC& companyName = PbkContactNameFormattingUtil::FieldText(
                aContactData, EPbkFieldIdCompanyName);
        result += TrimNameLength(aTitleType, companyName);
        }

    return result;
    }

TInt CPbkContactNameFormatterBase::ContactTitleLength
        (const MPbkFieldDataArray& aContactData) const
    {
    return CalculateContactTitleLength(aContactData, ETitleTypeNormal);
    }


TInt CPbkContactNameFormatterBase::ContactListTitleLength
        (const MPbkFieldDataArray& aContactData) const
    {
    return CalculateContactTitleLength(aContactData, ETitleTypeList);
    }


TBool CPbkContactNameFormatterBase::IsTitleField
		(TPbkFieldId aFieldId) const
    {
    switch (aFieldId)
        {
        case EPbkFieldIdLastName: // FALLTHROUGH
        case EPbkFieldIdFirstName: // FALLTHROUGH
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

HBufC* CPbkContactNameFormatterBase::FormatNameDescriptorL
        (const TDesC& aTitle) const
    {
    HBufC* formattedNameBuf = aTitle.AllocL();
    TPtr formattedName(formattedNameBuf->Des());
    formattedName.TrimAll();
    return formattedNameBuf;
    }

/**
 * Returns true if a space is needed between aLastName and aFirstName in the
 * formatted name.
 */
TBool CPbkContactNameFormatterBase::RequiresSpaceBetweenNames
        (const TDesC& aLastName, 
        const TDesC& aFirstName) const
    {
    return (aLastName.Length() > 0 && aFirstName.Length() > 0);
    }

//  End of File 
