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

// INCLUDE FILES
#include <exception>
#include <logview.h>
#include "logsreader.h"
#include "logsreaderobserver.h"
#include "logsevent.h"
#include "logsengdefs.h"
#include "logslogger.h"

// CONSTANTS

// ----------------------------------------------------------------------------
// LogsReader::LogsReader
// ----------------------------------------------------------------------------
//
LogsReader::LogsReader(
    RFs& fsSession,
    CLogClient& logClient,
    LogsEventStrings& strings,
    QList<LogsEvent*>& events,
    LogsReaderObserver& observer,
    bool readAllEvents) 
 : CActive( EPriorityStandard ),
   mLogViewRecent(0),
   mLogViewEvent(0),
   mDuplicatesView(0),
   mFsSession(fsSession), 
   mLogClient(logClient),
   mStrings(strings), 
   mEvents(events), 
   mObserver(observer),
   mReadAllEvents(readAllEvents),
   mIndex(0),
   mCurrentStateIndex(0),
   mCurrentStateMachine(0),
   mCurrentEventId(-1)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReader::LogsReader()" )
    
    TRAP_IGNORE( mLogClient.SetGlobalChangeObserverL( this ) )
    CActiveScheduler::Add( this ); 
    
    initializeReadStates();
    
    LOGS_QDEBUG( "logs [ENG] <- LogsReader::LogsReader()" )
}

// ----------------------------------------------------------------------------
// LogsReader::~LogsReader
// ----------------------------------------------------------------------------
//
LogsReader::~LogsReader()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReader::~LogsReader()" )
    
    Cancel();
    qDeleteAll( mReadStates );
    qDeleteAll( mModifyingStates );
    qDeleteAll( mDuplicateReadingStates );
    delete mLogViewRecent;
    delete mLogViewEvent;
    delete mDuplicatesView;
    qDeleteAll( mDuplicatedEvents );
    
    LOGS_QDEBUG( "logs [ENG] <- LogsReader::~LogsReader()" )
}

// ----------------------------------------------------------------------------
// LogsReader::start
// ----------------------------------------------------------------------------
//
int LogsReader::start()
    {
    LOGS_QDEBUG( "logs [ENG] -> LogsReader::start()" )
    
    TRAPD( err, startL() );
    
    LOGS_QDEBUG_2( "logs [ENG] <- LogsReader::start(), err:", err )
    return err;
    }

// ----------------------------------------------------------------------------
// LogsReader::stop
// ----------------------------------------------------------------------------
//
void LogsReader::stop()
    {
    LOGS_QDEBUG( "logs [ENG] -> LogsReader::stop()" )
    
    Cancel(); 
    
    LOGS_QDEBUG( "logs [ENG] <- LogsReader::stop()" )
    }

// ----------------------------------------------------------------------------
// LogsReader::updateDetails
// ----------------------------------------------------------------------------
//
void LogsReader::updateDetails(bool clearCached)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReader::updateDetails()" )
    foreach (LogsEvent* event, mEvents){
        event->prepareForContactMatching();
    }
    if ( clearCached ) {
        mContactCache.clear();
    }
    LogsReaderStateFillDetails* fillDetailsState = 
                new LogsReaderStateFillDetails(*this);
    fillDetailsState->fillDetails();
    delete fillDetailsState;
    LOGS_QDEBUG( "logs [ENG] <- LogsReader::updateDetails()" )
}

// ----------------------------------------------------------------------------
// LogsReader::markEventSeen
// ----------------------------------------------------------------------------
//
int LogsReader::markEventSeen(int eventId)
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsReader::markEventsSeen(), ids: ", eventId )
    TRAPD( err, markEventSeenL(eventId) );
    LOGS_QDEBUG_2( "logs [ENG] <- LogsReader::markEventsSeen(), err:", err )
    return err;
}

// ----------------------------------------------------------------------------
// LogsReader::readDuplicates
// ----------------------------------------------------------------------------
//
int LogsReader::readDuplicates(int eventId)
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsReader::readDuplicates(), ids: ", eventId )
    TRAPD( err, readDuplicatesL(eventId) );
    LOGS_QDEBUG_2( "logs [ENG] <- LogsReader::readDuplicates(), err:", err )
    return err;
}

