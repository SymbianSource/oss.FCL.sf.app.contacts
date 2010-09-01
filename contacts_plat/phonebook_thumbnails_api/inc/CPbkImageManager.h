/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     Class which loads a image from a image field.
*     This class can be used for image and image for call loading.
*
*/


#ifndef __CPbkImageManager_H__
#define __CPbkImageManager_H__

//  INCLUDES
#include <e32base.h>
#include <PbkFields.hrh> // TPbkFieldId

//  FORWARD DECLARATIONS
class CPbkContactEngine;
class MPbkImageOperation;
class MPbkImageGetObserver;
class CPbkContactItem;
class MPbkImageGetImageObserver;
class MPbkImageSetObserver;
class CFbsBitmap;
class TPbkImageLoadParameters;
class CPbkObserverAdapter;

// CLASS DECLARATIONS

/**
 * Phonebook image manager. Contains operations for attaching, removing and
 * querying images connected to Phonebook contacts.
 *
 * For all thumbnail images all the Set* operations convert the image to full 
 * color (24 bits/pixel) JPEG format and scale it to KPbkPersonalImageSize 
 * before connecting the image to a contact. Scaling does not change image's 
 * aspect ratio.
 * If the original image is in JPEG format and its size is <=KPbkPersonalImageSize
 * a copy of the original image is stored directly.
 *
 * For all call object images all the Set* operations convert the image to full 
 * color (24 bits/pixel) JPEG format and scale it to the total area of the 
 * screen available to the application before connecting the image to a contact. 
 * Scaling does not change image's aspect ratio.
 * If the original image is in JPEG format and its size is <= 
 * the application screen area
 * a copy of the original image is stored directly.
 * 
 * @see MPbkImageGetObserver
 * @see MPbkImageSetObserver
 */
class CPbkImageManager : public CBase
    {
    public: // Interface
        /**
         * Creates and returns a new image manager.
         * If given phonebook field id is something else than 
         * EPbkFieldIdThumbnailImage or EPbkFieldIdCODImageID then
         * "CPbkImageManager 1" panic will occur.
         * @param aEngine   Phonebook contact engine
         * @param aFieldId  Phonebook field type id of fetched image.
         * If id is something else than EPbkFieldIdThumbnailImage or 
         * EPbkFieldIdCodImageID then a EPbkInvalidArgument panic will occur.         
         */
        IMPORT_C static CPbkImageManager* NewL(CPbkContactEngine& aEngine);

        /**
         * Destructor.
         */
        ~CPbkImageManager();

        /**
         * Gets a image bitmap from a contact asynchronously.
         *
         * @param aFieldId  Phonebook field type id of fetched image.
         * @param aObserver     Observer for this operation.
         * @param aContactItem  Contact to retrieve the image from.
         * @return  a image operation object. Caller deletes the object 
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbkImageOperation* GetImageAsyncL(
                TPbkFieldId aFieldId,
                MPbkImageGetObserver& aObserver, 
                const CPbkContactItem& aContactItem);

        /**
         * Gets a parameterized image bitmap
         * from a contact asynchronously.
         *
         * @param aFieldId  Phonebook field type id of fetched image.
         * @param aObserver     Observer for this operation.
         * @param aContactItem  Contact to retrieve the image from.
         * @param aParams       Parameters for bitmap, see TPbkImageLoadParameters.
         *                      NOTE: iFrameNumber parameter has no meaning.
         *                      
         * @return  an image operation object. Caller deletes the object 
         *          when the operation is done or needs to be canceled.
         * @see TPbkImageLoadParameters
         */
        IMPORT_C MPbkImageOperation* GetImageAsyncL(
                TPbkFieldId aFieldId,
                MPbkImageGetObserver& aObserver, 
                const CPbkContactItem& aContactItem,
                TPbkImageLoadParameters& aParams);

        /**
         * Gets a image from a contact asynchronously.
         *
         * @param aFieldId  Phonebook field type id of fetched image.
         * @param aObserver     Observer for this operation.
         * @param aContactItem  Contact to retrieve the image from.
         * @return  an image operation object. Caller deletes the object 
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbkImageOperation* GetImageAsyncL(
                TPbkFieldId aFieldId,
                MPbkImageGetImageObserver& aObserver, 
                const CPbkContactItem& aContactItem);

        /**
         * Sets a image image for a contact asynchronously.
         *
         * @param aFieldId  Phonebook field type id of fetched image.
         * @param aObserver     Observer for this operation.
         * @param aContactItem  Contact to set the image to.
         * @param aBitmap       Bitmap to set as the image for aContact.
         * @return  an image operation object. Caller deletes the object 
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbkImageOperation* SetImageAsyncL(
                TPbkFieldId aFieldId,
                MPbkImageSetObserver& aObserver, 
                CPbkContactItem& aContactItem, 
                const CFbsBitmap& aBitmap);
                    
        /**
         * Sets a image image for a contact asynchronously.
         *
         * @param aFieldId  Phonebook field type id of fetched image.
         * @param aObserver     Observer for this operation.
         * @param aContactItem  Contact to set the image to.
         * @param aImageBuffer  image buffer to set as the image for aContact.
         * @return  an image operation object. Caller deletes the object 
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbkImageOperation* SetImageAsyncL(
                TPbkFieldId aFieldId,
                MPbkImageSetObserver& aObserver, 
                CPbkContactItem& aContactItem, 
                const TDesC8& aImageBuffer);

        /**
         * Sets a image image for a contact asynchronously.
         *
         * @param aFieldId  Phonebook field type id of fetched image.
         * @param aObserver     Observer for this operation.
         * @param aContactItem  Contact to set the image to.
         * @param aFileName     File name for a image to set as the image for aContact.
         * @return  an image operation object. Caller deletes the object 
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbkImageOperation* SetImageAsyncL(
                TPbkFieldId aFieldId,
                MPbkImageSetObserver& aObserver, 
                CPbkContactItem& aContactItem, 
                const TDesC& aFileName);

        /**
         * Returns true if aContactItem has a image image attached to it.
         * @param aFieldId  Phonebook field type id of fetched image.
         * @param aContactItem Contact that is queried for image.
         * @return ETrue if given contact has a image, EFalse otherwise.
         */
        IMPORT_C TBool HasImage(
                TPbkFieldId aFieldId, 
                const CPbkContactItem& aContactItem) const;

        /**
         * Removes any image from aContactItem.
         * @param aFieldId  Phonebook field type id of fetched image.
         * @param aContactItem Contact whose image is removed.
         */
        IMPORT_C void RemoveImage(
                TPbkFieldId aFieldId,
                CPbkContactItem& aContactItem);

    private: // Implementation
        CPbkImageManager(CPbkContactEngine& aEngine);
        void ConstructL();
    
    private: // Data members
        /// Ref: contact engine
        CPbkContactEngine& iEngine;

        /// Owns: handles adaptation of different observer APIs
        CPbkObserverAdapter* iPbkObserverAdapter;
    };

/**
 * Abstract asynchronous image operation. The operation can be cancelled by
 * deleting it.
 *
 * @see CPbkImageManager
 */ 
class MPbkImageOperation
    {
    public:  // Interface
        /**
         * Destructor. Cancels this image operation and releases any 
         * resources held by it.
         */
        virtual ~MPbkImageOperation() = 0;
    };

inline  MPbkImageOperation::~MPbkImageOperation()
    {
    }
    
#endif // __CPbkImageManager_H__

// End of File
