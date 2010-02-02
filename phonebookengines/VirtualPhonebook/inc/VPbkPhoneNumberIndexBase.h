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
* Description:  Phonenumber index array base
*
*/


#ifndef VPBKENGUTILS_VPBKPHONENUMBERINDEXBASE_H
#define VPBKENGUTILS_VPBKPHONENUMBERINDEXBASE_H

// INCLUDES
#include <e32base.h>
#include <RFastDigitString.h>

namespace VPbkEngUtils {

// CLASS DECLARATIONS

/**
 * Phone number lookup iterator base class.
 * @see RLookupPhoneNumber<> for client API.
 */
class RPhoneNumberLookupBase
    {
    public:  // Client interface
        /**
         * Releases resources used for storing the digit string.
         */
        IMPORT_C void Close();

        /**
         * Returns true when the complete index has been searched.
         */
        inline TBool AtEnd() const;

    protected:  // Interface for derived classes
        IMPORT_C RPhoneNumberLookupBase(TInt aNumDigits);
        inline TAny* Current() const;

    public:  // Internal interface
        void SetAtEnd();
        TInt CurrentPos() const;
        void SetCurrentState(TInt aPos, TAny* aKey);
        void EncodeDigitsL(const TDesC& aDigitsText);
        const RFastDigitString& PhoneNumber() const;
        TInt NumDigits() const;

    private:  // Data
        /// Phonenumber
        RFastDigitString iPhoneNumber;
        /// Number of digits
        TInt iNumDigits;
        /// Not Own: Current key
        TAny* iCurrentKey;
        /// Current position
        TInt iPos;
    };

/**
 * Phone number index base class.
 * @see CPhoneNumberIndex for client API.
 */
class CPhoneNumberIndexBase : public CBase
    {
    public:  // Client interface
        /**
         * Resets this index to empty.
         */
        IMPORT_C void Reset();
    
        /**
         * Searches the next match in a lookup.
         */
        IMPORT_C void LookupNext(RPhoneNumberLookupBase& aLookup) const;

    protected:  // Interface for derived classes
        IMPORT_C CPhoneNumberIndexBase();
        IMPORT_C ~CPhoneNumberIndexBase();
        IMPORT_C void InitLookupL
            (RPhoneNumberLookupBase& aLookup, const TDesC& aPhoneNumber) const;
        IMPORT_C void AddL(const TDesC& aPhoneNumber, TAny* aKey);
        IMPORT_C void RemoveAll(TAny* aKey);

    private:  // Data
        // REntry forward declaration
        class REntry;
        // Array of REtnry Entries 
        RArray<REntry> iEntries;
        // First free entry
        TInt iFirstFreeEntryIndex;
        // Own: Reversed phone number
        mutable HBufC* iReverseBuffer;
    };


// INLINE FUNCTIONS

inline TBool RPhoneNumberLookupBase::AtEnd() const
    {
    return (iPos < 0);
    }

inline TAny* RPhoneNumberLookupBase::Current() const
    {
    return iCurrentKey;
    }

} // namespace VPbkEngUtils

#endif  // VPBKENGUTILS_VPBKPHONENUMBERINDEXBASE_H
//End of file