// ----------------------------------------------------------------------------
// LogsReader::RunL
// ----------------------------------------------------------------------------
//
void LogsReader::RunL()
{
    LOGS_QDEBUG_3( "logs [ENG] -> LogsReader::RunL(), (state, status):", 
                   mCurrentStateIndex, iStatus.Int() )
    
    // Error handling in RunError
    __ASSERT_ALWAYS( iStatus.Int() == KErrNone, User::Leave( iStatus.Int() ) );
    
    if ( currentState().continueL() ){
        SetActive();
    }
    
    LOGS_QDEBUG( "logs [ENG] <- LogsReader::RunL()" )
}

// ----------------------------------------------------------------------------
// LogsReader::RunError
// ----------------------------------------------------------------------------
//
TInt LogsReader::RunError(TInt error)
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsReader::RunError(), err:", error )
    
    handleError(error);
    
    LOGS_QDEBUG( "logs [ENG] <- LogsReader::RunError()" )
    return KErrNone;
}

// ----------------------------------------------------------------------------
// LogsReader::DoCancel
// ----------------------------------------------------------------------------
//
void LogsReader::DoCancel()
{
    logView().Cancel();
    mDuplicatesView->Cancel();    
    mLogClient.Cancel();
}

// ----------------------------------------------------------------------------
// LogsReader::startL
// ----------------------------------------------------------------------------
//
void LogsReader::startL()
{
    prepareReadingL();
    
    initializeReadStates();
    
    if ( currentState().enterL() ){
        SetActive();
    }
} 

// ----------------------------------------------------------------------------
// LogsReader::markEventSeenL
// ----------------------------------------------------------------------------
//
void LogsReader::markEventSeenL(int eventId)
{
    prepareReadingL();
    
    mCurrentEventId = eventId;   
    initializeModifyingStates();
    
    if ( currentState().enterL() ){
        SetActive();
    }
}

// ----------------------------------------------------------------------------
// LogsReader::readDuplicatesL
// ----------------------------------------------------------------------------
//
void LogsReader::readDuplicatesL(int eventId)
{
    if ( IsActive() && mCurrentStateMachine != &mDuplicateReadingStates ){
        LOGS_QDEBUG( "logs [ENG] <-> LogsReader::readDuplicatesL(), cannot interrupt" )
        User::Leave(KErrInUse);
    }
    prepareReadingL();
    
    mCurrentEventId = eventId;
    initializeDuplicateReadingStates();
    
    if ( currentState().enterL() ){
        SetActive();
    }
}

// ----------------------------------------------------------------------------
// LogsReader::cancelCurrentRequestL
// ----------------------------------------------------------------------------
//
void LogsReader::cancelCurrentRequestL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReader::cancelCurrentRequestL()" )
    if ( IsActive() ) {
        if (mCurrentStateMachine == &mReadStates || 
            mCurrentStateMachine == &mDuplicateReadingStates){
            LOGS_QDEBUG( "logs [ENG] reading is in progress, cancelling" )
            Cancel();
        } else if (mCurrentStateMachine == &mModifyingStates) {
            LOGS_QDEBUG( "logs [ENG] <- LogsReader::removeEvent(), modifying is in progress, can't cancel" )
            User::Leave(KErrInUse);
        }
    }
        
    LOGS_QDEBUG( "logs [ENG] <- LogsReader::cancelCurrentRequestL()" )   
}



// ----------------------------------------------------------------------------
// LogsReader::HandleLogClientChangeEventL
//
// Called by Log Database engine when it notifies a change in database (e.g.
// Log database cleared)
// ----------------------------------------------------------------------------
//
void LogsReader::HandleLogClientChangeEventL(
TUid aChangeType,
TInt /*aChangeParam1*/,
TInt /*aChangeParam2*/,
TInt /*aChangeParam3*/ )
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsReader::HandleLogClientChangeEventL()",
                   aChangeType.iUid )
    if( aChangeType == KLogClientChangeEventLogCleared ){
        handleViewChange();
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsReader::HandleLogClientChangeEventL()" )
}

