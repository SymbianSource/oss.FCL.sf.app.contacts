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
*       Phonebook contact item field embedded thumbnail reader.
*       Reads an database-embedded picture from a field.
*
*/


#ifndef __CPbkEmbeddedThumbnail_H__
#define __CPbkEmbeddedThumbnail_H__

//  INCLUDES
#include "CPbkThumbnailReaderBase.h"
#include "CPbkThumbnailWriterBase.h"


// FORWARD DECLARATIONS
class TPbkContactItemField;
class TPbkImageLoadParameters;
class CPbkFFSCheck;

// CLASS DECLARATION

/**
 * Phonebook contact item field embedded thumbnail reader.
 * Reads an database-embedded picture from a field.
 */
NONSHARABLE_CLASS(CPbkEmbeddedThumbnailReader) : 
        public CPbkThumbnailReaderBase
    {
    public: // interface
        /**
         * Creates a new instance of this class.
         *
         * @param aField    field to read the thumbnail from.
         * @param aObserver observer to notify about progress of this 
         *                  operation.
         */
        static CPbkEmbeddedThumbnailReader* NewLC
            (const TPbkContactItemField& aField, 
            MPbkThumbnailGetObserver& aObserver);
   
        /**
         * Destructor. Cancels any previously executing read operation and
         * destroys this object.
         */
        ~CPbkEmbeddedThumbnailReader();

    public:  // from MPbkThumbnailReader
        void StartReadL(const TPbkImageLoadParameters* aParameters = NULL);

    private: // implementation
        CPbkEmbeddedThumbnailReader
            (const TPbkContactItemField& aField, 
            MPbkThumbnailGetObserver& aObserver);
        void ConstructL();

    private: // data members
        /// Ref: Contact item field
        const TPbkContactItemField& iField; 
    };


/**
 * Phonebook contact item field embedded thumbnail reader.
 * Reads an database-embedded thumbnail image from a field.
 */
NONSHARABLE_CLASS(CPbkEmbeddedThumbnailImageReader) : 
        public CPbkThumbnailImageReaderBase
    {
    public: // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aField    field to read the thumbnail image from.
         * @param aObserver observer to notify about progress of this 
         *                  operation.
         * @return new instance of this class.
         */
        static CPbkEmbeddedThumbnailImageReader* NewLC
            (const TPbkContactItemField& aField, 
            MPbkThumbnailGetImageObserver& aObserver);
   
        /**
         * Destructor. Cancels any previously executing read operation and
         * destroys this object.
         */
        ~CPbkEmbeddedThumbnailImageReader();

    public:  // From CPbkThumbnailImageReaderBase
        void DoStartReadL(const TPbkImageLoadParameters* aParameters = NULL);
        MPbkImageData* CreateImageDataLC();

    private: // Implementation
        CPbkEmbeddedThumbnailImageReader
            (const TPbkContactItemField& aField, 
            MPbkThumbnailGetImageObserver& aObserver);
        void ConstructL();
        const TDesC8& ImageBuffer() const;

    private: // data members
        /// Ref: Contact item field
        const TPbkContactItemField& iField; 
    };


/**
 * Phonebook contact item field embedded thumbnail writer.
 * Writes an database-embedded picture to a field.
 */
NONSHARABLE_CLASS(CPbkEmbeddedThumbnailWriter) : 
        public CPbkThumbnailWriterBase 
    {
    public: // interface
        /**
         * Creates a new instance of this class.
         *
         * @param aField    field to write the thumbnail to.
         * @param aObserver observer to notify about progress of this 
         *                  operation.
         */
        static CPbkEmbeddedThumbnailWriter* NewLC
            (TPbkContactItemField& aField, 
            MPbkThumbnailSetObserver& aObserver);
   
        /**
         * Destructor.
         */
        ~CPbkEmbeddedThumbnailWriter();

    private: // from CPbkThumbnailWriterBase
        CImageEncoder* CreateImageWriterL();
        void StoreImageL(const MPbkImageData& aImageData);
        void StoreImageL(CImageEncoder& aImageWriter);
        void DoCancel();
        TSize ImageSize() const;

    private: // implementation
        CPbkEmbeddedThumbnailWriter
            (TPbkContactItemField& aField, 
            MPbkThumbnailSetObserver& aObserver);
        void ConstructL();

    private: // data members
        /// Ref: Contact item field
        TPbkContactItemField& iField; 
        /// Own: image buffer
        HBufC8* iBitmapBlob;
        /// Own: Critical memory level checker
        CPbkFFSCheck* iFFSCheck;
    };

#endif // __CPbkEmbeddedThumbnail_H__

// End of File
