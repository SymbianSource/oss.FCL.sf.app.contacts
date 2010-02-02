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


#include <Pbk2PhoneNumberFormatterFactory.h>

// Phonebook 2
#include "CPbk2NumberGroupingFormatter.h"
#include "CPbk2DefaultPhoneNumberFormatter.h"

// System includes
#include <centralrepository.h>
#include <NumberGroupingCRKeys.h>

// --------------------------------------------------------------------------
// Pbk2PhoneNumberFormatterFactory::CreatePhoneNumberFormatterL
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2PhoneNumberFormatter*
        Pbk2PhoneNumberFormatterFactory::CreatePhoneNumberFormatterL
            ( TInt aMaxDisplayLength )
    {
    MPbk2PhoneNumberFormatter* ret = NULL;
    CRepository* repository = NULL;
    TInt value( 0 );
    TRAPD( err, repository = CRepository::NewL( KCRUidNumberGrouping ) );
    if( err == KErrNone )
        {
        err = repository->Get( KNumberGrouping, value );
        }
    delete repository;

    if( value == 1 )
        {
        ret = CPbk2NumberGroupingFormatter::NewL( aMaxDisplayLength );
        }
    else
        {
        ret = CPbk2DefaultPhoneNumberFormatter::NewL( aMaxDisplayLength );
        }
    return ret;
    }

// End of File