// ----------------------------------------------------------------------------
// LogsReader::HandleLogViewChangeEventAddedL
// Called by Log Database engine when it notifies us that it has added
// an event to database
// ----------------------------------------------------------------------------
//
void LogsReader::HandleLogViewChangeEventAddedL( 
TLogId /*aId*/, 
TInt /*aViewIndex*/, 
TInt /*aChangeIndex*/, 
TInt aTotalChangeCount )
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsReader::HandleLogViewChangeEventAddedL()",
                   aTotalChangeCount )
                   
    //Comment: HandleLogViewChangeEventAddedL seems to be called only once
    // (and aTotalChangeCount is 1) even if  there are multiple entries added
    // to database in a batch. This seems to happen at least in wins emulator
    // in Symbian 80a_200432. If problems in this area or changed behaviour,
    // we need to consider same kind of optimization to here as is in 
    //HandleLogViewChangeEventDeletedL

    handleViewChange(aTotalChangeCount);
    LOGS_QDEBUG( "logs [ENG] <- LogsReader::HandleLogViewChangeEventAddedL()" )
}

// ----------------------------------------------------------------------------
// LogsReader::HandleLogViewChangeEventChangedL
// Called by Log Database engine when it notifies us that it has
// changed an event in the database
// ----------------------------------------------------------------------------
//
void LogsReader::HandleLogViewChangeEventChangedL( 
TLogId /*aId*/, 
TInt /*aViewIndex*/, 
TInt /*aChangeIndex*/, 
TInt aTotalChangeCount )
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsReader::HandleLogViewChangeEventChangedL()",
                   aTotalChangeCount )
    handleViewChange(aTotalChangeCount);
    LOGS_QDEBUG( "logs [ENG] <- LogsReader::HandleLogViewChangeEventChangedL()" )
}

// ----------------------------------------------------------------------------
// LogsReader::HandleLogViewChangeEventDeletedL
// Called by Log Database engine when it notifies us that it has deleted
// an event in the database
// ----------------------------------------------------------------------------
//
void LogsReader::HandleLogViewChangeEventDeletedL( 
TLogId /*aId*/, 
TInt /*aViewIndex*/, 
TInt aChangeIndex, 
TInt aTotalChangeCount )
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsReader::HandleLogViewChangeEventDeletedL()",
                   aTotalChangeCount )
LOGS_QDEBUG_2( "logs [ENG] -> LogsReader::HandleLogViewChangeEventDeletedL() aChangeIndex=",
        aChangeIndex )
    // In order to prevent to re-reading the database multiple times, we call
    // HandleViewChangeL only once. This is because HandleLogViewChangeEventDeletedL
    // is called as many times as is the number of entries are deleted e.g. when 
    // deleteting old entries from database happens. However, aTotalChangeCount contains
    // total number of deletions in a batch, so we can optimize the call to
    // HandleViewChangeL to happen only even if we're called multiple times.
    // Value of aChangeIndex is increasing from 0 to ( aTotalChangeCount - 1 ).
    // we call HandleViewChangeL when the last notify has arrived.
  
if ( aChangeIndex == ( aTotalChangeCount - 1 ) ) 
    {
    LOGS_QDEBUG( "logs [ENG] <- LogsReader::HandleLogViewChangeEventDeletedL() handleViewChange" )
       handleViewChange(aTotalChangeCount);
   }               
                   
    LOGS_QDEBUG( "logs [ENG] <- LogsReader::HandleLogViewChangeEventDeletedL()" )
}

// ----------------------------------------------------------------------------
// LogsReader::setCurrentState
// ----------------------------------------------------------------------------
//
void LogsReader::setCurrentState(const LogsReaderStateBase& state)
{
    bool found(false);
    for( int i = 0; i < mCurrentStateMachine->count() && !found; i++ ){
        if ( mCurrentStateMachine->at(i) == &state ){
            mCurrentStateIndex = i;
            found = true;
            LOGS_QDEBUG_2( "logs [ENG] <-> LogsReader::setCurrentState, index:", 
                           mCurrentStateIndex )
        }
    }
}

