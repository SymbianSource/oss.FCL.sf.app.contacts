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
#include "logsdbconnector.h"
#include "logsevent.h"
#include "logsengdefs.h"
#include "logsreader.h"
#include "logslogger.h"
#include "logsremove.h"
#include <logcli.h>
#include <f32file.h>
#include <centralrepository.h>
#include <logsdomaincrkeys.h>

// CONSTANTS


// ----------------------------------------------------------------------------
// LogsDbConnector::LogsDbConnector
// ----------------------------------------------------------------------------
//
LogsDbConnector::LogsDbConnector( QList<LogsEvent*>& events, bool checkAllEvents ) 
: QObject(), 
  mModelEvents( events ), 
  mCheckAllEvents( checkAllEvents ),
  mLogClient( 0 ), 
  mReader( 0 ),
  mLogsRemove( 0 ),
  mRepository( 0 )
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsDbConnector::LogsDbConnector()" )
    mFsSession = new RFs();
}

// ----------------------------------------------------------------------------
// LogsDbConnector::~LogsDbConnector
// ----------------------------------------------------------------------------
//
LogsDbConnector::~LogsDbConnector()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::~LogsDbConnector()" )
    
    delete mReader;
    delete mLogsRemove;
    delete mLogClient;
    if ( mFsSession ){
        mFsSession->Close();
    }
    delete mFsSession;
    
    while ( !mEvents.isEmpty() ){
        delete mEvents.takeFirst();
    }
    
    delete mRepository;
    
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::~LogsDbConnector()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::init
// ----------------------------------------------------------------------------
//
int LogsDbConnector::init()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::init()" )
    
    TRAPD( err, initL() );
    handleTemporaryError(err);
    
    LOGS_QDEBUG_2( "logs [ENG] <- LogsDbConnector::init(), err:", err )
    
    return err;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::start
// ----------------------------------------------------------------------------
//
int LogsDbConnector::start()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::start()" )

#ifdef LOGSDBCONNECTOR_SIMULATION
    
    LOGS_QDEBUG( "logs [ENG]    Simulation enabled" )
    int numEvents = 16;
    QList<int> indexes;
    for ( int i = 0; i < numEvents; i++ ){
        QString number = QString("1223456%1").arg(i);
        LogsEvent* event = new LogsEvent;
        if ( i % 3 == 0 ){
            event->setDirection(LogsEvent::DirMissed);
            number += QString("DirMissed");        
        }
        else if ( i % 2 == 0 ){
            event->setDirection(LogsEvent::DirIn);
            number += QString("DirIn");
        } else {
            event->setDirection(LogsEvent::DirOut);
            number += QString("DirOut");
        }
        event->setNumber(number);
        mEvents.append(event);
        mModelEvents.append(event);
        indexes.append(i);
    }
    emit dataAdded( indexes );
    return 0;
    
#endif
    
    if ( !mReader ){
        LOGS_QDEBUG( "logs [ENG]    Not initialized, failure" )
        return -1;
    }
    int err = mReader->start();
    handleTemporaryError(err);
    
    LOGS_QDEBUG_2( "logs [ENG] <- LogsDbConnector::start(), err:", err )
    return err;
}


// ----------------------------------------------------------------------------
// LogsDbConnector::updateDetails
// ----------------------------------------------------------------------------
//
int LogsDbConnector::updateDetails(bool clearCached)
{
    if ( !mReader ){
        LOGS_QDEBUG( "logs [ENG]    Not initialized, failure" )
        return -1;
    }
    mReader->updateDetails(clearCached);    
    readCompleted( mEvents.count() ); //to notify of model update
    return 0;    
}

