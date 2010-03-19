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
      mEvent( 0 ),
      mDetailItemsCount( 0 )
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDetailsModel::LogsDetailsModel()" )
    
    mDbConnector = &dbConnector;
    
    mEvent = new LogsEvent( event );
    
    initContent();
    
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
    return mDetailItemsCount;
}

// -----------------------------------------------------------------------------
// From QAbstractItemModel
// -----------------------------------------------------------------------------
//
QVariant LogsDetailsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= mDetailItemsCount || index.row() < 0 ) {
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
void LogsDetailsModel::getDecorationData(int row, QList<QVariant>& iconList) const
{
    if ( mDetailIcons.contains(row) ){
        iconList << *mDetailIcons.find(row).value();
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::getDisplayData(int row, QStringList& texts) const
{
    if ( mDetailTexts.contains(row) ){
        texts << mDetailTexts.find(row).value();
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
        headerdata = tr("No number");
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
// VoIP local Uri :
// -----------------------------------------------------------------------------
//
QString LogsDetailsModel::getLocalUri(const LogsEvent& event) const
{
    QString localUri((""));
    if (event.logsEventData()){
        localUri = event.logsEventData()->localUrl();
    }
    return localUri;
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
            headervalue = tr("Call to Address:");
        }
        else{
            headervalue = tr("Call from Address:");
        }
    } 
    else {
        if ((isOutgoingCall() && isRemote)|| (!isOutgoingCall() && !isRemote)) {
            headervalue = tr("Call to Number:");
        }
        else {
            headervalue = tr("Call from Number:");
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
    mDetailItemsCount = mDetailTexts.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::initTexts()
{
	int row = 0;
	
	if (getCallerId(*mEvent).length()!= 0){
        QStringList remotePartyRow;
        remotePartyRow << getHeaderValue(getCallerId(*mEvent),true);
        remotePartyRow << getCallerId(*mEvent);
        mDetailTexts.insert(row++, remotePartyRow);
	}
		
	if (getLocalUri(*mEvent).length()!= 0) {
        QStringList localUriRow;
        localUriRow << getHeaderValue(getLocalUri(*mEvent),false);
        localUriRow << getLocalUri(*mEvent);
        mDetailTexts.insert(row++, localUriRow);
	}
		
    QStringList dateAndTimeRow;
    dateAndTimeRow << hbTrId("txt_dialer_ui_dblist_date_and_time") + tr(":");
    dateAndTimeRow << mEvent->time().toTimeSpec(Qt::LocalTime).toString();
    mDetailTexts.insert(row++, dateAndTimeRow);
    
    QStringList callDirectionRow;
    callDirectionRow << hbTrId("txt_dialer_ui_dblist_call_direction") + tr(":");
    callDirectionRow << mEvent->directionAsString();
    mDetailTexts.insert(row++, callDirectionRow);
    
    QStringList callTypeRow;
    callTypeRow << hbTrId("txt_dialer_ui_dblist_call_type") + tr(":");
    callTypeRow << mEvent->typeAsString();
    mDetailTexts.insert(row++, callTypeRow);
    
    QStringList callDurationRow;
    callDurationRow << hbTrId("txt_dialer_ui_dblist_call_duration") + tr(":");
    QTime n(0, 0, 0);
    QTime t = n.addSecs(mEvent->duration());                
    callDurationRow << t.toString("hh:mm:ss");
    mDetailTexts.insert(row, callDurationRow);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::initIcons()
{
	int row = 0;
	if (getCallerId(*mEvent).length()!= 0){
        HbIcon* remotePartyIcon = new HbIcon(logsRemotePartyInfoIconId);
        mDetailIcons.insert(row++, remotePartyIcon);
  	}
  	
  	if (getLocalUri(*mEvent).length()!= 0) {
        HbIcon* localUriIcon = new HbIcon(logsRemotePartyInfoIconId);        
        mDetailIcons.insert(row++, localUriIcon);
    }
    
    HbIcon* dateAndTimeIcon = new HbIcon(logsCallDateAndTimeIconId);
    mDetailIcons.insert(row++, dateAndTimeIcon);
    
    HbIcon* directionIcon = new HbIcon( LogsAbstractModel::directionIconName(*mEvent) );
    mDetailIcons.insert(row++, directionIcon);
    
    HbIcon* typeIcon = new HbIcon( LogsAbstractModel::typeIconName(*mEvent) );
    mDetailIcons.insert(row++, typeIcon);
    
    HbIcon* durationIcon = new HbIcon(logsCallDurationIconId);
    mDetailIcons.insert(row, durationIcon);
}