// ----------------------------------------------------------------------------
// LogsReader::logView
// ----------------------------------------------------------------------------
//
CLogView& LogsReader::logView()
{
    if ( mLogViewRecent ){
        return *mLogViewRecent;
    }
    return *mLogViewEvent;
}

// ----------------------------------------------------------------------------
// LogsReader::duplicatesView
// ----------------------------------------------------------------------------
//
CLogViewDuplicate& LogsReader::duplicatesView()
{
    return *mDuplicatesView;
}

// ----------------------------------------------------------------------------
// LogsReader::events
// ----------------------------------------------------------------------------
//
QList<LogsEvent*>& LogsReader::events()
{
    return mEvents;
}

// ----------------------------------------------------------------------------
// LogsReader::index
// ----------------------------------------------------------------------------
//
int& LogsReader::index()
{
    return mIndex;
}

// ----------------------------------------------------------------------------
// LogsReader::strings
// ----------------------------------------------------------------------------
//
LogsEventStrings& LogsReader::strings()
{
    return mStrings;
}

// ----------------------------------------------------------------------------
// LogsReader::reqStatus
// ----------------------------------------------------------------------------
//
TRequestStatus& LogsReader::reqStatus()
{
    return iStatus;
}

// ----------------------------------------------------------------------------
// LogsReader::observer
// ----------------------------------------------------------------------------
//
LogsReaderObserver& LogsReader::observer()
{
    return mObserver;
}

// ----------------------------------------------------------------------------
// LogsReader::contactCache
// ----------------------------------------------------------------------------
//
QHash<QString, ContactCacheEntry>& LogsReader::contactCache()
{
    return mContactCache;
}

// ----------------------------------------------------------------------------
// LogsReader::currentEventId
// ----------------------------------------------------------------------------
//
int LogsReader::currentEventId()
{
    return mCurrentEventId;
}

// ----------------------------------------------------------------------------
// LogsReader::logClient
// ----------------------------------------------------------------------------
//
CLogClient& LogsReader::logClient()
{
    return mLogClient;
}

// ----------------------------------------------------------------------------
// LogsReader::isRecentView
// ----------------------------------------------------------------------------
//
bool LogsReader::isRecentView()
{
    return ( mLogViewRecent != 0 );
}

// ----------------------------------------------------------------------------
// LogsReader::duplicatedEvents
// ----------------------------------------------------------------------------
//
QList<LogsEvent*>& LogsReader::duplicatedEvents()
{
    return mDuplicatedEvents;
}

// ----------------------------------------------------------------------------
// LogsReader::currentState
// ----------------------------------------------------------------------------
//
LogsReaderStateBase& LogsReader::currentState()
{
    return *(mCurrentStateMachine->at(mCurrentStateIndex));
}

// ----------------------------------------------------------------------------
// LogsReader::initializeReadStates
// ----------------------------------------------------------------------------
//
void LogsReader::initializeReadStates()
{
    if ( mReadStates.count() == 0 ){
        
        LogsReaderStateFiltering* filtering = createFilteringState();
        LogsReaderStateReading* reading = new LogsReaderStateReading(*this);
        LogsReaderStateFillDetails* fillDetails = new LogsReaderStateFillDetails(*this);
        LogsReaderStateDone* done = new LogsReaderStateDone(*this);
        filtering->setNextState(*reading);
        reading->setNextState(*fillDetails);
        fillDetails->setNextState(*done);
        mReadStates.append(filtering);
        mReadStates.append(reading);
        mReadStates.append(fillDetails);
        mReadStates.append(done);
    }
    mCurrentStateMachine = &mReadStates;
    setCurrentState(*mReadStates.at(0));
}

