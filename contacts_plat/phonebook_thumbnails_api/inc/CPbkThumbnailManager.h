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
*     Class which loads a thumbnail from a contact field.
*
*/


#ifndef __CPbkThumbnailManager_H__
#define __CPbkThumbnailManager_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS
/// Size of thumbnail images stored to Phonebook contacts
#define KPbkPersonalImageSize TSize(150,150)

//  FORWARD DECLARATIONS
class CPbkContactEngine;
class MPbkThumbnailOperation;
class MPbkThumbnailGetObserver;
class CPbkContactItem;
class MPbkThumbnailGetImageObserver;
class MPbkThumbnailSetObserver;
class CFbsBitmap;
class CPAlbImageData;
class TPbkImageLoadParameters;

// CLASS DECLARATION

/**
 * Phonebook thumbnail manager. Contains operations for attaching, removing and
 * querying thumbnail images connected to Phonebook contacts.
 *
 * All the Set* operations convert the image to full color (24 bits/pixel) 
 * JPEG format and scale it to KPbkPersonalImageSize before connecting the 
 * image to a contact. Scaling does not change image's aspect ratio.
 * If the original image is in JPEG format and its size is <=KPbkPersonalImageSize
 * a copy of the original image is stored directly.
 *
 * @see MPbkThumbnailOperation
 * @see MPbkThumbnailGetObserver
 * @see MPbkThumbnailSetObserver
 */
class CPbkThumbnailManager : public CBase
    {
    public: // Interface
        /**
         * Creates and returns a new thumbnail manager.
         * @param aEngine   Phonebook contact engine
         */
        IMPORT_C static CPbkThumbnailManager* NewL(CPbkContactEngine& aEngine);

        /**
         * Destructor.
         */
        ~CPbkThumbnailManager();

        /**
         * Gets a thumbnail bitmap from a contact asynchronously.
         *
         * @param aObserver     Observer for this operation.
         * @param aContactItem  Contact to retrieve the thumbnail from.
         * @return  a thumbnail operation object. Caller deletes the object 
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbkThumbnailOperation* GetThumbnailAsyncL(
            MPbkThumbnailGetObserver& aObserver, 
            const CPbkContactItem& aContactItem);

        /**
         * Gets a parameterized thumbnail bitmap
         * from a contact asynchronously.
         *
         * @param aObserver     Observer for this operation.
         * @param aContactItem  Contact to retrieve the thumbnail from.
         * @param aParams       Parameters for bitmap, see TPbkImageLoadParameters.
         *                      NOTE: iFrameNumber parameter has no meaning.
         *                      
         * @return  a thumbnail operation object. Caller deletes the object 
         *          when the operation is done or needs to be canceled.
         * @see TPbkImageLoadParameters
         */
        IMPORT_C MPbkThumbnailOperation* GetThumbnailAsyncL(
            MPbkThumbnailGetObserver& aObserver, 
            const CPbkContactItem& aContactItem,
            TPbkImageLoadParameters& aParams);

        /**
         * Gets a thumbnail image from a contact asynchronously.
         *
         * @param aObserver     Observer for this operation.
         * @param aContactItem  Contact to retrieve the thumbnail from.
         * @return  a thumbnail operation object. Caller deletes the object 
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbkThumbnailOperation* GetThumbnailImageAsyncL(
            MPbkThumbnailGetImageObserver& aObserver, 
            const CPbkContactItem& aContactItem);

        /**
         * Sets a thumbnail image for a contact asynchronously.
         *
         * @param aObserver     Observer for this operation.
         * @param aContactItem  Contact to set the thumbnail to.
         * @param aBitmap       Bitmap to set as the thumbnail for aContact.
         * @return  a thumbnail operation object. Caller deletes the object 
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbkThumbnailOperation* SetThumbnailAsyncL(
            MPbkThumbnailSetObserver& aObserver, 
            CPbkContactItem& aContactItem, 
            const CFbsBitmap& aBitmap);

        /**
         * Sets a thumbnail image for a contact asynchronously.
         *
         * @param aObserver     Observer for this operation.
         * @param aContactItem  Contact to set the thumbnail to.
         * @param aImageBuffer  Image buffer to set as the thumbnail for aContact.
         * @return  a thumbnail operation object. Caller deletes the object 
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbkThumbnailOperation* SetThumbnailAsyncL(
            MPbkThumbnailSetObserver& aObserver, 
            CPbkContactItem& aContactItem, 
            const TDesC8& aImageBuffer);

        /**
         * Sets a thumbnail image for a contact asynchronously.
         *
         * @param aObserver     Observer for this operation.
         * @param aContactItem  Contact to set the thumbnail to.
         * @param aFileName     File name for a image to set as the thumbnail for aContact.
         * @return  a thumbnail operation object. Caller deletes the object 
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbkThumbnailOperation* SetThumbnailAsyncL(
            MPbkThumbnailSetObserver& aObserver, 
            CPbkContactItem& aContactItem, 
            const TDesC& aFileName);

        /**
         * Returns true if aContactItem has a thumbnail image attached to it.
         * @param aContactItem Contact that is queried for thumbnail.
         * @return ETrue if given contact has a thumbnail, EFalse otherwise.
         */
        IMPORT_C TBool HasThumbnail(const CPbkContactItem& aContactItem) const;

        /**
         * Removes any thumbnail image from aContactItem.
         * @param aContactItem Contact whose thumbnail is removed.
         */
        IMPORT_C void RemoveThumbnail(CPbkContactItem& aContactItem);

    private: // Implementation
        CPbkThumbnailManager(CPbkContactEngine& aEngine);
        void ConstructL();

    private: // Data members
        /// Ref: contact engine
        CPbkContactEngine& iEngine;
    };


/**
 * Abstract asynchronous thumbnail operation. The operation can be cancelled by
 * deleting it.
 *
 * @see CPbkThumbnailManager
 */ 
class MPbkThumbnailOperation
    {
    public:  // Interface
        /**
         * Destructor. Cancels this thumbnail operation and releases any 
         * resources held by it.
         */
        virtual ~MPbkThumbnailOperation() =0;
    };

inline MPbkThumbnailOperation::~MPbkThumbnailOperation()
    {
    }

#endif // __CPbkThumbnailManager_H__

// End of File
