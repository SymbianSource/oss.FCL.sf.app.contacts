/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides macros for logging
*
*/


#ifndef SIMPLELOGGER_H
#define SIMPLELOGGER_H

//  INCLUDES
#include <e32def.h>


//-----------------------------------------------------------------------------
//  USER LOGGING SETTINGS
//-----------------------------------------------------------------------------

#ifndef _DEBUG

/***************************
* Logging method variants:
*   0 = No logging
*   1 = Flogger
*   2 = RDebug
***************************/ 

#define SLOGGER_LOGGING_METHOD      0   // UREL BUILD

#else

#ifdef __WINS__
#define SLOGGER_LOGGING_METHOD 2   // UDEB BUILD, WINS
#else
#define SLOGGER_LOGGING_METHOD 2   // UDEB BUILD, HW
#endif // __WINS__

#endif // _DEBUG


//-----------------------------------------------------------------------------
//  LOGGING SETTINGS (DO NOT EDIT)
//-----------------------------------------------------------------------------

#if SLOGGER_LOGGING_METHOD == 1      // Flogger

#include <flogger.h>
_LIT(KLogFolder,"logsext");
_LIT(KLogFile,"logsext.txt");

#elif SLOGGER_LOGGING_METHOD == 2    // RDebug

#include <e32svr.h>

#define DP_TP_PRINT(s) L##s
#define DP_STRA_PRINT(s) DP_TP_PRINT(s)
#define DP_STR_PRINT(t) DP_STRA_PRINT("[logsExt] ") L##t
#define DP_LIT_PRINT(s) TPtrC((const TText *) DP_STR_PRINT(s))

#endif




//-----------------------------------------------------------------------------
//  LOGGING MACROs (DO NOT EDIT)
//
//  USE THESE MACROS IN YOUR CODE
//-----------------------------------------------------------------------------

#if SLOGGER_LOGGING_METHOD == 1      // Flogger
#define _LOGDES(AA)        { RFileLogger::Write(KLogFolder(),KLogFile(),EFileLoggingModeAppend, AA ); }
#define _LOG(AA)           { RFileLogger::Write(KLogFolder(),KLogFile(),EFileLoggingModeAppend, _L(AA) ); }
#define _LOGP(AA,BB)       { RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(_L(AA)),BB); }
#define _LOGPP(AA,BB,CC)    { RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(_L(AA)),BB,CC); }
#define _LOGPPP(AA,BB,CC,DD) { RFileLogger::WriteFormat(KLogFolder(),KLogFile(),EFileLoggingModeAppend,TRefByValue<const TDesC>(_L(AA)),BB,CC,DD); }

#elif SLOGGER_LOGGING_METHOD == 2    // RDebug

#define _LOGDES(AA)          { RDebug::Print(AA); }
#define _LOG(AA) {RDebug::Print(DP_LIT_PRINT(AA));}
#define _LOGP(AA, BB) {RDebug::Print(DP_LIT_PRINT(AA),BB);}
#define _LOGPP(AA, BB, CC) RDebug::Print(DP_LIT_PRINT(AA), BB, CC);
#define _LOGPPP(AA, BB, CC, DD) RDebug::Print(DP_LIT_PRINT(AA), BB, CC, DD);

//#define _LOG(AA)             { RDebug::Print(_L(AA)); }
//#define _LOGP(AA,BB)         { RDebug::Print(_L(AA),BB); }
//#define _LOGPP(AA,BB,CC)     { RDebug::Print(_L(AA),BB,CC); }
//#define _LOGPPP(AA,BB,CC,DD) { RDebug::Print(_L(AA),BB,CC,DD); }

#else   // SLOGGER_LOGGING_METHOD == 0 or invalid -> Disable loggings
#define _LOGDES(AA)          // Example  HOLOGSTRING( aDes );
#define _LOG(AA)             // Example: HOLOGSTRING("Test");
#define _LOGP(AA,BB)         // Example: HOLOGSTRING("Test %d", aValue);
#define _LOGPP(AA,BB,CC)     // Example: HOLOGSTRING("Test %d %d", aValue1, aValue2);
#define _LOGPPP(AA,BB,CC,DD) // Example: HOLOGSTRING("Test %d %d %d", aValue1, aValue2, aValue3);

#endif  // SLOGGER_LOGGING_METHOD

#endif  // SIMPLELOGGER_H
