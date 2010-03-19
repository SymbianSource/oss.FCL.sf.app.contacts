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
#include "ut_logsdbconnector.h"
#include "logsdbconnector.h"
#include "logsevent.h"
#include "logsmodel.h"
#include "logsreader.h"
#include "centralrepository_stub_helper.h"

#include <QtTest/QtTest>

Q_DECLARE_METATYPE(QList<int>)

#define LOGS_TEST_CREATE_EVENT(eventName, index, eventState ) \
LogsEvent* eventName = new LogsEvent; \
eventName->setIndex(index); \
eventName->setLogId(index);\
eventName->setIsInView(true); \
eventName->mEventState = eventState; \
mDbConnector->mEvents.insert(index, eventName)

void UT_LogsDbConnector::initTestCase()
{

}

void UT_LogsDbConnector::cleanupTestCase()
{
}


void UT_LogsDbConnector::init()
{
    mDbConnector = new LogsDbConnector(mEvents);
}

void UT_LogsDbConnector::cleanup()
{
    delete mDbConnector;
    mDbConnector = 0;
}

void UT_LogsDbConnector::testConstructor()
{
    QVERIFY( mDbConnector );
}

void UT_LogsDbConnector::testInit()
{
    QVERIFY( !mDbConnector->mLogsRemove );
    QVERIFY( mDbConnector->init() == 0 );
    QVERIFY( mDbConnector->mReader );
    QVERIFY( mDbConnector->mLogsRemove );
    QVERIFY( mDbConnector->mLogEventStrings.iFetched.length() > 0 );
}

void  UT_LogsDbConnector::testClearList()
{
    QVERIFY( !mDbConnector->mLogsRemove );
    QVERIFY( !mDbConnector->clearList(LogsModel::TypeLogsClearAll) );
    
    mDbConnector->init();
    QVERIFY( mDbConnector->mLogsRemove );
    QVERIFY( mDbConnector->clearList(LogsModel::TypeLogsClearAll) );
}

void UT_LogsDbConnector::testClearEvents()
{
    QVERIFY( !mDbConnector->mLogsRemove );
    QList<int> events;
    events.append(1);
    QVERIFY( !mDbConnector->clearEvents(events) ); // sync
    
    mDbConnector->init();
    QVERIFY( mDbConnector->mLogsRemove );
    mDbConnector->clearEvents(events); 
    QVERIFY( !mDbConnector->clearEvents(events) ); // sync
}

void UT_LogsDbConnector::testMarkEventsSeen()
{
    QList<int> events;
    QSignalSpy spy( mDbConnector, SIGNAL(markingCompleted(int)) );
    
    // Not ready
    mDbConnector->mEventsSeen.clear();
    mDbConnector->markEventsSeen(events);
    QVERIFY( mDbConnector->mEventsSeen.count() == 0 );
    
    // No events
    mDbConnector->init();
    mDbConnector->markEventsSeen(events);
    QVERIFY( mDbConnector->mEventsSeen.count() == 0 );
    
    // Marking missed as seen (one is already seen)
    LOGS_TEST_CREATE_EVENT(event, 0, LogsEvent::EventAdded );
    event->setDirection(LogsEvent::DirMissed);
    LOGS_TEST_CREATE_EVENT(event2, 1, LogsEvent::EventAdded );
    event2->setDirection(LogsEvent::DirMissed);
    LOGS_TEST_CREATE_EVENT(event3, 2, LogsEvent::EventAdded );
    event3->setDirection(LogsEvent::DirMissed);
    events.append(0);
    events.append(1);
    QVERIFY( mDbConnector->markEventsSeen(events) );
    QVERIFY( mDbConnector->mEventsSeen.count() == 2 );
    QVERIFY( mDbConnector->mReader->mModifyingEventId == 0 ); // Started modifying
    
    // Trying to clear missed again, id is appended to mark list, old modifying process in ongoing
    // and is not interrupted
    events.append(2);
    QVERIFY( !mDbConnector->markEventsSeen(events) );
    QVERIFY( mDbConnector->mEventsSeen.count() == 3 );
    QVERIFY( mDbConnector->mReader->mModifyingEventId == 0 ); // Modifying still previous
    
    // Completed previous modifying, next one in queue is modified
    mDbConnector->mReader->Cancel();
    mDbConnector->eventModifyingCompleted();
    QVERIFY( mDbConnector->mEventsSeen.count() == 2 );
    QVERIFY( mDbConnector->mReader->mModifyingEventId == 1 ); // Started modifying next one
    QVERIFY( spy.count() == 0 ); // Not yet totally completed
    
    // Last pending gets completed
    mDbConnector->mEventsSeen.takeLast(); // Now only one pending left
    mDbConnector->eventModifyingCompleted();
    QVERIFY( mDbConnector->mEventsSeen.count() == 0 );
    QVERIFY( spy.count() == 1 ); // Completion was signaled with err 0
    QVERIFY( spy.takeFirst().at(0).toInt() == 0 );
    
    // Clearing all, ids are not appended as those are already in modification list
    mDbConnector->mEventsSeen.clear();
    mDbConnector->mEventsSeen.append(0);
    mDbConnector->mEventsSeen.append(1);
    mDbConnector->mEventsSeen.append(2);
    QVERIFY( !mDbConnector->markEventsSeen(events) );
    QVERIFY( mDbConnector->mEventsSeen.count() == 3 );
}

