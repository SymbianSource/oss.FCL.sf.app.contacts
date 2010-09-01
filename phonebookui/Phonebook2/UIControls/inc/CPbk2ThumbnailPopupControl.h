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
* Description:  Phonebook 2 thumbnail popup control.
*
*/


#ifndef CPBK2THUMBNAILPOPUPCONTROL_H
#define CPBK2THUMBNAILPOPUPCONTROL_H

//  INCLUDES
#include <coecntrl.h>
#include "MPbk2ThumbnailPopupControl.h"
#include "CPbk2ThumbnailPopup.h"

// FORWARD DECLARATIONS
class CEikListBox;
class CFbsBitmap;
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATION

/**
 * Phonebook 2 thumbnail popup control.
 */
NONSHARABLE_CLASS(CPbk2ThumbnailPopupControl) :
            public CCoeControl,
            public MPbk2ThumbnailPopupControl
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aThumbnailLocation Location where thumbnail is drawn.
         * @return  A new instance of this class.
         */
        static CPbk2ThumbnailPopupControl* NewL(
            TPbk2ThumbnailLocation aThumbnailLocation,
            const CEikListBox* aListBox );

        /**
         * Destructor.
         */
        virtual ~CPbk2ThumbnailPopupControl();

    public: // From MPbk2ThumbnailPopupControl
        void SetBitmap(CFbsBitmap* aBitmap);
        void SetThumbnailBackg(
                CFbsBitmap* aThumbBackg,
                CFbsBitmap* aThumbBackgMask );
        void MakeControlVisible(
                TBool aVisible );

    private: // From CCoeControl
        void Draw(
                const TRect& aRect) const;
        void HandleResourceChange(TInt aType);

    private:  // Implementation
        CPbk2ThumbnailPopupControl( TPbk2ThumbnailLocation aThumbnailLocation,
                                    const CEikListBox* aListBox );
        void ConstructL();
        void Layout();
        void SetupSkinContext();

    private: // Data
        /// Ref: Bitmap in the popup window
        CFbsBitmap* iBitmap;
        /// Ref: Thumbnail background
        CFbsBitmap* iThumbBackg;
        /// Ref: Thumbnail background mask
        CFbsBitmap* iThumbBackgMask;
        /// Own: Thumbnail location
        TPbk2ThumbnailLocation iThumbnailLocation;
        /// Ref: pointer to a listbox to calculate the window position
        const CEikListBox* iListBox;
        /// Own: skin background context
        CAknsBasicBackgroundControlContext* iBgContext;
        
        TSize iThumbSize;

    };

#endif // CPBK2THUMBNAILPOPUPCONTROL_H


// End of File
