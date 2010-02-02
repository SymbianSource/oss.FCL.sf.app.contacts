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
*     Class CPbkThumbnailManager implementation classes.
*
*/


#ifndef MPBK2IMAGEREADEROBSERVER_H
#define MPBK2IMAGEREADEROBSERVER_H

//  INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class CPbk2ImageReader;
class CFbsBitmap;

// CLASS DECLARATION

/**
 * Observer interface for CPbkThumbnailReader.
 */
class MPbk2ImageReaderObserver
	{
    public:
        /**
         * Called by CPbkImageReader when image read is complete.
         *
         * @param aReader   the completed reader.
         * @param aBitmap   the read image.
         */
        virtual void ImageReadComplete
            (CPbk2ImageReader& aReader, CFbsBitmap* aBitmap) =0;

        /**
         * Called by CPbkImageReader if image reading fails.
         *
         * @param aReader   the failed reader.
         * @param aError    error code of the failure.
         */
        virtual void ImageReadFailed(CPbk2ImageReader& aReader, TInt aError) =0;

        /**
         * Called by CPbkImageReader when image open is complete.
         * After this event CPbkImageReader functions FrameCount() and 
         * FrameInfo() can be called.
         * Default implementation is empty as most clients are interested of 
         * ImageReadComplete/Failed events only.
         *
         * @param aReader   the completed reader.
         */
        virtual void ImageOpenComplete(CPbk2ImageReader& /*aReader*/)
			{
			}
    };

#endif // MPBK2IMAGEREADEROBSERVER_H

// End of File
