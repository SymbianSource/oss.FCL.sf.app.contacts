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
* Description:  Phonebook 2 thumbnail popup control interface.
*
*/


#ifndef MPBK2THUMBNAILPOPUPCONTROL_H
#define MPBK2THUMBNAILPOPUPCONTROL_H

// FORWARD DECLARATIONS
class CEikListBox;
class CFbsBitmap;

// CLASS DECLARATION

/**
 * Phonebook 2 thumbnail popup control interface.
 */
class MPbk2ThumbnailPopupControl
    {
    public: // Interface

        /**
         * Virtual destructor.
         */
        virtual ~MPbk2ThumbnailPopupControl()
            {}

        /**
         * Changes the given bitmap to the control.
         * Takes ownership of the bitmap.
         *
         * @param aBitmap   The bitmap to show.
         */
        virtual void SetBitmap(CFbsBitmap* aBitmap) = 0;

        /**
         * Changes the thumbnail background.
         * No ownership transfer.
         *
         * @param aThumbBackg       Thumbnail background.
         * @param aThumbBackgMask   Thumbnail background mask.
         */
        virtual void SetThumbnailBackg(
                CFbsBitmap* aThumbBackg,
                CFbsBitmap* aThumbBackgMask ) = 0;

        /**
         * Shows or hides the control.
         *
         * @param aVisible      If ETrue the control is shown,
         *                      if EFalse it is hidden.
         */
        virtual void MakeControlVisible(
                TBool aVisible ) = 0;
    };

#endif // MPBK2THUMBNAILPOPUPCONTROL_H

// End of File
