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
* Description:  Phonenumber index array implementation
*
*/


#ifndef VPBKPHONENUMBERINDEXIMPL_H
#define VPBKPHONENUMBERINDEXIMPL_H

// INCLUDES
#include "VPbkPhoneNumberIndex.h"

namespace VPbkEngUtils {

// CLASS DECLARATION

/**
 * Internal entry class for CPhoneNumberIndexImpl.
 *
 * @see CPhoneNumberIndexImpl
 */
NONSHARABLE_CLASS( CPhoneNumberIndexBase::REntry )
    {
    public:
        /**
         * C++ constructor.
         */
        REntry();

        /**
         * Releases resources held by this entry
         */
        void Close();

        /**
         * Sets the contents of this entry.
         *
         * @param aPhoneNumber  the phone number to attach to this entry.
         * @param aKey          the key to associate with the phone number.
         */
        void SetL(const TDesC& aPhoneNumber, TAny* aKey);

        /**
         * Resets the contents of this entry.
         */
        void Reset();

        /**
         * Swaps this entry's contents with another entry.
         */
        void Swap(REntry& aOther);

        /**
         * @see RDigitString::Compare
         */
        TBool Compare(const RFastDigitString& aPhoneNumber, TInt aNumDigits) const;

        /**
         * Returns the key associated to this entry.
         */
        TAny* Key() const;

    private:  // Data
        RFastDigitString iPhoneNumber;
        TAny* iKey;
    };


// INLINE FUNCTIONS

// CPhoneNumberIndexImpl::RLookup
inline void RPhoneNumberLookupBase::SetAtEnd()
    {
    iPos = KErrNotFound;
    }

inline TInt RPhoneNumberLookupBase::CurrentPos() const
    {
    return iPos;
    }

inline void RPhoneNumberLookupBase::SetCurrentState
        (TInt aPos, TAny* aKey)
    {
    iPos = aPos;
    iCurrentKey = aKey;
    }

inline void RPhoneNumberLookupBase::EncodeDigitsL
        (const TDesC& aDigitsText)
    {
    iPhoneNumber.EncodeDigitsL(aDigitsText);
    }

inline const RFastDigitString& RPhoneNumberLookupBase::PhoneNumber() const
    {
    return iPhoneNumber;
    }

inline TInt RPhoneNumberLookupBase::NumDigits() const
    {
    return iNumDigits;
    }


// CPhoneNumberIndexImpl::REntry
inline void CPhoneNumberIndexBase::REntry::Close()
    {
    iPhoneNumber.Close();
    }

inline TBool CPhoneNumberIndexBase::REntry::Compare
        (const RFastDigitString& aPhoneNumber, TInt aNumDigits) const
    {
    return iPhoneNumber.Compare(aPhoneNumber, aNumDigits);
    }

inline TAny* CPhoneNumberIndexBase::REntry::Key() const
    {
    return iKey;
    }


} // namespace VPbkEngUtils

#endif //VPBKPHONENUMBERINDEXIMPL_H
//End of file


