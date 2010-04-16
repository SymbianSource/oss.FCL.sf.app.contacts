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
#include "logscontact.h"
#include "logsmodel.h"
#include "logsevent.h"
#include "logslogger.h"
#include "logseventdata.h"
#include "logsdbconnector.h"
#include "logscommondata.h"

//SYSTEM
#include <QVariant>
#include <xqservicerequest.h>
#include <qcontactmanager.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsContact::LogsContact(LogsEvent& event, LogsDbConnector& dbConnector)
  :  QObject(), 
     mDbConnector(dbConnector),
     mService(0),
     mCurrentRequest(TypeLogsContactSave),
     mContactId(0),
     mSaveAsOnlineAccount(false)
{
    mNumber = event.getNumberForCalling();
    mContactId = event.contactLocalId();
    mContact = contact();
    if ( event.eventType() == LogsEvent::TypeVoIPCall &&
         event.logsEventData() && 
         !event.logsEventData()->remoteUrl().isEmpty() ) {
         mSaveAsOnlineAccount = true;
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsContact::LogsContact(const QString& number, 
                         LogsDbConnector& dbConnector,
                         unsigned int contactId)
  :  QObject(), 
     mDbConnector(dbConnector),
     mService(0),
     mCurrentRequest(TypeLogsContactSave),
     mNumber(number),
     mContactId(contactId),
     mSaveAsOnlineAccount(false)
{
    mContact = contact();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsContact::~LogsContact()
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsContact::~LogsContact()" )
    delete mService;
}
    
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
LogsContact::RequestType LogsContact::allowedRequestType()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsContact::allowedRequestType()" )
    LogsContact::RequestType type = TypeLogsContactSave;
    
    if ( isContactInPhonebook() ) {
        type = TypeLogsContactOpen;
    }
    
    LOGS_QDEBUG_2( "logs [ENG] <- LogsContact::allowedRequestType(): ", type )
    return type;
}

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
bool LogsContact::isContactRequestAllowed()
{
    return ( isContactInPhonebook() || !mNumber.isEmpty() );
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::open()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsContact::open()")
    bool ret = false;
    if ( allowedRequestType() == TypeLogsContactOpen ) {
        mCurrentRequest = TypeLogsContactOpen;

        QList<QVariant> arguments;
        arguments.append( QVariant(mContactId) );
        ret = requestFetchService( "open(int)", arguments );
    }
    
    LOGS_QDEBUG_2( "logs [ENG] <- LogsContact::open(): ", ret )
    return ret;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::addNew()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsContact::save()" )
            
    bool ret = save("editCreateNew(QString,QString)");
    
    LOGS_QDEBUG_2( "logs [ENG] <- LogsContact::save(): ", ret )
    return ret;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::updateExisting()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsContact::updateExisting()" )
    
    bool ret = save("editUpdateExisting(QString,QString)");
    
    LOGS_QDEBUG( "logs [ENG] <- LogsContact::updateExisting()" )
    return ret;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::save(QString message)
{
    QList<QVariant> arguments;
 
    if ( !mNumber.isEmpty() ) {
        if ( mSaveAsOnlineAccount ){
            QString type = QContactOnlineAccount::DefinitionName;
            arguments.append( QVariant(type) );
        } else {
            QString type = QContactPhoneNumber::DefinitionName;
            arguments.append( QVariant(type) );
        }     
        arguments.append( QVariant(mNumber) );
    }
    
    bool ret(false);
    
    if ( arguments.count() == 2 ) {
        mCurrentRequest = TypeLogsContactSave;
        ret = requestFetchService( message, arguments );
    } else {
        LOGS_QDEBUG( "logs [ENG]  !No Caller ID, not saving the contact..")
    }
    
    return ret;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::requestFetchService( QString message, 
        const QList<QVariant> &arguments, bool sync )
{
    QString service("com.nokia.services.phonebookservices.Fetch");  

    delete mService;
    mService = 0;
    mService = new XQServiceRequest(service, message, sync);
    connect(mService, SIGNAL(requestCompleted(QVariant)), this, 
            SLOT(handleRequestCompleted(QVariant)));

    mService->setArguments(arguments);

    QVariant retValue;
    return mService->send(retValue);
}

// ----------------------------------------------------------------------------
// Phonebookservices define following return values:
// - contact wasn't modified (-2)
// - was deleted (-1)
// - was saved (1)
// - saving failed (0)
// ----------------------------------------------------------------------------
//
void LogsContact::handleRequestCompleted(const QVariant& result)
{
    bool retValOk = false;
    int serviceRetVal = result.toInt(&retValOk);
    LOGS_QDEBUG_3( "logs [ENG] -> LogsContact::handleRequestCompleted(), (retval, is_ok)", 
                   serviceRetVal, retValOk )
    
    bool modified = ( retValOk && serviceRetVal == 1 );
    
    //If existing contact was modified, cached match for the contact should be 
    //cleaned up, since remote party info might have been changed.
    //However, if remote party info is taken from symbian DB, it won't be 
    //updated
    bool clearCached = ( mCurrentRequest == TypeLogsContactOpen );
    if ( modified ) {
        mContact = contact();
        mDbConnector.updateDetails(clearCached);
    }
    
    if ( mCurrentRequest == TypeLogsContactOpen ) {
        emit openCompleted(modified);
    } else if ( mCurrentRequest == TypeLogsContactSave ) {
        emit saveCompleted(modified);
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
QContact LogsContact::contact()
{
    if ( mContactId ) {     
        return LogsCommonData::getInstance().contactManager().contact( mContactId ); 
    } else {
        return QContact();
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::isContactInPhonebook()
{
    QContactLocalId localId = mContactId;    
    return ( localId && localId == mContact.localId() );
}

// End of file

