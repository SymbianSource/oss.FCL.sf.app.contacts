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
* Description:  Phonebook 2 DRM manager.
*
*/


#ifndef CPBK2DRMMANAGER_H
#define CPBK2DRMMANAGER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class DRMCommon;
class CDRMHelper;
class CRepository;

// CLASS DECLARATION

/**
 * Phonebook2 DRM manager.
 */
class CPbk2DrmManager : public CBase
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2DrmManager* NewL();

        /**
         * Destructor.
         */
        IMPORT_C ~CPbk2DrmManager();

    public: // Interface

        /**
         * Check if the given file is DRM protected.
         * NB. If DRM protection information is not available
         * then ETrue is returned.
         *
         * @param aFileName     File name to check.
         * @param aIsProtected  ETrue if the file was protected,
         *                      EFalse if the file was not protected.
         * @return Common error code
         */
        IMPORT_C TInt IsProtectedFile(
                const TDesC& aFileName,
                TBool& aIsProtected );

        /**
         * Check is the given ringing tone acceptable.
         * NB. If DRM protection information is not available
         * then ETrue is returned.         
         *
         * @param aFileName    Ringing tone's file name.
         * @param aIsProtected ETrue if the file was protected,
         *                     EFalse if the file was not protected.
         * @return Common error code
         */
        IMPORT_C TInt IsRingingToneForbidden(
                const TDesC& aFileName,
                TBool& aIsProtected );

        /**
         * Check is the given thumbnail acceptable.
         * NB. If DRM protection information is not available
         * then ETrue is returned.
         *
         * @param aFileName    Thumbnail's file name.
         * @param aIsProtected ETrue if the file was protected,
         *                     EFalse if the file was not protected.
         * @return Common error code
         */
        IMPORT_C TInt IsThumbnailForbidden(
                const TDesC& aFileName,
                TBool& aIsProtected );

    private: // Implementation
        CPbk2DrmManager();
        void ConstructL();
        void ShowErrorNoteL(
                TInt aResource );
        TBool IsDrmRequiredForPlaybackL();
        TBool IsBlockedMimeTypeL(
                const TDesC& aMimeType );
        void CheckProtectedFileL(
                const TDesC& aFileName,
                TBool& aProtected );
        void CheckUnprotectedFileL(
                const TDesC& aFileName,
                TBool& aProtected );

    private: // For wmdrm support
    	/**
         * Checks if file is WMDRM protected.
         * @param aFileName Name of the file to be checked.
         * @return ETrue if file is WMDRM protected, EFalse otherwise.
         */
		TBool IsFileWMDRMProtectedL( const TDesC& aFileName,
									TBool& aIsProtected  ) const;            
    private: // Data
        /// Own: DRM support
        DRMCommon* iDrmClient;
        /// Own: DRM state
        TBool iDrmEnabled;
        /// Own: DRM helper
        CDRMHelper* iDrmHelper;
        /// Own: Music player features
        CRepository* iMusicPlayerFeatures;
    };

#endif // CPBK2DRMMANAGER_H

// End of File
