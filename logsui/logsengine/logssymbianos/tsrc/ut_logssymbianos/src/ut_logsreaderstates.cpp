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
#include "ut_logsreaderstates.h"
#include "logsreaderstates.h"
#include "logsevent.h"
#include "logscommondata.h"
#include "logclient_stubs_helper.h"
#include "qtcontacts_stubs_helper.h"
#include <logcli.h>
#include <logview.h>

#include <QtTest/QtTest>

_LIT( KTestInDirection, "Incoming" );
_LIT( KTestOutDirection, "Outgoing" );
_LIT( KTestMissedDirection, "Missed call" );

void UT_LogsReaderStates::initTestCase()
{
    int rfsDummy = 0;
    mLogClient = CLogClient::NewL( *((RFs*)&rfsDummy) );
    mStrings.iInDirection = DESC_TO_QSTRING( KTestInDirection() );
    mStrings.iOutDirection = DESC_TO_QSTRING( KTestOutDirection() );
    mStrings.iMissedDirection = DESC_TO_QSTRING( KTestMissedDirection() );  
}

void UT_LogsReaderStates::cleanupTestCase()
{
    delete mLogClient;
}


void UT_LogsReaderStates::init()
{
    reset();
    
    mCurrentState = 0;
    mIndex = 0;
    mLogView = 0;
    mLogViewEvent = 0;
    mIsRecentView = true;

    mLogView = CLogViewRecent::NewL(*mLogClient);
    mLogViewEvent = CLogViewEvent::NewL(*mLogClient);
    mDuplicatesView = CLogViewDuplicate::NewL(*mLogClient);
    
    const_cast<CLogEvent&>( mLogView->Event() ).SetDirection( KTestInDirection );
}

void UT_LogsReaderStates::cleanup()
{
    delete mLogView;
    mLogView = 0;
    delete mLogViewEvent;
    mLogViewEvent = 0;
    delete mDuplicatesView;
    mDuplicatesView = 0;
    while (!mEvents.isEmpty()){
        delete mEvents.takeFirst();
    }
    mContactCache.clear();
}

void UT_LogsReaderStates::reset()
{
    mError = 0;
    mTemporaryError = 0;
    mReadCompleted = false;
    mModifyCompleted = false;
    mReadCount = 0;
}

