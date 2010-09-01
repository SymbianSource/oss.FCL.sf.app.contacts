/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common headers of plugin implementation
*
*/


#ifndef CCAPPMYCARDCOMMON_H
#define CCAPPMYCARDCOMMON_H

// logging
#define KMyCardLogFile CCA_L("mycardplugin.txt")
#include "ccalogger.h"

// panic
#include <e32std.h>
#include "ccappmycard.hrh"
inline void Panic( TCCAppMyCardPanics aPanic )
    {
    _LIT( KPanicText, "CCAMyCard" );
    User::Panic( KPanicText, aPanic );
    }

#endif // CCAPPMYCARDCOMMON_H
