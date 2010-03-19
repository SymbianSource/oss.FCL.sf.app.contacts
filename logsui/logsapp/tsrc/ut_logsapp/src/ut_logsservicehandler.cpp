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

//USER
#include "ut_logsservicehandler.h"
#include "logsservicehandler.h"
#include "qthighway_stub_helper.h"

//SYSTEM
#include <QtTest/QtTest>

Q_DECLARE_METATYPE(LogsServices::LogsView)

void UT_LogsServiceHandler::initTestCase()
{
}

void UT_LogsServiceHandler::cleanupTestCase()
{
}


void UT_LogsServiceHandler::init()
{
    mService = new LogsServiceHandler();
}

void UT_LogsServiceHandler::cleanup()
{
    delete mService;
    mService = 0;
    QtHighwayStubHelper::reset();
}

void UT_LogsServiceHandler::testConstructor()
{
    QVERIFY( mService );
    QVERIFY( !mService->mIsAppStartedUsingService );
    QVERIFY( !mService->isStartedUsingService() );
    
    QtHighwayStubHelper::setIsService(true);
    LogsServiceHandler serviceHandler;
    QVERIFY( serviceHandler.mIsAppStartedUsingService );
    QVERIFY( serviceHandler.isStartedUsingService() );
    
}

void UT_LogsServiceHandler::testStart()
{
    qRegisterMetaType< LogsServices::LogsView >("LogsServices::LogsView");
    QSignalSpy spy(mService, SIGNAL(activateView(LogsServices::LogsView, bool)));
     
    // Wrong view
    QVERIFY( mService->start( 9999, true ) != 0 );
    QVERIFY( spy.count() == 0 );

    // Correct view
    QtHighwayStubHelper::reset();
    QVERIFY( mService->start( (int)LogsServices::ViewReceived, true  ) == 0 );
    QVERIFY( spy.count() == 1 );
    LogsServices::LogsView view = 
        qvariant_cast< LogsServices::LogsView >(spy.at(0).at(0));
    QVERIFY( view == LogsServices::ViewReceived );
    QVERIFY( mService->currentlyActivatedView() == LogsServices::ViewReceived  );
    QVERIFY( !QtHighwayStubHelper::utilToBackground() );  
}
