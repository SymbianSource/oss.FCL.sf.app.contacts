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
*     Thumbnail command event handling class.
*
*/


#ifndef __CPbkThumbnailCmd_H__
#define __CPbkThumbnailCmd_H__

//  INCLUDES
#include <e32base.h>    // CBase
#include "MPbkThumbnailOperationObservers.h" // MPbkThumbnailSetObserver
#include <MMGFetchVerifier.h>


// FORWARD DECLARATIONS
class CPbkContactItem;
class CEikMenuPane;
class CPbkContactEngine;
class CPbkThumbnailManager;
class MPbkThumbnailOperation;
class CAknWaitDialog;
class CPbkThumbnailPopup;
class CPbkDrmManager;


// CLASS DECLARATION

/**
 * Thumbnail command event handling class.
 */
NONSHARABLE_CLASS(CPbkThumbnailCmd) : 
        public CBase,
        private MPbkThumbnailSetObserver,
        private MMGFetchVerifier

    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aEngine           Phonebook contact engine.
         * @param aThumbnailPopup   the UI control that will display the 
         *                          thumbnail.
         * @return a new instance of this class.
         */
        static CPbkThumbnailCmd* NewL
            (CPbkContactEngine& aEngine,
			CPbkThumbnailPopup& aThumbnailPopup);
        
        /**
         * Destructor.
         */
        ~CPbkThumbnailCmd();

    public: // New functions
        /**
         * Fetch a thumbnail from the photo album, attach it to a contact and
         * display the thumbnail.
         *
         * @param aContactItem contact to attach the thumbnail to.
         * @return  ETrue if the thumbnail assignment was succesfully started,
         *          EFalse if the fetching was canceled.
         */
        TBool FetchThumbnailL(CPbkContactItem& aContactItem);

        /**
         * Removes the thumbnail from the a contact and remove it from display.
         *
         * @param aContactItem contact to remove photo from.
         * @return  ETrue if the thumbnail was removed, EFalse if the removal
         *          was canceled.
         */
        TBool RemoveThumbnailL(CPbkContactItem& aContactItem);

        /**
         * Menu item filtering.
		 * @param aResourceId menu pane resource id
		 * @param aMenuPane menu pane reference
		 * @param aContactItem contact item reference
         */
        void DynInitMenuPaneL(TInt aResourceId, 
            CEikMenuPane& aMenuPane, CPbkContactItem& aContactItem);

        /**
         * Checks if class instance is ready for deletion.
         *
         * @return  ETrue if class instance is ready for exit
         */
        TBool OkToExit();
        
	private: // from MPbkThumbnailSetObserver
        void PbkThumbnailSetComplete(MPbkThumbnailOperation& aOperation);
        void PbkThumbnailSetFailed(MPbkThumbnailOperation& aOperation, TInt aError);

    private: // from MMGFetchVerifier
        TBool VerifySelectionL(const MDesCArray* aSelectedFiles);

    private:  // Implementation
        CPbkThumbnailCmd
            (CPbkContactEngine& aEngine, 
            CPbkThumbnailPopup& aThumbnailPopup);
        void ConstructL();
        void Cancel();

    private:    // Data
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Ref: thumnail control
        CPbkThumbnailPopup& iThumbnailPopup;
        /// Own: thumbnail manager
        CPbkThumbnailManager* iThumbnailManager;
        /// Own: thumbnail set operation
        MPbkThumbnailOperation* iThumbOperation;
        /// Own: wait note dialog
        CAknWaitDialog* iWaitNote;
        /// Own: DRM support
        CPbkDrmManager* iDrmManager;
        /// Ref: parameter of FetchThumbnailL
        CPbkContactItem* iContactItem;
        /// Own: Flag for destruction status
        TBool* iDestroyed;        
    };

#endif // __CPbkThumbnailCmd_H__
            
// End of File
