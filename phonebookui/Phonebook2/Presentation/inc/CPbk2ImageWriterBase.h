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
*     Class CPbk2ImageManager implementation classes.
*
*/


#ifndef CPBK2IMAGEWRITERBASE_H
#define CPBK2IMAGEWRITERBASE_H

//  INCLUDES
#include <e32base.h>
#include "CPbk2ImageManagerImpl.h"
#include "MPbk2ImageReaderObserver.h"

//  FORWARD DECLARATIONS
class CImageEncoder;
class CBitmapScaler;
class MPbk2ImageData;
//class TPbkImageLoadParameters;

// CLASS DECLARATION

/**
 * Implementation base class for image writers.
 */
NONSHARABLE_CLASS(CPbk2ImageWriterBase) : 
        public CActive, 
        public MPbk2ImageWriter,
        public MPbk2ImageReaderObserver
    {
    protected: // interface
        /**
         * Constructor.
         * @param aObserver thumbnail set observer
         */
        CPbk2ImageWriterBase(MPbk2ImageSetObserver& aObserver);

        /**
         * Second phase constructor.
         */
        void ConstructL();

        /**
         * Destructor. Cancels any previously executing read operation and
         * destroys this object.
         */
        ~CPbk2ImageWriterBase();

    private:  // Abstract interface for derived classes
        /**
         * Creates an image writer for the thumbnail image to store.
         *
         * @return  New image writer object. Caller takes ownership of the 
         *          returned object.
         */
        virtual CImageEncoder* CreateImageWriterL() =0;

        /**
         * Stores image in aImageData as the thumbnail image.
         *
         * @param aImageData Images data
         */
        virtual void StoreImageL(const MPbk2ImageData& aImageData) =0;

        /**
         * Stores image created with writer returned by CreateImageWriterL().
         * @param aImageWriter image writer
         */
        virtual void StoreImageL(CImageEncoder& aImageWriter) =0;

        /**
         * Returns the image size
         * @return Image size
         */
         virtual TSize ImageSize() const = 0;

    public:  // from MPbkThumbnailWriter
        void StartWriteL(const CFbsBitmap& aBitmap);
        void StartWriteL(const TDesC8& aBuffer);
        void StartWriteL(const TDesC& aFileName);
        void CancelWrite();

    private:  // from MPbkImageReaderObserver
        void ImageReadComplete(CPbk2ImageReader& aReader, CFbsBitmap* aBitmap);
        void ImageReadFailed(CPbk2ImageReader& aReader, TInt aError);
        void ImageOpenComplete(CPbk2ImageReader& aReader);

    protected: // From CActive
        void DoCancel();

    private: // From CActive
        void RunL();
        TInt RunError(TInt aError);

    private:  // Implementation
        void NextState();
        void ExecuteStateL();
        void CheckFormatAndSizeL();
        void ScaleBitmapL();
        void CreateImageL();
        void ConvertToJpegL();
        void StoreImageL();
        void Complete();
        static TInt Complete(TAny* aSelf);
        void SetupReaderParams(TPbk2ImageManagerParams& aParams);
        
    private: // data members
        /// Ref: observer
        MPbk2ImageSetObserver& iObserver;
        /// Own: current state
        TInt iState;
        /// Own: image reader
        CPbk2ImageReader* iImageReader;
        /// Own: input image data
        MPbk2ImageData* iImageData;
        /// Own: bitmap to store
        CFbsBitmap* iBitmap;
        /// Own: bitmap scaler
        CBitmapScaler* iBitmapScaler;
        /// Own: image writer
        CImageEncoder* iImageEncoder;
    };

#endif // CPBK2IMAGEWRITERBASE_H

// End of File
