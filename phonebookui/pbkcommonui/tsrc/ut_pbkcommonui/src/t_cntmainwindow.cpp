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
#include <QObject>
#include <QSignalSpy>
#include <QKeyEvent>

#include "cntmainwindow.h"
#include "t_cntmainwindow.h"


void TestCntMainWindow::initTestCase()
{
    mWindow = 0;
}

void TestCntMainWindow::create()
{
    mWindow = new CntMainWindow();
    QVERIFY(mWindow != 0);
    QVERIFY(mWindow->mViewManager != 0);
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    QVERIFY(mWindow != 0);
    QVERIFY(mWindow->mViewManager == 0);
}

void TestCntMainWindow::keyPressEvent()
{
    QSignalSpy spy(mWindow, SIGNAL(keyPressed(QKeyEvent*)));
    
    QKeyEvent event(QEvent::KeyPress, Qt::Key_5, Qt::NoModifier);
    mWindow->keyPressEvent(&event);
    QCOMPARE(spy.count(), 0);
    QKeyEvent event2(QEvent::KeyRelease, Qt::Key_5, Qt::NoModifier);

    QKeyEvent event3(QEvent::KeyPress, Qt::Key_Yes, Qt::NoModifier);
    mWindow->keyPressEvent(&event3);
    QCOMPARE(spy.count(), 1);
    QKeyEvent event4(QEvent::KeyRelease, Qt::Key_Yes, Qt::NoModifier);

    QKeyEvent event5(QEvent::KeyPress, Qt::Key_No, Qt::NoModifier);
    mWindow->keyPressEvent(&event5);
    QCOMPARE(spy.count(), 2);
    QKeyEvent event6(QEvent::KeyRelease, Qt::Key_No, Qt::NoModifier);
}

void TestCntMainWindow::cleanupTestCase()
{
    mWindow->deleteLater();
}

// EOF