// ----------------------------------------------------------------------------
// LogsDbConnector::stateChanged
// ----------------------------------------------------------------------------
//
void LogsDbConnector::initL()
{
    if ( mReader ){
        // Already initialized
        return;
    }  
    User::LeaveIfError( mFsSession->Connect() );
    mLogClient = CLogClient::NewL( *mFsSession );
    mLogsRemove = new LogsRemove( *this, mCheckAllEvents );
    
    TLogString logString;
    //Texts in LOGWRAP.RLS / LOGWRAP.RSS        
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_DIR_IN ) );
    mLogEventStrings.iInDirection = DESC_TO_QSTRING( logString );
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_DIR_OUT ) );
    mLogEventStrings.iOutDirection = DESC_TO_QSTRING( logString );
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_DIR_MISSED ) );
    mLogEventStrings.iMissedDirection = DESC_TO_QSTRING( logString );
    //"Unknown" (Logwrap.rls)
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_REMOTE_UNKNOWN ) ); 
    mLogEventStrings.iUnKnownRemote = DESC_TO_QSTRING( logString );
    //"Incoming on alternate line"
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_DIR_IN_ALT ) ); 
    mLogEventStrings.iInDirectionAlt = DESC_TO_QSTRING( logString );
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_DIR_OUT_ALT ) );
    mLogEventStrings.iOutDirectionAlt = DESC_TO_QSTRING( logString );
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_DIR_FETCHED) );
    mLogEventStrings.iFetched = DESC_TO_QSTRING( logString );
    
    mReader = new LogsReader( 
        *mFsSession, *mLogClient, mLogEventStrings, mEvents, *this, mCheckAllEvents );
    
    mRepository = CRepository::NewL( KCRUidLogs );
}

