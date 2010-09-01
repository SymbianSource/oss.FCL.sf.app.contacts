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
*     List box model class for field listboxes displaying Phonebook fields.
*
*/


#ifndef __CPbkFieldFormatter_H__
#define __CPbkFieldFormatter_H__

//  INCLUDES
#include <e32base.h>        // CBase

// FORWARD DECLARATIONS
class TPbkContactItemField;
class MPbkPhoneNumberFormat;


// CLASS DECLARATION

/**
 * Formats Phonebook field contents for display.
 */
NONSHARABLE_CLASS(CPbkFieldFormatter) : 
        public CBase
    {
    public:  // Constructors and destructor
        /**
         * Creates an instance of this class.
         */
        static CPbkFieldFormatter* NewL();

        /**
         * Destructor.
         */
        ~CPbkFieldFormatter();

		/**
		 * Returns formatted contents of aField.
		 */
		TPtrC FormatFieldContentL(const TPbkContactItemField& aField);

		/**
		 * Sets the fields time formatter to aTimeFormat.
		 */
		void SetTimeFormatL(const TDesC& aTimeFormat);

    private:  // Implementation
        CPbkFieldFormatter();
        void ConstructL();
        TPtr ReallocBufferL(TInt aNewSize);

    private:    // Data
        /// Own: buffer for handling line strings
        HBufC* iBuffer;
		/// Own: time format
		HBufC* iTimeFormat;
        /// Own: number formatter
        MPbkPhoneNumberFormat* iNumberFormatter;
    };

#endif // __CPbkFieldFormatter_H__

// End of File
