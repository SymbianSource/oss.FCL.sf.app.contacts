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
*     Phonebook DRM policy managing class.
*
*/


#ifndef __CPbkDrmManager_H__
#define __CPbkDrmManager_H__

//  INCLUDES
#include <e32base.h>    // CBase


// FORWARD DECLARATIONS
class DRMCommon;
class CDRMHelper;


// CLASS DECLARATION

/**
 * Phonebook DRM policy managing class.
 */
class CPbkDrmManager : public CBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         */
        IMPORT_C static CPbkDrmManager* NewL();
        
        /**
         * Destructor.
         */
        IMPORT_C ~CPbkDrmManager();

    public: // interface
    
        /**
         * Check if the aFileName is a DRM protected file. 
         * NB. If DRM protection information is not available
         * then ETrue is returned.
         *
         * @param aFileName file name to check.
         * @return  ETrue if the file was protected,
         *          EFalse if the file was not protected.
         */
        IMPORT_C TBool IsProtectedFile(const TDesC& aFileName);        
        
        /**
         * Check if the ringing tone is a DRM protected file.
         * NB. If DRM protection information is not available
         * then ETrue is returned.
         *
         * @param aFileName, ringing tone's file name
         * @return ETrue if the file was protected
         *         EFalse if the file was not protected.
         */
        IMPORT_C TBool IsRingingToneProtectedL( const TDesC& aFileName );
        
        /**
         * Check if the thumbnail is a DRM protected file.
         * NB. If DRM protection information is not available
         * then ETrue is returned.
         *
         * @param aFileName, thumbnail's file name
         * @return ETrue if the file was protected
         *         EFalse if the file was not protected.
         */
        IMPORT_C TBool IsThumbnailProtectedL( const TDesC& aFileName );
        
        /**
         * Check if the aFileName has restricted (time or count based)
         * DRM protection. 
         * NB. If DRM protection information is not available
         * then ETrue is returned.
         *
         * @param aFileName file name to check.
         * @return  ETrue if the file has restricted protection,
         *          EFalse if the file doesn't have restricted protection.
         */
        IMPORT_C TBool HasFileRestrictedRights( const TDesC& aFileName );        
        

    private:  // Implementation
        CPbkDrmManager();
        void ConstructL();
        
    private:        
        void ShowErrorNoteL( TInt aResource );
        TBool IsDrmRequiredForPlaybackL();
        TBool IsBlockedMimeTypeL(const TDesC& aMimeType);
        TInt CheckProtectedFileL(const TDesC& aFileName);
        TInt CheckUnprotectedFileL(const TDesC& aFileName);

    private:    // Data
        /// Own: DRM support
        DRMCommon* iDrmClient;
        /// Own: DRM state
        TBool iDrmEnabled;
        /// Own: DRM helper
        CDRMHelper* iDrmHelper;
    };

#endif // __CPbkDrmManager_H__
            
// End of File
