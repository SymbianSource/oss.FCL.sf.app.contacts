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
* Description: 
*     Class CPbk2ImageManager implementation classes.
*
*/


#ifndef CPBK2IMAGEREADERBASE_H
#define CPBK2IMAGEREADERBASE_H

//  INCLUDES
#include <e32base.h>
#include "CPbk2ImageManagerImpl.h"
#include "MPbk2ImageReaderObserver.h"

//  FORWARD DECLARATIONS
class MPbk2ImageGetObserver;
class CPbk2ImageReader;
class TPbk2ImageManagerParams;


// CLASS DECLARATION

/**
 * Implementation base class for thumbnail readers.
 */
NONSHARABLE_CLASS(CPbk2ImageReaderBase) : 
        public CBase, 
        public MPbk2ImageReader,
        private MPbk2ImageReaderObserver
    {
    protected: // interface
        /**
         * Constructor.
		 * @param aObserver thumbnail get observer
         */
        CPbk2ImageReaderBase(MPbk2ImageGetObserver& aObserver);

        /**
         * Second phase constructor.
         */
        void ConstructL();
   
        /**
         * Destructor. Cancels any previously executing read operation and
         * destroys this object.
         */
        ~CPbk2ImageReaderBase();

        /**
         * Returns the image reader.
         * 
         * @return Image reader
         */
        CPbk2ImageReader& ImageReader()
			{
			return *iImageReader;
			}

    public:  // from MPbkThumbnailReader
        void CancelRead();

    private:  // from MPbkImageReaderObserver
        void ImageReadComplete(CPbk2ImageReader& aReader, CFbsBitmap* aBitmap);
        void ImageReadFailed(CPbk2ImageReader& aReader, TInt aError);
        void ImageOpenComplete(CPbk2ImageReader& aReader);

    private: // data members
        /// Ref: observer
        MPbk2ImageGetObserver& iObserver;
        /// Own: image reader
        CPbk2ImageReader* iImageReader;
    };


#endif // CPBK2IMAGEREADERBASE_H

// End of File
