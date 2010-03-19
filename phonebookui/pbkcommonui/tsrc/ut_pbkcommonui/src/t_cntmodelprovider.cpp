/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qtcontacts.h>

#include "cntmodelprovider.h"
#include "t_cntmodelprovider.h"

void TestCntModelProvider::initTestCase()
{
    mModelProvider = 0;
}


void TestCntModelProvider::instance()
{
    mModelProvider = CntModelProvider::instance();
    QVERIFY(mModelProvider != 0);

    int refcount = mModelProvider->referenceCount();
    QVERIFY(refcount == 1);
    mModelProvider->release();
}

void TestCntModelProvider::twoInstances()
{
    mModelProvider = 0;
    QVERIFY(mModelProvider == 0);
    mModelProvider = CntModelProvider::instance();
    QVERIFY(mModelProvider != 0);
    int refcount = mModelProvider->referenceCount();
    QVERIFY(refcount == 1);


    CntModelProvider *provider = 0;
    provider = CntModelProvider::instance();

    refcount = mModelProvider->referenceCount();
    QVERIFY(refcount == 2);


    mModelProvider->release();

    refcount = mModelProvider->referenceCount();
    QVERIFY(refcount == 1);

    provider->release();
}

void TestCntModelProvider::release()
{
    mModelProvider = 0;
    QVERIFY(mModelProvider == 0);
    mModelProvider = CntModelProvider::instance();
    QVERIFY(mModelProvider != 0);
    int refcount = mModelProvider->referenceCount();
    QVERIFY(refcount == 1);
    mModelProvider->release();
}

void TestCntModelProvider::contactModel()
{
    mModelProvider = 0;
    QVERIFY(mModelProvider == 0);
    mModelProvider = CntModelProvider::instance();
    QVERIFY(mModelProvider != 0);
    int refcount = mModelProvider->referenceCount();
    QVERIFY(refcount == 1);
    MobCntModel *model = mModelProvider->contactModel();
    QVERIFY(model != 0);
    MobCntModel *otherModel = mModelProvider->contactModel();
    QVERIFY(model == otherModel);
    mModelProvider->release();
}

void TestCntModelProvider::contactManager()
{
    mModelProvider = 0;
    QVERIFY(mModelProvider == 0);
    mModelProvider = CntModelProvider::instance();
    QVERIFY(mModelProvider != 0);
    int refcount = mModelProvider->referenceCount();
    QVERIFY(refcount == 1);
    QContactManager *manager = mModelProvider->contactManager();
    QVERIFY(manager != 0);
    QContactManager *othermanager = mModelProvider->contactManager();
    QVERIFY(manager == othermanager);
    mModelProvider->release();
}

void TestCntModelProvider::contactSimManager()
{
    mModelProvider = 0;
    QVERIFY(mModelProvider == 0);
    mModelProvider = CntModelProvider::instance();
    QVERIFY(mModelProvider != 0);
    int refcount = mModelProvider->referenceCount();
    QVERIFY(refcount == 1);
    QContactManager *manager = mModelProvider->contactSimManager();
    QVERIFY(manager != 0);
    QContactManager *othermanager = mModelProvider->contactSimManager();
    QVERIFY(manager == othermanager);
    mModelProvider->release();
}

void TestCntModelProvider::cleanupTestCase()
{

}

// EOF
