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
* Description:  Provides methods for fetching ringing tones into Phonebook.
*
*/


#ifndef CPBK2RINGINGTONEFETCH_H
#define CPBK2RINGINGTONEFETCH_H

//  INCLUDES
#include <e32base.h>  // CBase

// FORWARD DECLARATIONS
class CMediaFileList;

// CLASS DECLARATION
/**
 * @internal Only Phonebook internal use supported!
 *
 * Class for Fetching ringing tones to phonebook.
 */
NONSHARABLE_CLASS(CPbk2RingingToneFetch) :
        public CBase
    {
    public: // constructor & destructor
        /**
         * Creates a new instance of this class.
         * @return A new instance of this class.
         */
        IMPORT_C static CPbk2RingingToneFetch* NewL();

        /**
         * Destructor.
         */
        ~CPbk2RingingToneFetch();

    public: // interface
        /**
         * Fetch ringing tone.
         * @param aRingingToneFile the selected filename will be stored here,
         *        It will be set empty, if user selects default ringtone.
         * @return ETrue if user selected a file, EFalse otherwise
         */
    	IMPORT_C TBool FetchRingingToneL(
            TFileName& aRingingToneFile );

    private: // implementation
        CPbk2RingingToneFetch();
        void ConstructL();
        void SetMaxToneFileSizeL( CMediaFileList* aFl );

    private: // data members
        /// Own: prompt text for ringing tone popup list
        HBufC* iTitle;
        /// Own: text for emptying the ringing tone
        HBufC* iNoSound;
    };

#endif // CPBK2RINGINGTONEFETCH_H

//  End of File
