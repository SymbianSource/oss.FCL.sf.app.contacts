/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __SERVERDEFINES2_H__
#define __SERVERDEFINES2_H__

#include <e32def.h>
#include <e32base.h>

_LIT( KDateFormatFileName,                  "%1%2%3");
_LIT( KTimeFormatFileName,                  "%H%T%S");
_LIT( KCacheDateFormat,                     "%1-%2-%3");
_LIT( KCacheTimeFormat,                     "%H:%T:%S");
_LIT( KDateTimeFormat,                      "%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S");

const TInt KFirstParam                      = 0;
const TInt KSecondParam                     = 1;
const TInt KThirdParam                      = 2;
const TInt KFourthParam                     = 3;

#endif // #ifndef __SERVERDEFINES_H__

// End of File
