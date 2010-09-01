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
* Description:  Phonebook 2 list box data clipping interface.
*
*/


#ifndef MPBK2CLIPLISTBOXTEXT_H
#define MPBK2CLIPLISTBOXTEXT_H

// INCLUDE FILES
#include <e32std.h>

// CLASS DECLARATION

/**
 * Phonebook 2 list box data clipping interface.
 */
class MPbk2ClipListBoxText
    {
    public: // Interface

        /**
         * Clips data from the beginning of the string.
         *
         * @param aBuffer           Buffer which contains the
         *                          string to clip. At return
         *                          contains the result of the clipping.
         * @param aItemIndex        Index of the item in the model.
         * @param aSubCellNumber    Subcell or column number
         *                          of the position.
         * @return  ETrue if the string was clipped and three dots
         *          added to the beginning of it.
         */
        virtual TBool ClipFromBeginning(
                TDes& aBuffer,
                TInt aItemIndex,
                TInt aSubCellNumber ) = 0;
    };

#endif // MPBK2CLIPLISTBOXTEXT_H

// End of File
