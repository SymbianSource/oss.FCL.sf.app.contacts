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
*     Phonebook phone number formatting.
*
*/


#ifndef __CPbkPhoneNumberDefaultFormatter_H__
#define __CPbkPhoneNumberDefaultFormatter_H__

//  INCLUDES
#include <e32base.h>
#include "MPbkPhoneNumberFormat.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 * Phonebook number formatter. 
 */
NONSHARABLE_CLASS(CPbkPhoneNumberDefaultFormatter) : 
        public CBase,
		public MPbkPhoneNumberFormat
    {
    public:  // Constructors and destructor
        /**
         * Constructor.
         */
        static CPbkPhoneNumberDefaultFormatter* NewL
                (TInt aMaxPhoneNumberDisplayLength);

        /**
         * Destructor.
         */
		~CPbkPhoneNumberDefaultFormatter();

    public:  // from MPbkPhoneNumberFormat
		void SetMaxBufferLengthL(TInt aMaxLength);
        TPtrC FormatPhoneNumberForDisplay(const TDesC& aOriginalPhoneNumber);
    
    private: // implementation
        CPbkPhoneNumberDefaultFormatter();
        void ConstructL(TInt aMaxPhoneNumberDisplayLength);

    private: // data members
        /// Own: formatting buffer
        HBufC* iFormatterNumberBuffer;
    
    };

#endif // __CPbkPhoneNumberDefaultFormatter_H__
            
// End of File
