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
#include <xqservicerequest.h>

//SYSTEM

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsMessage::LogsMessage(LogsEvent& event)
    :QObject(), mIsAllowed( false ), mContactId( 0 ), mService( 0 )
{
    if ( event.logsEventData() && !event.logsEventData()->isCsCompatible() ){
        LOGS_QDEBUG( "logs [ENG]    LogsMessage::LogsMessage, not CS compatible" )
        mIsAllowed = false;
    } else {
        mIsAllowed = true;
        mNumber = event.getNumberForCalling();
        mContactId = event.contactLocalId();
        mDisplayName = event.remoteParty();
    }
    
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsMessage::LogsMessage(unsigned int contactId, const QString& number,
	 const QString& displayName)
    :QObject(), mIsAllowed( false ), mContactId( 0 ), mService( 0 )
{
    if ( number.length() == 0 ){
        LOGS_QDEBUG( "logs [ENG]    LogsMessage::LogsMessage, not CS compatible" )
        mIsAllowed = false;
    } else {
        mIsAllowed = true;
        mNumber = number;
        mContactId = contactId;
        mDisplayName = displayName;
    }
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsMessage::~LogsMessage()
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsMessage::~LogsMessage()" )
    delete mService;
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
    
    delete mService;
    mService = 0;
    mService = new XQServiceRequest("com.nokia.services.hbserviceprovider.conversationview", 
                                    "send(QString,qint32,QString)", false);
    bool sending = doSendMessageToNumber(*mService, mNumber, mDisplayName, mContactId);
    connect(mService, SIGNAL(requestCompleted(QVariant)), this, SLOT(requestCompleted(QVariant)));
    connect(mService, SIGNAL(requestError(int)), this, SLOT(requestError(int)));
    return sending;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsMessage::sendMessageToNumber(
        const QString& number, const QString& displayName, unsigned int contactId)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMessage::sendMessageToNumber()" )
    
    XQServiceRequest req("com.nokia.services.hbserviceprovider.conversationview", 
                         "send(QString,qint32,QString)", false);
    return doSendMessageToNumber(req, number, displayName, contactId);
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsMessage::requestCompleted(const QVariant& /*value*/)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMessage::requestCompleted()" )
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsMessage::requestError(int /*err*/)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMessage::requestError()" )
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsMessage::doSendMessageToNumber(
        XQServiceRequest& request, const QString& number, 
        const QString& displayName, unsigned int contactId)
{
    LOGS_QDEBUG_4( "logs [ENG] -> LogsMessage::doSendMessageToNumber(), (num, name, id)", 
            number, displayName, contactId )

    QList<QVariant> arguments;
    arguments.append(QVariant(number));
    arguments.append(QVariant(contactId));
    arguments.append(QVariant(displayName));
    request.setArguments(arguments);
    QVariant retValue;
    bool ret = request.send(retValue);
    LOGS_QDEBUG_2( "logs [ENG] <- LogsMessage::doSendMessageToNumber()", ret )
    
    return ret;
}

// End of file

