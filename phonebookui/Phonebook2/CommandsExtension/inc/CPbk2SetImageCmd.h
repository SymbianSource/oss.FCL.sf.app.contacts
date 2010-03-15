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
*     Set image command event handling class.
*
*/


#ifndef CPBK2SETIMAGECMD_H
#define CPBK2SETIMAGECMD_H

//  INCLUDES
#include "CPbk2ImageCmdBase.h"
#include <MPbk2ImageOperationObservers.h> // MPbk2ImageSetObserver
#include <MMGFetchVerifier.h>             // MMGFetchVerifier
#include <MediaFileTypes.hrh>
#include <badesca.h>
#include <AknProgressDialog.h>


// FORWARD DECLARATIONS
class CAknWaitDialog;
class CPbk2DrmManager;

// CLASS DECLARATION

/**
 * Set image command event handling class.
 */
NONSHARABLE_CLASS(CPbk2SetImageCmd) : 
        public CPbk2ImageCmdBase,
        public MProgressDialogCallback,
        private MPbk2ImageSetObserver,
        private MMGFetchVerifier        
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aUiControl 	A related ui control
         * @return a new instance of this class.
         */
        static CPbk2SetImageCmd* NewL(
                MPbk2ContactUiControl& aUiControl);
        
        /**
         * Destructor.
         */
        ~CPbk2SetImageCmd();
        
        /**
         * Wrap 'MGFetch::RunL'
         */
        static TBool MGFetchL( CDesCArray& aSelectedFiles,
                TMediaFileType aMediaType,
                TBool aMultiSelect,
                MMGFetchVerifier* aVerifier );
        
    private: // from CPbk2ImageCmdBase
        TBool ExecuteCommandL();
        
    private: // from MPbkImageSetObserver
        void Pbk2ImageSetComplete(
                MPbk2ImageOperation& aOperation);
        void Pbk2ImageSetFailed(
                MPbk2ImageOperation& aOperation,
                TInt aError);

    private: // from MProgressDialogCallback
        void DialogDismissedL( TInt aButtonId );
        
    private: // from MMGFetchVerifier
        TBool VerifySelectionL(
                const MDesCArray* aSelectedFiles);        
    private:  // Implementation
        CPbk2SetImageCmd(
                MPbk2ContactUiControl& aUiControl);
        void ConstructL();
        void Cancel();
        void DismissWaitNote();
        void SetImageRefL(
                const TDesC& aFileName);
        
        TBool DoVerifySelectionL(
                const MDesCArray* aSelectedFiles);
        void ShowErrorNoteL();
        
    private:    // Data
        /// Own: image set operation
        MPbk2ImageOperation* iImageOperation;
        /// Own: wait note dialog
        CAknWaitDialog* iWaitNote;
        /// Own: DRM support
        CPbk2DrmManager* iDrmManager;        
        TBool iVerificationFailed;
        TBool iImageSetFailed;
        TInt iImageSetError;
    };

#endif // CPBK2SETIMAGECMD_H
            
// End of File
