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


#ifndef __CPbkThumbnailManagerImpl_H__
#define __CPbkThumbnailManagerImpl_H__

//  INCLUDES
#include <e32base.h>
#include <cntdef.h>  // TStorageType
#include <PbkFields.hrh> // EPbkFieldIdThumbnailImage
#include "CPbkThumbnailManager.h"
#include "CPbkImageManager.h"

// CONSTANTS
// Size of thumbnail images stored to contacts
#define KPbkPersonalImageSize TSize(150,150)

//  FORWARD DECLARATIONS
class MPbkThumbnailGetObserver;
class MPbkThumbnailSetObserver;
class CPbkContactItem;
class CPbkFieldsInfo;
class TPbkImageLoadParameters;


// CLASS DECLARATION
/**
 * Abstract thumbnail reader interface.
 */
NONSHARABLE_CLASS(MPbkThumbnailReader) : 
        public MPbkThumbnailOperation, public MPbkImageOperation
    {
    public:  // Interface
        /**
         * Starts reading a thumbnail from the contact specified at creation
         * time.
         * @param aParameters Optional parameters for the thumbnail.
         * 
         * @see TPbkImageLoadParameters
         */
        virtual void StartReadL
            (const TPbkImageLoadParameters* aParameters = NULL) =0;

        /**
         * Cancels reading operation.
         */
        virtual void CancelRead() =0;
    };


/**
 * Abstract thumbnail image writer interface.
 */
NONSHARABLE_CLASS(MPbkThumbnailWriter) : 
        public MPbkThumbnailOperation, public MPbkImageOperation
    {
    public:  // Interface
        /**
         * Starts writing a bitmap to the contact.
         *
         * @param aBitmap   bitmap to save to contact.
         */
        virtual void StartWriteL(const CFbsBitmap& aBitmap) =0;

        /**
         * Starts writing an image buffer to the contact.
         *
         * @param aBuffer   image buffer to save to contact.
         */
        virtual void StartWriteL(const TDesC8& aBuffer) =0;

        /**
         * Starts writing a file image to the contact.
         *
         * @param aFileName   file image to save to contact.
         */
        virtual void StartWriteL(const TDesC& aFileName) = 0;

        /**
         * Cancels writing operation.
         */
        virtual void CancelWrite() =0;
    };


/**
 * Factory class for creating concrete MPbkThumbnailReader and 
 * MPbkThumbnailWriter instances.
 */
NONSHARABLE_CLASS(PbkThumbnailOperationFactory)
    {
    public:
        /**
         * Creates a concrete thumbnail reader object based on the type of 
         * the thumbnail field in the contact.
         *
         * @param aContact  contact to get thumbnail from.
         * @param aObserver observer to notify about progress of the operation.
         * @param aFieldId  Phonebook field type id of fetched image.
         * Default field type id is EPbkFieldIdThumbnailImage.
         * @return  thumbnail reader object or NULL if aContact contains no
         *          thumbnail.
         */
        static MPbkThumbnailReader* CreateReaderLC
            (const CPbkContactItem& aContact,
            MPbkThumbnailGetObserver& aObserver,
            TInt aFieldId = EPbkFieldIdThumbnailImage);

        /**
         * Creates a concrete thumbnail image reader object based on the type 
         * of the thumbnail field in the contact.
         *
         * @param aContact  contact to get thumbnail from.
         * @param aObserver observer to notify about progress of the operation.
         * @param aFieldId  Phonebook field type id of fetched image.
         * Default field type id is EPbkFieldIdThumbnailImage.
         * @return  thumbnail reader object or NULL if aContact contains no
         *          thumbnail.
         */
        static MPbkThumbnailReader* CreateImageReaderLC
            (const CPbkContactItem& aContact,
            MPbkThumbnailGetImageObserver& aObserver,
            TInt aFieldId = EPbkFieldIdThumbnailImage);

        /**
         * Creates a concrete writer object based on the type of the thumbnail
         * field in the contact or in global field typing. 
         *
         * @param aContact      contact to write thumbnail to.
         * @param aFieldsInfo   global Phonebook field types.
         * @param aObserver     observer to notify about progress of the 
         *                      operation.
         * @param aFieldId  Phonebook field type id of fetched image.
         * Default field type id is EPbkFieldIdThumbnailImage.         
         * @return  thumbnail writer object.
         */
        static MPbkThumbnailWriter* CreateWriterLC
            (CPbkContactItem& aContact, 
            const CPbkFieldsInfo& aFieldsInfo,
            MPbkThumbnailSetObserver& aObserver,
            TInt aFieldId = EPbkFieldIdThumbnailImage);

        /**
         * Returns true if aStorageType is supported for thumbnail field.
         */
        static TBool SupportedStorageType(TStorageType aStorageType);

    private:
        /// Namespace class, constructor not implemented.
        PbkThumbnailOperationFactory();
    };

#endif // __CPbkThumbnailManagerImpl_H__

// End of File
