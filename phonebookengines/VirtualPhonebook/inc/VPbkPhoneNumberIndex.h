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
* Description:  
*
*/


#ifndef VPBKENGUTILS_VPBKPHONENUMBERINDEX_H
#define VPBKENGUTILS_VPBKPHONENUMBERINDEX_H

// INCLUDES
#include <VPbkPhoneNumberIndexBase.h>

namespace VPbkEngUtils {

// CLASS DECLARATIONS

/**
 * Lookup iterator for CPhoneNumberIndex.
 * @see CPhoneNumberIndex<>::StartLookupL
 * @see CPhoneNumberIndexBase::LookupNext
 */
template<typename TKeyType>
class RPhoneNumberLookup : public RPhoneNumberLookupBase
    {
    public:  // Interface
        RPhoneNumberLookup();

        /**
         * Returns the key associated with the current match.
         * @pre !AtEnd()
         */
        TKeyType& Current() const;

    public:  // Internal interface
        RPhoneNumberLookup( TInt aNumDigits );
    };

/**
 * Phone number index. Associates keys with phone numbers.
 *
 * @param TKeyType  type of the key objects stored to this index. Only
 *                  references to the keys are stored to the index.
 */
template<typename TKeyType>
class CPhoneNumberIndex : public CPhoneNumberIndexBase
    {
    public:  // Constructor and destructor
        static CPhoneNumberIndex<TKeyType>* NewL();
        ~CPhoneNumberIndex();

    public:  // New functions
        /**
         * Starts a lookup by phone number for this index.
         *
         * @param aPhoneNumber  phone number to look up.
         * @param aNumDigits    number of digits to compare from the end of the
         *                      phone number.
         * @return  lookup object that is used to iterate over the matches. The
         *          returned object is pushed on the cleanup stack.
         * @see RPhoneNumberLookup<TKeyType>
         * @see LookupNext
         */
        RPhoneNumberLookup<TKeyType> StartLookupL( const TDesC& aPhoneNumber, 
                                                   TInt aNumDigits ) const;

        /**
         * Adds a new (phone number, key) pair to this index.
         *
         * @param aPhoneNumber  the phone number to add and associate with 
         *                      the key. Only the digit characters are used.
         * @param aKey          the key reference to associate with the phone number.
         */
        void AddL( const TDesC& aPhoneNumber, TKeyType& aKey );

        /**
         * Removes all associations to a specific key.
         *
         * @param aKey  the key reference to remove completely from this index.
         */
        void RemoveAll( TKeyType& aKey );
    };


template<typename TKeyType>
RPhoneNumberLookup<TKeyType>::RPhoneNumberLookup() :
    RPhoneNumberLookupBase(0)
    {
    }

template<typename TKeyType>
RPhoneNumberLookup<TKeyType>::RPhoneNumberLookup( TInt aNumDigits ) :
    RPhoneNumberLookupBase( aNumDigits )
    {
    }

template<typename TKeyType> inline
TKeyType& RPhoneNumberLookup<TKeyType>::Current() const
    {
    return *static_cast<TKeyType*>( RPhoneNumberLookupBase::Current() );
    }

template<typename TKeyType> inline
CPhoneNumberIndex<TKeyType>* CPhoneNumberIndex<TKeyType>::NewL()
    {
    return new (ELeave) CPhoneNumberIndex<TKeyType>;
    }

template<typename TKeyType>
CPhoneNumberIndex<TKeyType>::~CPhoneNumberIndex()
    {
    }

template<typename TKeyType> inline
RPhoneNumberLookup<TKeyType> CPhoneNumberIndex<TKeyType>::StartLookupL
        ( const TDesC& aPhoneNumber, TInt aNumDigits ) const
    {
    RPhoneNumberLookup<TKeyType> result( aNumDigits );
    CPhoneNumberIndexBase::InitLookupL( result, aPhoneNumber );
    return result;
    }

template<typename TKeyType> inline
void CPhoneNumberIndex<TKeyType>::AddL
        ( const TDesC& aPhoneNumber, TKeyType& aKey )
    {
    CPhoneNumberIndexBase::AddL(aPhoneNumber, &aKey );
    }

template<typename TKeyType> inline
void CPhoneNumberIndex<TKeyType>::RemoveAll( TKeyType& aKey )
    {
    CPhoneNumberIndexBase::RemoveAll( &aKey );
    }


} // namespace VPbkEngUtils

#endif  // VPBKENGUTILS_VPBKPHONENUMBERINDEX_H

