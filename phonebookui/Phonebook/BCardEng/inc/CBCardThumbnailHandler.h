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
*         Handling thumbnail save/retrieve operations and making these
*         operations synchronous
*
*/


#ifndef __CBCARDTHUMBNAILHANDLER_H__
#define __CBCARDTHUMBNAILHANDLER_H__

//  INCLUDES
#include <e32base.h>
#include <MPbkThumbnailOperationObservers.h>


// FORWARD DECLARATIONS
class MPbkThumbnailOperation;
class CPbkThumbnailManager;
class CPbkContactEngine;
class CPbkContactItem;
class CFbsBitmap;

// CLASS DECLARATION

/**
 * Class for handling thumbnail save/retrieve operations and making these
 * operations synchronous
 */
NONSHARABLE_CLASS(CBCardThumbnailHandler) : 
        public CBase, 
        private MPbkThumbnailGetObserver, 
        private MPbkThumbnailSetObserver,
        private MPbkThumbnailGetImageObserver
	{
    public: // Constructors and destructor
        /**
         * Creates a new BCard thumbnail handler.
         * @param aEngine Contact engine
         */
        static CBCardThumbnailHandler* NewL(CPbkContactEngine& aEngine);
         
        /**
         * Destructor. 
         */
        ~CBCardThumbnailHandler();
    
    private: // Constructors
        CBCardThumbnailHandler();
        void ConstructL(CPbkContactEngine& aEngine);

    public: // Interface
        /**
         * Saves the given 8bit descriptor to the given contact item
         * as thumbnail.
         *
         * @param aContactItem  Contact item where the thubmbnail is stored
         * @param aSource       Source thumbnail to store; a 8bit descriptor 
         *                      containing the image.
         */
        void SaveThumbnailL(CPbkContactItem& aContactItem, const TDesC8& aSource);
        
        /**
         * Gets the thumbnail from the contact.
         *
         * @param aContactItem  Contact item where the thumbnail is fetched.
         * @return              The thumbnail image of the contact as a
         *                      8bit descriptor.
         */
        const TDesC8& GetThumbnailL(CPbkContactItem& aContactItem);
        
        /**
         * Check if the given contact has an inserted thumbnail.
         *
         * @param aContactItem  Contact item to check for thumbnail existence.
         */
        TBool HasThumbnail(const CPbkContactItem& aContactItem) const;

    private: // From MPbkThumbnailGetImageObserver
        void PbkThumbnailGetImageComplete
            (MPbkThumbnailOperation& aOperation, CPbkImageDataWithInfo* aImageData);
        void PbkThumbnailGetImageFailed
            (MPbkThumbnailOperation& aOperation, TInt aError);
    
    private: // From MPbkThumbnailGetObserver
        void PbkThumbnailGetComplete(
            MPbkThumbnailOperation& aOperation, CFbsBitmap* aBitmap);
        void PbkThumbnailGetFailed
            (MPbkThumbnailOperation& aOperation, TInt aError);

    private: // From MPbkThumbnailSetObserver
        void PbkThumbnailSetComplete
            (MPbkThumbnailOperation& aOperation);
        void PbkThumbnailSetFailed
            (MPbkThumbnailOperation& aOperation, TInt aError);

    private: // Implementation
        TBool IsAlreadyExportableL(CPbkContactItem& aContactItem);
        HBufC8* GetThumbnailBitmapL(CPbkContactItem& aContactItem);
        void ProcessError(MPbkThumbnailOperation& aOperation, TInt aError);
        void ProcessComplete(MPbkThumbnailOperation& aOperation);
   
    private: // Data
		/// Own: phonebook thumbnail manager
        CPbkThumbnailManager* iManager;
		/// Own: asynchronous thumbnail operation
        MPbkThumbnailOperation* iThumbOperation;
		/// Own: image data
        CPbkImageDataWithInfo* iImageData;
		/// Own: the bitmap
        CFbsBitmap* iBitmap;
		/// Own: thumbnail bitmap jpeg descriptor
        HBufC8* iImageBlob;
		/// Own: active scheduler
        CActiveSchedulerWait iWait;
		/// Own: error code
        TInt iError;
    };

#endif // __CBCARDTHUMBNAILHANDLER_H__
            
// End of File
