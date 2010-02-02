/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 debugging support.
*
*/


#ifndef PBK2DEBUG_H
#define PBK2DEBUG_H

// INCLUDES
#include <Pbk2Config.hrh>
#include <e32std.h>
#include <e32svr.h>

// MACROS
#ifdef _DEBUG

    /// Helper macro for defining debug-only test code
    #define PBK2_DEBUG_ONLY(f) f

     /// Helper macro for calling invariant on function entry and
     /// (normal) exit. Declare typedef SelfType as the type of
     /// the class you're using this macro in.
    #define PBK2_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT \
        TPbk2DebugCallInvariantOnEntryAndExit<SelfType> \
        pbk2_debug_test_invariant(this)

#else // _DEBUG

    /// Helper macro for defining debug-only test code (empty release version)
    #define PBK2_DEBUG_ONLY(f)

    /// Helper macro for calling invariant on function entry and
    /// (normal) exit. This is empty release build version.
    #define PBK2_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT

#endif // _DEBUG

/**
 * Phonebook 2 debug printing implementation.
 *
 */
#ifdef PBK2_ENABLE_DEBUG_PRINT

    // Debug logging is enabled

    /// Warn if this is a release build
    #ifndef __WINSCW__
    #ifndef _DEBUG
        #if defined(__VC32__)
            #pragma message("Warning: Phonebook2 debug printing is on in release build!")
        #else // __GCC32__
            #warning "Phonebook2 debug printing is on in release build!"
        #endif // __VC32__
    #endif // _DEBUG
    #endif // __WINSCW__

    /// Macro for defining debug-only literal strings
    #define PBK2_DEBUG_STRING(s) _L(s)

    #ifdef PBK2_ENABLE_DEBUG_LOGGER

        /// Print to log file
        #define PBK2_DEBUG_PRINT Pbk2PrintToLog

        #include <flogger.h>

        namespace {

        void Pbk2PrintToLog( TRefByValue<const TDesC> aFormat, ... )
            {
            _LIT( KLogDir, "Pbk2" );
            _LIT( KLogName, "Pbk2.log" );

            VA_LIST args;
            VA_START( args, aFormat );
            RFileLogger::WriteFormat
                (KLogDir, KLogName, EFileLoggingModeAppend, aFormat, args);
            VA_END( args );
            }

        } /// namespace

    #else // PBK2_ENABLE_DEBUG_LOGGER

        /// Prints to debugger output
        #define PBK2_DEBUG_PRINT RDebug::Print

    #endif // PBK2_ENABLE_DEBUG_LOGGER

#else // PBK2_ENABLE_DEBUG_PRINT

    /// Dummy struct for checking that all PBK2_DEBUG_PRINT's define string
    /// literals using space-saving PBK2_DEBUG_STRING.
    struct TPbk2EmptyDebugString { };

    /// Macro for defining debug-only literal strings (empty release version)
    #define PBK2_DEBUG_STRING(s) TPbk2EmptyDebugString()

    /// Empty debug print function for release builds.
    inline void PBK2_DEBUG_PRINT(TPbk2EmptyDebugString)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1>
    inline void PBK2_DEBUG_PRINT(TPbk2EmptyDebugString,T1)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2>
    inline void PBK2_DEBUG_PRINT(TPbk2EmptyDebugString,T1,T2)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3>
    inline void PBK2_DEBUG_PRINT(TPbk2EmptyDebugString,T1,T2,T3)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4>
    inline void PBK2_DEBUG_PRINT(TPbk2EmptyDebugString,T1,T2,T3,T4)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5>
    inline void PBK2_DEBUG_PRINT(TPbk2EmptyDebugString,T1,T2,T3,T4,T5)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6>
    inline void PBK2_DEBUG_PRINT(TPbk2EmptyDebugString,T1,T2,T3,T4,T5,T6)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    inline void PBK2_DEBUG_PRINT(TPbk2EmptyDebugString,T1,T2,T3,T4,T5,T6,T7)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
    inline void PBK2_DEBUG_PRINT(TPbk2EmptyDebugString,T1,T2,T3,T4,T5,T6,T7,T8)
        {
        }

#endif // PBK2_ENABLE_DEBUG_PRINT

#ifdef _DEBUG

    /// Asserts and checks that statement doesn't allocate any memory.
    /// If allocation happens evaluates failed.
    #define PBK2_ASSERT_NO_ALLOC(statement,failed) \
        { __UHEAP_FAILNEXT(1); \
        TRAPD(err, statement); \
        __UHEAP_RESET; \
        if (err==KErrNoMemory) { failed; } }

#else // _DEBUG

    /// Release version of macro. Just executes the statement.
    #define PBK2_ASSERT_NO_ALLOC(statement,failed) statement

#endif // _DEBUG


// CLASS DECLARATION

/**
 * Helper class for calling class invariant automatically on function
 * entry and exit.
 */
template<class T>
class TPbk2DebugCallInvariantOnEntryAndExit
    {
    public: // Construction and destruction

        /**
         * Constructor.
         *
         * @param aObj  The object.
         */
        inline TPbk2DebugCallInvariantOnEntryAndExit( const T* aObj ):
                iObj( aObj )
            {
            iObj->__DbgTestInvariant();
            }

        /**
         * Destructor.
         */
        inline ~TPbk2DebugCallInvariantOnEntryAndExit()
            {
            iObj->__DbgTestInvariant();
            }

    private: // Data
        /// Ref: The object
        const T* iObj;
    };

#endif // PBK2DEBUG_H

// End of File
