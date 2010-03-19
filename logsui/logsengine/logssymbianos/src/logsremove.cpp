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

//USER
#include "logsremove.h"
#include "logsmodel.h"
#include "logslogger.h"
#include "logsremoveobserver.h"
#include <logcli.h>
#include <f32file.h>
#include <logview.h>
//SYSTEM

// CONSTANTS
_LIT( KMaxLogsTime, "99991130:235959.999999");

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsRemove::LogsRemove( LogsRemoveObserver& observer, bool readingAllEvents )
: CActive(EPriorityStandard),
  mObserver(observer),
  mReadingAllEvents(readingAllEvents),
  mLogClient(0),
  mRecentView(0)
{
	CActiveScheduler::Add( this );
    mFsSession = new RFs();
}

// -----------------------------------------------------------------------------
// LogsRemove::~LogsRemove
// -----------------------------------------------------------------------------
//
LogsRemove::~LogsRemove()
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsRemove::~LogsRemove()" )
    Cancel();
    delete mRecentView;
    delete mLogClient;
    if ( mFsSession ){
        mFsSession->Close();
    }
    delete mFsSession;
}

// ----------------------------------------------------------------------------
// LogsRemove::RunL
// ----------------------------------------------------------------------------
//
void LogsRemove::RunL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsRemove::RunL()->" )
   if (iStatus.Int() == KErrNone){
    	LOGS_QDEBUG( "logs [ENG] -> LogsRemove::RunL() KErrNone" )
    	if ( !DeleteNextEvent() ){
            mObserver.removeCompleted();
    	}
    }
    else {
    	User::Leave( iStatus.Int() );
    	LOGS_QDEBUG( "logs [ENG] -> LogsRemove::RunL() !KErrNone" )
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsRemove::RunL()<-" )
}

// ----------------------------------------------------------------------------
// LogsRemove::DoCancel
// ----------------------------------------------------------------------------
//
void LogsRemove::DoCancel()
{
	if ( mLogClient ){
        mLogClient->Cancel();
    }
}

// ----------------------------------------------------------------------------
// LogsRemove::RunError
// ----------------------------------------------------------------------------
//
TInt LogsRemove::RunError(TInt aError)
{
	mObserver.logsRemoveErrorOccured(aError);
	return KErrNone;	
}


// ----------------------------------------------------------------------------
// LogsRemove::init
// ----------------------------------------------------------------------------
//
int LogsRemove::init()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsRemove::init()" )    
    TRAPD( err, initL() );
    LOGS_QDEBUG_2( "logs [ENG] <- LogsRemove::init(), err:", err )    
    return err;
}


// ----------------------------------------------------------------------------
// LogsRemove::clearList
// ----------------------------------------------------------------------------
//
bool LogsRemove::clearList(LogsModel::ClearType cleartype)
{
    LOGS_QDEBUG( "logs [ENG] <- LogsRemove::clearList->" )
    if ( init() != KErrNone ) {
        return false;
    }
    bool clearingStarted(false);
    TTime time( KMaxLogsTime );
    if ( !IsActive() ){
        if (cleartype == LogsModel::TypeLogsClearAll){
            mLogClient->ClearLog( time, iStatus );
        }
        else{
            mLogClient->ClearLog( cleartype, iStatus );
        }
        SetActive();  
        clearingStarted = true;
    } else {
    }
    LOGS_QDEBUG_2( "logs [ENG] <- LogsRemove::clearList, started", clearingStarted )
    return clearingStarted;
}

// ----------------------------------------------------------------------------
// LogsRemove::initL
// ----------------------------------------------------------------------------
//
void LogsRemove::initL()
{
    if ( mLogClient ){
        // Already initialized
        return;
    }  
    User::LeaveIfError( mFsSession->Connect() );
    mLogClient = CLogClient::NewL( *mFsSession );
}


// ----------------------------------------------------------------------------
// LogsRemove::clearEvents
// ----------------------------------------------------------------------------
//
int LogsRemove::clearEvents(const QList<int>& eventIds, bool& async)
{
    TRAPD( err, clearEventsL(eventIds, async) );
    return err;
}
    
// ----------------------------------------------------------------------------
// LogsRemove::clearEventsL
// ----------------------------------------------------------------------------
//
void LogsRemove::clearEventsL(const QList<int>& eventIds, bool& async)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsRemove::clearEventL()")

    async = false;
    
    initL();
    
    if ( mReadingAllEvents ){
        // When all events are read, recent view cannot be used for 
        // event removal.
        Cancel();
        mRemovedEvents = eventIds;
        async = DeleteNextEvent();
    } else {
        if ( !mRecentView ) {
            mRecentView = CLogViewRecent::NewL( *mLogClient );
        }
        foreach( int currId, eventIds ){
            LOGS_QDEBUG_2( "logs [ENG]  Removing, currid: ", currId )
            mRecentView->RemoveL( currId );
        }
        delete mRecentView;
        mRecentView = NULL;
    }
    
    LOGS_QDEBUG_2( "logs [ENG] <- LogsRemove::clearEventL(): async", async ) 
}

// ----------------------------------------------------------------------------
// LogsRemove::DeleteNextEvent
// ----------------------------------------------------------------------------
//
bool LogsRemove::DeleteNextEvent()
{
    bool deleting(false);
    if ( !mRemovedEvents.isEmpty() ){
        int currId = mRemovedEvents.takeFirst();
        LOGS_QDEBUG_2( "logs [ENG]  LogsRemove::DeleteNextEvent, id: ", currId )
        mLogClient->DeleteEvent( currId, iStatus );
        SetActive();
        deleting = true;
    }
    return deleting;  
}

// End of file

