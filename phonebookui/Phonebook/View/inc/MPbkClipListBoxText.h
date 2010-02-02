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
*       Interface to be used by listbox model for clipping data.
*
*/


#ifndef __MPbkClipListBoxText_H__
#define __MPbkClipListBoxText_H__

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Interface for listbox data string clipping.
 */
class MPbkClipListBoxText
	{
	public: // Interface
        /**
         * Clips data from the beginning of the string.
         *
         * @param   aBuffer         Buffer which contains the string to clip.
                                    As return the result of clipping
         * @param   aItemIndex      Index of the item in model.
         * @param   aSubCellNumber  Subcell or column number of the position. 
         * @return                  ETrue if sting was clipped and three dots
         *                          added to the beginning.
         */	
		virtual TBool ClipFromBeginning(TDes& aBuffer, TInt aItemIndex,
			TInt aSubCellNumber) = 0;
	};
	
#endif // __MPbkClipListBoxText_H__
	
// End of File
