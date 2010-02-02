/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include <e32std.h>

#include "MLogsExtensionFactory.h"
#include "clogsextensionfactory.h"

/**
 * Standard Symbian OS DLL entry point function.
 */
#ifndef EKA2
// PC-lint error: #40 Undeclared identifier 'TDllReason'
// PC-lint error: #10 Expecting ';'
// Explanation: 'TDllReason' not declared in EKA2
GLDEF_C TInt E32Dll( TDllReason /*aReason*/ )
    {
    return KErrNone;
    }
#endif // EKA2


// Entry point to use this polymorphic dll
EXPORT_C MLogsExtensionFactory* CreateExtensionFactoryL()
    {    
    return CLogsExtensionFactory::NewL();
    }

// End of File