void UT_LogsReaderStates::testStateBase()
{
    LogsReaderStateBase state(*this);
    LogsReaderStateBase state2(*this);
    QVERIFY( state.mNextState == 0 );
    QVERIFY( state2.mNextState == 0 );
    
    state.setNextState(state2);
    QVERIFY( state.mNextState == &state2 );
    QVERIFY( state2.mNextState == 0 );
    
    QVERIFY( !state.enterL() );
    QVERIFY( !state2.enterL() );
    
    QVERIFY( !state.continueL() );
    QVERIFY( !state2.continueL() );
    
    QVERIFY( !state2.enterNextStateL() );
    QVERIFY( mCurrentState == 0 );
    
    QVERIFY( !state.enterNextStateL() );
    QVERIFY( mCurrentState == &state2 );
    
    CLogEvent* logEvent = CLogEvent::NewL();
    CleanupStack::PushL(logEvent);
    logEvent->SetNumber( _L("1234") );
    logEvent->SetId( 100 );
    LogsEvent* logsEvent = new LogsEvent;
    int index( 0 );
    QVERIFY( state.constructAndInsertEventL(*logEvent, logsEvent, index ) );
    logsEvent = 0;
    QVERIFY( mEvents.count() == 1 );
    QVERIFY( mEvents.at(0)->number() == "1234" );
    QVERIFY( mEvents.at(0)->isInView() );
    QVERIFY( index == 1 );
    
    logEvent->SetNumber( _L("2234") );
    logEvent->SetId( 101 );
    logsEvent = new LogsEvent;
    QVERIFY( state.constructAndInsertEventL(*logEvent, logsEvent, index ) );
    logsEvent = 0;
    QVERIFY( mEvents.count() == 2 );
    QVERIFY( mEvents.at(1)->number() == "2234" );
    QVERIFY( mEvents.at(1)->index() == 1 );
    QVERIFY( mEvents.at(1)->isInView() );
    QVERIFY( index == 2 );
    
    // Invalid event discarded
    logEvent->SetNumber( _L("") );
    logEvent->SetId( 102 );
    logsEvent = new LogsEvent;
    QVERIFY( !state.constructAndInsertEventL(*logEvent, logsEvent, index ) );
    QVERIFY( mEvents.count() == 2 );
    QVERIFY( index == 2 );
    
    logEvent->SetNumber( _L("2234") );
    logEvent->SetId( 101 );
    logsEvent = new LogsEvent;
    index = 0;
    QVERIFY( state.updateAndInsertEventL( *logEvent, logsEvent, index ) );
    QVERIFY( mEvents.count() == 3 );
    QVERIFY( index == 1 );
    
    state.resetEvents();
    QVERIFY( !mEvents.at(0)->isInView() );
    QVERIFY( !mEvents.at(1)->isInView() );
    
    LogsEvent* event = state.takeMatchingEvent(*logEvent);
    QVERIFY( event );
    QVERIFY( mEvents.count() == 2 );
    delete event;
    logEvent->SetId( 200 );
    LogsEvent* event2 = state.takeMatchingEvent(*logEvent);
    QVERIFY( !event2 );
    QVERIFY( mEvents.count() == 2 );

    CleanupStack::PopAndDestroy( logEvent );    
        
    //event with Id not found
    QVERIFY( !mEvents.isEmpty() );
    QVERIFY( !state.eventById(0) );
    
    //event list is empty
    while ( !mEvents.isEmpty() ) {
        delete mEvents.takeFirst();
    }
    QVERIFY( !state.eventById(0) );
    
    //event with Id found
    LogsEvent* event3 = new LogsEvent;
    event3->setLogId( 300 );
    mEvents.append(event3);
    QVERIFY( state.eventById(300) == event3 );
    QVERIFY( !mEvents.isEmpty() );
    
}

void UT_LogsReaderStates::testStateFiltering()
{
    LogsReaderStateFiltering state(*this);
    QVERIFY( !state.mFilterList );
    
    // State can be used only with recent view
    mIsRecentView = false;
    TRAPD( err, state.enterL() );
    QVERIFY( err == KErrNotFound );
    mIsRecentView = true;
    
    // Recent list fetching starts ok
    QVERIFY( state.enterL() );
    QVERIFY( state.mFilterList );
    QVERIFY( state.mFilterList->Count() == 1 );
    
    // Recent list fetching does not start
    LogClientStubsHelper::setStubAsyncCallPossible(false);
    QVERIFY( !state.enterL() );
    LogClientStubsHelper::setStubAsyncCallPossible(true);
    
    // Doesn't continue as no next state
    QVERIFY( !state.continueL() );
    
    // Continues as there's next state
    LogsReaderStateFiltering state2(*this);
    state.setNextState(state2);
    QVERIFY( state.continueL() );
}

void UT_LogsReaderStates::testStateFilteringAll()
{
    LogsReaderStateFilteringAll state(*this);
    QVERIFY( !state.mFilterList );
    
    // State can be used only with full event view
    mIsRecentView = true;
    TRAPD( err, state.enterL() );
    QVERIFY( err == KErrNotFound );
    mIsRecentView = false;
    
    // Fetching starts ok
    QVERIFY( state.enterL() );
    QVERIFY( state.mFilterList );
    QVERIFY( state.mFilterList->Count() == 1 );
    
    // Recent list fetching does not start
    LogClientStubsHelper::setStubAsyncCallPossible(false);
    QVERIFY( !state.enterL() );
    LogClientStubsHelper::setStubAsyncCallPossible(true);
}

