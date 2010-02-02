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
* Description:  Virtual phonebook engine utils
*
*/


#include "VPbkEngUtils.h"

namespace VPbkEngUtils {

// FUNCTIONS

void ExpandL(HBufC*& aBuffer, TInt aLength)
	{
    if (!aBuffer)
        {
        aBuffer = HBufC::NewL(aLength);
        }
    TPtr bufPtr(aBuffer->Des());
    const TInt maxLength = bufPtr.MaxLength();
    if (maxLength < aLength)
        {
        aBuffer = aBuffer->ReAllocL(Max(2*maxLength, aLength));
        }
	}

const TDesC& ReverseL(HBufC*& aBuffer, const TDesC& aText)
    {
    const TInt length = aText.Length();
	ExpandL(aBuffer, length);
	TPtr bufPtr(aBuffer->Des());
    bufPtr.Zero();
    for (TInt i = length-1; i >= 0; --i)
        {
        bufPtr.Append(aText[i]);
        }
    return *aBuffer;
    }

/**
 * Counts and returns the number of decimal digit characters in aText.
 */
TInt CountDigits(const TDesC& aText)
    {
    TInt result = 0;
    const TInt length = aText.Length();
    for (TInt textPos = 0; textPos < length; ++textPos)
        {
        const TInt numVal = TChar(aText[textPos]).GetNumericValue();
        if (numVal >= 0 && numVal <= 9)
            {
            ++result;
            }
        }
    return result;
    }

} // namespace VPbkEngUtils

