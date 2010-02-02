/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file defines logging macros for Logs application.
*
*/


#ifndef LogsDEBUG_H
#define LogsDEBUG_H

#include <e32std.h>
#include <e32svr.h>


// uncomment to enable logging to file. 
// #define LOGS_ENABLE_FILELOGGER 
    
#ifdef _DEBUG
    
    /// Macro for defining debug-only literal strings
    #define LOGS_DEBUG_STRING(s) _L(s)
    
    #ifdef LOGS_ENABLE_FILELOGGER 
        // Prints debug to file

        #include <e32std.h>

        #define LOGS_DEBUG_PRINT   FPrint
        
        #include <flogger.h>
        
        namespace {
        
        inline void FPrint( const TRefByValue<const TDesC> aFmt, ... )
            {
            _LIT(KLogFile,"Logs.txt");
            _LIT(KLogDir,"Logs");
            VA_LIST list;
            VA_START( list, aFmt );
            RFileLogger::WriteFormat( KLogDir, KLogFile, EFileLoggingModeAppend, aFmt, list );
            }

        } // namespace
    
    #else // LOGS_ENABLE_FILELOGGER
    
        // Prints to debugger output
        #define LOGS_DEBUG_PRINT RDebug::Print
        
    #endif // LOGS_ENABLE_FILELOGGER

 
// ===========================================================================
#else // _DEBUG // No loggings
// ===========================================================================

    /// Dummy struct for checking that all LOGS_DEBUG_PRINT's define string
    /// literals using space-saving LOGS_DEBUG_STRING.
    struct TLogsEmptyDebugString { };

    /// Macro for defining debug-only literal strings (empty release version)
    #define LOGS_DEBUG_STRING(s) TLogsEmptyDebugString()

    /// Empty debug print function for release builds.
    inline void LOGS_DEBUG_PRINT(TLogsEmptyDebugString)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1>
    inline void LOGS_DEBUG_PRINT(TLogsEmptyDebugString,T1)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2>
    inline void LOGS_DEBUG_PRINT(TLogsEmptyDebugString,T1,T2)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3>
    inline void LOGS_DEBUG_PRINT(TLogsEmptyDebugString,T1,T2,T3)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4>
    inline void LOGS_DEBUG_PRINT(TLogsEmptyDebugString,T1,T2,T3,T4)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5>
    inline void LOGS_DEBUG_PRINT(TLogsEmptyDebugString,T1,T2,T3,T4,T5)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6>
    inline void LOGS_DEBUG_PRINT(TLogsEmptyDebugString,T1,T2,T3,T4,T5,T6)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    inline void LOGS_DEBUG_PRINT(TLogsEmptyDebugString,T1,T2,T3,T4,T5,T6,T7)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
    inline void LOGS_DEBUG_PRINT(TLogsEmptyDebugString,T1,T2,T3,T4,T5,T6,T7,T8)
        {
        }

#endif // _DEBUG

#endif // LogsDEBUG_H
// End of File