void UT_LogsReaderStates::testStateReading()
{
    LogsReaderStateReading state(*this);
    
    // Reading starts ok
    QVERIFY( state.enterL() );
    
    // Reading doesn't start ok as no items in view
    LogClientStubsHelper::setViewCount(0);
    QVERIFY( !state.enterL() );
    
    // Reading continues as more events in view exist, new event is added
    QVERIFY( mEvents.count() == 0 );
    const_cast<CLogEvent&>( mLogView->Event() ).SetNumber( _L("12345") );
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 100 );
    LogClientStubsHelper::setViewCount(2);
    QVERIFY( state.continueL() );
    QVERIFY( mEvents.count() == 1 );
    QVERIFY( mIndex == 1 );
    
    // Reading does not continue as no more events in view
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 101 );
    QVERIFY( !state.continueL() );
    QVERIFY( mEvents.count() == 2 );
    QVERIFY( mIndex == 2 );
     
    // Test where view is re-read and mathing event is updated
    mIndex = 0;
    state.mEventIndex = 0;
    LogClientStubsHelper::setViewCount(1);
    QVERIFY( !state.continueL() );
    QVERIFY( mEvents.count() == 2 );
    QVERIFY( mIndex == 1 );
    
    // Test where searching duplicates for missed call event
    mIndex = 0;
    state.mEventIndex = 0;
    const_cast<CLogEvent&>( mLogView->Event() ).SetDirection( KTestMissedDirection );
    LogClientStubsHelper::setViewCount(1); // effective also for duplicates view
    
    // Duplicate finding activation fails
    LogClientStubsHelper::setStubAsyncCallPossible(false);
    QVERIFY( !state.continueL() );
    QVERIFY( !state.mCheckingMissed );
    QVERIFY( mEvents.count() > 0 );
    QVERIFY( mEvents.at(0)->duplicates() == 0 );
    
    // Duplicate finding activation ok
    mIndex = 0;
    state.mEventIndex = 0;
    LogClientStubsHelper::setStubAsyncCallPossible(true);
    QVERIFY( state.continueL() );
    QVERIFY( state.mCheckingMissed );
    QVERIFY( !state.continueL() );
    QVERIFY( !state.mCheckingMissed );
    QVERIFY( mEvents.count() > 0 );
    QVERIFY( mEvents.at(0)->duplicates() == 1 );
    
    // Duplicate finding not done for full event view
    qDeleteAll(mEvents);
    mEvents.clear();
    mIsRecentView = false;
    mIndex = 0;
    state.mEventIndex = 0;
    state.mCheckingMissed = false;
    LogClientStubsHelper::setStubAsyncCallPossible(true);
    QVERIFY( !state.continueL() ); // No next state
    QVERIFY( !state.mCheckingMissed );
}

void UT_LogsReaderStates::testStateReading2()
{
    // Test reading when max size has been defined
    mIndex = 0;
    LogsCommonData::getInstance().configureReadSize(3);
    LogClientStubsHelper::setViewCount(5);
    
    LogsReaderStateReading state(*this);
    
    // Reading starts ok
    QVERIFY( state.enterL() );
    QVERIFY( state.mReadSizeCounter == 0 );
    
     // Reading continues as more events in view exist, new event is added
    QVERIFY( mEvents.count() == 0 );
    const_cast<CLogEvent&>( mLogView->Event() ).SetNumber( _L("12345") );
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 100 );
    QVERIFY( state.continueL() );
    QVERIFY( mEvents.count() == 1 );
    QVERIFY( mIndex == 1 );
    
    // Reading continues as max size has not been reached
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 101 );
    QVERIFY( state.continueL() );
    QVERIFY( mEvents.count() == 2 );
    QVERIFY( mIndex == 2 );
    
    // Reading does not continue as max size has been reached
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 102 );
    QVERIFY( !state.continueL() );
    QVERIFY( mEvents.count() == 3 );
    QVERIFY( mIndex == 3 );
    LogsCommonData::getInstance().configureReadSize(-1);
}

