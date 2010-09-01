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
* Description:  Digit string handler class
*
*/


// INCLUDES
#include "RDigitString.h"
#include "TFixedDigitString.h"
#include "Error.h"
#include "VPbkEngUtils.h"

namespace VPbkEngUtils {

RDigitString::RDigitString() :
    iSubCount(0), iSubCapacity(0), iDigits(NULL)
    {
    }

void RDigitString::Close()
    {
    delete [] iDigits;
    iDigits = NULL;
    iSubCount = iSubCapacity = 0;
    }

TInt RDigitString::Length() const
    {
    TInt result = 0;
    if (iSubCount > 0)
        {
        const TInt lastIndex = iSubCount-1;
        result = lastIndex * TFixedDigitString::MaxLength() + 
            iDigits[lastIndex].Length();
        }
    return result;
    }

TBool RDigitString::IsEmpty() const
    {
    return (iSubCount == 0);
    }

TInt RDigitString::MaxLength()
    {
    return TFixedDigitString::MaxLength() * KMaxTInt16;
    }

TInt RDigitString::operator[](TInt aIndex) const
    {
    const TInt subIndex = aIndex / TFixedDigitString::MaxLength();
    __ASSERT_ALWAYS( subIndex >= 0 && subIndex < iSubCount, 
                     Error::Panic(Error::EPanicInvalidIndex));
    const TInt digitIndex = aIndex % TFixedDigitString::MaxLength();
    return iDigits[subIndex][digitIndex];
    }

TBool RDigitString::Compare(const RDigitString& aOther, TInt aNumDigits) const
    {
    __ASSERT_DEBUG( aNumDigits >= 0 && aNumDigits <= MaxLength(), 
        Error::Panic(Error::EPanicInvalidValue) );

    TInt subIndex = 0;
    for (; subIndex < iSubCount && subIndex < aOther.iSubCount; ++subIndex)
        {
        if ( !iDigits[subIndex].Compare(
                aOther.iDigits[subIndex], 
                Min(aNumDigits, TFixedDigitString::MaxLength())) )
            {
            // Substrings don't match
            return EFalse;
            }
        if (aNumDigits > 0)
            {
            aNumDigits -= TFixedDigitString::MaxLength();
            if (aNumDigits <= 0)
                {
                // All requested digits compared succesfully
                return ETrue;
                }
            }
        }

    // Return true only if both strings ran out of digits to compare at the 
    // same time
    return (subIndex >= iSubCount && subIndex >= aOther.iSubCount);
    }

void RDigitString::Reset()
    {
    // Just reset the count but keep capacity
    iSubCount = 0;
    }

void RDigitString::EncodeDigitsL(const TDesC& aDigitsText)
    {
    // Count digits
    const TInt countDigits = CountDigits(aDigitsText);
    __ASSERT_ALWAYS( countDigits <= MaxLength(), User::Leave(KErrOverflow) );

    // Check capacity and allocate more if required
    const TInt subStringsRequired = 
        (countDigits + TFixedDigitString::MaxLength()-1) / 
            TFixedDigitString::MaxLength();
    if (subStringsRequired > iSubCapacity)
        {
        TFixedDigitString* newDigits = 
            new(ELeave) TFixedDigitString[subStringsRequired];
        delete [] iDigits;
        iDigits = newDigits;
        iSubCapacity = TInt16(subStringsRequired);
        }

    // Encode the digits and update iSubCount
    TPtrC digitsText(aDigitsText);
    iSubCount = 0;
    TFixedDigitString* subString = iDigits; 
    while (iSubCount < subStringsRequired)
        {
        subString->EncodeDigits(digitsText);
        ++subString;
        ++iSubCount;
        }
    }

} // namespace VPbkEngUtils
//End of file

