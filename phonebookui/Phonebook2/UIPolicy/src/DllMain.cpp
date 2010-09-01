/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Standard Symbian OS DLL entry point.
*
*/


// Phonebook 2
#include "CPbk2ContactViewSortPolicy.h"
#include "CPbk2ContactCopyPolicy.h"
#include "CPbk2ContactFindPolicy.h"
#include "CPbk2ContactDuplicatePolicy.h"
#include "CPbk2ContactNameConstructionPolicy.h"
#include "CPbk2SortOrderProvider.h"
#include "Pbk2InternalUID.h"

// System includes
#include <ecom/implementationproxy.h>


/// Implementation table
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KVPbkSortPolicyImplementationUID,
        CPbk2ContactViewSortPolicy::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KVPbkCopyPolicyImplementationUID,
        CPbk2ContactCopyPolicy::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KVPbkFindPolicyImplementationUID,
        CPbk2ContactFindPolicy::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KVPbkContactDuplicatePolicyImplementationUID,
        CPbk2ContactDuplicatePolicy::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KVPbkContactNameConstructionPolicyImplementationUID,
        CPbk2ContactNameConstructionPolicy::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KPbk2AllContactsViewSortOrderProviderImplementationUID,
        CPbk2SortOrderProvider::NewL )
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// The one and only exported function that is the ECom entry point
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy
        ( TInt& aTableCount )
    {
    aTableCount =
        sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    }

// End of File
