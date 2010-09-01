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


#include "VPbkPhoneNumberIndexImpl.h"

namespace VPbkEngUtils {

// IMPLEMENTATION

// CPhoneNumberIndexBase::REntry

CPhoneNumberIndexBase::REntry::REntry() : 
    iKey(NULL)
    {
    }

void CPhoneNumberIndexBase::REntry::SetL(const TDesC& aPhoneNumber, TAny* aKey)
    {
    iPhoneNumber.EncodeDigitsL(aPhoneNumber);
    iKey = aKey;
    }

void CPhoneNumberIndexBase::REntry::Reset()
    {
    iPhoneNumber.Reset();
    iKey = NULL;
    }

void CPhoneNumberIndexBase::REntry::Swap(REntry& aOther)
    {
    REntry temp(*this);
    *this = aOther;
    aOther = temp;
    }


} // namespace VPbkEngUtils
