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
* Description:  This is class is for logging purposes.
*
*/


#ifndef _CCALOGGER_H
#define _CCALOGGER_H

// logging is done only in debug
#ifdef _DEBUG
#include <flogger.h>
#include <e32debug.h>

//Logging constants
_LIT(KLogDir, "cca");

#define CCA_L(s) _L(s)
#define CCA_L8(s) _L8(s)

#define CCA_DP CCADebugWriteFormat


/**
* Log file printing utility function
* @since Series 60 v5.0
*/
inline void CCADebugWriteFormat( TRefByValue<const TDesC> aFile, TRefByValue<const TDesC> aFmt,...) //lint !e960
    {

    TBuf< 256 > buffer;

        {
        VA_LIST list;
        VA_START (list, aFmt); //lint !e960
        TBuf< 300 > formatted;
        formatted.FormatList( aFmt, list );
        buffer.Append( formatted.Left( buffer.MaxLength() - buffer.Length() ) ); 
        }
    
    RFileLogger::Write( KLogDir, aFile, EFileLoggingModeAppend, buffer );
    RDebug::RawPrint(buffer);
  
    }

/**
* Log file printing utility function
* @since Series 60 v5.0
*/

inline void CCADebugWriteFormat( TRefByValue<const TDesC> aFile, TRefByValue<const TDesC8> aFmt,...) //lint !e960
    {
    TBuf8< 256 > buffer;
        {
        VA_LIST list;
        VA_START (list, aFmt); //lint !e960
        TBuf8< 300 > formatted;
        formatted.FormatList( aFmt, list );
        buffer.Append( formatted.Left( buffer.MaxLength() - buffer.Length() ) ); 
        }    
    RFileLogger::Write( KLogDir, aFile, EFileLoggingModeAppend, buffer );
    RDebug::RawPrint(buffer);
    }


#else // _DEBUG

struct TCCAEmptyDebugString { };

#define CCA_L(s) TCCAEmptyDebugString()
#define CCA_L8(s) TCCAEmptyDebugString()

/// Empty debug print function for release builds.
inline void CCA_DP(TCCAEmptyDebugString)
    {
    }

template<class T1>
inline void CCA_DP(TCCAEmptyDebugString,T1)
    {
    }

template<class T1,class T2>
inline void CCA_DP(TCCAEmptyDebugString,T1,T2)
    {
    }

template<class T1,class T2,class T3>
inline void CCA_DP(TCCAEmptyDebugString,T1,T2,T3)
    {
    }

template<class T1,class T2,class T3,class T4>
inline void CCA_DP(TCCAEmptyDebugString,T1,T2,T3,T4)
    {
    }

template<class T1,class T2,class T3,class T4,class T5>
inline void CCA_DP(TCCAEmptyDebugString,T1,T2,T3,T4,T5)
    {
    }

template<class T1,class T2,class T3,class T4,class T5,class T6>
inline void CCA_DP(TCCAEmptyDebugString,T1,T2,T3,T4,T5,T6)
    {
    }

template<class T1,class T2,class T3,class T4,class T5,class T6,class T7>
inline void CCA_DP(TCCAEmptyDebugString,T1,T2,T3,T4,T5,T6,T7)
    {
    }

template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
inline void CCA_DP(TCCAEmptyDebugString,T1,T2,T3,T4,T5,T6,T7,T8)
    {
    }

#endif // _DEBUG

//TODO: Change
#define PERF_LOGGING

#ifdef PERF_LOGGING
#include <flogger.h>

_LIT( KPerfLogDir,                           "cca" );
_LIT( KPerfLogFile,                          "performancelogger.txt" );

#define CCA_PERF WriteToPerfLog

inline void WriteToPerfLog(  )
    {
    TBuf<30> dateString;
    TTime time;
    time.HomeTime();
    _LIT(KDateString,"%-B%:0%J%:1%T%:2%S%.%*C4");    
    time.FormatL(dateString,KDateString);
    
    RFileLogger::Write( KPerfLogDir, KPerfLogFile, EFileLoggingModeAppend, dateString );  
    }

#endif // PERF_LOGGING


#endif // _CCALOGGER_H
            
// End of File
