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
*     Phonebook thumbnail popup window control class definition.
*
*/


#ifndef __CPbkThumbnailPopupControl_H__
#define __CPbkThumbnailPopupControl_H__

//  INCLUDES
#include <coecntrl.h> // CCoeControl
#include <calslbs.h>  // TAknLayoutRect
#include "MPbkThumbnailPopupControl.h"
// FORWARD DECLARATIONS
class CFbsBitmap;
/// Use for testing only!!!
class CPbkThumbnailPopupControl_TestAccess;

// CLASS DECLARATION

/**
 *  Control for thumbnail popup window.
 */
NONSHARABLE_CLASS(CPbkThumbnailPopupControl) 
		: public CCoeControl,
		  public MPbkThumbnailPopupControl
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         */
        static CPbkThumbnailPopupControl* NewL();
        
        /**
         * Destructor.
         */
        virtual ~CPbkThumbnailPopupControl();

    public: // interface
        /**
         * Change the bitmap aBitmap in the control.
         * Takes ownership of the bitmap
         */
        void ChangeBitmap(CFbsBitmap* aBitmap, CEikListBox* aListBox );
   
        void SetThumbnailBackg( CFbsBitmap* aThumbBackg, CFbsBitmap* aThumbBackgMask );
        
        void MakeControlVisible( TBool aVisible );
        
    private: // from CCoeControl
        /**
         * Draw this control
         */
    	void Draw(const TRect& aRect) const;

    private:  // Implementation
        /**
         * Standard C++ constructor.
         */
        CPbkThumbnailPopupControl();

        /**
         * Performs the 2nd phase of the construction.
         */
        void ConstructL();
        
        /// Use for testing only!!!
        friend class CPbkThumbnailPopupControl_TestAccess;

    private:    // Data
        /// Ref: Bitmap in the popup window
        CFbsBitmap* iBitmap;
        /// Own: bitmap crop amount
        TPoint iCrop;
        /// Own: Controls size
        TSize iControlSize;
        /// Ref: Thumbnail background
        CFbsBitmap* iThumbBackg;
        /// Ref: Thumbnail background mask
        CFbsBitmap* iThumbBackgMask;
        /// Own: Shadow width
        TInt iShadowWidth;
        /// Own: Shadow height
        TInt iShadowHeight;
        
    };

#endif // __CPbkThumbnailPopupControl_H__
            
// End of File
