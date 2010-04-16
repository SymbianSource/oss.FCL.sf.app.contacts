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

#include <QObject>
#include <qcontactmanager.h>
#include <qcontactdetailfilter.h>
#include <qcontactphonenumber.h>
#include <qcontactname.h>
#include <qcontactonlineaccount.h>
#include <hbglobal.h>
#include "logsevent.h"
#include "logseventparser.h"
#include "logseventdata.h"
#include "logsengdefs.h"
#include "logslogger.h"

// CONSTANTS

// ----------------------------------------------------------------------------
// logsEvent::logsEvent
// ----------------------------------------------------------------------------
//
LogsEvent::LogsEvent() 
 :  mLogId(-1),
    mDirection(DirUndefined),
    mEventType(TypeUndefined),
    mUid(0),
    mLogsEventData(0),
    mDuplicates(0),                
    mRingDuration(0),
    mIsRead(false),
    mIsALS(false),
    mDuration(0),
    mIndex(0),
    mIsInView(false),
    mEventState(EventAdded),
    mIsLocallySeen(false)
{
}

// ----------------------------------------------------------------------------
// LogsEvent::LogsEvent
// ----------------------------------------------------------------------------
//
LogsEvent::LogsEvent( const LogsEvent& event )
{
    mLogId = event.mLogId;        
    mDirection = event.mDirection;
    mEventType = event.mEventType;
    mUid = event.mUid;
    if ( event.mLogsEventData ){
        mLogsEventData = new LogsEventData( *event.mLogsEventData );
    } else {
        mLogsEventData = 0;
    }
    mRemoteParty = event.mRemoteParty;
    mNumber = event.mNumber;
    mDuplicates = event.mDuplicates;               
    mTime = event.mTime;       
    mRingDuration = event.mRingDuration;
    mIsRead = event.mIsRead;  
    mIsALS = event.mIsALS;
    mDuration = event.mDuration;
    
    mIndex = event.mIndex;
    mIsInView = event.mIsInView;
    mEventState = event.mEventState;
    mIsLocallySeen = event.mIsLocallySeen;
}

// ----------------------------------------------------------------------------
// LogsEvent::~LogsEvent
// ----------------------------------------------------------------------------
//
LogsEvent::~LogsEvent() 
{
    delete mLogsEventData;
}

// ----------------------------------------------------------------------------
// LogsEvent::initializeEventL
// ----------------------------------------------------------------------------
//
void LogsEvent::initializeEventL( 
    const CLogEvent& source, 
    const LogsEventStrings& strings )
{
    LogsEventParser::parseL(source, *this, strings);
    LOGS_QDEBUG_5( "LogsEvent::initializeEvent (num,dir,logid,state):", 
                   mNumber, mDirection, mLogId, mEventState )
}
    

// ----------------------------------------------------------------------------
// LogsEvent::isEmergencyNumber
// Checks wether the number is an emergency number
// ----------------------------------------------------------------------------

bool LogsEvent::isEmergencyNumber(const QString& number)
{
    return ( number == logsEmergencyCall911 || number == logsEmergencyCall );             
}

// ----------------------------------------------------------------------------
// LogsEvent::setDuration()
// ----------------------------------------------------------------------------
//
void LogsEvent::setDuration( int duration )
{
    mDuration = duration;
}

// ----------------------------------------------------------------------------
// LogsEvent::setALS()
// ----------------------------------------------------------------------------
//
void LogsEvent::setALS( bool isALS ) 
{
	mIsALS = isALS;	
}

// ----------------------------------------------------------------------------
// LogsEvent::setLogId
// ----------------------------------------------------------------------------
//
void LogsEvent::setLogId( int logId )
{
    mLogId = logId;
}

// ----------------------------------------------------------------------------
// LogsEvent::setNumber
// ----------------------------------------------------------------------------
//
bool LogsEvent::setNumber( const QString& number )
    {
    bool changed( mNumber != number );
    mNumber = number;
    return changed;
    }

// ----------------------------------------------------------------------------
// LogsEvent::setRemoteParty
// ----------------------------------------------------------------------------
//
void LogsEvent::setRemoteParty( const QString& remoteParty )
{
    if ( mEventState == EventNotUpdated && mRemoteParty != remoteParty ){
        LOGS_QDEBUG( "logs [ENG] <-> LogsEvent::setRemoteParty, event updated")
        mEventState = LogsEvent::EventUpdated;
    }
    mRemoteParty = remoteParty;
}

