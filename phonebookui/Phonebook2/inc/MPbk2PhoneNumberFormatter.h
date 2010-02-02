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
* Description:  Phonebook 2 phone number formatter interface.
*
*/


#ifndef MPBK2PHONENUMBERFORMATTER_H
#define MPBK2PHONENUMBERFORMATTER_H

// CLASS DECLARATION

/**
 * Phonebook 2 phone number formatter interface.
 */
class MPbk2PhoneNumberFormatter
    {
    public: // Interface

        /**
         * Virtual destructor.
         */
        virtual ~MPbk2PhoneNumberFormatter()
            {}

        /**
         * Sets formatting buffer capacity.
         *
         * @param aMaxLength    Length of the phone number to be formatted.
         */
        virtual void SetMaxBufferLengthL(
                TInt aMaxLength ) = 0;

        /**
         * Formats phone number for display. The formatting is done on
         * best-effort basis. If there is insufficient memory to perform the
         * formatting the original phone number may be returned.
         *
         * @param aOriginalPhoneNumber      The phone number to format.
         * @return  The formatted phone number. The return value is
         *          valid until this function is called again.
         */
        virtual TPtrC FormatPhoneNumberForDisplay(
                const TDesC& aOriginalPhoneNumber ) = 0;
    };

#endif // MPBK2PHONENUMBERFORMATTER_H

// End of File
