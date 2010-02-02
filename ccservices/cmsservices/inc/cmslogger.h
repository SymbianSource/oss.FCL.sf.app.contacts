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


#ifndef _CMSLOGGER_H
#define _CMSLOGGER_H

// logging is done only in debug
#ifdef _DEBUG
#include <flogger.h>
#include <e32debug.h>

//Logging constants
_LIT(KLogDir, "cmsservices");

#define CMS_L(s) _L(s)
#define CMS_L8(s) _L8(s)

#define CMS_DP CMSDebugWriteFormat


/**
* Log file printing utility function
* @since Series 60 v5.0
*/
inline void CMSDebugWriteFormat( TRefByValue<const TDesC> aFile, TRefByValue<const TDesC> aFmt,...) //lint !e960
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

inline void CMSDebugWriteFormat( TRefByValue<const TDesC> aFile, TRefByValue<const TDesC8> aFmt,...) //lint !e960
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

struct TCMSEmptyDebugString { };

#define CMS_L(s) TCMSEmptyDebugString()
#define CMS_L8(s) TCMSEmptyDebugString()

/// Empty debug print function for release builds.
inline void CMS_DP(TCMSEmptyDebugString)
    {
    }

template<class T1>
inline void CMS_DP(TCMSEmptyDebugString,T1)
    {
    }

template<class T1,class T2>
inline void CMS_DP(TCMSEmptyDebugString,T1,T2)
    {
    }

template<class T1,class T2,class T3>
inline void CMS_DP(TCMSEmptyDebugString,T1,T2,T3)
    {
    }

template<class T1,class T2,class T3,class T4>
inline void CMS_DP(TCMSEmptyDebugString,T1,T2,T3,T4)
    {
    }

template<class T1,class T2,class T3,class T4,class T5>
inline void CMS_DP(TCMSEmptyDebugString,T1,T2,T3,T4,T5)
    {
    }

template<class T1,class T2,class T3,class T4,class T5,class T6>
inline void CMS_DP(TCMSEmptyDebugString,T1,T2,T3,T4,T5,T6)
    {
    }

template<class T1,class T2,class T3,class T4,class T5,class T6,class T7>
inline void CMS_DP(TCMSEmptyDebugString,T1,T2,T3,T4,T5,T6,T7)
    {
    }

template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
inline void CMS_DP(TCMSEmptyDebugString,T1,T2,T3,T4,T5,T6,T7,T8)
    {
    }

#endif // _DEBUG

//TODO: Change
#define CMS_PERF_LOGGING

#ifdef CMS_PERF_LOGGING
#include <flogger.h>

_LIT( KPerfLogDir,                           "cmsservices" );
_LIT( KPerfLogFile,                          "performancelogger.txt" );

#define CMS_PERF WriteToPerfLog

inline void WriteToPerfLog(  )
    {
    TBuf<30> dateString;
    TTime time;
    time.HomeTime();
    _LIT(KDateString,"%-B%:0%J%:1%T%:2%S%.%*C4");    
    time.FormatL(dateString,KDateString);
    
    RFileLogger::Write( KPerfLogDir, KPerfLogFile, EFileLoggingModeAppend, dateString );  
    }

#endif // CMS_PERF_LOGGING


#endif // _CMSLOGGER_H
            
// End of File
