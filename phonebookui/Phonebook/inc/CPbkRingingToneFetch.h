/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       Class for fetching ringing tones to phonebook.
*
*/


#ifndef __CPbkRingingToneFetch_H__
#define __CPbkRingingToneFetch_H__

//  INCLUDES
#include <e32base.h>  // CBase

// FORWARD DECLARATIONS
class CPbkContactItem;
class CPbkContactEngine;
class CFLDFileListContainer;

// CLASS DECLARATION
/**
 * @internal Only Phonebook internal use supported!
 *
 * Class for Fetching ringing tones to phonebook.
 */
class CPbkRingingToneFetch 
        : public CBase
    {
    public: // constructor & destructor
        /**
         * Creates a new instance of this class.
		 * @param reference to phonebook engine
         */
        IMPORT_C static CPbkRingingToneFetch* NewL(CPbkContactEngine& aEngine);

        /**
         * Destructor.
         */
        ~CPbkRingingToneFetch();

    public: // interface
        /**
         * Fetch ringing tone.
		 * @param aRingingToneFile the selected filename will be stored here
		 * @return ETrue if user selected a file, EFalse otherwise
         */
        IMPORT_C TBool FetchRingingToneL(TFileName& aRingingToneFile);

        /**
         * Assign fetched ringing tone to the open contact aItem.
		 * @param aItem the contact item to assign the tone with
		 * @param aRingingToneFile the ringing tone to assign
         */
        IMPORT_C void SetRingingToneL(CPbkContactItem& aItem,
			TFileName& aRingingToneFile);

    private: // implementation
        void ConstructL();
        CPbkRingingToneFetch(CPbkContactEngine& aEngine);
        void HandleRingingToneRemovalL(const TDesC& aOldTone);
        void SetMaxToneFileSizeL( CFLDFileListContainer* aFl );
        
    private: // data members
        /// Ref: contact engine reference
        CPbkContactEngine& iEngine;
        /// Own: prompt text for ringing tone popup list
        HBufC* iTitle;
        /// Own: text for emptying the ringing tone
        HBufC* iNoSound;
    };

#endif // __CPbkRingingToneFetch_H__

//  End of File
