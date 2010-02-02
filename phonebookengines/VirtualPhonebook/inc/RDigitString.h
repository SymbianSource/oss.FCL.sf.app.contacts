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


#ifndef VPBKENGUTILS_RDIGITSTRING_H
#define VPBKENGUTILS_RDIGITSTRING_H

// INCLUDES
#include <e32std.h>

namespace VPbkEngUtils {

// FORWARD DECLARATIONS
class TFixedDigitString;


// CLASS DECLARATIONS

/**
 * Variable-length digit string.
 * Phone number matching helper class: encodes decimal digit characters to BCD 
 * (Binary Coded Decimal) format for efficient matching.
 */
class RDigitString
    {
    public:
        /**
         * Default constructor. Resets this digit string to empty.
         */
        RDigitString();

        /**
         * Releases resources used for storing the digit string. This object
         * must not be used after Close() has been called to it.
         */
        void Close();

        /**
         * Returns the length of this digit string.
         *
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
         * @return  true if aNumDigits digits match, false otherwise. Undefined
         *          if aNumDigits < 0 || aNumDigits > MaxLength().
         */
        TBool Compare(const RDigitString& aOther, TInt aNumDigits) const;

        /**
         * Resets this digit string to empty.
         *
         * @post Length() == 0
         */
        void Reset();

        /**
         * Encodes digit characters from text to this digit string.
         *
         * @param aDigitsText   text to encode decimal digit characters from.
         *                      Nondigit characters are skipped.
         * @exception KErrOverflow  If aDigitsText contains too many digits to
         *                          fit into this data structure. If this
         *                          function leaves this object's state is 
         *                          unchanged.
         * @exception KErrNoMemory  If encoding runs out of memory. If this 
         *                          function leaves this object's state is 
         *                          unchanged.
         */
        void EncodeDigitsL(const TDesC& aDigitsText);

    private:  // Data
        /// Number of active elements in iDigits
        TInt16 iSubCount;
        /// Maxmimum size of the array iDigits points to
        TInt16 iSubCapacity;
        /// Own. Encoded digits
        TFixedDigitString* iDigits;
    };


} // namespace VPbkEngUtils

#endif  // VPBKENGUTILS_RDIGITSTRING_H

