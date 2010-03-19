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
* Description:  
*
*/


#include <QtTest/QtTest>

#include "ut_logsdbconnector.h"
#include "ut_logsreader.h"
#include "ut_logsreaderstates.h"
#include "ut_logseventparser.h"
#include "ut_logseventdataparser.h"
#include "testresultxmlparser.h"
#include "ut_logsremove.h"


int main(int argc, char *argv[]) 
{
    bool promptOnExit(true);
    for (int i=0; i<argc; i++) {
        if (QString(argv[i]) == "-noprompt")
            promptOnExit = false;
    }
    printf("Running tests...\n");
    
    QApplication app(argc, argv);
    TestResultXmlParser parser;
    
    UT_LogsDbConnector ut_logsDbConnector;
    QString resultFileName = "c:/ut_logs_logsDbConnector.xml";
    QStringList args_logsDbConnector( "ut_logsDbConnector");
    args_logsDbConnector << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsDbConnector, args_logsDbConnector);
    parser.parseAndPrintResults(resultFileName,true); 
   
	UT_LogsRemove ut_logsRemove;
    resultFileName = "c:/ut_logs_logsRemove.xml";
    QStringList args_logsRemove( "ut_logsRemove");
    args_logsRemove << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsRemove, args_logsRemove);
    parser.parseAndPrintResults(resultFileName,true); 
	
    UT_LogsReader ut_logsReader;
    resultFileName = "c:/ut_logs_logsReader.xml";
    QStringList args_logsReader( "ut_logsReader");
    args_logsReader << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsReader, args_logsReader);
    parser.parseAndPrintResults(resultFileName,true); 
    
    UT_LogsReaderStates ut_logsReaderStates;
    resultFileName = "c:/ut_logs_logsReaderStates.xml";
    QStringList args_logsReaderStates( "ut_logsReaderStates");
    args_logsReaderStates << "-xml" << "-o" << resultFileName;
    QT_TRAP_THROWING( QTest::qExec(&ut_logsReaderStates, args_logsReaderStates); )
    parser.parseAndPrintResults(resultFileName,true); 
    
    UT_LogsEventParser ut_logsEvent;
    resultFileName = "c:/ut_logs_logsEventParser.xml";
    QStringList args_logsEvent( "ut_logsEvent");
    args_logsEvent << "-xml" << "-o" << resultFileName;
    QT_TRAP_THROWING( QTest::qExec(&ut_logsEvent, args_logsEvent); )
    parser.parseAndPrintResults(resultFileName,true);
    
    UT_LogsEventDataParser ut_logsEventData;
    resultFileName = "c:/ut_logs_logsEventDataParser.xml";
    QStringList args_logsEventData( "ut_logsEventData");
    args_logsEventData << "-xml" << "-o" << resultFileName;
    QT_TRAP_THROWING( QTest::qExec(&ut_logsEventData, args_logsEventData); )
    parser.parseAndPrintResults(resultFileName,true);
    


    if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }
    return 0;   
}



 