// ----------------------------------------------------------------------------
// LogsReader::initializeDuplicateReadingStates
// ----------------------------------------------------------------------------
//
void LogsReader::initializeDuplicateReadingStates()
{
    if ( mDuplicateReadingStates.count() == 0 ){
        LogsReaderStateFiltering* filtering = createFilteringState();
        LogsReaderStateSearchingEvent* searching = 
                    new LogsReaderStateSearchingEvent(*this);
        LogsReaderStateFindingDuplicates* findingDuplicates = 
                    new LogsReaderStateFindingDuplicates(*this);
        LogsReaderStateReadingDuplicates* readingDuplicates = 
                    new LogsReaderStateReadingDuplicates(*this);
        LogsReaderStateReadingDuplicatesDone* done = 
                    new LogsReaderStateReadingDuplicatesDone(*this);
        filtering->setNextState(*searching);
        searching->setNextState(*findingDuplicates);
        findingDuplicates->setNextState(*readingDuplicates);
        readingDuplicates->setNextState(*done);
        mDuplicateReadingStates.append(filtering);
        mDuplicateReadingStates.append(searching); 
        mDuplicateReadingStates.append(findingDuplicates); 
        mDuplicateReadingStates.append(readingDuplicates);
        mDuplicateReadingStates.append(done);        
    }
    mCurrentStateMachine = &mDuplicateReadingStates;
    setCurrentState(*mDuplicateReadingStates.at(0)); 
}

// ----------------------------------------------------------------------------
// LogsReader::initializeModifyingStates
// ----------------------------------------------------------------------------
//
void LogsReader::initializeModifyingStates()
{
    if ( mModifyingStates.count() == 0 ){
        LogsReaderStateFiltering* filtering = createFilteringState();
        LogsReaderStateSearchingEvent* searching = 
                    new LogsReaderStateSearchingEvent(*this);
        LogsReaderStateMarkingDuplicates* duplicates = 
                    new LogsReaderStateMarkingDuplicates(*this);
        LogsReaderStateModifyingDone* done = new LogsReaderStateModifyingDone(*this);
        filtering->setNextState(*searching);
        searching->setNextState(*duplicates);
        duplicates->setNextState(*done);
        mModifyingStates.append(filtering);
        mModifyingStates.append(searching); 
        mModifyingStates.append(duplicates);
        mModifyingStates.append(done);        
    }
    mCurrentStateMachine = &mModifyingStates;
    setCurrentState(*mModifyingStates.at(0)); 
}

// ----------------------------------------------------------------------------
// LogsReader::handleViewChange
// ----------------------------------------------------------------------------
//
void LogsReader::handleViewChange(int totalChangeCount)
{
    //we do not update view untill deletion is done
    if ( totalChangeCount > 0 ){
        int err = start();
        handleError( err );
    } 
}

// ----------------------------------------------------------------------------
// LogsReader::handleError
// ----------------------------------------------------------------------------
//
void LogsReader::handleError(int error)
{
    if ( error == KErrAccessDenied || error == KErrInUse ){
        LOGS_QDEBUG( "logs [ENG] <-> LogsReader::handleError(), temp err" )
        mObserver.temporaryErrorOccurred(error); 
    } else if ( error != 0 ){
        LOGS_QDEBUG( "logs [ENG] <-> LogsReader::handleError(), fatal err" )
        mObserver.errorOccurred(error); 
    }
}

// ----------------------------------------------------------------------------
// LogsReader::createLogViewL
// ----------------------------------------------------------------------------
//
void LogsReader::createLogViewL()
{
    if ( mReadAllEvents ){
        if ( !mLogViewEvent ){
            mLogViewEvent = CLogViewEvent::NewL( mLogClient, *this );
        }
    }
    else if ( !mLogViewRecent ) {
        mLogViewRecent = CLogViewRecent::NewL( mLogClient, *this );
    }
}

// ----------------------------------------------------------------------------
// LogsReader::createFilteringState
// ----------------------------------------------------------------------------
//
LogsReaderStateFiltering* LogsReader::createFilteringState()
{
    LogsReaderStateFiltering* filtering = 0;
    if ( mReadAllEvents ) {
        filtering = new LogsReaderStateFilteringAll(*this);
    } 
    else {
        filtering = new LogsReaderStateFiltering(*this);
    }
    return filtering;
}

// ----------------------------------------------------------------------------
// LogsReader::prepareReadingL
// ----------------------------------------------------------------------------
//
void LogsReader::prepareReadingL()
{
    cancelCurrentRequestL();

    createLogViewL();
    
    if ( !mDuplicatesView ){
        mDuplicatesView = CLogViewDuplicate::NewL( mLogClient, *this );
    }   
    mIndex = 0;
}

