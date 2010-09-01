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
* Description:  PCS Server Debug macros
*
*/

#ifndef __PCS_DEBUG_H__
#define __PCS_DEBUG_H__

#include <e32svr.h>
#include <e32std.h>
#include <badesca.h>
#include <f32file.h>


//==============================================================================
// winscw udeb
//------------------------------------------------------------------------------

#ifdef __WINS__
#ifdef _DEBUG

#define PRINT( txt )                                \
    { TBuf<512> __buf; __buf.Format(txt);                          RDebug::Print( _L("[PCS] %S"), &__buf ); }
#define PRINT1( txt, arg1 )                         \
    { TBuf<512> __buf; __buf.Format(txt,arg1);                     RDebug::Print( _L("[PCS] %S"), &__buf ); }
#define PRINT2( txt, arg1, arg2 )                   \
    { TBuf<512> __buf; __buf.Format(txt,arg1,arg2);                RDebug::Print( _L("[PCS] %S"), &__buf ); }
#define PRINT3( txt, arg1, arg2, arg3 )             \
    { TBuf<512> __buf; __buf.Format(txt,arg1,arg2,arg3);           RDebug::Print( _L("[PCS] %S"), &__buf ); }
#define PRINT4( txt, arg1, arg2, arg3, arg4 )       \
    { TBuf<512> __buf; __buf.Format(txt, arg1,arg2,arg3,arg4);     RDebug::Print( _L("[PCS] %S"), &__buf ); }
#define PRINT5( txt, arg1, arg2, arg3, arg4, arg5 ) \
    { TBuf<512> __buf; __buf.Format(txt,arg1,arg2,arg3,arg4,arg5); RDebug::Print( _L("[PCS] %S"), &__buf ); }
#define PRINT_BOOT_PERFORMANCE( txt )
#define PRINT1_BOOT_PERFORMANCE( txt, arg1 )

#endif // _DEBUG
#endif // __WINS__

//==============================================================================
// armv5 udeb
//------------------------------------------------------------------------------

#ifndef __WINS__
#ifdef _DEBUG

#include <flogger.h>

_LIT(KLogDirFullName, "c:\\logs\\");
_LIT(KLogDir,         "Pcs");
_LIT(KLogFile,        "PcsServer.txt");

//#define __BOOT_PERFORMANCE
#ifdef __BOOT_PERFORMANCE // Boot Peformance logs only

#define PRINT( txt )
#define PRINT1( txt, arg1 )
#define PRINT2( txt, arg1, arg2 )
#define PRINT3( txt, arg1, arg2, arg3 )
#define PRINT4( txt, arg1, arg2, arg3, arg4 )
#define PRINT5( txt, arg1, arg2, arg3, arg4, arg5 )
#define PRINT_BOOT_PERFORMANCE( txt )                                \
    RFileLogger::Write(KLogDir,KLogFile,EFileLoggingModeAppend,txt);
#define PRINT1_BOOT_PERFORMANCE( txt, arg1 )                         \
    RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,TRefByValue<const TDesC>(txt),arg1);

#else

#define PRINT( txt )                                \
    RFileLogger::Write(KLogDir,KLogFile,EFileLoggingModeAppend,txt);
#define PRINT1( txt, arg1 )                         \
    RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,TRefByValue<const TDesC>(txt),arg1);
#define PRINT2( txt, arg1, arg2 )                   \
    RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,TRefByValue<const TDesC>(txt),arg1,arg2);
#define PRINT3( txt, arg1, arg2, arg3 )             \
    RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,TRefByValue<const TDesC>(txt),arg1,arg2,arg3);
#define PRINT4( txt, arg1, arg2, arg3, arg4 )       \
    RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,TRefByValue<const TDesC>(txt),arg1,arg2,arg3,arg4);
#define PRINT5( txt, arg1, arg2, arg3, arg4, arg5 ) \
    RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,TRefByValue<const TDesC>(txt),arg1,arg2,arg3,arg4,arg5);
