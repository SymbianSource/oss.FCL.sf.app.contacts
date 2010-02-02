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
* Description:  Debugging utilities
*
*/



#ifndef VPBKDEBUG_H
#define VPBKDEBUG_H

//  INCLUDES
#include "VPbkConfig.hrh"
#include "VPbkDebugEntryAndExit.h"

#include <e32std.h>
#include <e32svr.h>

// MACROS
#ifdef _DEBUG

    /// Helper macro for defining debug-only test code
    #define VPBK_DEBUG_ONLY(f) f

#else

    /// Helper macro for defining debug-only test code (empty release version)
    #define VPBK_DEBUG_ONLY(f)

#endif

#ifdef VPBK_ENABLE_DEBUG_PRINT
    // Debug logging is enabled

    // warn if this is a release build
    #ifndef __WINSCW__
    #ifndef _DEBUG
        #if defined(__VC32__)
            #pragma message("Warning: VirtualPhonebook debug printing is on in release build!")
        #else // __GCC32__
            #warning "VirtualPhonebook debug printing is on in release build!"
        #endif // __VC32__
    #endif
    #endif // __WINSCW__

    /**
     * Macro for defining debug-only literal strings.
     */
    #define VPBK_DEBUG_STRING(s) _L(s)

    #ifdef VPBK_ENABLE_DEBUG_LOGGER
        // Print to log file
        #define VPBK_DEBUG_PRINT PbkPrintToLog

        #include <flogger.h>

        namespace {
        void PbkPrintToLog( TRefByValue<const TDesC> aFormat, ... )
            {
            _LIT( KLogDir, "VPbk" );
            _LIT( KLogName, "VPbk.log" );

            VA_LIST args;
            VA_START( args, aFormat );
            RFileLogger::WriteFormat
                (KLogDir, KLogName, EFileLoggingModeAppend, aFormat, args);
            VA_END( args );
            }
        }  // namespace

    #else
        /**
         * Prints to debugger output.
         */
        #define VPBK_DEBUG_PRINT RDebug::Print

    #endif // #ifdef VPBK_ENABLE_DEBUG_LOGGER

#else

    /**
     * Dummy struct for checking that all VPBK_DEBUG_PRINT's define string
     * literals using space-saving VPBK_DEBUG_STRING.
     */
    struct TPbkEmptyDebugString { };

    /**
     * Macro for defining debug-only literal strings (empty release version)
     */
    #define VPBK_DEBUG_STRING(s) TPbkEmptyDebugString()

    /// Empty debug print function for release builds.
    inline void VPBK_DEBUG_PRINT(TPbkEmptyDebugString)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1>
    inline void VPBK_DEBUG_PRINT(TPbkEmptyDebugString,T1)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2>
    inline void VPBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3>
    inline void VPBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2,T3)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4>
    inline void VPBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2,T3,T4)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5>
    inline void VPBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2,T3,T4,T5)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6>
    inline void VPBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2,T3,T4,T5,T6)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    inline void VPBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2,T3,T4,T5,T6,T7)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
    inline void VPBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2,T3,T4,T5,T6,T7,T8)
        {
        }

#endif


#ifdef VPBK_ENABLE_PROFILE
    #define __VPBK_PROFILE_START(aBin) RDebug::ProfileStart(aBin)
    #define __VPBK_PROFILE_END(aBin)   RDebug::ProfileEnd(aBin)

    // Needs VPBK_ENABLE_DEBUG_LOGGER
    #define __VPBK_PROFILE_DISPLAY(aBin) \
        { \
        TProfile result; \
        RDebug::ProfileResult(&result, aBin, 1); \
        VPBK_DEBUG_PRINT \
            (VPBK_DEBUG_STRING("Profile bin %d:  Calls: %d, Time: %d\n" ), \
        aBin, result.iCount, result.iTime); \
        }
    #define __VPBK_PROFILE_RESET(aBin) RDebug::ProfileReset(aBin, 1)
#else
    #define __VPBK_PROFILE_START(aBin)
    #define __VPBK_PROFILE_END(aBin)
    #define __VPBK_PROFILE_DISPLAY(aBin)
    #define __VPBK_PROFILE_RESET(aBin)

#endif // VPBK_ENABLE_PROFILE


#ifdef _DEBUG

/**
 * Asserts and checks that statement doesn't allocate any memory. If
 * allocation happens evaluates failed.
 */
#define VPBK_ASSERT_NO_ALLOC(statement,failed) \
    { __UHEAP_FAILNEXT(1); \
    TRAPD(err, statement); \
    __UHEAP_RESET; \
    if (err==KErrNoMemory) { failed; } }

#else

/**
 * Release version of macro. Just executes statement.
 */
#define VPBK_ASSERT_NO_ALLOC(statement,failed) statement

#endif

#endif // VPBKDEBUG_H

// End of File
