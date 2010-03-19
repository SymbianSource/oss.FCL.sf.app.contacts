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

#include "ut_logsmodel.h"
#include "ut_logsdetailsmodel.h"
#include "ut_logsmatchesmodel.h"
#include "ut_logsfilter.h"
#include "ut_logscustomfilter.h"
#include "ut_logscall.h"
#include "ut_logscontact.h"
#include "ut_logsmessage.h"
#include "ut_logsevent.h"
#include "ut_logseventdata.h"
#include "ut_logsthumbnailmanager.h"
#include "ut_logscommondata.h"
#include "testresultxmlparser.h"


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
    
    UT_LogsModel ut_logsModel;
    QString resultFileName = "c:/ut_logs_logsModel.xml";
    QStringList args_logsModel( "ut_logsModel");
    args_logsModel << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsModel, args_logsModel);
    parser.parseAndPrintResults(resultFileName,true); 
    
    UT_LogsDetailsModel ut_logsDetailsModel;
    resultFileName = "c:/ut_logs_logsDetailsModel.xml";
    QStringList args_logsDetailsModel( "ut_logsDetailsModel");
    args_logsDetailsModel << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsDetailsModel, args_logsDetailsModel);
    parser.parseAndPrintResults(resultFileName,true); 
    
    UT_LogsMatchesModel ut_logsMatchesModel;
    resultFileName = "c:/ut_logs_logsMatchesModel.xml";
    QStringList args_logsMatchesModel( "ut_logsMatchesModel");
    args_logsMatchesModel << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsMatchesModel, args_logsMatchesModel);
    parser.parseAndPrintResults(resultFileName,true); 
    
    UT_LogsThumbnailManager ut_logsThumbnailManager;
    resultFileName = "c:/ut_logs_logsThumbnailManager.xml";
    QStringList args_logsThumbnailManager( "ut_logsThumbnailManager");
    args_logsThumbnailManager << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsThumbnailManager, args_logsThumbnailManager);
    parser.parseAndPrintResults(resultFileName,true); 
    
    UT_LogsFilter ut_logsFilter;
    resultFileName = "c:/ut_logs_logsFilter.xml";
    QStringList args_logsFilter( "ut_logsFilter");
    args_logsFilter << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsFilter, args_logsFilter);
    parser.parseAndPrintResults(resultFileName,true); 
    
    UT_LogsCustomFilter ut_logsCustomFilter;
    resultFileName = "c:/ut_logs_logsCustomFilter.xml";
    QStringList args_logsCustomFilter( "ut_logsCustomFilter");
    args_logsCustomFilter << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsCustomFilter, args_logsCustomFilter);   
    parser.parseAndPrintResults(resultFileName,true); 

    UT_LogsCall ut_logscall;
    resultFileName = "c:/ut_logs_logsCall.xml";
    QStringList args_logsCall( "ut_logsCall");
    args_logsCall << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logscall, args_logsCall);    
    parser.parseAndPrintResults(resultFileName,true); 
    
    UT_LogsContact ut_logsContact;
    resultFileName = "c:/ut_logs_logsContact.xml";
    QStringList args_logsContact( "ut_logsContact");
    args_logsContact << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsContact, args_logsContact);    
    parser.parseAndPrintResults(resultFileName,true); 
    
    UT_LogsMessage ut_logsMessage;
    resultFileName = "c:/ut_logs_logsMessage.xml";
    QStringList args_logsMessage( "ut_logsMessage");
    args_logsMessage << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsMessage, args_logsMessage);    
    parser.parseAndPrintResults(resultFileName,true); 
 
    UT_LogsEvent ut_logsEvent;
    resultFileName = "c:/ut_logs_logsEvent.xml";
    QStringList args_logsEvent( "ut_logsEvent");
    args_logsEvent << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsEvent, args_logsEvent);
    
    parser.parseAndPrintResults(resultFileName,true); 
    
    UT_LogsEventData ut_logsEventdata;
    resultFileName = "c:/ut_logs_logsEventData.xml";
    QStringList args_logsEventData( "ut_logsEventData");
    args_logsEventData << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsEventdata, args_logsEventData);
    
    UT_LogsCommonData ut_logsCommonData;
    resultFileName = "c:/ut_logs_logsCommonData.xml";
    QStringList args_logsCommonData( "ut_logsCommonData");
    args_logsCommonData << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_logsCommonData, args_logsCommonData);
    
    parser.parseAndPrintResults(resultFileName,true); 

    if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }
    return 0;   
}



 
