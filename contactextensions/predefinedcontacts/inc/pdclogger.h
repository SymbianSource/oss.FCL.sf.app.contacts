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
* Description:    Define log macro and the path for saving log.
*
*/


#ifndef DCDLOGGER_H 
#define DCDLOGGER_H 

#ifdef _DEBUG 
#define USE_FILE_LOGGER 
#endif 

#ifdef USE_FILE_LOGGER 

    #include <flogger.h> 
    _LIT(KLogFile,"predefinedcontacts.txt"); 
    _LIT(KLogFolder,"predefinedcontacts"); 
    #define LOGTEXT(AAA)                RFileLogger::Write(KLogFolder,KLogFile,EFileLoggingModeAppend,AAA)

    #define LOGTEXT2(AAA,BBB)           RFileLogger::WriteFormat(KLogFolder, \
                                                                KLogFile, \
                                                                EFileLoggingModeAppend, \
                                                                (AAA), \
                                                                (BBB))
    
  /*  #define LOGTEXT3(AAA,BBB,CCC)       RFileLogger::WriteFormat(KLogFolder(), \ 
                                                                 KLogFile(), \ 
                                                                 EFileLoggingModeAppend, \ 
                                                                 (AAA), \ 
                                                                 BBB, \ 
                                                                 CCC) 
    /*
    #define LOGHEXDUMP(AAA,BBB,CCC,DDD) RFileLogger::HexDump(KLogFolder(), \ 
                                                             KLogFile(), \ 
                                                             EFileLoggingModeAppend, \ 
                                                             AAA,\ 
                                                             BBB,\ 
                                                             CCC,\ 
                                                             DDD)    
                                                           
*/
#else 

    #define LOGTEXT(AAA)                
    #define LOGTEXT2(AAA,BBB)           
    #define LOGTEXT3(AAA,BBB,CCC)       
    #define LOGHEXDUMP(AAA,BBB,CCC,DDD) 
    
#endif // USE_FILE_LOGGER 

#endif // DCDLOGGER_H 

// End Of File 
