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
*        Declaration file for CPbkConstants class
*
*/


#ifndef __CPbkConstants_H__
#define __CPbkConstants_H__

//  INCLUDES
#include <e32base.h>
#include <PhonebookVariant.hrh>

// FORWARD DECLARATIONS
class RResourceFile;
class CPbkLocalVariationManager;

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook constants.
 */
class CPbkConstants : 
        public CBase
	{
    public: // constructor and destructor
        /**
         * Constructor
		 * @param aResourceFile reference to resources
         */
        IMPORT_C static CPbkConstants* NewL(RResourceFile& aResourceFile);

        /**
         * Destructor
         */
        ~CPbkConstants();

    public:  // interface
        /**
         * Returns the "unnamed" contact text.
         */
        IMPORT_C const TDesC& UnnamedTitle() const;

        /**
         * Returns the group labels max length.
         */
        IMPORT_C static TInt GroupLabelLength();

        /**
         * Returns the editor/form labels max length.
         */
        IMPORT_C static TInt FormEditorLength();

        /**
         * Compression inactivity timer length in seconds.
         */
        IMPORT_C static TInt CompressionInactivityTimerLength();

        /**
         * Ringing tone preview play delay.
         */
        IMPORT_C static TInt RingingTonePreviewDelay();

        /**
         * Thumbnail show delay.
         */
        IMPORT_C static TInt ThumbnailPopupDelay();

        /**
         * Editor dialogs fields editors length.
         */
        IMPORT_C static TInt FieldEditorLength();

        /**
         * Search field length from Find spec.
         */
        IMPORT_C static TInt SearchFieldLength();

        /**
         * Used to check is the feature defined by aFeature on or off.
         * @param aFeature the feature to check
         * @return ETrue if the feature is on, EFalse otherwise
         */
        IMPORT_C TBool LocallyVariatedFeatureEnabled
            (TPbkLocalVariantFlags aFeature);

    private: // Implementation
        CPbkConstants();
        void ConstructL(RResourceFile& aResourceFile);

    private: // member data
        /// Own: unnamed text
        HBufC* iUnnamedTitle;
		/// Own: local variation manager
		CPbkLocalVariationManager* iLocalVariationManager;
	};

#endif // __CPbkConstants_H__
            
// End of File
