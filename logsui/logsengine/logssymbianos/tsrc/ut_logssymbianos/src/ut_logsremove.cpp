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
#include "ut_logsremove.h"
#include "logsremove.h"
#include "logsremoveobserver.h"
#include "logclient_stubs_helper.h"
#include <logcli.h>

#include <QtTest/QtTest>

void UT_LogsRemove::initTestCase()
{
}

void UT_LogsRemove::cleanupTestCase()
{
}

// ----------------------------------------------------------------------------
// UT_LogsRemove::removeCompleted
// ----------------------------------------------------------------------------
//
void UT_LogsRemove::removeCompleted()
{   
    mRemoveCompleted = true;
}

// ----------------------------------------------------------------------------
// UT_LogsRemove::logsRemoveErrorOccured
// ----------------------------------------------------------------------------
//
void UT_LogsRemove::logsRemoveErrorOccured(int err)
{
    Q_UNUSED(err)
}

void UT_LogsRemove::init()
{
    mRemoveCompleted = false;
    mErrorOccurred = false;
    mError = 0;
    mLogsRemove = new LogsRemove(*this, false);
}

void UT_LogsRemove::cleanup()
{
    delete mLogsRemove;
}

void UT_LogsRemove::testConstructor()
{
    QVERIFY( mLogsRemove );
    QVERIFY( mLogsRemove->mFsSession );
    QVERIFY( !mLogsRemove->mLogClient );
}

void UT_LogsRemove::testclearList()
{
    //clearing doesn't start, since initialization failed
    LogClientStubsHelper::setLeaveWithError(KErrNoMemory);
    QVERIFY( !mLogsRemove->IsActive() );
    QVERIFY( !mLogsRemove->clearList(LogsModel::TypeLogsClearAll) );
    QVERIFY( !mLogsRemove->IsActive() );

    //clearing is ok
    LogClientStubsHelper::setLeaveWithError(KErrNone);
    QVERIFY( mLogsRemove->clearList(LogsModel::TypeLogsClearMissed) );
    QVERIFY( mLogsRemove->IsActive() );
    
    //clearing doesn't start, since already active
    QVERIFY( !mLogsRemove->clearList(LogsModel::TypeLogsClearAll) );
}

void UT_LogsRemove::testClearEvents()
{
    // Clearing with recent view (no ids)
    QList<int> events;
    bool async( false );
    QVERIFY( mLogsRemove->clearEvents(events, async) == 0 );
    QVERIFY( !async );
    QVERIFY( mLogsRemove->mRemovedEvents.count() == 0 );
    
    // Clearing with recent view (ids)
    events.append(2);
    events.append(100);
    QVERIFY( mLogsRemove->clearEvents(events, async) == 0 );
    QVERIFY( !async );
    QVERIFY( mLogsRemove->mRemovedEvents.count() == 0 );
    
    // Clearing with all events (no ids)
    events.clear();
    LogsRemove removeWithAllEvents(*this, true);
    QVERIFY( removeWithAllEvents.clearEvents(events, async) == 0 );
    QVERIFY( !async );
    QVERIFY( removeWithAllEvents.mRemovedEvents.count() == 0 );
    
    // Clearing with all events (ids)
    events.append(99);
    events.append(100);
    QVERIFY( removeWithAllEvents.clearEvents(events, async) == 0 );
    QVERIFY( async );
    QVERIFY( removeWithAllEvents.mRemovedEvents.count() == 1 );
    QVERIFY( removeWithAllEvents.mRemovedEvents.at(0) == 100 );
    
}

void UT_LogsRemove::testDoCancel()
{
    mLogsRemove->DoCancel();
}

void UT_LogsRemove::testRunL()
{
   // Test ok scenario
    mLogsRemove->init();
    mLogsRemove->Cancel();
    mLogsRemove->iStatus = KErrNone;
    mLogsRemove->RunL();
    QVERIFY( mRemoveCompleted );
    
    // Ok scenario when more events to delete
    mRemoveCompleted = false;
    mLogsRemove->mRemovedEvents.append(2);
    mLogsRemove->mRemovedEvents.append(4);
    mLogsRemove->Cancel();
    mLogsRemove->RunL();
    QVERIFY( !mRemoveCompleted );
    QVERIFY( mLogsRemove->mRemovedEvents.count() == 1 );
    
    mLogsRemove->Cancel();
    mLogsRemove->RunL();
    QVERIFY( !mRemoveCompleted );
    QVERIFY( mLogsRemove->mRemovedEvents.count() == 0 );
    
    // Ok scenario when no more events to delete
    mLogsRemove->Cancel();
    mLogsRemove->RunL();
    QVERIFY( mRemoveCompleted );
    QVERIFY( mLogsRemove->mRemovedEvents.count() == 0 );
    
    // Test failure
    mLogsRemove->Cancel();
    mLogsRemove->iStatus = KErrNotFound;
    TRAPD( err, mLogsRemove->RunL() );
    QVERIFY( err == KErrNotFound );
}

void UT_LogsRemove::testRunError()
{
    // No fatal callback for access denied error
    TInt err = KErrAccessDenied;
    QVERIFY( mLogsRemove->RunError(err) == KErrNone );
}

void UT_LogsRemove::testInit()
{
    //initialization failes
    LogClientStubsHelper::setLeaveWithError(KErrNoMemory);
    QVERIFY( mLogsRemove->init() == KErrNoMemory );
    QVERIFY( !mLogsRemove->mLogClient );
    LogClientStubsHelper::setLeaveWithError(KErrNone);
            
    //initializing for the first time
    QVERIFY( mLogsRemove->init() == KErrNone );
    QVERIFY( mLogsRemove->mLogClient );
    CLogClient* oldClient = mLogsRemove->mLogClient;
    
    //2d time should be also ok
    QVERIFY( mLogsRemove->init() == KErrNone );
    QVERIFY( mLogsRemove->mLogClient );
    QVERIFY( oldClient == mLogsRemove->mLogClient );    
}

