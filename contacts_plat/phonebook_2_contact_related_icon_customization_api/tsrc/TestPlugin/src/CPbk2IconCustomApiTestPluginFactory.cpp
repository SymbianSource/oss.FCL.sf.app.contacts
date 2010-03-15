/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/


// INCLUDE FILES
#include <e32base.h>
#include <ecom\implementationproxy.h>

#include "CPbk2IconCustomApiTestPlugin.h"


// ----------------------------------------------------------------------------
// ImplementationTable, from ECOM architecture
// ----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(
        KCPbk2IconCustomApiTestPluginImplValue,
        CPbk2IconCustomApiTestPlugin::NewL )
    };


// ----------------------------------------------------------------------------
// ImplementationGroupProxy function, from ECOM architecture. Return ECOM
// implementations, which are registered with the ECOM resource definition.
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount =  sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// End of file

