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
*       Default Phonebook phone number formatter.
*
*/


// INCLUDE FILES
#include "CPbkPhoneNumberDefaultFormatter.h"
#include <AknUtils.h>


// ================= MEMBER FUNCTIONS =======================

inline CPbkPhoneNumberDefaultFormatter::CPbkPhoneNumberDefaultFormatter()
	{
	}

inline void CPbkPhoneNumberDefaultFormatter::ConstructL
        (TInt aMaxPhoneNumberDisplayLength)
    {
    iFormatterNumberBuffer = HBufC::NewL(aMaxPhoneNumberDisplayLength);
    }

CPbkPhoneNumberDefaultFormatter* CPbkPhoneNumberDefaultFormatter::NewL
        (TInt aMaxPhoneNumberDisplayLength)
    {
    CPbkPhoneNumberDefaultFormatter* self = new(ELeave) CPbkPhoneNumberDefaultFormatter;
    CleanupStack::PushL(self);
    self->ConstructL(aMaxPhoneNumberDisplayLength);
    CleanupStack::Pop(self);
    return self;
    }

CPbkPhoneNumberDefaultFormatter::~CPbkPhoneNumberDefaultFormatter()
	{
    delete iFormatterNumberBuffer;
	}

void CPbkPhoneNumberDefaultFormatter::SetMaxBufferLengthL
		(TInt aMaxLength)
	{
    if (aMaxLength > iFormatterNumberBuffer->Des().MaxLength())
        {
        iFormatterNumberBuffer = iFormatterNumberBuffer->ReAllocL(aMaxLength);
        }
	}

TPtrC CPbkPhoneNumberDefaultFormatter::FormatPhoneNumberForDisplay
		(const TDesC& aOriginalPhoneNumber)
	{
    // convert number according to active number setting
    if (aOriginalPhoneNumber.Length() <= iFormatterNumberBuffer->Des().MaxLength())
        {
        TPtr formatterNumber = iFormatterNumberBuffer->Des();
        formatterNumber.Copy(aOriginalPhoneNumber);
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formatterNumber);
        return formatterNumber;
        }
    else
        {
    	return aOriginalPhoneNumber;
        }
	}


// End of File

