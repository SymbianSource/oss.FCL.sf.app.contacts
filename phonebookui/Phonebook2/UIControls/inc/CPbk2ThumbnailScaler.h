/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Class for scaling bitmaps for the thumbnail control.
*
*/


#ifndef CPBK2THUMBNAILSCALER_H
#define CPBK2THUMBNAILSCALER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CFbsBitmap;
class CBitmapScaler;

/**
 * Observer interface for the scaling process.
 */
NONSHARABLE_CLASS(MPbk2ThumbnailScalerObserver)
    {
    public:
        /**
         * Called when the thumbnail scaling completes.
         * @param aError KErrNone if the scaling was done successfully,
         *               otherwise one of the system wide error codes.
         * @param aBitmap Result of the scaling operation
         *                callee takes the ownership.
         */
        virtual void ThumbnailScalingComplete
            (TInt aError, CFbsBitmap* aBitmap) = 0;
    };

/**
 * Class for scaling bitmaps for the thumbnail control.
 */
NONSHARABLE_CLASS(CPbk2ThumbnailScaler) : 
        public CActive
    {
    public:
        /**
         * Static 2-phased constructor.
         * @param aObserver Observer for the scaling process.
         * @return Newly created instance of CPbk2ThumbnailScaler.
         */
        static CPbk2ThumbnailScaler* NewL
            (MPbk2ThumbnailScalerObserver& aObserver);

        /**
         * Standard C++ destructor.
         */
        ~CPbk2ThumbnailScaler();
    public: // interface
        /**
         * Creates thumbnail from the givan bitmap.
         * @param aBitmap Bitmap that will be converted to thumbnail.
         *                This class takes ownership of the bitmap.
         */
        void CreateThumbnail(CFbsBitmap* aBitmap);

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);

    private: // Implementation
        /**
         * C++ constructor.
         * @param aObserver Observer for the scaling process.
         */
        CPbk2ThumbnailScaler(MPbk2ThumbnailScalerObserver& aObserver);

        /**
         * Starts the scaling process.
         * @param aBitmap The bitmap to be scaled.
         * @param aSize Target size of the bitmap.
         */
        void StartScaleL(CFbsBitmap* aBitmap, const TSize& aSize);

        /**
         * Crops the bitmap if needed.
         * @param aBitmap Bitmap to be cropped.
         */
        void DoCropL(CFbsBitmap* aBitmap);
    private: // Data
        /// Ref: Observer for the scaling process.
        MPbk2ThumbnailScalerObserver& iObserver;
        /// Own: Bitmap scaler
        CBitmapScaler* iScaler;
        /// Own: The bitmap that is scaled.
        CFbsBitmap* iBitmap;
    };

#endif // CPBK2THUMBNAILSCALER_H
//End of file
