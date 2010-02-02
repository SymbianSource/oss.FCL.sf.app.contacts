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
* Description:  Phonebook 2 default phone number formatter.
*
*/


#ifndef CPBK2DEFAULTPHONENUMBERFORMATTER_H
#define CPBK2DEFAULTPHONENUMBERFORMATTER_H

// INCLUDES
#include <e32base.h>
#include <MPbk2PhoneNumberFormatter.h>

// CLASS DECLARATION

/**
 * Phonebook 2 default phone number formatter.
 *
 * Responsible for formatting phone number.
 */
NONSHARABLE_CLASS(CPbk2DefaultPhoneNumberFormatter) :
        public CBase,
        public MPbk2PhoneNumberFormatter
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aMaxDisplayLength     Maximum phone number display length.
         * @return  A new instance of this class.
         */
        static CPbk2DefaultPhoneNumberFormatter* NewL(
                TInt aMaxDisplayLength );

        /**
         * Destructor
         */
        virtual ~CPbk2DefaultPhoneNumberFormatter();

    public: // From MPbk2PhoneNumberFormatter
        void SetMaxBufferLengthL(
                TInt aMaxLength );
        TPtrC FormatPhoneNumberForDisplay(
                const TDesC& aOriginalPhoneNumber );

    private: // Implementation
        CPbk2DefaultPhoneNumberFormatter();
        void ConstructL(
            TInt aMaxDisplayLength );

    private: // Data
        /// Own: Formatting buffer
        HBufC* iFormatterNumberBuffer;
    };

#endif // CPBK2DEFAULTPHONENUMBERFORMATTER_H

// End of File