// ----------------------------------------------------------------------------
// LogsEvent::validate
// ----------------------------------------------------------------------------
//
bool LogsEvent::validate()
{
    return ( !mNumber.isEmpty() || !mRemoteParty.isEmpty() || 
           ( mLogsEventData && !mLogsEventData->remoteUrl().isEmpty() ) );
}

// ----------------------------------------------------------------------------
// LogsEvent::directionAsString
// ----------------------------------------------------------------------------
//
QString LogsEvent::directionAsString() const
{
    QString dir;
    if ( mDirection == DirIn ) {
        dir = hbTrId("txt_dialer_ui_dblist_call_direction_val_received");
    } else if ( mDirection == DirOut ) {
        dir = hbTrId("txt_dialer_ui_dblist_call_direction_val_dialled");
    } else if ( mDirection == DirMissed ) {
        dir = hbTrId("txt_dialer_ui_dblist_call_direction_val_missed");
    } else {
        dir = QObject::tr("Direction unknown");
    }
    return dir;
}

// ----------------------------------------------------------------------------
// LogsEvent::typeAsString
// ----------------------------------------------------------------------------
//
QString LogsEvent::typeAsString() const
{
    QString type;
    if ( mEventType == TypeVoiceCall ) {
        type = hbTrId("txt_dialer_ui_dblist_call_type_val_voice_call");
    } else if ( mEventType == TypeVideoCall ) {
        type = hbTrId("txt_dialer_ui_dblist_call_type_val_video_call");
    } else if ( mEventType == TypeVoIPCall ) {
        type = hbTrId("txt_dialer_ui_dblist_call_type_val_voip_call");
    } else {
        type = QObject::tr("Type unknown");
    }
    return type;
}

// ----------------------------------------------------------------------------
// LogsEvent::getNumberForCalling
// ----------------------------------------------------------------------------
//
QString LogsEvent::getNumberForCalling()
{
    if (mNumber.isEmpty() && mEventType == TypeVoIPCall && mLogsEventData ) {
        return mLogsEventData->remoteUrl();
    }
    return mNumber;
}

// ----------------------------------------------------------------------------
// LogsEvent::setDirection
// ----------------------------------------------------------------------------
//
bool LogsEvent::setDirection( LogsDirection direction )
{
    bool changed( mDirection != direction );
    mDirection = direction;
    return changed;
}

// ----------------------------------------------------------------------------
// LogsEvent::setEventUid
// ----------------------------------------------------------------------------
//
void LogsEvent::setEventUid( int uid )
{
    mUid = uid;
}

// ----------------------------------------------------------------------------
// LogsEvent::setEventType
// ----------------------------------------------------------------------------
//
void LogsEvent::setEventType( LogsEventType eventType )
{
    mEventType = eventType;
}

// ----------------------------------------------------------------------------
// LogsEvent::ALS()
// ----------------------------------------------------------------------------
//
bool LogsEvent::ALS() const
{
	return mIsALS;
}
	
// ----------------------------------------------------------------------------
// LogsEvent::logId
// ----------------------------------------------------------------------------
//
int LogsEvent::logId() const
{
    return mLogId;
}

// ----------------------------------------------------------------------------
// LogsEvent::setIsRead
// ----------------------------------------------------------------------------    
bool LogsEvent::setIsRead(bool isRead)
{
    bool changed( mIsRead != isRead );
    mIsRead = isRead;
    return changed;
}


// ----------------------------------------------------------------------------
// LogsEvent::Number
// ----------------------------------------------------------------------------
//
const QString& LogsEvent::number() const
{
    return mNumber;
}

// ----------------------------------------------------------------------------
// LogsEvent::RemoteParty
// ----------------------------------------------------------------------------
//
const QString& LogsEvent::remoteParty() const
{
    return mRemoteParty;
}

// ----------------------------------------------------------------------------
// LogsEvent::Direction
// ----------------------------------------------------------------------------
//
LogsEvent::LogsDirection LogsEvent::direction() const
{
    return mDirection;
}

// ----------------------------------------------------------------------------
// LogsEvent::eventUid
// ----------------------------------------------------------------------------
//
int LogsEvent::eventUid() const
{
    return mUid;
}