void UT_LogsReaderStates::testStateFillDetails()
{
    LogsReaderStateFillDetails state(*this);
    int contactId = 2;
    
    // No events, nothing to check
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 0 );
    
    // Remote name exists already
    LogsEvent* logsEvent = new LogsEvent;
    logsEvent->setIsInView(true);
    logsEvent->setRemoteParty( "remote" );
    LogsEvent* logsEvent2 = new LogsEvent;
    logsEvent2->setRemoteParty( "remote2" );
    logsEvent2->setIsInView(true);
    mEvents.append( logsEvent );
    mEvents.append( logsEvent2 );
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 0 );
   
    // Some events, nothing yet in cache, match from phonebook not found
    QtContactsStubsHelper::setContactId( contactId );
    logsEvent->setRemoteParty( "" );
    logsEvent->setNumber( "99999999" );
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 0 );
    QVERIFY( !logsEvent->contactLocalId() );
    
    // Some events, nothing yet in cache, match from phonebook found (international format)
    QtContactsStubsHelper::setContactNames("first", "last");
    logsEvent->setNumber( "+3581112222" );
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 1 );
    QVERIFY( logsEvent->contactLocalId() == contactId );
    
    // Some events, nothing yet in cache, match from phonebook found (local format)
    mContactCache.clear();
    logsEvent->setRemoteParty( "" );
    logsEvent->setNumber( "11112222" );
    logsEvent->setLogsEventData(NULL);
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 1 );
    QVERIFY( logsEvent->contactLocalId() == contactId );
    
    // Some events, matching info found from cache
    logsEvent->setLogsEventData(NULL);
    logsEvent->setRemoteParty( "" );
    QVERIFY( !logsEvent->contactLocalId() );
    QVERIFY( !state.enterL() );
    QVERIFY( mContactCache.count() == 1 );
    QVERIFY( logsEvent->remoteParty().length() > 0 );
    QVERIFY( logsEvent->contactLocalId() == contactId );
}

void UT_LogsReaderStates::testStateDone()
{
    LogsReaderStateDone state(*this);
    mIndex = 3;
    LogClientStubsHelper::setViewCount(3);
    QVERIFY( !state.enterL() );
    QVERIFY( mReadCompleted );
    QVERIFY( mReadCount == 3 );
    
    // Reading hasn't gone through whole db view (e.g. maxsize has been defined)
    reset();
    mIndex = 2;
    QVERIFY( !state.enterL() );
    QVERIFY( mReadCompleted );
    QVERIFY( mReadCount == 2 );
}

void UT_LogsReaderStates::testStateSearchingEvent()
{
    // Searching starts ok
    LogsReaderStateSearchingEvent state(*this);
    state.mUpdatingEvent = true;
    mModifyingEventId = 10;
    QVERIFY( state.enterL() );
    QVERIFY( !state.mUpdatingEvent );
    
    // Searching doesn't start ok as no items in view
    LogClientStubsHelper::setViewCount(0);
    QVERIFY( !state.enterL() );
      
    // Searching event continues
    mIndex = 0;
    mModifyingEventId = -1;
    const_cast<CLogEvent&>( mLogView->Event() ).SetId( 100 );
    LogClientStubsHelper::setViewCount(2);
    state.mUpdatingEvent = false;
    QVERIFY( state.continueL() );

    // Last event handled, no target event found, entering next state
    mIndex = 2;
    QVERIFY( !state.continueL() );
    
    // Target event found, updating event
    mIndex = 0;
    mModifyingEventId = 100;
    QVERIFY( state.continueL() );
    QVERIFY( state.mUpdatingEvent );
    QVERIFY( mLogView->Event().Flags() & KLogEventRead );

    // Event updated, entering next state
    LogsReaderStateBase state2(*this);
    state.setNextState(state2);
    QVERIFY( !state.continueL() );
}


