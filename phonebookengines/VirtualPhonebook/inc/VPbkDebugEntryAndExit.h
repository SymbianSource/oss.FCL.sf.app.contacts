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
* Description:  Debugging utilities for logging function entry/exit
*
*/



#ifndef VPBKDEBUG_ENTRYANDEXIT_H
#define VPBKDEBUG_ENTRYANDEXIT_H

//  INCLUDES
#include "VPbkConfig.hrh"

#include <e32std.h>
#include <e32svr.h>

#if defined( VPBK_ENABLE_DEBUG_PRINT ) && defined ( __MODULE__ ) && defined( __LINE__ )
    // Debug logging is enabled and the required macros are available in the
    // compiler.
    // Ps. It seems that these are not available in WINSCW environment.

    /**
     * Constants for FUNC_ENTRY and FUNC_EXIT macros and examples of the
     * corresponding output.
     */
    _LIT( KFuncEntry,  "%S:%d >>> %S" ); // "example.cpp:609 >>> ExampleL"
    _LIT( KFuncExit, "%S:%d <<< %S" );   // "example.cpp:642 <<< ExampleL"
    const TInt KMaxDesC8Length(128);     // limit output to 128 characters

    #ifdef VPBK_ENABLE_DEBUG_LOGGER
        #include <flogger.h>
        _LIT( KLogDir, "VPbk" );
        _LIT( KLogName, "VPbk.log" );

        #define FUNC_ENTRY FuncEntryToLog
        #define FUNC_EXIT FuncExitToLog

        namespace
            {
            /**
             * Print function entry to log file
             */
            void FuncEntryToLog()
                {
                TBuf<KMaxDesC8Length> moduleBuf;
                moduleBuf.Copy( _L8(__MODULE__).Left(KMaxDesC8Length) );
                TBuf<KMaxDesC8Length> funcBuf;
                funcBuf.Copy( _L8(__func__).Left(KMaxDesC8Length) );
                RFileLogger::WriteFormat(
                    KLogDir(), KLogName(), EFileLoggingModeAppend,
                    TRefByValue<const TDesC>( KFuncEntry() ),
                    &moduleBuf, __LINE__, &funcBuf );
                }
            /**
             * Print function exit to log file
             */
            void FuncExitToLog()
                {
                TBuf<KMaxDesC8Length> moduleBuf;
                moduleBuf.Copy( _L8(__MODULE__).Left(KMaxDesC8Length) );
                TBuf<KMaxDesC8Length> funcBuf;
                funcBuf.Copy( _L8(__func__).Left(KMaxDesC8Length) );
                RFileLogger::WriteFormat(
                    KLogDir(), KLogName(), EFileLoggingModeAppend,
                    TRefByValue<const TDesC>( KFuncExit() ),
                    &moduleBuf, __LINE__, &funcBuf );
                }
            }  // namespace
    #else // VPBK_ENABLE_DEBUG_LOGGER
        #define FUNC_ENTRY FuncEntryToRDebug
        #define FUNC_EXIT FuncExitToRDebug

        namespace
            {
            /**
             * Print function entry to RDebug 
             */
            void FuncEntryToRDebug()
                {
                TBuf<KMaxDesC8Length> moduleBuf;
                moduleBuf.Copy( _L8(__MODULE__).Left(KMaxDesC8Length) );
                TBuf<KMaxDesC8Length> funcBuf;
                funcBuf.Copy( _L8(__func__).Left(KMaxDesC8Length) );
                RDebug::Print( KFuncEntry, &moduleBuf, __LINE__, &funcBuf );
                }
            /**
             * Print function exit to RDebug 
             */
            void FuncExitToRDebug()
                {
                TBuf<KMaxDesC8Length> moduleBuf;
                moduleBuf.Copy( _L8(__MODULE__).Left(KMaxDesC8Length) );
                TBuf<KMaxDesC8Length> funcBuf;
                funcBuf.Copy( _L8(__func__).Left(KMaxDesC8Length) );
                RDebug::Print( KFuncExit, &moduleBuf, __LINE__, &funcBuf );
                }
            }  // namespace
    #endif // #ifdef VPBK_ENABLE_DEBUG_LOGGER

#else // VPBK_ENABLE_DEBUG_PRINT
    #define FUNC_ENTRY()
    #define FUNC_EXIT()
#endif


#ifdef _DEBUG
    /**
     * Helper macro for calling invariant on function entry and (normal) exit.
     * Declare typedef SelfType as the type of the class you're using this
     * macro in.
     */
    #define VPBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT \
                TPbkDebugCallInvariantOnEntryAndExit<SelfType> \
                __pbk_debug_test_invariant(this)

#else

    /**
     * Helper macro for calling invariant on function entry and (normal) exit.
     * This is empty release build version.
     */
    #define VPBK_DEBUG_TEST_INVARIANT_ON_ENTRY_AND_EXIT

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

#endif // VPBKDEBUG_ENTRYANDEXIT_H

// End of File
