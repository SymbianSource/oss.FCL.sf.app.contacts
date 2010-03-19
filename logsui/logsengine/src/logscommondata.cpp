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
#include "logscommondata.h"
#include "logslogger.h"

//SYSTEM
#include <qcontactmanager.h>


static LogsCommonData* mLogsCommonInstance = 0;
 
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsCommonData::LogsCommonData() : 
    mContactManager(0), mMaxReadSize(-1), mMaxReadSizeDir(LogsEvent::DirUndefined)
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsCommonData::LogsCommonData()" )
}
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsCommonData::~LogsCommonData()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsCommonData::~LogsCommonData()" )
    delete mContactManager;
    LOGS_QDEBUG( "logs [ENG] <- LogsCommonData::~LogsCommonData()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsCommonData& LogsCommonData::getInstance() 
{ 
    if ( !mLogsCommonInstance ){
        mLogsCommonInstance = new LogsCommonData;
    }
    return *mLogsCommonInstance; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsCommonData::freeCommonData()
{
    delete mLogsCommonInstance;
    mLogsCommonInstance = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QContactManager& LogsCommonData::contactManager()
{
    if (!mContactManager){
        LOGS_QDEBUG( "logs [ENG] -> LogsCommonData::contactManager(), create mgr" )
        mContactManager = new QContactManager("symbian");
        LOGS_QDEBUG( "logs [ENG] <- LogsCommonData::contactManager()" )
    }
    return *mContactManager;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsCommonData::configureReadSize(int maxSize, LogsEvent::LogsDirection dir)
{
    mMaxReadSize = maxSize;
    mMaxReadSizeDir = dir;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsCommonData::maxReadSize() const
{
    return mMaxReadSize;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsEvent::LogsDirection LogsCommonData::maxReadSizeDirection() const
{
    return mMaxReadSizeDir;
}

// End of file

