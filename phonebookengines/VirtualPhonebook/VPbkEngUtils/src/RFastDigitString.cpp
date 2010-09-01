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


#include "RFastDigitString.h"

using namespace VPbkEngUtils;

/**
 * Compares iRestDigits if iLocalizedDigits matches fully.
 * @see Compare
 */
TBool RFastDigitString::CompareRestDigits
        (const RFastDigitString& aOther, TInt aNumDigits) const
    {
    TBool result = ETrue;
    if (aNumDigits == 0)
        {
        // Compare all digits
        result = iRestDigits.Compare(aOther.iRestDigits, 0);
        }
    else
        {
        // Compare what's left
        aNumDigits -= iLocalizedDigits.MaxLength();
        if (aNumDigits > 0)
            {
            result = iRestDigits.Compare(aOther.iRestDigits, aNumDigits);
            }
        }
    return result;
    }

void RFastDigitString::EncodeDigitsL(const TDesC& aDigitsText)
    {
    TFixedDigitString newLocalizedDigits;
    TPtrC digitsPtr(aDigitsText);
    newLocalizedDigits.EncodeDigits(digitsPtr);
    iRestDigits.EncodeDigitsL(digitsPtr);
    iLocalizedDigits = newLocalizedDigits;
    }

//End of file
        
