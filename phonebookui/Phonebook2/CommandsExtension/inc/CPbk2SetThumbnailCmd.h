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
*     Set thumbnail command event handling class.
*
*/


#ifndef CPBK2SETTHUMBNAILCMD_H
#define CPBK2SETTHUMBNAILCMD_H

//  INCLUDES
#include "CPbk2ThumbnailCmdBase.h"
#include <MPbk2ImageOperationObservers.h> // MPbk2ImageSetObserver
#include <MMGFetchVerifier.h>


// FORWARD DECLARATIONS
class CAknWaitDialog;
class CPbk2DrmManager;

// CLASS DECLARATION

/**
 * Thumbnail command event handling class.
 */
NONSHARABLE_CLASS(CPbk2SetThumbnailCmd) : 
        public CPbk2ThumbnailCmdBase,
        private MPbk2ImageSetObserver,
        private MMGFetchVerifier
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aUiControl 	A related ui control
         * @return a new instance of this class.
         */
        static CPbk2SetThumbnailCmd* NewL
            (MPbk2ContactUiControl& aUiControl);
        
        /**
         * Destructor.
         */
        ~CPbk2SetThumbnailCmd();
        
    private: // from CPbk2ThumbnailCmdBase
        TBool ExecuteCommandL();
        
	private: // from MPbkThumbnailSetObserver
        void Pbk2ImageSetComplete(MPbk2ImageOperation& aOperation);
        void Pbk2ImageSetFailed(MPbk2ImageOperation& aOperation, TInt aError);

    private: // from MMGFetchVerifier
        TBool VerifySelectionL(const MDesCArray* aSelectedFiles);
        
    private:  // Implementation
        CPbk2SetThumbnailCmd
            (MPbk2ContactUiControl& aUiControl);
        void ConstructL();
        void Cancel();
        void DismissWaitNote();
        
    private:    // Data
        /// Own: thumbnail set operation
        MPbk2ImageOperation* iThumbOperation;
        /// Own: wait note dialog
        CAknWaitDialog* iWaitNote;
        /// Own: DRM support
        CPbk2DrmManager* iDrmManager;
    };

#endif // CPBK2SETTHUMBNAILCMD_H
            
// End of File
