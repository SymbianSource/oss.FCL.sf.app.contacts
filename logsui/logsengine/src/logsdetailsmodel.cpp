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

#include "logsdetailsmodel.h"
#include "logsevent.h"
#include "logslogger.h"
#include "logsengdefs.h"
#include "logsdbconnector.h"
#include "logseventdata.h"
#include "logscontact.h"
#include "logscommondata.h"
#include <hbglobal.h>
#include <hblineedit.h>
#include <hbicon.h>
#include <QStringList>

Q_DECLARE_METATYPE(LogsContact*)

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsDetailsModel::LogsDetailsModel( LogsDbConnector& dbConnector, LogsEvent& event ) 
    : LogsAbstractModel(),
      mEvent( 0 )
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDetailsModel::LogsDetailsModel()" )
    
    mDbConnector = &dbConnector;
    
    mEvent = new LogsEvent( event );
    
    initContent();
    
    if ( mEvent->direction() == LogsEvent::DirMissed && 
            !mEvent->isRead() && mEvent->duplicates() > 0 ){
        // Read duplicates to get all occurences
        connect( mDbConnector, SIGNAL(duplicatesRead()), this, SLOT(duplicatesRead()) );
        mDbConnector->readDuplicates(mEvent->logId());
    }
    
    LOGS_QDEBUG( "logs [ENG] <- LogsDetailsModel::LogsDetailsModel()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsDetailsModel::~LogsDetailsModel()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDetailsModel::~LogsDetailsModel()" )
    
    delete mEvent;
    qDeleteAll(mDetailIcons);
    qDeleteAll(mDuplicates);
    
    LOGS_QDEBUG( "logs [ENG] <- LogsDetailsModel::~LogsDetailsModel()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::clearEvent()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDetailsModel::clearEvent()" )
    QList<int> eventIds;
    eventIds.append(mEvent->logId());
    mDbConnector->clearEvents(eventIds);    
    LOGS_QDEBUG( "logs [ENG] <- LogsDetailsModel::clearEvent()" )
}

// -----------------------------------------------------------------------------
// From QAbstractItemModel
// -----------------------------------------------------------------------------
//
int LogsDetailsModel::rowCount(const QModelIndex & /* parent */) const
{
    return mDetailTexts.count();
}

// -----------------------------------------------------------------------------
// From QAbstractItemModel
// -----------------------------------------------------------------------------
//
QVariant LogsDetailsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= mDetailTexts.count() || index.row() < 0 ) {
        return QVariant();
    }

    if (role == Qt::DisplayRole){
        QStringList text;
        getDisplayData(index.row(), text);
        return QVariant(text);
    } else if (role == Qt::DecorationRole) {
        QList<QVariant> icons;
        getDecorationData(index.row(), icons);
        return QVariant(icons);
    } 
    LogsModelItemContainer item(mEvent);
    return doGetData(role, item);
}

// -----------------------------------------------------------------------------
// From QAbstractItemModel
// -----------------------------------------------------------------------------
//
QVariant LogsDetailsModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    
    if (role == Qt::DisplayRole){
        return QVariant(getHeaderData(*mEvent));
    }
  
    return QVariant();
}

