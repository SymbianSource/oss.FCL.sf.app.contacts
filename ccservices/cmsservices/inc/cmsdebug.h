/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMS Debug macros
*
*/

#ifndef __CMS_DEBUG_H__
#define __CMS_DEBUG_H__

#include <e32svr.h>
#include <e32std.h>
#include <f32file.h>

#ifdef _DEBUG

   #ifdef __WINS__ 

       #define PRINT( txt )                RDebug::Print( txt );
       #define PRINT1( txt, arg1 )         RDebug::Print( txt, arg1 );
       #define PRINT2( txt, arg1, arg2 )   RDebug::Print( txt, arg1, arg2 );
       #define PRINT3( txt, arg1, arg2, arg3 )   RDebug::Print( txt, arg1, arg2,arg3 );
   #else

       #include <flogger.h>

      _LIT(KLogDirFullName, "c:\\logs\\");
      _LIT(KLogDir,         "cmsservices");
      _LIT(KLogFile,        "cmsdebuglog.txt");

      #define PRINT( txt )               RFileLogger::Write(KLogDir,KLogFile,EFileLoggingModeAppend,txt);                                                       
      #define PRINT1( txt, arg1 )        RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,TRefByValue<const TDesC>(txt),arg1);																
      #define PRINT2( txt, arg1, arg2 )  RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,TRefByValue<const TDesC>(txt),arg1,arg2);			
      #define PRINT3( txt, arg1, arg2, arg3 )  RFileLogger::WriteFormat(KLogDir,KLogFile,EFileLoggingModeAppend,TRefByValue<const TDesC>(txt),arg1,arg2,arg3);													


    #endif // __WINS__

#else

    #define PRINT( txt )
    #define PRINT1( txt, arg1 )
    #define PRINT2( txt, arg1, arg2 )
    #define PRINT3( txt, arg1, arg2, arg3 )

#endif // _DEBUG


#endif // __CMS_DEBUG_H__

// End of File
