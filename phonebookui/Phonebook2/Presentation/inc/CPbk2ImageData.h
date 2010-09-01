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
*     Image data abstraction hierarchy.
*
*/


#ifndef CPBK2IMAGEDATA_H
#define CPBK2IMAGEDATA_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

//  FORWARD DECLARATIONS
class CPbk2AttachmentFile;

// CLASS DECLARATIONS

/**
 * Abstract thumbnail image data interface. Provides polymorphism between 
 * images stored in a file or buffer.
 */
class MPbk2ImageData
    {
    public:
        /**
         * Virtual destructor.
         */
        virtual ~MPbk2ImageData()
			{
			}

        /**
         * Returns the image data in a buffer.
         * @return The image data in a buffer.
         */
        virtual const TDesC8& GetBufferL() const =0;

        /**
         * Returns the image data in a file.
         * @return The image data in a file.
         */
        virtual const TDesC& GetFileNameL() const =0;
    };


/**
 * Image file implementation of MPbk2ImageData.
 */
NONSHARABLE_CLASS(CPbk2ImageFileData) :
        public CBase, 
        public MPbk2ImageData
    {
    public:  // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aFileName name of the image file to encapsulate.
         * @return a new instance of this class.
         */
        static CPbk2ImageFileData* NewL(const TDesC& aFileName);

        /**
         * Destructor.
         */
        ~CPbk2ImageFileData();

    public:  // from MPbk2ImageData
        const TDesC8& GetBufferL() const;
        const TDesC&  GetFileNameL() const;

    private:  // Implementation
        CPbk2ImageFileData(const TDesC& aFileName);

    private:  // Data
		/// Own: file name
        TFileName iFileName;
		/// Own: buffer
        mutable HBufC8* iBuffer;
    };


/**
 * Image buffer implementation of MPbk2ImageData.
 */
NONSHARABLE_CLASS(CPbk2ImageBufferData) :
        public CBase, 
        public MPbk2ImageData
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
        static CPbk2ImageBufferData* NewL
            (const TDesC8& aBuffer, const TDesC& aBaseName=KNullDesC);

        /**
         * Destructor.
         * Deletes any file created in GetFileNameL().
         */
        ~CPbk2ImageBufferData();

    public:  // from MPbk2ImageData
        const TDesC8& GetBufferL() const;
        const TDesC&  GetFileNameL() const;

    private:  // Implementation
        CPbk2ImageBufferData(const TDesC8& aBuffer, const TDesC& aBaseName);

    private:  // Data
		/// Ref: buffer
        const TDesC8& iBuffer;
		/// Own: file name
        TFileName iBaseName;
		/// Own: file server session
        mutable RFs iFsSession;
		/// Own: attachment file
        mutable CPbk2AttachmentFile* iFile;
    };


#endif // CPBK2IMAGEDATA_H

// End of File
