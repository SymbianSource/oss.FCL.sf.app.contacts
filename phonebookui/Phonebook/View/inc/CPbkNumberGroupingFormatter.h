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
*     Phonebook Phone number formatting.
*
*/


#ifndef __CPbkNumberGroupingFormatter_H__
#define __CPbkNumberGroupingFormatter_H__

//  INCLUDES
#include "MPbkPhoneNumberFormat.h"

// FORWARD DECLARATIONS
class CPNGNumberGrouping;

// CLASS DECLARATION

/**
 * Phonebook Contact editor field base class. 
 */
NONSHARABLE_CLASS(CPbkNumberGroupingFormatter) : 
        public CBase,
		public MPbkPhoneNumberFormat
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aMaxPhoneNumberDisplayLength max phone number display length
         */
		static CPbkNumberGroupingFormatter* NewL
            (TInt aMaxPhoneNumberDisplayLength);
        /**
         * Destructor.
         */
		~CPbkNumberGroupingFormatter();

    public:  // from MPbkPhoneNumberFormat
		void SetMaxBufferLengthL(TInt aMaxLength);
        TPtrC FormatPhoneNumberForDisplay(const TDesC& aOriginalPhoneNumber);

    private: // Implementation
        CPbkNumberGroupingFormatter();
		void ConstructL(TInt aMaxPhoneNumberDisplayLength);
                
    private:  // Data
		/// Own: number grouping object
		CPNGNumberGrouping* iNumberGrouping;
        /// Own: active number formatting buffer
        HBufC* iFormatterNumberBuffer;
    };

#endif // __CPbkNumberGroupingFormatter_H__
            
// End of File