void UT_LogsDbConnector::testStart()
{
    // No reader, starting fails
    QVERIFY( mDbConnector->start() == -1 );
    
    // Starting ok
    mDbConnector->init();
    QVERIFY( mDbConnector->start() == 0 );
}

void UT_LogsDbConnector::testReadCompleted()
{
    qRegisterMetaType< QList<int> >("QList<int>");
    QSignalSpy spyAdded(mDbConnector, SIGNAL(dataAdded(QList<int>)));
    QSignalSpy spyRemoved(mDbConnector, SIGNAL(dataRemoved(QList<int>)));
    QSignalSpy spyUpdated(mDbConnector, SIGNAL(dataUpdated(QList<int>)));

    // No events, no signal
    mDbConnector->readCompleted(0);
    QVERIFY( spyAdded.count() == 0 );
    QVERIFY( spyRemoved.count() == 0 );
    QVERIFY( spyUpdated.count() == 0 );
    
    // Events exists, their indexes are signaled
    LOGS_TEST_CREATE_EVENT(event, 0, LogsEvent::EventAdded );
    mDbConnector->readCompleted(1);
    QVERIFY( spyAdded.count() == 1 );
    QList<int> addedIndexes = qvariant_cast< QList<int> >(spyAdded.at(0).at(0));
    QVERIFY( addedIndexes.count() == 1 );
    QVERIFY( addedIndexes.at(0) == 0 );
    QVERIFY( spyRemoved.count() == 0 );
    QVERIFY( spyUpdated.count() == 0 );
    QVERIFY( mDbConnector->mEvents.count() == 1 );
    QVERIFY( mEvents.count() == 1 );
    
    // 2 more events added, their indexes are signaled
    event->mEventState = LogsEvent::EventNotUpdated;
    event->setIndex(2);
    LOGS_TEST_CREATE_EVENT(event2, 0, LogsEvent::EventAdded );
    LOGS_TEST_CREATE_EVENT(event3, 1, LogsEvent::EventAdded );
    mDbConnector->readCompleted(3);
    QVERIFY( spyAdded.count() == 2 );
    QList<int> addedIndexes2 = qvariant_cast< QList<int> >(spyAdded.at(1).at(0));
    QVERIFY( addedIndexes2.count() == 2 );
    QVERIFY( addedIndexes2.at(0) == 0 );
    QVERIFY( addedIndexes2.at(1) == 1 );
    QVERIFY( spyRemoved.count() == 0 );
    QVERIFY( spyUpdated.count() == 0 );
    QVERIFY( mDbConnector->mEvents.count() == 3 );
    QVERIFY( mEvents.count() == 3 );
    
    // One of the events updated (index 1)
    event->mEventState = LogsEvent::EventNotUpdated;
    event2->mEventState = LogsEvent::EventNotUpdated;
    event3->mEventState = LogsEvent::EventUpdated;
    mDbConnector->readCompleted(3); 
    QVERIFY( spyAdded.count() == 2 );
    QVERIFY( spyRemoved.count() == 0 );
    QVERIFY( spyUpdated.count() == 1 );
    QList<int> updatedIndexes = qvariant_cast< QList<int> >(spyUpdated.at(0).at(0));
    QVERIFY( updatedIndexes.count() == 1 );
    QVERIFY( updatedIndexes.at(0) == 1 );
    QVERIFY( mDbConnector->mEvents.count() == 3 );
    QVERIFY( mEvents.count() == 3 );
    
    // One of the events removed (index 2)
    event->setIsInView(false);
    event3->mEventState = LogsEvent::EventNotUpdated;
    mDbConnector->readCompleted(2);
    QVERIFY( spyAdded.count() == 2 );
    QVERIFY( spyRemoved.count() == 1 );
    QVERIFY( spyUpdated.count() == 1 );
    QList<int> removedIndexes = qvariant_cast< QList<int> >(spyRemoved.at(0).at(0));
    QVERIFY( removedIndexes.count() == 1 );
    QVERIFY( removedIndexes.at(0) == 2 ); // index 2
    QVERIFY( mDbConnector->mEvents.count() == 2 );
    QVERIFY( mEvents.count() == 2 );
    
}

