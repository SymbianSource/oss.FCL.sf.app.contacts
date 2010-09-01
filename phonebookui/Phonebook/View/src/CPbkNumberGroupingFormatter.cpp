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
*           Methods for Phonebook Phone number formatting.
*
*/


// INCLUDE FILES
#include "CPbkNumberGroupingFormatter.h"
#include <NumberGrouping.h>
#include <AknUtils.h>

// Define to force number grouping on for testing purposes
#undef PBK_FORCE_NUMBERGROUPING_ON

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
const TInt KGroupingFactor = 2;

}  // namespace

// ================= MEMBER FUNCTIONS =======================

inline CPbkNumberGroupingFormatter::CPbkNumberGroupingFormatter()
	{
	}

inline void CPbkNumberGroupingFormatter::ConstructL
        (TInt aMaxPhoneNumberDisplayLength)
	{
    // The grouping object takes care of calculating the
    // size of the grouped number, just pass the length of
    // non-grouped number
	iNumberGrouping = CPNGNumberGrouping::NewL(aMaxPhoneNumberDisplayLength);

    iFormatterNumberBuffer = HBufC::NewL(KGroupingFactor*aMaxPhoneNumberDisplayLength);

#ifdef PBK_FORCE_NUMBERGROUPING_ON
	iNumberGrouping->iForceLanguage = ELangAmerican;
#endif
	}

CPbkNumberGroupingFormatter* CPbkNumberGroupingFormatter::NewL
        (TInt aMaxPhoneNumberDisplayLength)
	{
	CPbkNumberGroupingFormatter* self = new(ELeave) CPbkNumberGroupingFormatter;
	CleanupStack::PushL(self);
	self->ConstructL(aMaxPhoneNumberDisplayLength);
	CleanupStack::Pop(self);
	return self;
	}

CPbkNumberGroupingFormatter::~CPbkNumberGroupingFormatter()
	{
	delete iNumberGrouping;
    delete iFormatterNumberBuffer;
	}

void CPbkNumberGroupingFormatter::SetMaxBufferLengthL
        (TInt aMaxLength)
	{
    if (aMaxLength > iNumberGrouping->MaxDisplayLength())
        {
		CPNGNumberGrouping* newNumberGrouping =
            CPNGNumberGrouping::NewL(aMaxLength);
		delete iNumberGrouping;
		iNumberGrouping = newNumberGrouping;
        iFormatterNumberBuffer =
            iFormatterNumberBuffer->ReAllocL(KGroupingFactor*aMaxLength);
        }

#ifdef PBK_FORCE_NUMBERGROUPING_ON
    iNumberGrouping->iForceLanguage = ELangAmerican;
#endif
	}

TPtrC CPbkNumberGroupingFormatter::FormatPhoneNumberForDisplay
		(const TDesC& aOriginalPhoneNumber)
	{
    if (aOriginalPhoneNumber.Length() <= iNumberGrouping->MaxDisplayLength())
        {
	    iNumberGrouping->Set(aOriginalPhoneNumber);
	    TPtr formatterNumber = iFormatterNumberBuffer->Des();
        formatterNumber.Copy(iNumberGrouping->FormattedNumber());
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formatterNumber);
        return formatterNumber;
        }
    else
        {
        // Too long number to format, just return the original. This is in line
        // with MPbkPhoneNumberFormat interface best-effort promise.
        return aOriginalPhoneNumber;
        }
	}

// End of File
