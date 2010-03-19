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
#include "logsmessage.h"
#include "logslogger.h"
#include "logseventdata.h"

//SYSTEM

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsMessage::LogsMessage(LogsEvent& event)
    :QObject(), mIsAllowed( false ), mContactId( 0 )
{
    if ( event.logsEventData() && !event.logsEventData()->isCsCompatible() ){
        LOGS_QDEBUG( "logs [ENG]    LogsMessage::LogsMessage, not CS compatible" )
        mIsAllowed = false;
    } else {
        mIsAllowed = true;
        mNumber = event.getNumberForCalling();
        mContactId = event.contactLocalId();
    }
    
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsMessage::LogsMessage(unsigned int contactId, const QString& number)
    :QObject(), mIsAllowed( false ), mContactId( 0 )
{
    if ( number.length() == 0 ){
        LOGS_QDEBUG( "logs [ENG]    LogsMessage::LogsMessage, not CS compatible" )
        mIsAllowed = false;
    } else {
        mIsAllowed = true;
        mNumber = number;
        mContactId = contactId; 
    }
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsMessage::~LogsMessage()
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsMessage::~LogsMessage()" )
}
    
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
bool LogsMessage::isMessagingAllowed()
{
    return mIsAllowed;
}    

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsMessage::sendMessage()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMessage::sendMessage()" )
    
    // TODO: sending not possible at the moment
    
    LOGS_QDEBUG( "logs [ENG] <- LogsMessage::sendMessage()" )
    
    return false;
}

// End of file

