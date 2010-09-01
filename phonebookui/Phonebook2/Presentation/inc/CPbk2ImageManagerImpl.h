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


#ifndef CPBK2IMAGEMANAGERIMPL_H
#define CPBK2IMAGEMANAGERIMPL_H

//  INCLUDES
#include "CPbk2ImageManager.h"
#include "VPbkFieldType.hrh" // TVPbkFieldStorageType

// CONSTANTS
// Size of thumbnail images stored to contacts
#define KPbk2PersonalImageSize TSize(150,150)

//  FORWARD DECLARATIONS
class MPbk2ImageGetObserver;
class MPbk2ImageSetObserver;
class CVPbkContactManager;
class MVPbkBaseContactField;


// CLASS DECLARATION
/**
 * Abstract thumbnail reader interface.
 */
class MPbk2ImageReader : 
        public MPbk2ImageOperation
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
            (const TPbk2ImageManagerParams* aParameters = NULL) =0;

        /**
         * Cancels reading operation.
         */
        virtual void CancelRead() =0;
    };


/**
 * Abstract thumbnail image writer interface.
 */
class MPbk2ImageWriter : 
        public MPbk2ImageOperation
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
 * Factory class for creating concrete MPbk2ImageReader and 
 * MPbk2ImageWriter instances.
 */
class Pbk2ImageOperationFactory
    {
    public:
        /**
         * Creates a concrete image reader object based on the type of 
         * the thumbnail field in the contact.
         *
         * @param aContactManager Contact manager
         * @param aContact  contact to get thumbnail from.
         * @param aObserver observer to notify about progress of the operation.
         * @param aFieldType  Phonebook field type of fetched image.
         * Default field type is EPbkFieldIdThumbnailImage.
         * @return  thumbnail reader object or NULL if aContact contains no
         *          thumbnail.
         */
        static MPbk2ImageReader* CreateReaderLC
            (CVPbkContactManager& aContactManager,
            const MVPbkBaseContact& aContact,
            MPbk2ImageGetObserver& aObserver,
            const MVPbkFieldType& aFieldType);

        /**
         * Creates a concrete writer object based on the type of the thumbnail
         * field in the contact or in global field typing. 
         *
         * @param aContactManager Contact manager
         * @param aContact      contact to write thumbnail to.
         * @param aObserver     observer to notify about progress of the 
         *                      operation.
         * @param aFieldType  Phonebook field type of fetched image.
         * Default field type is EPbkFieldIdThumbnailImage.         
         * @return  thumbnail writer object.
         */
        static MPbk2ImageWriter* CreateWriterLC
            (CVPbkContactManager& aContactManager,
            MVPbkStoreContact& aContact, 
            MPbk2ImageSetObserver& aObserver,
            const MVPbkFieldType& aFieldType);

        /**
         * Returns true if aStorageType is supported for thumbnail field.
         *
         * @param aField    Contact field 
         * @return ETrue if field is supported, other EFalse
         */
        static TBool SupportedStorageType(const MVPbkBaseContactField* aField);

    private:
        /// Namespace class, constructor not implemented.
        Pbk2ImageOperationFactory();
    };

#endif // CPBK2IMAGEMANAGERIMPL_H

// End of File
