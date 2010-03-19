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
#include "logsdbconnector_stub_helper.h"
#include "logsevent.h"

// CONSTANTS

QString logsLastCalledFunction = "";

void LogsDbConnectorStubHelper::reset()
{
    logsLastCalledFunction = "";
}

QString LogsDbConnectorStubHelper::lastCalledFunction()
{
    return logsLastCalledFunction;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::LogsDbConnector
// ----------------------------------------------------------------------------
//
LogsDbConnector::LogsDbConnector( QList<LogsEvent*>& events, bool checkAllEvents ) 
: QObject(), mModelEvents( events ), mCheckAllEvents( checkAllEvents )
{
}

// ----------------------------------------------------------------------------
// LogsDbConnector::~LogsDbConnector
// ----------------------------------------------------------------------------
//
LogsDbConnector::~LogsDbConnector()
{
    while ( !mEvents.isEmpty() ){
        delete mEvents.takeFirst();
    }
}

// ----------------------------------------------------------------------------
// LogsDbConnector::init
// ----------------------------------------------------------------------------
//
int LogsDbConnector::init()
{
    logsLastCalledFunction = "init";
    return 0;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::start
// ----------------------------------------------------------------------------
//
int LogsDbConnector::start()
{
    logsLastCalledFunction = "start";
    return 0;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::updateDetails
// ----------------------------------------------------------------------------
//
int LogsDbConnector::updateDetails(bool /*clearCached*/)
{
    logsLastCalledFunction = "updateDetails";
    return 0;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::clearList
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::clearList(LogsModel::ClearType /*cleartype*/)
{
    logsLastCalledFunction = "clearList";
    return true;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::clearMissedCallsCounter
// ----------------------------------------------------------------------------
//
int LogsDbConnector::clearMissedCallsCounter()
{
    logsLastCalledFunction = "clearMissedCallsCounter";
    return 0;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::clearEvents
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::clearEvents(const QList<int>& /*ids*/)
{    
    logsLastCalledFunction = "clearEvents";
    return true;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::markEventsSeen
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::markEventsSeen(const QList<int>& ids)
{
    bool started(false);
    logsLastCalledFunction = "markEventsSeen";
    foreach( int currId, ids ){
        if ( !mEventsSeen.contains(currId) ){
            mEventsSeen.append(currId);
            started = true;
        }
    }
    return started;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::readCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::readCompleted(int /*numRead*/)
{
}

// ----------------------------------------------------------------------------
// LogsDbConnector::errorOccurred
// ----------------------------------------------------------------------------
//
void LogsDbConnector::errorOccurred(int err)
{
    Q_UNUSED(err)
}

// ----------------------------------------------------------------------------
// LogsDbConnector::readCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::removeCompleted()
{
}

// ----------------------------------------------------------------------------
// LogsDbConnector::errorOccurred
// ----------------------------------------------------------------------------
//
void LogsDbConnector::logsRemoveErrorOccured(int err)
{
    Q_UNUSED(err)
}

// ----------------------------------------------------------------------------
// LogsDbConnector::temporaryErrorOccurred
// ----------------------------------------------------------------------------
//
void LogsDbConnector::temporaryErrorOccurred(int err)
{
    Q_UNUSED(err)
}

// ----------------------------------------------------------------------------
// LogsDbConnector::eventModifyingCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::eventModifyingCompleted()
{

}

// ----------------------------------------------------------------------------
// LogsDbConnector::doMarkEventSeen
// ----------------------------------------------------------------------------
//
int LogsDbConnector::doMarkEventSeen()
{
    return 0;
}
