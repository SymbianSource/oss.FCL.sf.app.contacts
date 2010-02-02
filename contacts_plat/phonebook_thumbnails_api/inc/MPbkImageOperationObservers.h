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
*      Phonebook asyncronous image operation observer interfaces.
*
*/


#ifndef __MPbkImageOperationObservers_H__
#define __MPbkImageOperationObservers_H__

//  INCLUDES
#include <e32def.h>

//  FORWARD DECLARATIONS
class MPbkImageOperation;
class CFbsBitmap;
class CPbkImageDataWithInfo;

// CLASS DECLARATION

/**
 * Observer interface for CPbkImageManager get operation.
 * Only one of the event callbacks is called once by the operation.
 *
 * @see CPbkImageManager::GetImageAsyncL
 */
class MPbkImageGetObserver
    {
    public:
        /**
         * Called when image loading is completed.
         *
         * @param aOperation    the completed operation.
         * @param aBitmap       The loaded bitmap. Callee is responsible of 
         *                      the bitmap.
         */
        virtual void PbkImageGetComplete
            (MPbkImageOperation& aOperation, CFbsBitmap* aBitmap) =0;

        /**
         * Called if the image loading fails.
         *
         * @param aOperation    the failed operation.
         * @param aError        error code of the failure.
         */
        virtual void PbkImageGetFailed
            (MPbkImageOperation& aOperation, TInt aError) =0;
    };


/**
 * Observer interface for CPbkImageManager get image operation.
 * Only one of the event callbacks is called once by the operation.
 *
 * @see CPbkImageManager::GetImageAsyncL
 */
class MPbkImageGetImageObserver
    {
    public:
        /**
         * Called when image loading is completed.
         *
         * @param aOperation    the completed operation.
         * @param aImageData    The image image. Caller is responsible
         *                      of the object.
         */
        virtual void PbkImageGetImageComplete
            (MPbkImageOperation& aOperation, CPbkImageDataWithInfo* aImageData) =0;

        /**
         * Called if the image loading fails.
         *
         * @param aOperation    the failed operation.
         * @param aError        error code of the failure.
         */
        virtual void PbkImageGetImageFailed
            (MPbkImageOperation& aOperation, TInt aError) =0;
    };


/**
 * Observer interface for CPbkImageManager set operations.
 * Only one of the event callbacks is called once by the operation.
 *
 * @see CPbkImageManager::SetImageAsyncL
 */
class MPbkImageSetObserver
    {
    public:
        /**
         * Called when image setting is completed.
         *
         * @param aOperation    the completed operation.
         */
        virtual void PbkImageSetComplete
            (MPbkImageOperation& aOperation) =0;

        /**
         * Called if the image setting fails.
         *
         * @param aOperation    the failed operation.
         * @param aError        error code of the failure.
         */
        virtual void PbkImageSetFailed
            (MPbkImageOperation& aOperation, TInt aError) =0;
    };

#endif // __MPbkImageOperationObservers_H__

// End of File
