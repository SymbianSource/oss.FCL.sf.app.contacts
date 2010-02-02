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

// Generic Virtual Phonebook utility classes and functions

#ifndef VPBKENGUTILS_H
#define VPBKENGUTILS_H

#include <e32std.h>

namespace VPbkEngUtils {

// FUNCTIONS

    /**
     * Allocates or reallocates a buffer descriptor.
     *
     * @param aBuffer	the buffer descriptor to allocate or reallocate.
     * @param aLength	the minimum length required for the buffer descriptor.
     */
    void ExpandL(HBufC*& aBuffer, TInt aLength);

    /**
     * Reverses a descriptor.
     *
     * @param aBuffer   buffer to use for reversing. The buffer is allocated or
     *                  expaneded if needed.
     * @param aText     the descriptor to reverse.
     * @return the reversed descriptor.
     */
    const TDesC& ReverseL(HBufC*& aBuffer, const TDesC& aText);

    /**
     * Counts and returns the number of decimal digit characters in aText.
     */
    TInt CountDigits(const TDesC& aText);

} // namespace VPbkEngUtils


#endif //VPBKENGUTILS_H
//End of file