// -----------------------------------------------------------------------------
// From LogsAbstractModel
// -----------------------------------------------------------------------------
//
QVariant LogsDetailsModel::createContact(const LogsModelItemContainer& item) const
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDetailsModel::createContact()" )
    
    QVariant var = LogsAbstractModel::createContact(item);
    LogsContact* contact = qVariantValue<LogsContact*>(var);
    if (contact){
        QObject::connect(contact, SIGNAL(openCompleted(bool)),
                         this, SLOT(contactActionCompleted(bool)));
        QObject::connect(contact, SIGNAL(saveCompleted(bool)),
                         this, SLOT(contactActionCompleted(bool)));
    }
    return var;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::contactActionCompleted(bool modified)
{
    if ( modified ){
        mEvent->prepareForContactMatching();
        if ( mEvent->updateRemotePartyFromContacts(
                LogsCommonData::getInstance().contactManager() ).length() > 0 ) {
            initContent();
            reset();
        }
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::duplicatesRead()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDetailsModel::duplicatesRead()" )
    
    qDeleteAll( mDuplicates );
    mDuplicates.clear();
    mDuplicates = mDbConnector->takeDuplicates();
    
    initContent();
    reset();
    
    // Someone else might be reading duplicates as well, don't interfere with them.
    disconnect( mDbConnector, SIGNAL(duplicatesRead()), this, SLOT(duplicatesRead()) );

    LOGS_QDEBUG( "logs [ENG] <- LogsDetailsModel::duplicatesRead()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::getDecorationData(int row, QList<QVariant>& iconList) const
{
    if ( row < mDetailIcons.count() ){
        iconList << *mDetailIcons.at(row);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::getDisplayData(int row, QStringList& texts) const
{
    if ( row < mDetailTexts.count() ){
        texts << mDetailTexts.at(row);
    }
}

// -----------------------------------------------------------------------------
// Caller id:
// num
// or remote url
// or no num
// -----------------------------------------------------------------------------
//
QString LogsDetailsModel::getCallerId(const LogsEvent& event) const
{
    QString callerId("");
    if (event.remoteParty().length() > 0 && event.number().length() > 0){
    	callerId = event.number();
    }
    
    if (event.remoteParty().length() > 0 && getRemoteUri(event).length() > 0){
    	callerId = getRemoteUri(event);
    }
    return callerId;
}

// -----------------------------------------------------------------------------
// Caller id:
// name
// or num
// or remote url
// or no num
// -----------------------------------------------------------------------------
//
QString LogsDetailsModel::getHeaderData(const LogsEvent& event) const
{
    QString headerdata(event.remoteParty());
    if ( headerdata.length() == 0 && event.number().length() > 0 ){
        headerdata = event.number();
    }
    
    if ( headerdata.length() == 0 ){
        headerdata = getRemoteUri(event);
    }
    
    if ( headerdata.length() == 0 ){
        headerdata = hbTrId("txt_dial_dblist_call_id_val_unknown_number");
    }
    return headerdata;
}

// -----------------------------------------------------------------------------
// VoIP remote Uri :
// -----------------------------------------------------------------------------
//
QString LogsDetailsModel::getRemoteUri(const LogsEvent& event) const
{
    QString remoteUri((""));
    if (event.logsEventData()){
        remoteUri = event.logsEventData()->remoteUrl();
    }
    return remoteUri;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::getNumberToClipboard()
{
    HbLineEdit *cliptmp = new HbLineEdit("");
 
    QString num = mEvent->getNumberForCalling(); 
    cliptmp->setText(num);
    cliptmp->setSelection(0, num.length());
    cliptmp->copy();
    delete cliptmp;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsDetailsModel::isAddress(QString value) const
{
	QChar c('@');
	return value.contains(c);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsDetailsModel::isOutgoingCall() const
{
    return (mEvent->direction() == LogsEvent::DirOut);	
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QString LogsDetailsModel::getHeaderValue(QString value,bool isRemote) const
{
    QString headervalue("");
    if (isAddress(value)){
        if ((isOutgoingCall() && isRemote)|| (!isOutgoingCall() && !isRemote)){
            headervalue = hbTrId("txt_dialer_ui_dblist_call_id");
        }
        else{
            headervalue = hbTrId("txt_dialer_ui_dblist_call_id");
        }
    } 
    else {
        if ((isOutgoingCall() && isRemote)|| (!isOutgoingCall() && !isRemote)) {
            headervalue = hbTrId("txt_dialer_ui_dblist_call_id");
        }
        else {
            headervalue = hbTrId("txt_dialer_ui_dblist_call_id");
        }
    }
    
    return headervalue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::initContent()
{
    qDeleteAll(mDetailIcons);
    mDetailIcons.clear();
    mDetailTexts.clear();
    initTexts();
    initIcons();
    
    Q_ASSERT( mDetailIcons.count() == mDetailTexts.count() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::initTexts()
{
	if (getCallerId(*mEvent).length()!= 0){
        QStringList remotePartyRow;
        remotePartyRow << getHeaderValue(getCallerId(*mEvent),true);
        remotePartyRow << getCallerId(*mEvent);
        mDetailTexts.append(remotePartyRow);
	}
	
	// TODO: if more than one date and time rows, first row has text "Last call event"
	// but there's no localization string for that yet
	bool firstOfMultipleDates( mDuplicates.count() > 0 );
	addDateAndTimeTextRow(*mEvent, firstOfMultipleDates);
    
    QStringList callDirectionRow;
    callDirectionRow << hbTrId("txt_dialer_ui_dblist_call_direction");
    callDirectionRow << mEvent->directionAsString();
    mDetailTexts.append(callDirectionRow);
    
    QStringList callTypeRow;
    callTypeRow << hbTrId("txt_dialer_ui_dblist_call_type");
    callTypeRow << mEvent->typeAsString();
    mDetailTexts.append(callTypeRow);
    
    if ( mEvent->direction() != LogsEvent::DirMissed ){
        QStringList callDurationRow;
        callDurationRow << hbTrId("txt_dialer_ui_dblist_call_duration");
        QTime n(0, 0, 0);
        QTime t = n.addSecs(mEvent->duration());                
        callDurationRow << t.toString("hh:mm:ss");
        mDetailTexts.append(callDurationRow);
    }
    
    foreach ( LogsEvent* event, mDuplicates ){
        addDateAndTimeTextRow(*event);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::initIcons()
{
	if (getCallerId(*mEvent).length()!= 0){
        HbIcon* remotePartyIcon = new HbIcon(logsRemotePartyInfoIconId);
        mDetailIcons.append(remotePartyIcon);
  	}
    
    HbIcon* dateAndTimeIcon = new HbIcon(logsCallDateAndTimeIconId);
    mDetailIcons.append(dateAndTimeIcon);
    
    HbIcon* directionIcon = new HbIcon( LogsAbstractModel::directionIconName(*mEvent) );
    mDetailIcons.append(directionIcon);
    
    HbIcon* typeIcon = new HbIcon( LogsAbstractModel::typeIconName(*mEvent) );
    mDetailIcons.append(typeIcon);
    
    if ( mEvent->direction() != LogsEvent::DirMissed ){
        HbIcon* durationIcon = new HbIcon(logsCallDurationIconId);
        mDetailIcons.append(durationIcon);
    }
    
    foreach ( LogsEvent* event, mDuplicates ){
        // Having multiple date and time icon instances has no performance
        // penalty due resource sharing inside HbIcon impl
        HbIcon* dateAndTimeIcon = new HbIcon(logsCallDateAndTimeIconId);
        mDetailIcons.append(dateAndTimeIcon);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::addDateAndTimeTextRow(
        const LogsEvent& event, bool firstOfMultipleDates)
{
    QStringList dateAndTimeRow;
    if ( firstOfMultipleDates ){
        dateAndTimeRow << hbTrId("txt_dial_dblist_last_call_event");
    } else {
        dateAndTimeRow << hbTrId("txt_dialer_ui_dblist_date_and_time");
    }
    dateAndTimeRow << event.time().toTimeSpec(Qt::LocalTime).toString();
    mDetailTexts.append(dateAndTimeRow);
}
