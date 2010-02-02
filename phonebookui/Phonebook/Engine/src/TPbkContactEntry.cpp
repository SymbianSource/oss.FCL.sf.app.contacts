/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*		Phonebook cache entry class
*
*/


// INCLUDE FILES
#include <TPbkContactEntry.h>


// ==================== MEMBER FUNCTIONS ====================

/**
 * Reserves space for a HBufC member.
 * @param aBuf          the buffer member to be enlarged
 * @param aMaxLength    the maximum number of characters the buffer should be
 *                      able to hold. If aMaxLength==0 this function does 
 *                      nothing.
 * @return  a new buffer if aBuf needed to be enlared or aBuf otherwise. If a
 *          new buffer is returned existing aBuf is deleted.
 */
EXPORT_C HBufC* TPbkContactEntry::ReserveL(HBufC* aBuf, TInt aMaxLength)
    {
    HBufC* result = aBuf;
    if (aMaxLength > 0)
        {
        if (aBuf)
            {
            TPtr bufPtr(aBuf->Des());
            if (bufPtr.MaxLength() < aMaxLength)
                {
                // Buffer exists, but not large enough -> ReAlloc
                result = aBuf->ReAllocL(aMaxLength);
                }
            }
        else
            {
            // Buffer does not exist -> allocate
            result = HBufC::NewL(aMaxLength);
            }
        }
    return result;
    }

// End of File
