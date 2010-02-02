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
* Description:  Phonebook 2 image manager.
*
*/


#ifndef CPBK2IMAGEMANAGER_H
#define CPBK2IMAGEMANAGER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkFieldType;
class MVPbkStoreContact;
class MVPbkBaseContact;
class MPbk2ImageOperation;
class MPbk2ImageGetObserver;
class MPbk2ImageSetObserver;
class CFbsBitmap;
class TPbk2ImageManagerParams;
class CVPbkContactManager;
class MVPbkBaseContactFieldCollection;


// CLASS DECLARATIONS

/**
 * Phonebook 2 image manager. Contains operations for attaching, removing and
 * querying images connected to Phonebook 2 contacts.
 *
 * For all thumbnail images all the Set* operations convert the image to full
 * color (24 bits/pixel) JPEG format and scale it to KPbk2PersonalImageSize
 * before connecting the image to a contact. Scaling does not change image's
 * aspect ratio.
 * If the original image is in JPEG format and its size is
 *  <=KPbk2PersonalImageSize
 * a copy of the original image is stored directly.
 *
 * For all call object images all the Set* operations convert the image to
 * full color (24 bits/pixel) JPEG format and scale it to the total area of
 * the screen available to the application before connecting the image to a
 * contact. Scaling does not change image's aspect ratio.
 * If the original image is in JPEG format and its size is <=
 * the application screen area
 * a copy of the original image is stored directly.
 *
 * @see MPbk2ImageGetObserver
 * @see MPbk2ImageSetObserver
 */
class CPbk2ImageManager : public CBase
    {
    public: // Constructor and destructor

        /**
         * Creates new instance of this class.
         *
         * @param aContactManager   Contact manager
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2ImageManager* NewL(
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2ImageManager();

    public: // Interface

        /**
         * Gets a image bitmap from a contact asynchronously.
         *
         * @param aParams       Operation parameters. Not used if NULL.
         * @param aContact      A contact from where the image will be get.
         * @param aFieldType    A field type from where the image
         *                      will be get.
         * @param aObserver     Observer for this operation.
         * @return  An image operation object. Caller deletes the object
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbk2ImageOperation* GetImageAsyncL(
                TPbk2ImageManagerParams* aParams,
                MVPbkBaseContact& aContact,
                const MVPbkFieldType& aFieldType,
                MPbk2ImageGetObserver& aObserver );

        /**
         * Sets a image for a contact asynchronously.
         *
         * @param aContact      A contact to where the image will be set.
         * @param aFieldType    A field type to where the image will be set.
         * @param aObserver     Observer for this operation.
         * @param aBitmap       Bitmap to set as the image for contact.
         * @return  An image operation object. Caller deletes the object
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbk2ImageOperation* SetImageAsyncL(
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType,
                MPbk2ImageSetObserver& aObserver,
                const CFbsBitmap& aBitmap );

        /**
         * Sets a image for a contact asynchronously.
         *
         * @param aContact      A contact to where the image will be set.
         * @param aFieldType    A field type to where the image will be set.
         * @param aObserver     Observer for this operation.
         * @param aImageBuffer  Image buffer to set as the image for contact.
         * @return  a image operation object. Caller deletes the object
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbk2ImageOperation* SetImageAsyncL(
            MVPbkStoreContact& aContact,
            const MVPbkFieldType& aFieldType,
            MPbk2ImageSetObserver& aObserver,
            const TDesC8& aImageBuffer);

        /**
         * Sets a image for a contact asynchronously.
         *
         * @param aContact      A contact to where the image will be set.
         * @param aFieldType    A field type to where the image will be set.
         * @param aObserver     Observer for this operation.
         * @param aFileName     File name for a image to set as the image
         *                      for contact.
         * @return  An image operation object. Caller deletes the object
         *          when the operation is done or needs to be canceled.
         */
        IMPORT_C MPbk2ImageOperation* SetImageAsyncL(
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType,
                MPbk2ImageSetObserver& aObserver,
                const TDesC& aFileName );

        /**
         * Returns true if a contact has an image attached to it in
         * a specified field.
         *
         * @param aContact      Contact that is queried for the image.
         * @param aFieldType    A field type from where to look
         *                      for the image.
         * @return  ETrue if given contact has an image in the
         *          specified field, EFalse otherwise.
         */
        IMPORT_C TBool HasImage(
                const MVPbkBaseContact& aContact,
                const MVPbkFieldType& aFieldType ) const;

        /**
         * Removes any image from a contact at the specified field.
         *
         * @param aContact      Contact whose image is removed.
         * @param aFieldType    Field type from where the image
         *                      should be removed.
         */
        IMPORT_C void RemoveImage(
                MVPbkStoreContact& aContact,
                const MVPbkFieldType& aFieldType );

    private: // Implementation
        CPbk2ImageManager(
                CVPbkContactManager& aContactManager );
        void ConstructL();

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
    };

/**
 * Abstract asynchronous image operation. The operation can be cancelled by
 * deleting it.
 *
 * @see CPbk2ImageManager
 */
class MPbk2ImageOperation
    {
    public: // Interface

        /**
         * Destructor. Cancels this image operation and releases any
         * resources held by it.
         */
        virtual ~MPbk2ImageOperation()
                {};
    };

#endif // CPBK2IMAGEMANAGER_H

// End of File
