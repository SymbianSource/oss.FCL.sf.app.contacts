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
* Description:  Phonebook 2 phone number formatter factory.
*
*/


#ifndef PBK2PHONENUMBERFORMATTERFACTORY_H
#define PBK2PHONENUMBERFORMATTERFACTORY_H

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MPbk2PhoneNumberFormatter;

// CLASS DECLARATION

/**
 * Phonebook 2 phone number formatter factory.
 *
 * Responsible for creating appropriate phone number formatter.
 */
NONSHARABLE_CLASS(Pbk2PhoneNumberFormatterFactory)
    {
    public: // Interface

        /**
         * Creates and returns phone number formatter implementation.
         *
         * @param aMaxDisplayLength     Maximum length of a phone number on
         *                              the display. The returned phone
         *                              number formatter may not support
         *                              longer phone numbers.
         * @return  Phone number formatter.
         */
        IMPORT_C static MPbk2PhoneNumberFormatter* CreatePhoneNumberFormatterL(
                TInt aMaxDisplayLength );
    };

#endif // PBK2PHONENUMBERFORMATTERFACTORY_H

// End of File
