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
#include "ut_logsmainwindow.h"
#include "logsmainwindow.h"
#include "qthighway_stub_helper.h"

//SYSTEM
#include <QtTest/QtTest>
#include <hbmainwindow.h>
#include <QKeyEvent>

void UT_LogsMainWindow::initTestCase()
{

}

void UT_LogsMainWindow::cleanupTestCase()
{

}


void UT_LogsMainWindow::init()
{
    mMainWindow = new LogsMainWindow;
}

void UT_LogsMainWindow::cleanup()
{
    delete mMainWindow;
    mMainWindow = 0;
}

void UT_LogsMainWindow::testKeyPressEvent()
{
    QSignalSpy spy( mMainWindow, SIGNAL(callKeyPressed()) );
    // Not handled
    QKeyEvent event( QEvent::KeyPress, Qt::Key_Hangup, Qt::NoModifier);
    mMainWindow->keyPressEvent( &event );
    QVERIFY( spy.count() == 0 );
    
    // Handled
    QKeyEvent event2( QEvent::KeyPress, Qt::Key_Yes, Qt::NoModifier);
    mMainWindow->keyPressEvent( &event2 );
    QVERIFY( spy.count() == 1 );
    
}

void UT_LogsMainWindow::testEvent()
{
     QSignalSpy spy( mMainWindow, SIGNAL(appFocusGained()) );
     // Not interesting event
     QEvent event(QEvent::MaxUser);
     mMainWindow->event(&event);
     QVERIFY( spy.count() == 0 );
     
     // Focus in event
     QEvent event2(QEvent::FocusIn);
     mMainWindow->event(&event2);
     QVERIFY( spy.count() == 1 );
}

void UT_LogsMainWindow::testSendAppToBackground()
{
    QtHighwayStubHelper::reset();
    mMainWindow->sendAppToBackground();
    QVERIFY( QtHighwayStubHelper::utilToBackground() );  
}
 
