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

#include "ut_logscntfinder.h"
#include "ut_logspredictivetranslator.h"
#include "ut_logscntentry.h"

#include "testresultxmlparser.h"


int main(int argc, char *argv[]) 
{
    bool promptOnExit(true);
    bool useQApplication(true);
    for (int i=0; i<argc; i++) {
        if (QString(argv[i]) == "-noprompt")
            promptOnExit = false;
        else if (QString(argv[i]) == "-noqapp")
            useQApplication = false;
    }
    printf("Running tests...\n");
    
    QApplication* app = 0;
    if ( useQApplication ) 
        app = new QApplication(argc, argv);
    
    TestResultXmlParser parser;
    
    UT_LogsCntFinder ut_logsCntFinder;
    QString resultFileName = "c:/ut_logs_logsCntFinder.xml";
    QStringList args_logsCntFinder( "ut_logsCntFinder");
    args_logsCntFinder << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsCntFinder, args_logsCntFinder);
    parser.parseAndPrintResults(resultFileName,true); 
    
    UT_LogsPredictiveTranslator ut_logsPredictiveTranslator;
    resultFileName = "c:/ut_logs_logsPredictiveTranslator.xml";
    QStringList args_logsCntFinder1( "ut_LogsPredictiveTranslator" );
    args_logsCntFinder1 << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsPredictiveTranslator, args_logsCntFinder1);
    parser.parseAndPrintResults(resultFileName,true); 

    UT_LogsCntEntry ut_logscntentry;
    resultFileName = "c:/ut_logs_logscntentry.xml";
    QStringList args_logsCntFinder2( "ut_logscntentry" );
    args_logsCntFinder2 << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logscntentry, args_logsCntFinder2);
    parser.parseAndPrintResults(resultFileName,true); 
    
    if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }
    delete app;
    return 0;   
}



 
