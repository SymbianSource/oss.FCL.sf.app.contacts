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
#include "ut_logscommondata.h"
#include "logscommondata.h"

#include <QtTest/QtTest>

void UT_LogsCommonData::initTestCase()
{
}

void UT_LogsCommonData::cleanupTestCase()
{
}


void UT_LogsCommonData::init()
{
}

void UT_LogsCommonData::cleanup()
{
}

void UT_LogsCommonData::testGetInstance()
{
    QVERIFY( &LogsCommonData::getInstance() != 0 );
    QVERIFY( &LogsCommonData::getInstance() == &LogsCommonData::getInstance() );
}

void UT_LogsCommonData::testGetContactManager()
{
    QVERIFY( &LogsCommonData::getInstance().contactManager() != 0 );
    QVERIFY( &LogsCommonData::getInstance().contactManager() == &LogsCommonData::getInstance().contactManager() );
}

void UT_LogsCommonData::testFreeCommonData()
{
    LogsCommonData* comData = &LogsCommonData::getInstance();
    QVERIFY( comData != 0 );
    LogsCommonData::freeCommonData();
}


