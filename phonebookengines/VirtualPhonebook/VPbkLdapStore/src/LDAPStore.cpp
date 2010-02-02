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
* Description:  This file contains LDAP Store ECOM entry point.
*
*/


// Includes
#include <e32std.h>
#include <implementationproxy.h>
#include "contactstoredomain.h"
#include "ldapuid.h"

// Store domain
typedef LDAPStore::CContactStoreDomain* ( *NewLPtr )( TAny* );

// Interface implementation map
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KLDAPStoreDomainImplementationUID,
        static_cast<NewLPtr>( LDAPStore::CContactStoreDomain::NewL ) )
    };

// --------------------------------------------------------------------------
// ImplementationGroupProxy
// The exported function that is the ECom entry point.
// --------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }



// End of file
