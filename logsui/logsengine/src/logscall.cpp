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
#include "logscall.h"
#include "logsmodel.h"
#include "logsevent.h"
#include "logslogger.h"
#include "logseventdata.h"

//SYSTEM
#include <QVariant>
#include <xqservicerequest.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsCall::LogsCall(LogsEvent& event) 
    : QObject(), mDefaultCall( TypeLogsCallNotAvailable )
{
    if ( event.logsEventData() ) {
        mServiceId = event.logsEventData()->serviceId();
    }

    if (event.eventType() == LogsEvent::TypeVoIPCall ) {
        mCalls.append(LogsCall::TypeLogsVoIPCall);
        if ( event.logsEventData() && event.logsEventData()->isCsCompatible()){
            mCalls.append(LogsCall::TypeLogsVoiceCall);
            mCalls.append(LogsCall::TypeLogsVideoCall);
        }
    } else if (event.eventType() == LogsEvent::TypeVoiceCall || 
               event.eventType() == LogsEvent::TypeVideoCall ) {
        mCalls.append(LogsCall::TypeLogsVoiceCall);
        mCalls.append(LogsCall::TypeLogsVideoCall);
    }
    
    if (event.eventType() == LogsEvent::TypeVoiceCall){
        mDefaultCall = TypeLogsVoiceCall;
    } else if (event.eventType() == LogsEvent::TypeVideoCall) {
        mDefaultCall = TypeLogsVideoCall;
    } else if (event.eventType() == LogsEvent::TypeVoIPCall) {
        mDefaultCall = TypeLogsVoIPCall;
    }
    mNumber = event.getNumberForCalling();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsCall::LogsCall(unsigned int contactId, const QString& number) 
    : QObject(), mDefaultCall( TypeLogsCallNotAvailable ), mServiceId(0)
{
    Q_UNUSED(contactId);
    mNumber = number;
    
    if ( mNumber.length() > 0 ) {
        mCalls.append(LogsCall::TypeLogsVoiceCall);
        mCalls.append(LogsCall::TypeLogsVideoCall);
        mDefaultCall = TypeLogsVoiceCall;
    }
}

// -----------------------------------------------------------------------------
// LogsCall::~LogsCall
// -----------------------------------------------------------------------------
//
LogsCall::~LogsCall()
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsCall::~LogsCall()" )
}

// ----------------------------------------------------------------------------
// LogsCall::isAllowedCallType
// ----------------------------------------------------------------------------
//
bool LogsCall::isAllowedCallType()
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsCall::isAllowedCallType()" )
    return (mCalls.count() > 0);
}
    
// ----------------------------------------------------------------------------
// LogsCall::allowedCallTypes
// Voip call not possible at the moment. However, if voip event is CS
// compatible, voice and video call are allowed.
// ----------------------------------------------------------------------------
//
QList<LogsCall::CallType> LogsCall::allowedCallTypes()
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsCall::allowedCallTypes()" )
    
    return mCalls;
}    

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
LogsCall::CallType LogsCall::defaultCallType()
{
    return mDefaultCall;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsCall::callToNumber(LogsCall::CallType callType, const QString& number,
        unsigned int serviceId)
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsCall::callToNumber(), type", callType )
        
    if (callType == TypeLogsVoiceCall) {
        createcall("com.nokia.services.telephony", "dial(QString)", number, false);
    } else if (callType == TypeLogsVideoCall) {
        createcall("com.nokia.services.telephony", "dialVideo(QString)", number, false);
    } else if (callType == TypeLogsVoIPCall){
        if ( serviceId ){
        
            // Service id should(?) be used with every call type. If logs entry
            // is from xsp with video call support, then the executed
            // video call should be mapped to the right xsp?
            // also contact must be passed if available if change service is
            // provided (no point change service and try to call service specific
            // uri with another service)?
            createCallWithService("com.nokia.services.telephony", 
                "dialVoipService(QString,int)", number, false, serviceId);
        }
        else {
            // This is likely to fail as long as telephony does not
            // offer any kind of service selection. Normally voip call
            // should always have service id set but if it's missing
            // for some reason, then this provides call failure UI.
            createcall("com.nokia.services.telephony", 
                "dialVoip(QString)", number, false);
        }
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsCall::callToNumber()" )
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsCall::call(LogsCall::CallType callType)
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsCall::call(), type", callType )
    
    callToNumber(callType, mNumber, mServiceId);
    
    LOGS_QDEBUG( "logs [ENG] <- LogsCall::call()" )
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsCall::initiateCallback()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsCall::initiateCallback()" )
    
    if (mDefaultCall != TypeLogsCallNotAvailable ){
        call(mDefaultCall);
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsCall::initiateCallback()" )
}

// ----------------------------------------------------------------------------
// LogsCall::createcall
// ----------------------------------------------------------------------------
//
void LogsCall::createcall(QString service, QString type, QString num, bool sync)
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsCall::createcall(), num", num )
    XQServiceRequest snd(service, type, sync);
    snd << num;
    QVariant retValue;
    snd.send(retValue);
    LOGS_QDEBUG( "logs [ENG] <- LogsCall::createcall()" )
}

// ----------------------------------------------------------------------------
// LogsCall::createCallWithService
// ----------------------------------------------------------------------------
//
void LogsCall::createCallWithService(QString service, QString type, QString num, 
        bool sync, unsigned int serviceId )
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsCall::createCallWithService(), num", num )
    LOGS_QDEBUG_2( "logs [ENG] -> LogsCall::createCallWithService(), num", serviceId )
    XQServiceRequest snd(service, type, sync);
    snd << num << serviceId;
    QVariant retValue;
    snd.send(retValue);
    LOGS_QDEBUG( "logs [ENG] <- LogsCall::createCallWithService()" )
}

// End of file

