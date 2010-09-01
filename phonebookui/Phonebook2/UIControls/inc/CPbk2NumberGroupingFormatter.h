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
* Description:  Phonebook 2 number grouping phone number formatter.
*
*/


#ifndef CPBK2NUMBERGROUPINGFORMATTER_H
#define CPBK2NUMBERGROUPINGFORMATTER_H

// INCLUDES
#include <e32base.h>
#include <MPbk2PhoneNumberFormatter.h>

// FORWARD DECLARATIONS
class CPNGNumberGrouping;

// CLASS DECLARATION

/**
 * Phonebook 2 number grouping phone number formatter.
 *
 * Responsible for formatting phone number.
 */
NONSHARABLE_CLASS(CPbk2NumberGroupingFormatter) :
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
           static CPbk2NumberGroupingFormatter* NewL(
            TInt aMaxDisplayLength );

        /**
         * Destructor.
         */
        virtual ~CPbk2NumberGroupingFormatter();

    public: // From MPbk2PhoneNumberFormatter
        void SetMaxBufferLengthL(
                TInt aMaxLength );
        TPtrC FormatPhoneNumberForDisplay(
                const TDesC& aOriginalPhoneNumber );

    private: // Implementation
        CPbk2NumberGroupingFormatter();
        void ConstructL(
                TInt aMaxDisplayLength );

    private: // Data
        /// Own: Number grouping object
        CPNGNumberGrouping* iNumberGrouping;
        /// Own: Formatting buffer
        HBufC* iFormatterNumberBuffer;
    };

#endif // CPBK2NUMBERGROUPINGFORMATTER_H

// End of File