// ----------------------------------------------------------------------------
// LogsEvent::eventType
// ----------------------------------------------------------------------------
//
LogsEvent::LogsEventType LogsEvent::eventType() const
{
    return mEventType;
}

// ----------------------------------------------------------------------------
// LogsEvent::isRead
// ----------------------------------------------------------------------------
 bool LogsEvent::isRead() const
{
	return mIsRead;
}

// ----------------------------------------------------------------------------
// LogsEvent::RingDuration
//
// For ring duation feature
// ----------------------------------------------------------------------------
//
int LogsEvent::ringDuration() const
{
    return mRingDuration;
}

// ----------------------------------------------------------------------------
// LogsEvent::duration
// ----------------------------------------------------------------------------
//
int LogsEvent::duration() const
{
    return mDuration;
}

// ----------------------------------------------------------------------------
// LogsEvent::SetRingDuration
// ----------------------------------------------------------------------------
//
void LogsEvent::setRingDuration( int ringDuration )
{
    mRingDuration = ringDuration;
}

// ----------------------------------------------------------------------------
// LogsEvent::LogsEventData
// ----------------------------------------------------------------------------
//
LogsEventData* LogsEvent::logsEventData() const
{
    return mLogsEventData;
}

// ----------------------------------------------------------------------------
// LogsEvent::setLogsEventData
// ----------------------------------------------------------------------------
//
void LogsEvent::setLogsEventData( LogsEventData* logsEventData )
{
    delete mLogsEventData;    
    mLogsEventData = logsEventData;
}

// ----------------------------------------------------------------------------
// LogsEvent::setTime
//
// Time needed in recent views and in detail view (not needed in event view)
// ----------------------------------------------------------------------------
//
bool LogsEvent::setTime( const QDateTime& time )
{
    bool changed( mTime != time );
    mTime = time;
    return changed;
}

// ----------------------------------------------------------------------------
// LogsEvent::time
// ----------------------------------------------------------------------------
//
QDateTime LogsEvent::time() const
{ 
    return mTime;        
}

// ----------------------------------------------------------------------------
// LogsEvent::setDuplicates
//
// Duplicates needed only in missed calls view
// ----------------------------------------------------------------------------
//
void LogsEvent::setDuplicates( int duplicates )
{
    mDuplicates = duplicates;
}

// ----------------------------------------------------------------------------
// LogsEvent::duplicates
// ----------------------------------------------------------------------------
//
int LogsEvent::duplicates() const
{
    return mDuplicates;
}

// ----------------------------------------------------------------------------
// LogsEvent::setIndex
// ----------------------------------------------------------------------------
//
void LogsEvent::setIndex(int index)
{
    mIndex = index;
}

// ----------------------------------------------------------------------------
// LogsEvent::index
// ----------------------------------------------------------------------------
//
int LogsEvent::index() const
{
    return mIndex;
}

// ----------------------------------------------------------------------------
// LogsEvent::setIsInView
// ----------------------------------------------------------------------------
//
void LogsEvent::setIsInView(bool isInView)
{
    mIsInView = isInView;
}

// ----------------------------------------------------------------------------
// LogsEvent::isInView
// ----------------------------------------------------------------------------
//
bool LogsEvent::isInView() const
{
    return mIsInView;
}

// ----------------------------------------------------------------------------
// LogsEvent::eventState
// ----------------------------------------------------------------------------
//
LogsEvent::LogsEventState LogsEvent::eventState() const
{
    return mEventState;
}


// ----------------------------------------------------------------------------
// LogsEvent::setContactLocalId
// ----------------------------------------------------------------------------
//
void LogsEvent::setContactLocalId( unsigned int id )
{
    if ( !mLogsEventData ) {
        mLogsEventData = new LogsEventData;
    }
    mLogsEventData->setContactLocalId(id);
}


