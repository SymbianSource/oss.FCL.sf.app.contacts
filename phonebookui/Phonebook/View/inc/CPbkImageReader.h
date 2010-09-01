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
*     Phonebook image reader class definition.
*
*/


#ifndef __CPbkImageReader_H__
#define __CPbkImageReader_H__

//  INCLUDES
#include <e32base.h>    // CBase
#include "TPbkImageLoadParameters.h"

// FORWARD DECLARATIONS
class MPbkImageReaderObserver;
class CImageDecoder;
class CBitmapScaler;
class TFrameInfo;

// CLASS DECLARATIONS

/**
 * Phonebook image file or buffer -> bitmap reader.
 */
NONSHARABLE_CLASS(CPbkImageReader) : 
        public CActive
    {
    public: // interface
        /**
         * Creates a new instance of this class.
         *
         * @param aObserver observer to notify about progress of this 
         *        operation.
         */
        static CPbkImageReader* NewL(MPbkImageReaderObserver& aObserver);
   
        /**
         * Destructor. Cancels any executing read operation and destroys this
         * object. Observer is not notified about the cancellation.
         */
        ~CPbkImageReader();

        /**
         * Reads bitmap from file. Cancels any previously executing read 
         * operation. Observer is not notified about the cancellation.
         *
         * @param aFileName name of the image file to read
         * @param aParams   optional parameters, @see TPbkImageLoadParameters
         */
        void ReadFromFileL
            (const TDesC& aFileName, 
            const TPbkImageLoadParameters* aParams=NULL);

        /**
         * Reads bitmap from buffer. Cancels any previously executing read 
         * operation. Observer is not notified about the cancellation.
         *
         * @param aBuffer   the buffer to read the image from. Caller must keep
         *                  the buffer alive as long as this reader is 
         *                  executing
         * @param aParams   optional parameters, @see TPbkImageLoadParameters
         */
        void ReadFromBufferL
            (const TDesC8& aBuffer, 
            const TPbkImageLoadParameters* aParams=NULL);

        /**
         * Recognizes image format in a file.
         *
         * @param aFileName file to recognize image format from.
         * @see ImageFormat().
         */
        void RecognizeFormatFromFileL(const TDesC& aFileName);

        /**
         * Recognizes image format in a buffer.
         *
         * @param aBuffer buffer to recognize image format from.
         * @see ImageFormat().
         */
        void RecognizeFormatFromBufferL(const TDesC8& aBuffer);

        /**
         * Returns Mime string of the recognized format. Recognizing
         * function has to be called before this, otherwise Mime string
         * is not known and NULL descriptor will be returned.
         *
         * @return Mime string of the recognized format.
         */
        const TDesC8& MimeString() const;

        /**
         * Returns information about a frame of the image.
         * Can be called after this object calls ImageOpenComplete to its
         * observer.
         *
         * @param aFrame    number of the frame to get inforamtion of.
         * @param aInfo     frame information is written to this parameter.
         * @precond aFrame>=0 && aFrame<FrameCount()
         * @see CImageDecoder::FrameInfo
         */
        void FrameInfo(TInt aFrame, TFrameInfo& aInfo) const;

        /**
         * Returns the frame count of the image.
         * Can be called after this object calls ImageOpenComplete to its
         * observer.
         *
         * @see CImageDecoder::FrameCount
         */
        TInt FrameCount() const;

        /**
         * This is overridden from CActive because 
         * observers may be called from RunL. If observers then call
         * Cancel method, the CActive implementation does not call DoCancel.
         * However, in this case it has to be called so that resources 
         * are freed immediately in case of Cancel.
         */
        void Cancel();

    private: // From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();
       
    private: // implementation
        CPbkImageReader(MPbkImageReaderObserver& aObserver);
        void ConstructL();
        void NextStateL();
        void ConvertImageToBitmapL();
        void ScaleBitmapL();
        void Complete();
        void InitReadL(const TPbkImageLoadParameters* aParams);
        void CloseImage();

    private: // data members
        /// Ref: observer
        MPbkImageReaderObserver& iObserver;
        /// Own: currect asynchronous step
        TInt iState;
        /// Own: reading parameters
        TPbkImageLoadParameters iParams;
        /// Own: file server session
        RFs iFsSession;
        /// Own: image decoder
        CImageDecoder* iImageDecoder;        
        /// Own: read bitmap
        CFbsBitmap* iBitmap;
        /// Own: bitmap scaler
        CBitmapScaler* iBitmapScaler;
        /// Own: Mime string of the read image
        HBufC8* iMimeString;
    };

#endif // __CPbkImageReader_H__
	
// End of File
