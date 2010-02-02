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
* Description:  Standard Symbian OS DLL entry point function.
*
*/


// Phonebook 2
#include "CPbk2MigrationSupport.h"
#include <Pbk2InternalUID.h>

// System includes
#include <ecom/implementationproxy.h>

/// Implementation table
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KPbk2MigrationSupportImplementationUID,
        CPbk2MigrationSupport::NewL )
    };

// --------------------------------------------------------------------------
// ImplementationGroupProxy
// --------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy
        ( TInt& aTableCount )
    {
    aTableCount =
        sizeof( ImplementationTable ) / sizeof( ImplementationTable[0] );
    return ImplementationTable;
    }

// End of File
