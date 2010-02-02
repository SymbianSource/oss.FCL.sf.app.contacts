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
* Description:     Panic functions.
*
*/





// INCLUDE FILES
#include "SpdiaPanic.h"

_LIT(KModuleName, "SDM-CTL-PANIC");
// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// Panic(TSpdiaPanic aPanic)
// Panic in the clock application
// ---------------------------------------------------------
//
GLDEF_C void Panic(TSpdiaPanic aPanic)
    {
    User::Panic(KModuleName, aPanic);
    }
// End of file
