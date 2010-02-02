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
*     Image data abstraction hierarchy.
*
*/


#ifndef __PbkImageData_H__
#define __PbkImageData_H__

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

//  FORWARD DECLARATIONS
class CPbkAttachmentFile;

// CLASS DECLARATIONS

/**
 * Abstract thumbnail image data interface. Provides polymorphism between 
 * images stored in a file or buffer.
 */
class MPbkImageData
    {
    public:
        /**
         * Virtual destructor.
         */
        virtual ~MPbkImageData()
			{
			}

        /**
         * Returns the image data in a buffer.
         */
        virtual const TDesC8& GetBufferL() const =0;

        /**
         * Returns the image data in a file.
         */
        virtual const TDesC& GetFileNameL() const =0;
    };


/**
 * Image file implementation of MPbkImageData.
 */
class CPbkImageFileData :
        public CBase, public MPbkImageData
    {
    public:  // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aFileName name of the image file to encapsulate.
         * @return a new instance of this class.
         */
        static CPbkImageFileData* NewL(const TDesC& aFileName);

        /**
         * Destructor.
         */
        ~CPbkImageFileData();

    public:  // from MPbkImageData
        const TDesC8& GetBufferL() const;
        const TDesC&  GetFileNameL() const;

    private:  // Implementation
        CPbkImageFileData(const TDesC& aFileName);

    private:  // Data
		/// Own: file name
        TFileName iFileName;
		/// Own: buffer
        mutable HBufC8* iBuffer;
    };


/**
 * Image buffer implementation of MPbkImageData.
 */
class CPbkImageBufferData :
        public CBase, public MPbkImageData
    {
    public:  // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aBuffer       reference to the image buffer to encapsulate. 
         *                      The buffer must exist as long as the returned 
         *                      object exists.
         * @param aBaseName     base name to use for any image file created
         *                      from the buffer.
         * @return a new instance of this class.
         */
        static CPbkImageBufferData* NewL
            (const TDesC8& aBuffer, const TDesC& aBaseName=KNullDesC);

        /**
         * Destructor.
         * Deletes any file created in GetFileNameL().
         */
        ~CPbkImageBufferData();

    public:  // from MPbkImageData
        const TDesC8& GetBufferL() const;
        const TDesC&  GetFileNameL() const;

    private:  // Implementation
        CPbkImageBufferData(const TDesC8& aBuffer, const TDesC& aBaseName);

    private:  // Data
		/// Ref: buffer
        const TDesC8& iBuffer;
		/// Own: file name
        TFileName iBaseName;
		/// Own: file server session
        mutable RFs iFsSession;
		/// Own: attachment file
        mutable CPbkAttachmentFile* iFile;
    };


/**
 * Image data with image info.
 */
class CPbkImageDataWithInfo :
        public CBase
    {
    public: // Interface
        /**
         * Construction parameters for CPbkImageDataWithInfo.
         */
        struct TParams
            {
            /**
             * Default constructor. Initializes all fields to default values.
             */
            IMPORT_C TParams();
            
            /**
             * Image data. CPbkImageDataWithInfo takes ownership of this field 
             * when this struct is passed to CPbkImageDataWithInfo::NewL().
             */
            MPbkImageData* iImageData;

            /**
             * Image size in pixels.
             */
            TSize iSizeInPixels;

            /**
             * Image format's MIME type.
             */
            const TDesC8* iMimeType;
            };

        /**
         * Creates a new instance of this class.
         *
         * @param aParams   construction parameters. This object takes 
         *                  ownership of aParams.iImageData if this function
         *                  does not leave.
         * @return a new instance of this class.
         */
        IMPORT_C static CPbkImageDataWithInfo* NewL(const TParams& aParams);

        /**
         * Destructor.
         */
        ~CPbkImageDataWithInfo();

        /**
         * Returns the image data.
         */
        IMPORT_C const MPbkImageData& ImageData() const;

        /**
         * Returns the image size in pixels.
         */
        IMPORT_C TSize ImageSizeInPixels() const;

        /**
         * Returns the image format's MIME type.
         */
        IMPORT_C const TDesC8& MimeType() const;

    private: // Implementation
        CPbkImageDataWithInfo();
        void ConstructL(const TParams& aParams);

    private:  // Data
        /// Own: parameters
        TParams iParams;
    };

#endif // __PbkImageData_H__

// End of File