void UT_LogsReaderStates::testStateFindingDuplicates()
{
      // Finding starts ok
    LogsReaderStateFindingDuplicates state(*this);
    QVERIFY( state.enterL() );
    
    // Duplicates cannot be searched for some reason
    LogClientStubsHelper::reset();
    LogClientStubsHelper::setStubAsyncCallPossible(false);
    QVERIFY( !(LogClientStubsHelper::stubViewFlags() & KLogEventRead) );
    QVERIFY( !state.enterL() );
    QVERIFY( !(LogClientStubsHelper::stubViewFlags() & KLogEventRead) );
    LogClientStubsHelper::setStubAsyncCallPossible(true);
    
    // Duplicates found, read flag for view is set, no next state to enter
    QVERIFY( !state.continueL() );
    QVERIFY( LogClientStubsHelper::stubViewFlags() & KLogEventRead );
}

void UT_LogsReaderStates::testStateModifyingDone()
{
    LogsReaderStateModifyingDone state(*this);
    
    // Modified event not found for some reason
    LogsEvent* logsEvent = new LogsEvent;
    logsEvent->setIsInView(true);
    mModifyingEventId = 100;
    mEvents.append( logsEvent );
    QVERIFY( !mModifyCompleted );
    QVERIFY( !state.enterL() );
    QVERIFY( mModifyCompleted );
    QVERIFY( !logsEvent->isSeenLocally() );
    
    // Modified event found and is set to locally read
    mModifyCompleted = false;
    logsEvent->setLogId(100);
    QVERIFY( !state.enterL() );
    QVERIFY( mModifyCompleted );
    QVERIFY( logsEvent->isSeenLocally() );
}

// ----------------------------------------------------------------------------
// From LogsReaderStateContext
// ----------------------------------------------------------------------------
//
void UT_LogsReaderStates::setCurrentState(const LogsReaderStateBase& state)
{
    mCurrentState = &state;
}

// ----------------------------------------------------------------------------
// From LogsReaderStateContext
// ----------------------------------------------------------------------------
//
CLogView& UT_LogsReaderStates::logView()
{
    if ( mIsRecentView ){
        return *mLogView;
    }
    return *mLogViewEvent;
}

CLogViewDuplicate& UT_LogsReaderStates::duplicatesView()
{
    return *mDuplicatesView;
}

QList<LogsEvent*>& UT_LogsReaderStates::events()
{
    return mEvents;
}

int& UT_LogsReaderStates::index()
{
    return mIndex;
}

LogsEventStrings& UT_LogsReaderStates::strings()
{
    return mStrings;
}

TRequestStatus& UT_LogsReaderStates::reqStatus()
{
    return mReqStatus;
}

LogsReaderObserver& UT_LogsReaderStates::observer()
{
    return *this;
}

QHash<QString, ContactCacheEntry>& UT_LogsReaderStates::contactCache()
{
    return mContactCache;
}

int UT_LogsReaderStates::modifyingEventId()
{
    return mModifyingEventId;
}

CLogClient& UT_LogsReaderStates::logClient()
{
    return *mLogClient;
}

bool UT_LogsReaderStates::isRecentView()
{
    return mIsRecentView;
}

// ----------------------------------------------------------------------------
// From LogsReaderObserver
// ----------------------------------------------------------------------------
//
void UT_LogsReaderStates::readCompleted(int readCount)
{
    mReadCompleted = true;
    mReadCount = readCount;
}
void UT_LogsReaderStates::errorOccurred(int err)
{
    mError = err;
}
void UT_LogsReaderStates::temporaryErrorOccurred(int err)
{
    mTemporaryError = err;
}
void UT_LogsReaderStates::eventModifyingCompleted()
{
    mModifyCompleted = true;
}
