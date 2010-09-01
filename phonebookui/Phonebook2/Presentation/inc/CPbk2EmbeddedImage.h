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
*       Phonebook2 contact item field embedded thumbnail reader.
*       Reads an database-embedded picture from a field.
*
*/


#ifndef CPBK2EMBEDDEDIMAGE_H
#define CPBK2EMBEDDEDIMAGE_H

//  INCLUDES
#include "CPbk2ImageReaderBase.h"
#include "CPbk2ImageWriterBase.h"


// FORWARD DECLARATIONS
class MVPbkBaseContactField;
class MVPbkStoreContactField;
class TPbk2ImageManagerParams;
class CVPbkContactManager;
class MPbk2ImageData;

// CLASS DECLARATION

/**
 * Phonebook contact item field embedded thumbnail reader.
 * Reads a database-embedded picture from a field.
 */
NONSHARABLE_CLASS(CPbk2EmbeddedImageReader) : 
        public CPbk2ImageReaderBase
    {
    public: // interface
        /**
         * Creates a new instance of this class.
         *
         * @param aField    field to read the thumbnail from.
         * @param aObserver observer to notify about progress of this 
         *                  operation.
         */
        static CPbk2EmbeddedImageReader* NewLC(
                const MVPbkBaseContactField& aField,
                MPbk2ImageGetObserver& aObserver );
   
        /**
         * Destructor. Cancels any previously executing read operation and
         * destroys this object.
         */
        ~CPbk2EmbeddedImageReader();

    public:  // from MPbk2ImageReader
        void StartReadL(const TPbk2ImageManagerParams* aParameters = NULL);
        HBufC8* CreateImageDataLC();

    private: // implementation
        CPbk2EmbeddedImageReader(
                const MVPbkBaseContactField& aField,
                MPbk2ImageGetObserver& aObserver );
        void ConstructL();
		const TDesC8& ImageBuffer() const;
		
    private: // data members
        /// Ref: Contact item field
        const MVPbkBaseContactField& iField; 
        /// Own: Image buffer
        HBufC8* iImageBuffer;
    };


/**
 * Phonebook contact item field embedded thumbnail writer.
 * Writes a database-embedded picture to a field.
 */
NONSHARABLE_CLASS(CPbk2EmbeddedImageWriter) : 
        public CPbk2ImageWriterBase 
    {
    public: // interface
        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager Contact manager
         * @param aField    field to write the thumbnail to.
         * @param aObserver observer to notify about progress of this 
         *                  operation.
         */
        static CPbk2EmbeddedImageWriter* NewLC
            (CVPbkContactManager& aContactManager,
            MVPbkStoreContactField& aField, 
            MPbk2ImageSetObserver& aObserver);
   
        /**
         * Destructor.
         */
        ~CPbk2EmbeddedImageWriter();

    private: // from CPbkThumbnailWriterBase
        CImageEncoder* CreateImageWriterL();
        void StoreImageL(const MPbk2ImageData& aImageData);
        void StoreImageL(CImageEncoder& aImageWriter);
        void DoCancel();
        TSize ImageSize() const;

    private: // implementation
        CPbk2EmbeddedImageWriter
            (CVPbkContactManager& aContactManager,
            MVPbkStoreContactField& aField, 
            MPbk2ImageSetObserver& aObserver);
        void ConstructL();
		TBool IsCodImageL() const;
		TBool IsSameFieldType(const MVPbkFieldType& aFieldType) const;
    private: // data members
        /// Ref: Contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Contact item field
        MVPbkStoreContactField& iField; 
        /// Own: image buffer
        HBufC8* iBitmapBlob;
        
    };

#endif // CPBK2EMBEDDEDIMAGE_H

// End of File
