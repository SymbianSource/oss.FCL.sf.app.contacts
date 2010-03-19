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

#include <QtGui>
#include <QtTest/QtTest>
#include <QDebug>
#include <QIcon>
#include <QSignalSpy>

#include "mobcnticonmanager.h"
#include "ut_mobcnticonmanager.h"

const int WAIT_TIME1 = 500;
const int WAIT_TIME2 = 900;
const QString path1 = "c:\\data\\images\\designer.png";
const QString path2 = "c:\\data\\images\\monkey_on_64x64.png";
const QString path3 = "e:\\images\\non.jpeg";

void TestMobCntIconManager::initTestCase()
{
    mIconMgr = new MobCntIconManager();
}

void TestMobCntIconManager::cleanupTestCase()
{
    delete mIconMgr;
    mIconMgr = 0;
}

void TestMobCntIconManager::init()
{
    initTestCase();
}
     
void TestMobCntIconManager::cleanup()
{
    cleanupTestCase();
}

void TestMobCntIconManager::testNonExistingIcon()
{
    QSignalSpy spy(mIconMgr, SIGNAL(contactIconReady(int)));
    Q_ASSERT(spy.isValid());
    QCOMPARE( spy.count(), 0 );
    mIcon = mIconMgr->contactIcon(path3, 0);
    Q_ASSERT(mIcon.isNull());
    QTest::qWait( WAIT_TIME1 );
    QCOMPARE(spy.count(), 0);    
}
   
void TestMobCntIconManager::testOneExistingIcon()
{
    QSignalSpy spy(mIconMgr, SIGNAL(contactIconReady(int)));
    Q_ASSERT(spy.isValid());
    QCOMPARE( spy.count(), 0 );
    mIcon = mIconMgr->contactIcon(path1, 10);
    Q_ASSERT(mIcon.isNull());
    QTest::qWait( WAIT_TIME1 );
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toInt() == 10);
    mIcon = mIconMgr->contactIcon(path1, 10);
    Q_ASSERT(!mIcon.isNull());
}
    
void TestMobCntIconManager::testCancel()
{
    QSignalSpy spy(mIconMgr, SIGNAL(contactIconReady(int)));
    Q_ASSERT(spy.isValid());
    QCOMPARE( spy.count(), 0 );
    mIcon = mIconMgr->contactIcon(path1, 10);
    Q_ASSERT(mIcon.isNull());
    mIcon = mIconMgr->contactIcon(path2, 11);
    Q_ASSERT(mIcon.isNull());
    mIcon = mIconMgr->contactIcon(path3, 0);
    Q_ASSERT(mIcon.isNull());
    mIconMgr->thumbnailLoad();
    mIconMgr->cancel();
    QCOMPARE(spy.count(), 0);
}

void TestMobCntIconManager::testMultipleExistingIcon()
{
    QSignalSpy spy(mIconMgr, SIGNAL(contactIconReady(int)));
    Q_ASSERT(spy.isValid());
    QCOMPARE( spy.count(), 0 );
    QIcon icon1;
    QIcon icon2;
    icon1 = mIconMgr->contactIcon(path1, 10);
    Q_ASSERT(icon1.isNull());
    icon2 = mIconMgr->contactIcon(path2, 11);
    Q_ASSERT(icon2.isNull());
    QTest::qWait( WAIT_TIME2);    
    qDebug()<<spy.count();    
    QCOMPARE(spy.count(), 2); 
    QList<QVariant> arguments = spy.takeFirst();
    qDebug()<< arguments.at(0).toInt();
    QVERIFY(arguments.at(0).toInt() == 10);
    arguments = spy.takeLast();
    qDebug()<< arguments.at(0).toInt();
    QVERIFY(arguments.at(0).toInt() == 11 );
    mIcon = mIconMgr->contactIcon(path1, 10);
    Q_ASSERT(!mIcon.isNull());
    mIcon = mIcon = mIconMgr->contactIcon(path2, 11);
    Q_ASSERT(!mIcon.isNull());
}