// ----------------------------------------------------------------------------
// LogsEvent::contactLocalId
// ----------------------------------------------------------------------------
//
unsigned int LogsEvent::contactLocalId() const
{
    unsigned int id = 0;
    if ( mLogsEventData ) {
        id = mLogsEventData->contactLocalId();
    }
    return id;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
QString LogsEvent::updateRemotePartyFromContacts(QContactManager& manager)
{
    QContactDetailFilter phoneFilter;  
    if ( mEventType == TypeVoIPCall && mLogsEventData && !mLogsEventData->remoteUrl().isEmpty() ) {
        phoneFilter.setDetailDefinitionName( QContactOnlineAccount::DefinitionName, 
                                             QContactOnlineAccount::FieldAccountUri);
        phoneFilter.setValue(mLogsEventData->remoteUrl());
        phoneFilter.setMatchFlags(QContactFilter::MatchExactly);
    } else if ( !mNumber.isEmpty() ){
         // remove non-significant parts from number for better matching
        phoneFilter.setDetailDefinitionName( QContactPhoneNumber::DefinitionName,  
                                             QContactPhoneNumber::FieldNumber);
        phoneFilter.setValue(stripPhoneNumber(mNumber));
        phoneFilter.setMatchFlags(QContactFilter::MatchEndsWith);
    } else {
        // Searching not possible
        return QString(); 
    }
    
    LOGS_QDEBUG_2( "logs [ENG]    Try to find contact for num:", phoneFilter.value().toString() )
    
    QString contactNameStr;
    QList<QContactLocalId> matchingContacts = manager.contactIds(phoneFilter);
    LOGS_QDEBUG_2( "logs [ENG]    Number of matches:", matchingContacts.size() )
    if (matchingContacts.size() == 1) {
        // If multiple matches, don't dare to use any
        QContact match = manager.contact(matchingContacts.at(0));
        // QContactManager::synthesiseDisplayLabel would be more clean but
        // it returns currently "Unnamed" in case of missing name which we
        // cannot use.
        QContactName contactName = match.detail(QContactName::DefinitionName);
        contactNameStr = parseContactName(contactName);   
        if (contactNameStr.length() > 0){
            LOGS_QDEBUG_3( "getRemotePartyFromContacts, (name, num):", 
                           contactNameStr, mNumber );
            // Fill event with new contact info
            setRemoteParty( contactNameStr );
            setContactLocalId( matchingContacts.at(0) );
        }
    }
    return contactNameStr;
}


// ----------------------------------------------------------------------------
// LogsEvent::stripPhoneNumber
// ----------------------------------------------------------------------------
//
QString LogsEvent::stripPhoneNumber(const QString& num)
{
    // Remove international part from beginning if starts with '+'
    // and leading digit can be removed if doesn't start with '+'
    // NOTE: since international part is not fixed length, this
    // approach is not bulletproof (i.e. if international part is
    // only one digit long, part of group identification code is ignored
    // which might lead to incorrect matching in case where user
    // would have two contacts with same subscriber number part but for
    // different operator (quite unlikely).

    if ( num.length() == 0 ){
        return num;
    }
    QString modifiedNum( num );
    if ( modifiedNum.at(0) == '+' ) {
        // QString handles automatically case of removing too much
        const int removePlusAndInternationalPart = 4;
        modifiedNum.remove( 0, removePlusAndInternationalPart );
    }
    else {
        const int removeFirstDigit = 1;
        modifiedNum.remove( 0, removeFirstDigit );
    }

    return modifiedNum;
}

// ----------------------------------------------------------------------------
// LogsEvent::parseContactName
// ----------------------------------------------------------------------------
//
QString LogsEvent::parseContactName(const QContactName& name)
{
    QString firstName = name.value(QContactName::FieldFirst);
    QString lastName = name.value(QContactName::FieldLast);
    QString parsedName;
    if (!lastName.isEmpty()) {
        if (!firstName.isEmpty()) {
            parsedName = 
                QString(QLatin1String("%1 %2")).arg(firstName).arg(lastName);
        } 
        else {
            parsedName = lastName;
        }
    } else if (!firstName.isEmpty()) {
        parsedName = firstName;
    }
    return parsedName;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsEvent::prepareForContactMatching()
{
    // If event would be in "added" state when contact mathing is done, event state
    // would not go to "updated" in case of associated contact was modified or
    // new matching contact was found. This would prevent list update.
    if ( mEventState == EventAdded ) {
        LOGS_QDEBUG( "logs [ENG] <-> LogsEvent::prepareForContactMatching, prepared")
        mEventState = EventNotUpdated;
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsEvent::markedAsSeenLocally(bool markedAsSeen)
{
    mIsLocallySeen = markedAsSeen;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsEvent::isSeenLocally() const
{
    return mIsLocallySeen;
}

// End of file

