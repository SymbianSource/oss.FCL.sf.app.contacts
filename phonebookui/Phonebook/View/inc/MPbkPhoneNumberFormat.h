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
*     Phonebook phone number formatting interface.
*
*/


#ifndef __MPbkPhoneNumberFormat_H__
#define __MPbkPhoneNumberFormat_H__

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
 * Phonebook phone number formatting interface.
 *
 * @see PbkFieldFormatterFactory::CreatePhoneNumberFormatterL
 */
class MPbkPhoneNumberFormat
    {
    public:
        /**
         * Virtual destructor.
         */
        virtual ~MPbkPhoneNumberFormat()
			{
			}

		/**
		 * Sets formatting buffer capacity. Ensures that 
		 * FormatPhoneNumberForDisplay is able to format a phone number of 
		 * length aMaxLength.
		 *
		 * @param aMaxLength	length of the phone number to be formatted.
		 */
		virtual void SetMaxBufferLengthL(TInt aMaxLength) =0;

        /**
         * Formats phone number for display. The formatting is done on 
         * best-effort basis. If there is insufficient memory to perform the
         * formatting the original phone number may be returned.
         *
         * @param aOriginalPhoneNumber  the phone number to format.
         * @return  the formatted phone number. The return value is valid until
         *          this function is called again.
         */
        virtual TPtrC FormatPhoneNumberForDisplay
            (const TDesC& aOriginalPhoneNumber) =0;
    };

#endif // __MPbkPhoneNumberFormat_H__

// End of File
