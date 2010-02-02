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
* Description:  Fixed digit string handler class
*
*/


// INCLUDES
#include "TFixedDigitString.h"
#include "Error.h"

namespace VPbkEngUtils {

TFixedDigitString::TFixedDigitString() :
    // Init all digits to KNaNBCD
	iBcdDigits(~TUint(0))
	{
	}

TInt TFixedDigitString::Length() const
    {
    TUint mask = KNaNBCD;
    TInt length = 0;
    while ( (length < KMaxDigits) && ((iBcdDigits & mask) ^ mask))
        {
        mask <<= KBitsPerDigit;
        ++length;
        }
    return length;
    }

TBool TFixedDigitString::IsEmpty() const
    {
    return (iBcdDigits == ~TUint(0));
    }

TInt TFixedDigitString::operator[](TInt aIndex) const
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < KMaxDigits, 
        Error::Panic(Error::EPanicInvalidIndex) );

    const TInt shift = aIndex * KBitsPerDigit;
    // Extract the digit from iBcdDigits
    const TUint result = (iBcdDigits & (0xf << shift)) >> shift;
    if (result != KNaNBCD)
        {
        return TInt(result);
        }
    else
        {
        // aIndex >= Length()
        Error::Panic(Error::EPanicInvalidIndex);
        return KNaN;
        }
    }

#ifndef NDEBUG
// Checking version of Compare for debug builds
TBool TFixedDigitString::Compare(const TFixedDigitString& aOther, TInt aNumDigits) const
    {
    __ASSERT_DEBUG( aNumDigits >= 0 && aNumDigits <= KMaxDigits, 
        Error::Panic(Error::EPanicInvalidValue) );
    return DoCompare(aOther, aNumDigits);
    }
#endif
        
void TFixedDigitString::Reset()
    {
    // Reset all digits to KNaNBCD
    iBcdDigits = ~TUint(0);
    }


void TFixedDigitString::EncodeDigits(TPtrC& aDigitsText)
    {
    // Reset all digits to zero
    iBcdDigits = 0;

    // Init loop counters
    TInt shift = 0;
    TInt textPos = 0;
    const TInt textLen = aDigitsText.Length();

    while (textPos < textLen && shift < KMaxBits)
        {
        const TInt numVal = TChar(aDigitsText[textPos++]).GetNumericValue();
        if (numVal >= 0 && numVal <= 9)
            {
            // Encode the digit to current position
            iBcdDigits |= TUint(numVal) << shift;
            shift += KBitsPerDigit;
            }
        }

    // Set unused digits to KNanBCD
    if (shift < KMaxBits)
        {
        iBcdDigits |= ~TUint(0) << shift;
        }

    // Adjust aDigitsText to point to the first character that was not
    // considered for encoding
    aDigitsText.Set(aDigitsText.Mid(textPos));
    }


} // namespace VPbkEngUtils

//End of file