#define PRINT_BOOT_PERFORMANCE( txt )
#define PRINT1_BOOT_PERFORMANCE( txt, arg1 )

#endif // __BOOT_PERFORMANCE

#endif // __WINS__
#endif // _DEBUG

//==============================================================================
// winscw urel AND armv5 urel
//------------------------------------------------------------------------------

#ifndef _DEBUG

#define PRINT( txt )
#define PRINT1( txt, arg1 )
#define PRINT2( txt, arg1, arg2 )
#define PRINT3( txt, arg1, arg2, arg3 )
#define PRINT4( txt, arg1, arg2, arg3, arg4 )
#define PRINT5( txt, arg1, arg2, arg3, arg4, arg5 )
#define PRINT_BOOT_PERFORMANCE( txt )
#define PRINT1_BOOT_PERFORMANCE( txt, arg1 )

#endif // _DEBUG

//==============================================================================


#ifdef _DEBUG
#define PRINTQUERY( txt, queryArg )         CPcsDebug::PrintQueryL(txt, queryArg);
#define PRINTQUERYLIST( txt, queryListArg ) CPcsDebug::PrintQueryListL(txt, queryListArg);
#define PRINTMATCHLOC( txt, matchLocsArg )  CPcsDebug::PrintMatchLoc(txt, matchLocsArg);
#define PRINTMATCHSET( txt, matchSetArg )   CPcsDebug::PrintMatchSet(txt, matchSetArg);
#else
#define PRINTQUERY( txt, queryArg )
#define PRINTQUERYLIST( txt, queryListArg )
#define PRINTMATCHLOC( txt, matchLocsArg )
#define PRINTMATCHSET( txt, matchSetArg )
#endif // _DEBUG

#define __LATENCY_MARK( str )     CPcsDebugWrapper::__LatencyMarkStartL(str);
#define __LATENCY_MARKEND( str )  CPcsDebugWrapper::__LatencyMarkEnd(str);

// Forward declarations
class CPsQueryItem;
class CPsQuery;
class TPsMatchLocation;


class CPcsDebug : public CBase
{
	public:
	    IMPORT_C static CPcsDebug* NewL();
	    IMPORT_C void Mark(TRefByValue<const TDesC> str,...);
	    IMPORT_C void UnMark(TRefByValue<const TDesC> str,...);
	    IMPORT_C static void PrintQueryL(const TDesC& aPreTxt, CPsQuery& aQuery);
	    IMPORT_C static void PrintQueryListL(const TDesC& aPreTxt, RPointerArray<CPsQuery>& aPsQueryList);
        IMPORT_C static void PrintMatchLoc(const TDesC& aPreTxt, RArray<TPsMatchLocation>& aMatchLocs);
        IMPORT_C static void PrintMatchSet(const TDesC& aPreTxt, RPointerArray<TDesC>& aMatchSet);
        IMPORT_C static void PrintMatchSet(const TDesC& aPreTxt, CDesCArray& aMatchSet);
	    
	private:
	    void ConstructL();
	    
	private:
		TTime startTime;
		TTime endTime;
};


class CPcsDebugArr : public CBase
{
	public:
	    IMPORT_C static CPcsDebugArr* NewL();
	    IMPORT_C void Push(CPcsDebug& dbg);
	    IMPORT_C CPcsDebug* Pop();
	    IMPORT_C TBool IsEmpty();
	    
    public:
        ~CPcsDebugArr();
        	    
	private:	    
	    RPointerArray<CPcsDebug> debugArray;
};

class CPcsDebugWrapper : public CBase 
{
	public:
	    IMPORT_C static void __LatencyMarkStartL(TRefByValue<const TDesC> str);
	    IMPORT_C static void __LatencyMarkEnd(TRefByValue<const TDesC> str);
};

#endif // __PCS_DEBUG_H__

// End of File
