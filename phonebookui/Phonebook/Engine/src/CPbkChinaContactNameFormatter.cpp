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
*       for APAC region variant.
*
*/


// INCLUDE FILES
#include    "CPbkChinaContactNameFormatter.h"
#include    "MPbkFieldData.h"
#include    "TPbkTitleFieldFilter.h"
#include    <cntviewbase.h>
#include    <PbkEngUtils.h>


namespace {

// LOCAL CONSTANTS AND MACROS
const TUint KChineseUnicodeSpanBegin = 0x3400;
const TUint KChineseUnicodeSpanEnd = 0x9fff;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_GetContactTitle = 1,
    EPanicPreCond_PbkFieldTime,
    EPanicPreCond_PbkFieldAt
    };
#endif


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkChinaContactNameFormatter");
    User::Panic(KPanicText, aReason);
    }
#endif

/**
 * Returns true if aCh is a Chinese character.
 */
inline TBool IsChinese(TChar aCh)
    {
    // Chinese character unicode span
    return ((aCh >= KChineseUnicodeSpanBegin)
		&& (aCh <= KChineseUnicodeSpanEnd));
    }

/**
 * Returns true if first non-space character in aText is a Chinese character.
 * @precond aText.Length() > 0
 */
inline TBool NameContainsChinese(const TDesC& aText)
    {
    TBool ret = EFalse;
    for (TInt i=0; i< aText.Length(); ++i)
        {
        if (IsChinese(aText[i]))
            {
            ret = ETrue;
            break;
            }
        }
    return ret;
    }

// MODULE DATA STRUCTURES

}  // namespace

// ==================== MEMBER FUNCTIONS ====================


inline CPbkChinaContactNameFormatter::CPbkChinaContactNameFormatter()
    {
    // CBase::operator new(TLeave) resets all members
    }

inline void CPbkChinaContactNameFormatter::ConstructL
        (const TDesC& aUnnamedText,
        const RContactViewSortOrder& aSortOrder)
    {
    BaseConstructL(aUnnamedText);

    // cache sort order
    iSortOrder.CopyL(aSortOrder);

    iTitleFieldFilter = new(ELeave) TTitleFieldFilter();
    }

CPbkChinaContactNameFormatter* CPbkChinaContactNameFormatter::NewL
        (const TDesC& aUnnamedText,
        const RContactViewSortOrder& aSortOrder)
    {
    CPbkChinaContactNameFormatter* self = new(ELeave) CPbkChinaContactNameFormatter;
    CleanupStack::PushL(self);
    self->ConstructL(aUnnamedText, aSortOrder);
    CleanupStack::Pop(self); 
    return self;
    }

CPbkChinaContactNameFormatter::~CPbkChinaContactNameFormatter()
    {
    delete iTitleFieldFilter;
    iSortOrder.Close();
    }

// Inlined for speed
inline void CPbkChinaContactNameFormatter::DoGetContactTitle
         (const MPbkFieldDataArray& aContactData, 
         TDes& aTitle) const
    {
    aTitle.Zero();

    const TDesC& lastName = PbkContactNameFormattingUtil::FieldText(
            aContactData, EPbkFieldIdLastName);

    PbkEngUtils::TrimAllAppend(lastName, aTitle);

    const TDesC& firstName = PbkContactNameFormattingUtil::FieldText(
        aContactData, EPbkFieldIdFirstName);

    if (RequiresSpaceBetweenNames(lastName,firstName))
        {
        aTitle.Append(' ');
        }
    PbkEngUtils::TrimAllAppend(firstName, aTitle);

    if (aTitle.Length() == 0)
        {
        const TDesC& companyName = PbkContactNameFormattingUtil::FieldText(
                aContactData, EPbkFieldIdCompanyName);
        PbkEngUtils::TrimAllAppend(companyName, aTitle);
        }

    __ASSERT_DEBUG(aTitle.Length() == ContactTitleLength(aContactData),
        Panic(EPanicPostCond_GetContactTitle));
    }

void CPbkChinaContactNameFormatter::GetContactListTitle
         (const MPbkFieldDataArray& aContactData, 
         TDes& aTitle) const
    {
    // Just delegate - list title is similar to normal title in APAC
    DoGetContactTitle(aContactData, aTitle);
    }

void CPbkChinaContactNameFormatter::GetContactTitle
         (const MPbkFieldDataArray& aContactData, 
         TDes& aTitle) const
    {
    DoGetContactTitle(aContactData, aTitle);
    }



const MPbkFieldDataArray& CPbkChinaContactNameFormatter::FilterContactTitleFields
        (const MPbkFieldDataArray& aContactData) const
    {
    iTitleFieldFilter->Set(aContactData, iSortOrder);
    return *iTitleFieldFilter;
    }

/**
 * Returns true if a space is needed between aLastName and aFirstName in the
 * formatted name. The space is needed only when last name or first name
 * contain only latin characters or the user inserts the space herself.
 */
TBool CPbkChinaContactNameFormatter::RequiresSpaceBetweenNames
        (const TDesC& aLastName, const TDesC& aFirstName) const
    {
    return
        (
            // Both the names exist
           (aLastName.Length() > 0 && aFirstName.Length() > 0) &&
            // AND
           (
            // either the last char of the last name is a space
            PbkContactNameFormattingUtil::LastCharIsSpace(aLastName) ||
            // OR the first char of the first name is a space
            PbkContactNameFormattingUtil::FirstCharIsSpace(aFirstName) ||
            // OR both the names contain only non-chinese characters
            (!NameContainsChinese(aLastName) && !NameContainsChinese(aFirstName))
           )
        );
    }

const MPbkFieldDataArray& CPbkChinaContactNameFormatter::FilterContactFieldsForFind
    (const MPbkFieldDataArray& aContactData) const
    {
    // Delegate
    return FilterContactTitleFields(aContactData);
    }


//  End of File
