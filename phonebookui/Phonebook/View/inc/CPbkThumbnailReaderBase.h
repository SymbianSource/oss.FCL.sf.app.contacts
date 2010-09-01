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


#ifndef __CPbkThumbnailReaderBase_H__
#define __CPbkThumbnailReaderBase_H__

//  INCLUDES
#include <e32base.h>
#include "CPbkThumbnailManagerImpl.h"
#include "MPbkImageReaderObserver.h"

//  FORWARD DECLARATIONS
class MPbkThumbnailGetObserver;
class CPbkImageReader;
class MPbkThumbnailGetImageObserver;
class MPbkImageData;
class CPbkImageDataWithInfo;
class TPbkImageLoadParameters;


// CLASS DECLARATION

/**
 * Implementation base class for thumbnail readers.
 */
NONSHARABLE_CLASS(CPbkThumbnailReaderBase) : 
        public CBase, 
        public MPbkThumbnailReader,
        private MPbkImageReaderObserver
    {
    protected: // interface
        /**
         * Constructor.
		 * @param aObserver thumbnail get observer
         */
        CPbkThumbnailReaderBase(MPbkThumbnailGetObserver& aObserver);

        /**
         * Second phase constructor.
         */
        void ConstructL();
   
        /**
         * Destructor. Cancels any previously executing read operation and
         * destroys this object.
         */
        ~CPbkThumbnailReaderBase();

        /**
         * Returns the image reader.
         */
        CPbkImageReader& ImageReader()
			{
			return *iImageReader;
			}

    public:  // from MPbkThumbnailReader
        void CancelRead();

    private:  // from MPbkImageReaderObserver
        void ImageReadComplete(CPbkImageReader& aReader, CFbsBitmap* aBitmap);
        void ImageReadFailed(CPbkImageReader& aReader, TInt aError);

    private: // data members
        /// Ref: observer
        MPbkThumbnailGetObserver& iObserver;
        /// Own: image reader
        CPbkImageReader* iImageReader;
    };


/**
 * Implementation base class for thumbnail image readers.
 */
NONSHARABLE_CLASS(CPbkThumbnailImageReaderBase) : 
        public CBase, 
        public MPbkThumbnailReader,
        private MPbkImageReaderObserver
    {
    protected: // Interface
        /**
         * Constructor.
         * @param aObserver observer to notify about progress of this 
         *        operation
         */
        CPbkThumbnailImageReaderBase
            (MPbkThumbnailGetImageObserver& aObserver);

        /**
         * Second phase constructor.
         */
        void ConstructL();

        /**
         * Destructor. Cancels any previously executing read operation and
         * destroys this object.
         */
        ~CPbkThumbnailImageReaderBase();

        /**
         * Returns the image reader.
         */
        CPbkImageReader& ImageReader()
			{
			return *iImageReader;
			}

    private:  // Abstract interface for derived classes
        /**
         * Starts image reader.
         * 
         * @param aParameters  Optional parameters for resulting thumbnail
         */
        virtual void DoStartReadL
			(const TPbkImageLoadParameters* aParameters = NULL) =0;

        /**
         * Creates and returns an image data object.
         */
        virtual MPbkImageData* CreateImageDataLC() =0;

    public:  // From MPbkThumbnailReader
        void StartReadL(const TPbkImageLoadParameters* aParameters = NULL);
        void CancelRead();

    private:  // From MPbkImageReaderObserver
        void ImageReadComplete
            (CPbkImageReader& aReader, CFbsBitmap* aBitmap);
        void ImageReadFailed(CPbkImageReader& aReader, TInt aError);
        void ImageOpenComplete(CPbkImageReader& aReader);

    private: // Implementation
        void HandleImageOpenCompleteL();

    private: // data members
        /// Ref: observer
        MPbkThumbnailGetImageObserver& iObserver;
        /// Own: image reader
        CPbkImageReader* iImageReader;
        /// Own: image data
        CPbkImageDataWithInfo* iImageData;
    };

#endif // __CPbkThumbnailReaderBase_H__

// End of File
