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
#include "ut_logsmainwindow.h"
#include "ut_logscomponentrepository.h"
#include "ut_logsviewmanager.h"
#include "ut_logsbaseview.h"
#include "ut_logsrecentcallsview.h"
#include "ut_logsdetailsview.h"
#include "ut_logsmatchesview.h"
#include "ut_logseffecthandler.h"
#include "ut_logsservicehandler.h"
#include "testresultxmlparser.h"

#include <QtTest/QtTest>


#define UT_ARGS( args, file )\
    QStringList args( "ut_logsapp");\
    args << "-silent" << "-xml" << "-o" << file


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
    
    UT_LogsMainWindow ut_logsMainWindow;
    QString resultFileNameForMainWindow = "c:/ut_logsMainWindow.xml";
    UT_ARGS( args_logsMainWindow, resultFileNameForMainWindow );
    QTest::qExec( &ut_logsMainWindow, args_logsMainWindow );
    parser.parseAndPrintResults(resultFileNameForMainWindow);
    
    UT_LogsComponentRepository ut_logsRepository;
    QString resultFileNameForComponentRepository = "c:/ut_logsRepository.xml";
    UT_ARGS( args_logsRepository, resultFileNameForComponentRepository );
    QTest::qExec( &ut_logsRepository, args_logsRepository );
    parser.parseAndPrintResults(resultFileNameForComponentRepository);
    
    UT_LogsViewManager ut_logsViewManager;
    QString resultFileNameForViewManager = "c:/ut_logsViewManager.xml";
    UT_ARGS( args_logsViewManager, resultFileNameForViewManager );
    QTest::qExec( &ut_logsViewManager, args_logsViewManager );
    parser.parseAndPrintResults(resultFileNameForViewManager);

    UT_LogsBaseView ut_logsBaseView;
    QString resultFileNameForBaseView = "c:/ut_logsBaseView.xml";
    UT_ARGS( args_logsBaseView, resultFileNameForBaseView );
    QTest::qExec( &ut_logsBaseView, args_logsBaseView );
    parser.parseAndPrintResults(resultFileNameForBaseView);
    
    UT_LogsRecentCallsView ut_logsRecentCallsView;
    QString resultFileNameForRecentCallsView = "c:/ut_logsRecentCallsView.xml";
    UT_ARGS( args_logsRecentCallsView, resultFileNameForRecentCallsView );
    QTest::qExec( &ut_logsRecentCallsView, args_logsRecentCallsView );
    parser.parseAndPrintResults(resultFileNameForRecentCallsView);
    
    UT_LogsDetailsView ut_logsDetailsView;
    QString resultFileNameForDetailsView = "c:/ut_logsDetailsView.xml";
    UT_ARGS( args_logsDetailsView, resultFileNameForDetailsView );
    QTest::qExec( &ut_logsDetailsView, args_logsDetailsView );
    parser.parseAndPrintResults(resultFileNameForDetailsView);
    
    UT_LogsMatchesView ut_logsMatchesView;
    QString resultFileNameForMatchesView = "c:/ut_logsMatchesView.xml";
    UT_ARGS( args_logsMatchesView, resultFileNameForMatchesView );
    QTest::qExec( &ut_logsMatchesView, args_logsMatchesView );
    parser.parseAndPrintResults(resultFileNameForMatchesView);
   
    UT_LogsEffectHandler ut_logsEffectHandler;
    QString resultFileNameForEffectHandler = "c:/ut_logsEffectHandler.xml";
    UT_ARGS( args_logsEffectHandler, resultFileNameForEffectHandler );
    QTest::qExec( &ut_logsEffectHandler, args_logsEffectHandler );
    parser.parseAndPrintResults(resultFileNameForEffectHandler);

    UT_LogsServiceHandler ut_logsServiceHandler;
    QString resultFileNameForServiceHandler = "c:/ut_logsServiceHandler.xml";
    UT_ARGS( args_logsServiceHandler, resultFileNameForServiceHandler );
    QTest::qExec( &ut_logsServiceHandler, args_logsServiceHandler );
    parser.parseAndPrintResults(resultFileNameForServiceHandler);
    
    if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }
    return 0;   
}

