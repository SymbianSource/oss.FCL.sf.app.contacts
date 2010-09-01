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
*     PbkFieldFormatterFactory implementation factory.
*
*/


#ifndef __PbkFieldFormatterFactory_H__
#define __PbkFieldFormatterFactory_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MPbkPhoneNumberFormat;
class PbkFieldFormatterFactory;

// CLASS DECLARATION

/**
 * MPbkPhoneNumberFormat implementation factory.
 *
 * @see MPbkPhoneNumberFormat
 */
NONSHARABLE_CLASS(PbkFieldFormatterFactory)
    {
    public:  // Interface
        /**
         * Creates and returns a MPbkPhoneNumberFormat implementation.
		 *
		 * @param aMaxPhoneNumberDisplayLength	maximum length of a phone 
		 *          number on the display. The returned phone number formatter
		 *          may not support longer phone numbers.
         * @return a phone number formatted implementation.
         */
        static MPbkPhoneNumberFormat* CreatePhoneNumberFormatterL
            (TInt aMaxPhoneNumberDisplayLength);

    private:  // Disabled functions
        PbkFieldFormatterFactory();
        PbkFieldFormatterFactory(const PbkFieldFormatterFactory&);
    };

#endif // __PbkFieldFormatterFactory_H__
            
// End of File
