/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef _UT_SIM_UTILITY_H_
#define _UT_SIM_UTILITY_H_
#include <QtTest/QtTest>
#include <QObject>

#include <qtcontacts.h>

#ifdef SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER
#include <etelmm_etel_test_server.h>
#else
#include <etelmm.h>
#endif
#include <mmtsy_names.h>
#include "cntsimutility.h"
QTM_USE_NAMESPACE


#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

//TESTED_CLASS=
//TESTED_FILES=

/*!
*/
class TestCntSimUtility : public QObject
{
Q_OBJECT

public:
    TestCntSimUtility();
    virtual ~TestCntSimUtility();
public:
   static bool returnErrorInStub_1;
   static bool returnErrorInStub_2;
public slots:
    void adnCacheStatusReady(CntSimUtility::CacheStatus& cacheStatus, int error);
    void simInfoReady(CntSimUtility::SimInfo& simInfo, int error);
    void availableStoresReady(CntSimUtility::AvailableStores& availableStores, int error);


private slots:
   void initTestCase();
   void cleanupTestCase();
   void TestStoreCreation();
   void TestGetSimInfo();
   void TestGetAvailableStores();
   void TestVerifyPin2Code();
   void TestIsFdnActive();
   void TestSetFdnStatus();
   void TestGetLastImportTime();
   void TestSetLastImportTime();
   //Async
   //async request
   void TestStartGetSimInfo();
   void TestStartGetAvailableStores();
   void TestNotifyAdnCacheStatus();
private:
   void initManager(QString simStore);
   void outputSimInfo(CntSimUtility::SimInfo &results);
private:
    CntSimUtility* m_SimStore;

    bool mAdnStorePresent;
    bool mSdnStorePresent;
    bool mSimPresent;
    int mAdnStoreEntries;
    int mSdnStoreEntries;
    bool mSimError;
    bool mWaitingForAdnCache;
    bool mImportInProgress;
    bool mAdnEntriesPresent;



};

#endif //_UT_SIM_UTILITY_H_
