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


#ifndef VPBKENGUTILS_TFIXEDDIGITSTRING_H
#define VPBKENGUTILS_TFIXEDDIGITSTRING_H

// INCLUDES
#include <e32std.h>

namespace VPbkEngUtils {

// FORWARD DECLARATIONS


// CLASS DECLARATIONS

/**
 * Fixed-length digit string.
 * Phone number matching helper class: encodes decimal digit characters to BCD 
 * (Binary Coded Decimal) format for efficient matching.
 */
class TFixedDigitString
    {
    public:
        /**
         * Default constructor. Resets this digit string to empty.
         */
        TFixedDigitString();

        /**
         * Returns the length of this digit string.
         * @return the number of digits in this digit string,
         */
        TInt Length() const;

        /**
         * Returns true if this digit string is empty.
         */
        TBool IsEmpty() const;

        /**
         * Returns the maximum number of digits this object may contain.
         */
        static TInt MaxLength();

        /**
         * Returns non-modifiable decimal digit at position aIndex.
         *
         * @pre  aIndex >= 0 && aIndex < Length()
         * @post result >= 0 && result <= 9
         */
        TInt operator[](TInt aIndex) const;

        /**
         * Compares the digits in this string to another digit string.
         *
         * @param aOther    the other digit string to compare this string with.
         * @param aNumDigits    the number of digits to compare. Zero means
         *                      all digits are compared.
         * @pre Result is undefined if aNumDigits < 0 || aNumDigits > MaxLength().
         * @return true if aNumDigits digits match
         */
        TBool Compare(const TFixedDigitString& aOther, TInt aNumDigits) const;

        /**
         * Resets this digit string to empty.
         * @post Length() == 0
         */
        void Reset();

        /**
         * Encodes digit characters from text to this digit string.
         *
         * @param aDigitsText   text to encode decimal digit characters from.
         *                      Nondigit characters are skipped. The encoded 
         *                      and skipped characters are removed from the 
         *                      parameter. At maximum MaxLength() digits
         *                      are encoded.
         */
        void EncodeDigits(TPtrC& aDigitsText);

    private:  // Implementation
        enum 
            {
            KBitsPerDigit = 4,
            KMaxDigits = 8*sizeof(TUint) / KBitsPerDigit,
            KMaxBits = KMaxDigits * KBitsPerDigit,
            KNaNBCD = 0xf,
            KNaN = -1
            };
        inline TBool DoCompare(const TFixedDigitString& aOther, TInt aNumDigits) const;

    private:  // Data
        /// Own. Bcd digits to be compared
        TUint iBcdDigits;
    };


// INLINE FUNCTIONS

inline TInt TFixedDigitString::MaxLength()
    {
    return KMaxDigits;
    }

/**
 * Inline implementation of Compare.
 */
inline TBool TFixedDigitString::DoCompare
        (const TFixedDigitString& aOther, TInt aNumDigits) const
    {
    const TUint mask = ~TUint(0) >> ((KBitsPerDigit*(KMaxDigits - aNumDigits)) % (KMaxBits));
    if ((iBcdDigits ^ aOther.iBcdDigits) & mask)
        return EFalse;
    else
        return ETrue;
    }

#ifdef NDEBUG
// Inline version of TFixedDigitString::Compare for non-debug builds
inline TBool TFixedDigitString::Compare
        (const TFixedDigitString& aOther, TInt aNumDigits) const
    {
    return DoCompare(aOther, aNumDigits);
    }
#endif


} // namespace VPbkEngUtils

#endif  // VPBKENGUTILS_TFIXEDDIGITSTRING_H

