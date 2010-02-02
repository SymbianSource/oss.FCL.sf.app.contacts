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
*     Class CPbkThumbnailManager implementation classes.
*
*/


#ifndef __CPbkThumbnailWriterBase_H__
#define __CPbkThumbnailWriterBase_H__

//  INCLUDES
#include <e32base.h>
#include "CPbkThumbnailManagerImpl.h"
#include "MPbkImageReaderObserver.h"

//  FORWARD DECLARATIONS
class CImageEncoder;
class CBitmapScaler;
class MPbkImageData;
class TPbkImageLoadParameters;

// CLASS DECLARATION

/**
 * Implementation base class for image writers.
 */
NONSHARABLE_CLASS(CPbkThumbnailWriterBase) : 
        public CActive, 
        public MPbkThumbnailWriter,
        public MPbkImageReaderObserver
    {
    protected: // interface
        /**
         * Constructor.
         * @param aObserver thumbnail set observer
         * @param aFieldId Field id of image
         */
        CPbkThumbnailWriterBase(MPbkThumbnailSetObserver& aObserver);

        /**
         * Second phase constructor.
         */
        void ConstructL();

        /**
         * Destructor. Cancels any previously executing read operation and
         * destroys this object.
         */
        ~CPbkThumbnailWriterBase();

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
         */
        virtual void StoreImageL(const MPbkImageData& aImageData) =0;

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
        void ImageReadComplete(CPbkImageReader& aReader, CFbsBitmap* aBitmap);
        void ImageReadFailed(CPbkImageReader& aReader, TInt aError);
        void ImageOpenComplete(CPbkImageReader& aReader);

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
        void SetupReaderParams(TPbkImageLoadParameters& aParams);

    private: // data members
        /// Ref: observer
        MPbkThumbnailSetObserver& iObserver;
        /// Own: current state
        TInt iState;
        /// Own: image reader
        CPbkImageReader* iImageReader;
        /// Own: input image data
        MPbkImageData* iImageData;
        /// Own: bitmap to store
        CFbsBitmap* iBitmap;
        /// Own: bitmap scaler
        CBitmapScaler* iBitmapScaler;
        /// Own: image writer
        CImageEncoder* iImageEncoder;
    };

#endif // __CPbkThumbnailWriterBase_H__

// End of File
