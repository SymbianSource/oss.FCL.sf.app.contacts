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
* Description:  
*
*/


// INCLUDES
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "CContactStoreDomain.h"
#include <VPbkPrivateUid.h>

typedef VPbkSimStore::CContactStoreDomain* (*NewLPtr)(TAny*);

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KVPbkSimStoreDomainImplementationUID, 
        static_cast<NewLPtr>(VPbkSimStore::CContactStoreDomain::NewL))
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// end if file
