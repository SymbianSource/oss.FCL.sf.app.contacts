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
*     Image command event handling class.
*
*/


#ifndef __CPbkCodImageCmd_H__
#define __CPbkCodImageCmd_H__

//  INCLUDES
#include "MPbkImageOperationObservers.h" // MPbkImageSetObserver
#include <MMGFetchVerifier.h>
#include <e32base.h>    // CBase
#include <badesca.h>
#include <cntdef.h>
#include <AknServerApp.h>  // MAknServerAppExitObserver
// FORWARD DECLARATIONS
class CPbkContactItem;
class CEikMenuPane;
class CPbkContactEngine;
class CPbkImageManager;
class MPbkImageOperation;
class CAknWaitDialog;
class CPbkCodImageCmd;
class MCodCmdObserver;
class CDocumentHandler;
class CPbkImageDataWithInfo;
class CFileMan;
// CLASS DECLARATION

/**
 * Image command event handling class.
 */
NONSHARABLE_CLASS(CPbkCodImageCmd) : 
        public CBase,
        private MPbkImageSetObserver,
        private MPbkImageGetImageObserver,
        private MMGFetchVerifier,
        private MAknServerAppExitObserver
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aEngine Phonebook contact engine.
         * @param aCodCmdObserver Observer
         * @return a new instance of this class.
         */
        static CPbkCodImageCmd* NewL
            (CPbkContactEngine& aEngine);
        
        /**
         * Destructor.
         */
        ~CPbkCodImageCmd();

    public: // New functions
        /**
         * Fetch a image from the photo album, attach it to a contact and
         * display the image.
         *
         * @param aId contact id to attach the image to.
         * @return  The selected filename if the image assignment was successfully started,
         *          NULL is returned if the image file is not selected.
         */
        TBool FetchImageL(TContactItemId aId);

        /**
         * Removes the image from the contact.
         *
         * @param aId contact id to remove photo from.
         * @return  ETrue if the image was removed, EFalse if the removal
         *          was canceled.
         */
        TBool RemoveImageL(TContactItemId aId);

        /**
         * Views the image from the contact.
         *
         * @param aId contact id.
         * @return  ETrue if the image was removed, EFalse if the removal
         *          was canceled.
         */
        void ViewImageL(TContactItemId aId);

    private: // from MPbkImageSetObserver
        void PbkImageSetComplete(MPbkImageOperation& aOperation);
        void PbkImageSetFailed(MPbkImageOperation& aOperation, TInt aError);

    private: // from MPbkImageGetImageObserver
        void PbkImageGetImageComplete
            (MPbkImageOperation& aOperation, CPbkImageDataWithInfo* aImageData);
        void PbkImageGetImageFailed
            (MPbkImageOperation& aOperation, TInt aError);
        void DoViewImageL(const TDesC8& aImageData);
    private: // from MMGFetchVerifier
        TBool VerifySelectionL(const MDesCArray* aSelectedFiles);

    private: // from MAknServerAppExitObserver
        void HandleServerAppExit(TInt aReason);

    private:  // Implementation
        CPbkCodImageCmd
            (CPbkContactEngine& aEngine);
        void ConstructL();
        void Cancel();
        void SetImageFilenameL();
        TInt DeleteTempFile();
        TInt DeleteTempFolder();
    private:    // Data
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Own: image manager
        CPbkImageManager* iPbkImageManager;
        /// Own: image set operation
        MPbkImageOperation* iImageOperation;
        /// Own: wait note dialog
        CAknWaitDialog* iWaitNote;
        /// Owns: contact item
        CPbkContactItem* iContactItem;
        /// Own: selected file
        CDesCArray* iSelectedFile;
        /// Own: Document handler
        CDocumentHandler* iDocumentHandler;        
        /// Own: Image data
        CPbkImageDataWithInfo* iImageData;
        /// Own: Filename
        HBufC* iFileName;
        /// Own: File management utility
        CFileMan* iFileMan;
    };

#endif // __CPbkCodImageCmd_H__
            
// End of File
