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
* Description:  Fast digit string handler class
*
*/


#ifndef VPBKENGUTILS_RFASTDIGITSTRING_H
#define VPBKENGUTILS_RFASTDIGITSTRING_H

// INCLUDES
#include <RDigitString.h>
#include <TFixedDigitString.h>

namespace VPbkEngUtils {

// CLASS DECLARATIONS

/**
 * Data-localization optimized variable-length digit string. This class is
 * source-compatible with RDigitString.
 *
 * @see RDigitString
 */
class RFastDigitString
    {
    public:
        /**
         * @see RDigitString::RDigitString
         */
        RFastDigitString();

        /**
         * @see RDigitString::Close
         */
        void Close();

        /**
         * @see RDigitString::Length
         */
        TInt Length() const;

        /**
         * @see RDigitString::IsEmpty
         */
        TBool IsEmpty() const;

        /**
         * @see RDigitString::MaxLength
         */
        static TInt MaxLength();

        /**
         * @see RDigitString::operator[]
         */
        TInt operator[](TInt aIndex) const;

        /**
         * @see RDigitString::Compare
         */
        TBool Compare(const RFastDigitString& aOther, TInt aNumDigits) const;

        /**
         * @see RDigitString::Reset
         */
        void Reset();

        /**
         * @see RDigitString::EncodeDigitsL
         */
        void EncodeDigitsL(const TDesC& aDigitsText);

    private:  // Implementation
        TBool CompareRestDigits
            (const RFastDigitString& aOther, TInt aNumDigits) const;

    private:  // Data
        /// Own. Localized digits
        TFixedDigitString iLocalizedDigits;
        /// Own. Rest of the digits
        RDigitString iRestDigits;
    };

inline RFastDigitString::RFastDigitString()
    {
    }

inline void RFastDigitString::Close()
    {
    iRestDigits.Close();
    iLocalizedDigits.Reset();
    }

inline TInt RFastDigitString::Length() const
    {
    return (iLocalizedDigits.Length() + iRestDigits.Length());
    }

inline TBool RFastDigitString::IsEmpty() const
    {
    return iLocalizedDigits.IsEmpty();
    }

inline TInt RFastDigitString::MaxLength()
    {
    return (TFixedDigitString::MaxLength() + RDigitString::MaxLength());
    }
    
inline TInt RFastDigitString::operator[](TInt aIndex) const
    {
    if (aIndex < iLocalizedDigits.MaxLength())
        {
        return iLocalizedDigits[aIndex];
        }
    else
        {
        return iRestDigits[aIndex - iLocalizedDigits.MaxLength()];
        }
    }

inline TBool RFastDigitString::Compare
        (const RFastDigitString& aOther, TInt aNumDigits) const
    {
    TBool result = iLocalizedDigits.Compare
        (aOther.iLocalizedDigits, Min(aNumDigits, iLocalizedDigits.MaxLength()));
    if (result)
        {
        result = CompareRestDigits(aOther, aNumDigits);
        }
    return result;
    }

inline void RFastDigitString::Reset()
    {
    iRestDigits.Reset();
    iLocalizedDigits.Reset();
    }

} // namespace VPbkEngUtils

#endif  // VPBKENGUTILS_RFASTDIGITSTRING_H

