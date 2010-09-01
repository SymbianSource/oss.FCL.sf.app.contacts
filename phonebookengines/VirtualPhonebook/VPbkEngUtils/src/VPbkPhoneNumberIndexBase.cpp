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



// INCLUDES
#include "VPbkPhoneNumberIndexBase.h"
#include "VPbkPhoneNumberIndexImpl.h"
#include "VPbkEngUtils.h"

namespace VPbkEngUtils {

// IMPLEMENTATION

EXPORT_C CPhoneNumberIndexBase::CPhoneNumberIndexBase()
    {
    }

EXPORT_C CPhoneNumberIndexBase::~CPhoneNumberIndexBase()
    {
    Reset();
    delete iReverseBuffer;
    }

EXPORT_C void CPhoneNumberIndexBase::InitLookupL
        (RPhoneNumberLookupBase& aLookup, const TDesC& aPhoneNumber) const
    {
    aLookup.EncodeDigitsL(ReverseL(iReverseBuffer, aPhoneNumber));
    aLookup.SetCurrentState(0, NULL);
    // Make the initial lookup
    LookupNext(aLookup);
    }

EXPORT_C void CPhoneNumberIndexBase::LookupNext(RPhoneNumberLookupBase& aLookup) const
    {
    if (aLookup.AtEnd())
        {
        return;
        }

    for (TInt i = aLookup.CurrentPos(); i < iFirstFreeEntryIndex; ++i)
        {
        const REntry& entry = iEntries[i];
        if (entry.Compare(aLookup.PhoneNumber(), aLookup.NumDigits()))
            {
            aLookup.SetCurrentState(i+1, entry.Key());
            return;
            }
        }

    // No matches found
    aLookup.SetAtEnd();
    }

EXPORT_C void CPhoneNumberIndexBase::AddL(const TDesC& aPhoneNumber, TAny* aKey)
    {
    const TDesC& reversedPhoneNumber = ReverseL(iReverseBuffer, aPhoneNumber);

    if (iFirstFreeEntryIndex < iEntries.Count())
        {
        // Reuse the first free entry and move the free entry index forward
        iEntries[iFirstFreeEntryIndex++].SetL(reversedPhoneNumber, aKey);
        }
    else
        {
        REntry newEntry;
        newEntry.SetL(reversedPhoneNumber, aKey);
        const TInt err = iEntries.Append(newEntry);
        if (err != KErrNone)
            {
            newEntry.Close();
            User::Leave(err);
            }
        else
            {
            ++iFirstFreeEntryIndex;
            }
        }
    }

EXPORT_C void CPhoneNumberIndexBase::RemoveAll(TAny* aKey)
    {
    for (TInt i = 0; i < iFirstFreeEntryIndex; ++i)
        {
        REntry& entry = iEntries[i];
        while (entry.Key() == aKey)
            {
            // Reset the removed entry
            entry.Reset();
            // Swap the removed element with the last non-free element
            entry.Swap(iEntries[--iFirstFreeEntryIndex]);
            }
        }
    }

EXPORT_C void CPhoneNumberIndexBase::Reset()
    {
    for (TInt i = iEntries.Count()-1; i >= 0; --i)
        {
        iEntries[i].Close();
        }
    iEntries.Reset();
    // Entries should be rebuild so clear this also.
    iFirstFreeEntryIndex = 0;
    }

EXPORT_C RPhoneNumberLookupBase::RPhoneNumberLookupBase(TInt aNumDigits) :
    iNumDigits(aNumDigits)
    {
    }

EXPORT_C void RPhoneNumberLookupBase::Close()
    {
    iPhoneNumber.Close();
    }

} // namespace VPbkEngUtils

