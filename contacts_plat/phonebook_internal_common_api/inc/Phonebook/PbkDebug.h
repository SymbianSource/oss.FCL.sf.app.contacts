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
* Description: 
*     Phonebook debugging utilities.
*
*/


#ifndef __PbkDebug_H__
#define __PbkDebug_H__

//  INCLUDES
#include <PbkConfig.hrh>

#include <e32std.h>
#include <e32svr.h>


// MACROS
#ifdef _DEBUG

    /// Helper macro for defining debug-only test code
    #define PBK_DEBUG_ONLY(f) f

    /**
     * Helper macro for calling invariant on function entry and (normal) exit.
     * Declare typedef SelfType as the type of the class you're using this
     * macro in.
     */
    #define PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT TPbkDebugCallInvariantOnEntryAndExit<SelfType> __pbk_debug_test_invariant(this)

#else

    /**
     * Helper macro for calling invariant on function entry and (normal) exit.
     * This is empty release build version.
     */
    #define PBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT

    /// Helper macro for defining debug-only test code (empty release version)
    #define PBK_DEBUG_ONLY(f)

#endif

#ifdef PBK_ENABLE_DEBUG_PRINT
    // Debug logging is enabled

    /**
     * Macro for defining debug-only literal strings.
     */
    #define PBK_DEBUG_STRING(s) _L(s)

    #ifdef PBK_ENABLE_DEBUG_LOGGER
        // Print to log file
        #define PBK_DEBUG_PRINT PbkPrintToLog

        #include <flogger.h>

        namespace {
        void PbkPrintToLog( TRefByValue<const TDesC> aFormat, ... )
            {
            _LIT( KLogDir, "Phonebook" );
            _LIT( KLogName, "Phonebook.log" );

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
        #define PBK_DEBUG_PRINT RDebug::Print

    #endif // #ifdef PBK_ENABLE_DEBUG_LOGGER

#else

    /**
     * Dummy struct for checking that all PBK_DEBUG_PRINT's define string
     * literals using space-saving PBK_DEBUG_STRING.
     */
    struct TPbkEmptyDebugString { };

    /**
     * Macro for defining debug-only literal strings (empty release version)
     */
    #define PBK_DEBUG_STRING(s) TPbkEmptyDebugString()

    /// Empty debug print function for release builds.
    inline void PBK_DEBUG_PRINT(TPbkEmptyDebugString)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1>
    inline void PBK_DEBUG_PRINT(TPbkEmptyDebugString,T1)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2>
    inline void PBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3>
    inline void PBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2,T3)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4>
    inline void PBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2,T3,T4)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5>
    inline void PBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2,T3,T4,T5)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6>
    inline void PBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2,T3,T4,T5,T6)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    inline void PBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2,T3,T4,T5,T6,T7)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
    inline void PBK_DEBUG_PRINT(TPbkEmptyDebugString,T1,T2,T3,T4,T5,T6,T7,T8)
        {
        }

#endif


#ifdef PBK_ENABLE_PROFILE
    #define __PBK_PROFILE_START(aBin) RDebug::ProfileStart(aBin)
    #define __PBK_PROFILE_END(aBin)   RDebug::ProfileEnd(aBin)

    // Needs PBK_ENABLE_DEBUG_LOGGER
    #define __PBK_PROFILE_DISPLAY(aBin) \
        { \
        TProfile result; \
        RDebug::ProfileResult(&result, aBin, 1); \
        PBK_DEBUG_PRINT \
            (PBK_DEBUG_STRING("Profile bin %d:  Calls: %d, Time: %d\n" ), \
        aBin, result.iCount, result.iTime); \
        }
    #define __PBK_PROFILE_RESET(aBin) RDebug::ProfileReset(aBin, 1)
#else
    #define __PBK_PROFILE_START(aBin)
    #define __PBK_PROFILE_END(aBin)
    #define __PBK_PROFILE_DISPLAY(aBin)
    #define __PBK_PROFILE_RESET(aBin)

#endif // PBK_ENABLE_PROFILE


#ifdef _DEBUG

/**
 * Asserts and checks that statement doesn't allocate any memory. If
 * allocation happens evaluates failed.
 */
#define PBK_ASSERT_NO_ALLOC(statement,failed) \
    { __UHEAP_FAILNEXT(1); \
    TRAPD(err, statement); \
    __UHEAP_RESET; \
    if (err==KErrNoMemory) { failed; } }

#else

/**
 * Release version of macro. Just executes statement.
 */
#define PBK_ASSERT_NO_ALLOC(statement,failed) statement

#endif

// CLASS DECLARATION

/**
 * Helper class for calling class invariant automatically on function
 * entry and exit.
 */
template<class T>
class TPbkDebugCallInvariantOnEntryAndExit
    {
    public:
        /**
         * Constructor.
         * @param aObj object
         */
        inline TPbkDebugCallInvariantOnEntryAndExit(const T* aObj)
            : iObj(aObj)
            {
            iObj->__DbgTestInvariant();
            }

        /**
         * Destructor
         */
        inline ~TPbkDebugCallInvariantOnEntryAndExit()
            {
            iObj->__DbgTestInvariant();
            }

    private: // data
        /// Ref: the object
        const T* iObj;
    };


#endif // __PbkDebug_H__

// End of File