// ----------------------------------------------------------------------------
// LogsDbConnector::clearList
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::clearList(LogsModel::ClearType cleartype)
{
    bool clearingStarted(false);
	if ( mLogsRemove ){
		clearingStarted = mLogsRemove->clearList(cleartype);
	}
	return clearingStarted;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::clearEvent
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::clearEvents(const QList<int>& eventIds)
{
    bool asyncClearingStarted(false);
    if ( mLogsRemove ){
        bool async(false);
        int err = mLogsRemove->clearEvents(eventIds, async);
        asyncClearingStarted = ( !err && async );
    }    
    return asyncClearingStarted;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::markEventsSeen
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::markEventsSeen(const QList<int>& eventIds)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::markEventsSeen()" )
    
    if ( !mReader ){
        return false;
    }
    
    foreach( int currId, eventIds ){
        if ( !mEventsSeen.contains(currId) ){
            mEventsSeen.append(currId);
        }
    }

    LOGS_QDEBUG_2( "logs [ENG] -> event ids:", mEventsSeen );  
    
    int err = doMarkEventSeen();
    LOGS_QDEBUG_2( "logs [ENG] <- LogsDbConnector::markEventsSeen(), marking err:", 
                   err )
    return ( err == 0 );
}

// ----------------------------------------------------------------------------
// LogsDbConnector::clearMissedCallsCounter
// ----------------------------------------------------------------------------
//
int LogsDbConnector::clearMissedCallsCounter()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::clearMissedCallsCounter()" )
    if ( !mRepository ){
        return -1;
    }
    TInt value(0);
    int err = mRepository->Get( KLogsNewMissedCalls, value );
    if ( !err && value != 0 ){
        err = mRepository->Set( KLogsNewMissedCalls, 0 );
    }
    LOGS_QDEBUG_2( "logs [ENG] <- LogsDbConnector::clearMissedCallsCounter(), err", err )
    return err;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::handleTemporaryError
// ----------------------------------------------------------------------------
//
void LogsDbConnector::handleTemporaryError(int& error)
{
     if ( error == KErrAccessDenied ){
        LOGS_QDEBUG(
            "logs [ENG] LogsDbConnector::handleTemporaryError, DB temp unavailable" )
        // TODO: handle temporary error in some meaningful way
        error = 0;
    }
}

// ----------------------------------------------------------------------------
// LogsDbConnector::deleteRemoved
// ----------------------------------------------------------------------------
//
void LogsDbConnector::deleteRemoved(int newEventCount)
{
    // Remove events which are not anymore in db nor in model,
    // such events are always at end of list
    while ( mEvents.count() > newEventCount ){
        delete mEvents.takeLast();
    }
}

// ----------------------------------------------------------------------------
// LogsDbConnector::removeCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::removeCompleted()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::removeCompleted()" )
    emit clearingCompleted(0);
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::removeCompleted()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::logsRemoveErrorOccured
// ----------------------------------------------------------------------------
//
void LogsDbConnector::logsRemoveErrorOccured(int err)
{
    LOGS_QDEBUG_2( "logs [ENG] <-> LogsDbConnector::logsRemoveErrorOccured(), err:", err )
    
    emit clearingCompleted(err);
    // TODO: error handling
    
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::logsRemoveErrorOccured()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::readCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::readCompleted(int readCount)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::readCompleted()" )
    LOGS_QDEBUG_EVENT_ARR(mEvents)
    
    // Find out updated, added and removed events
    mRemovedEventIndexes.clear();
    mUpdatedEventIndexes.clear();
    mAddedEventIndexes.clear();
    for ( int i = 0; i < mEvents.count(); i++ ){
        if ( !mEvents.at(i)->isInView() ){
            mRemovedEventIndexes.append( mEvents.at(i)->index() );
        } else if ( mEvents.at(i)->eventState() == LogsEvent::EventUpdated ) {
            mUpdatedEventIndexes.append( mEvents.at(i)->index() );
        } else if ( mEvents.at(i)->eventState() == LogsEvent::EventAdded ) {
            mAddedEventIndexes.append( mEvents.at(i)->index() );
        }
    }
    
    bool doModelDataReset( !mRemovedEventIndexes.isEmpty() ||
                           !mAddedEventIndexes.isEmpty() || 
                           !mUpdatedEventIndexes.isEmpty() );
    if ( doModelDataReset ){
        mModelEvents.clear();
        int numValidEvents = qMin(mEvents.count(), readCount);
        for ( int i = 0; i < numValidEvents; i++ ){    
            mModelEvents.append(mEvents.at(i));
        }
    }
    
    if ( !mRemovedEventIndexes.isEmpty() ){
        emit dataRemoved(mRemovedEventIndexes);
    }             
    if ( !mAddedEventIndexes.isEmpty() ){
        emit dataAdded(mAddedEventIndexes);
    }
    if ( !mUpdatedEventIndexes.isEmpty() ){
        emit dataUpdated(mUpdatedEventIndexes);
    }
    
    deleteRemoved(readCount);

    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::readCompleted()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::errorOccurred
// ----------------------------------------------------------------------------
//
void LogsDbConnector::errorOccurred(int err)
{
    LOGS_QDEBUG_2( "logs [ENG] <-> LogsDbConnector::errorOccurred(), err:", err )

    // TODO: error handling
    
    handleModifyingCompletion(err);
    
    Q_UNUSED(err)
}

// ----------------------------------------------------------------------------
// LogsDbConnector::temporaryErrorOccurred
// ----------------------------------------------------------------------------
//
void LogsDbConnector::temporaryErrorOccurred(int err)
{
    LOGS_QDEBUG_2( 
            "logs [ENG] -> LogsDbConnector::temporaryErrorOccurred(), err:", err )
    
    handleTemporaryError(err);
    
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::temporaryErrorOccurred()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::eventModifyingCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::eventModifyingCompleted()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::eventModifyingCompleted()" )
    
    if ( handleModifyingCompletion() ){
        // Possible to continue modifying
        int err = doMarkEventSeen();
        if ( err != 0 ){
            // But failed for some reason
            LOGS_QDEBUG( "logs [ENG]    Couldn't continue modifying" )
            handleModifyingCompletion(err);
        }
    }
    
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::eventModifyingCompleted()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::doMarkEventSeen
// ----------------------------------------------------------------------------
//
int LogsDbConnector::doMarkEventSeen()
{
    int err = -1;
    if ( mEventsSeen.count() > 0 ){
        err = mReader->markEventSeen(mEventsSeen.at(0));
    }
    return err;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::handleModifyingCompletion
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::handleModifyingCompletion(int err)
{
    bool continueModify( false );
    if ( err || mEventsSeen.count() == 1 ){
        mEventsSeen.clear();
        emit markingCompleted(err);
    } else if ( mEventsSeen.count() > 1 ){
        // Item was modified succesfully and more to modify, remove handled
        // item from queue
        mEventsSeen.takeFirst();
        continueModify = true;
    } else {
    
    }
    return continueModify;
}