void UT_LogsDbConnector::testErrorOccurred()
{
    // If pending event modifying, completion is signaled
    QSignalSpy spy( mDbConnector, SIGNAL(markingCompleted(int)) );
    mDbConnector->mEventsSeen.append(0);
    mDbConnector->mEventsSeen.append(1);
    mDbConnector->errorOccurred(-3);
    QVERIFY( spy.count() == 1 ); // Completion was signaled with err -3
    QVERIFY( spy.takeFirst().at(0).toInt() == -3 );
    QVERIFY( mDbConnector->mEventsSeen.count() == 0 );
}

void UT_LogsDbConnector::testUpdateDetails()
{
    QSignalSpy spyUpdated(mDbConnector, SIGNAL(dataUpdated(QList<int>)));
    QVERIFY( !mDbConnector->mReader );
    QVERIFY( mDbConnector->updateDetails(false) == -1 );
    QVERIFY( spyUpdated.count() == 0 );
    
    mDbConnector->init();
    QVERIFY( mDbConnector->mReader );
    LOGS_TEST_CREATE_EVENT(event, 0, LogsEvent::EventUpdated );
    QVERIFY( mDbConnector->updateDetails(false) == 0 );
    QVERIFY( spyUpdated.count() == 1 );
}

void UT_LogsDbConnector::testClearMissedCallsCounter()
{
    // Not ready
    QVERIFY( mDbConnector->clearMissedCallsCounter() != 0 );
    
    // Ready and value is changed
    mDbConnector->init();
    CentralRepositoryStubHelper::reset();
    CentralRepositoryStubHelper::setCurrentVal(5);
    QVERIFY( mDbConnector->clearMissedCallsCounter() == 0 );
    QVERIFY( CentralRepositoryStubHelper::currentVal() == 0 );
    
    // Ready and no need to change value as it is already zero
    QVERIFY( mDbConnector->clearMissedCallsCounter() == 0 );
    QVERIFY( CentralRepositoryStubHelper::currentVal() == 0 );
    
    // Fails with some error
    CentralRepositoryStubHelper::setCurrentVal(100);
    CentralRepositoryStubHelper::setFailCode(KErrNotFound);
    QVERIFY( mDbConnector->clearMissedCallsCounter() != 0 );
}
