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
#include <logwrap.hrh>
#include <logengdurations.h>
#include <QDateTime>

#include "logseventparser.h"
#include "logsevent.h"
#include "logseventdata.h"
#include "logsengdefs.h"
#include "LogsApiConsts.h"     //Additional event UIDs
#include "logslogger.h"


// ----------------------------------------------------------------------------
// LogsEventParser::parseL
// ----------------------------------------------------------------------------
//
void LogsEventParser::parseL( 
    const CLogEvent& source, 
    LogsEvent& dest,
    const LogsEventStrings& strings )
{
    bool dataChanged = false;
    
    //Set remote party information
    QString newRemoteParty = 
        QString::fromUtf16( source.RemoteParty().Ptr(), source.RemoteParty().Length() );
    if ( !isUnknownRemoteParty(dest, newRemoteParty) && 
         newRemoteParty != strings.iUnKnownRemote &&
         source.RemoteParty() != KLogsPrivateText && 
         source.RemoteParty() != KLogsPayphoneText ){
        dest.setRemoteParty( newRemoteParty );
    }
        
    dataChanged |= dest.setNumber( 
        QString::fromUtf16( source.Number().Ptr(), source.Number().Length() ) );
    
    // Set direction
    QString sourceDirection = 
        QString::fromUtf16( 
                source.Direction().Ptr(), source.Direction().Length() );
    
    // TODO: it would be better to have symbian descriptors in strings arr so
    // that symbian log event direction could be compared without conversion
    LogsEvent::LogsDirection newDirection = LogsEvent::DirUndefined;
    
    if( sourceDirection == strings.iInDirection ||
        sourceDirection == strings.iInDirectionAlt ){
        newDirection = LogsEvent::DirIn;
    } else if( sourceDirection == strings.iOutDirection ||
               sourceDirection == strings.iOutDirectionAlt ){
        newDirection = LogsEvent::DirOut;
    } else if( sourceDirection == strings.iMissedDirection ){
        newDirection = LogsEvent::DirMissed;
    }
    dataChanged |= dest.setDirection(newDirection);
    
    dataChanged |= dest.setIsRead(source.Flags() & KLogEventRead);
    
    // Set time
    TDateTime dateTime = source.Time().DateTime();    
    QDate qDate( dateTime.Year(), dateTime.Month() + 1, dateTime.Day() + 1);
    QTime qTime( dateTime.Hour(), dateTime.Minute(), dateTime.Second() );
    QDateTime qDateTime(qDate, qTime, Qt::UTC );    
    dataChanged |= dest.setTime( qDateTime );
    
    // Set duration (do not cause update even if duration changed)
    if ( source.DurationType() == KLogDurationValid ){
        dest.setDuration(source.Duration());
    }
    
    int currLogId = dest.logId();
    int newLogId = source.Id();
    bool logIdChanged( currLogId != newLogId );
    if ( currLogId < 0 || logIdChanged ){
        dest.mEventState = LogsEvent::EventAdded;
    } else if ( !logIdChanged && dataChanged ){       
        dest.mEventState = LogsEvent::EventUpdated;
    } else {
        dest.mEventState = LogsEvent::EventNotUpdated;
    }
    dest.setLogId( newLogId ); // Store unique identifier
    
    LogsEventData* logsEventData = new LogsEventData();
    if ( logsEventData->parse(source) != 0 ){
        // Parsing failed, clear event data
        delete logsEventData;
        dest.setLogsEventData( 0 );
    } else {
        dest.setLogsEventData( logsEventData );  //Ownership transferred
    }
    
    // Resolve event type based on current event data
    resolveEventType(dest);
    
    eventTypeSpecificParsing(dest);
    
    dest.setDuplicates( 0 );
    
    if ( dest.validate() ) {
        dest.setIsInView(true); // Important
    }
}

// ----------------------------------------------------------------------------
// LogsEventParser::resolveEventType
// ----------------------------------------------------------------------------
//
void LogsEventParser::resolveEventType(LogsEvent& dest)
{
    LogsEvent::LogsEventType type( LogsEvent::TypeVoiceCall );
    if ( dest.mLogsEventData ){
        if ( dest.mLogsEventData->isVoIP() ){
            type = LogsEvent::TypeVoIPCall;
        } else if ( dest.mLogsEventData->isVT() ){
            type = LogsEvent::TypeVideoCall;
        } else if ( dest.mLogsEventData->isPoC() ){
            // PoC not supported at the moment
            type = LogsEvent::TypeUndefined;
        } else if ( dest.mLogsEventData->msgPartsNumber() > 0 ){
            // Messages not supported at the moment
            type = LogsEvent::TypeUndefined;
        }
    }
    LOGS_QDEBUG_2( "LogsEventParser::resolveEventType, type:", type )
    dest.setEventType( type );
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsEventParser::eventTypeSpecificParsing(LogsEvent& dest)
{
    if ( isUnknownRemoteParty(dest, dest.mRemoteParty) ) {
        // Detect VoIP's strange unkown remote party handling
        // and clear remote party value to treat unknown voip event
        // same way as any other unknown event.
        dest.mRemoteParty.clear();
    }
}
    
// ----------------------------------------------------------------------------
// VoIP uses remote url as remote party name in case of 
// unknown remote party (probably because DB has to have some identifier
// in number or remote party fields).
// ----------------------------------------------------------------------------
//
bool LogsEventParser::isUnknownRemoteParty(LogsEvent& dest, const QString& remoteParty)
{
     if ( dest.mEventType == LogsEvent::TypeVoIPCall && 
          dest.mLogsEventData && dest.mLogsEventData->remoteUrl() == remoteParty ){
          return true;
     }
     return false;
}

// End of file

