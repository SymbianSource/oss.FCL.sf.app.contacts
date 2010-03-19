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

#include "logsmodel.h"
#include "logsdbconnector.h"
#include "logsevent.h"
#include "logslogger.h"
#include "logscall.h"
#include "logscontact.h"
#include "logsengdefs.h"
#include "logsdetailsmodel.h"
#include "logsmatchesmodel.h"
#include "logsmessage.h"
#include "logseventdata.h"
#include "logscommondata.h"
#include <hbicon.h>
#include <QStringList>

Q_DECLARE_METATYPE(LogsEvent *)
Q_DECLARE_METATYPE(LogsCall *)
Q_DECLARE_METATYPE(LogsDetailsModel *)
Q_DECLARE_METATYPE(LogsMessage *)
Q_DECLARE_METATYPE(LogsContact *)
Q_DECLARE_METATYPE(LogsMatchesModel *)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsModel::LogsModel(LogsModelType modelType) : 
    LogsAbstractModel(), mModelType(modelType)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsModel::LogsModel()" )
    
    initIcons();
    
    bool allEvents( mModelType == LogsFullModel );
    mDbConnector = new LogsDbConnector( mEvents, allEvents );
    connect( mDbConnector, SIGNAL( dataAdded(QList<int>) ), 
            this, SLOT( dataAdded(QList<int>) ));
    connect( mDbConnector, SIGNAL( dataUpdated(QList<int>) ), 
            this, SLOT( dataUpdated(QList<int>) ));
    connect( mDbConnector, SIGNAL( dataRemoved(QList<int>) ), 
            this, SLOT( dataRemoved(QList<int>) ));
    mDbConnector->init();
    mDbConnector->start();
    
    LOGS_QDEBUG( "logs [ENG] <- LogsModel::LogsModel()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsModel::~LogsModel()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsModel::~LogsModel()" )
    
    delete mDbConnector;
    
    LogsCommonData::freeCommonData();
    
    LOGS_QDEBUG( "logs [ENG] <- LogsModel::~LogsModel()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsModel::clearList(LogsModel::ClearType cleartype)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsModel::clearList()" )
    return mDbConnector->clearList(cleartype);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsModel::markEventsSeen(LogsModel::ClearType cleartype)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsModel::markEventsSeen()" )

    QList<int> markedEvents;
    foreach ( LogsEvent* event, mEvents ){
        if ( matchEventWithClearType(*event, cleartype) && 
                !event->isRead() && !event->isSeenLocally() ){
            markedEvents.append(event->logId());
        }
    }
    connect( mDbConnector, SIGNAL(markingCompleted(int)), 
             this, SIGNAL(markingCompleted(int)) );
    bool retVal = mDbConnector->markEventsSeen(markedEvents);
    LOGS_QDEBUG_2( "logs [ENG] <- LogsModel::markEventsSeen()", retVal )
    return retVal;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsModel::clearMissedCallsCounter()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsModel::clearMissedCallsCounter()" )
    int err = mDbConnector->clearMissedCallsCounter();
    LOGS_QDEBUG_2( "logs [ENG] <- LogsModel::clearMissedCallsCounter(), err", err )
    return err;
}

// -----------------------------------------------------------------------------
// From QAbstractListModel
// -----------------------------------------------------------------------------
//
int LogsModel::rowCount(const QModelIndex & /* parent */) const
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsModel::rowCount()", mEvents.count() )
    return mEvents.count();
}

// -----------------------------------------------------------------------------
// From QAbstractItemModel
// -----------------------------------------------------------------------------
//
QVariant LogsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || 
         index.row() >= mEvents.count() || 
         index.row() < 0 ) {
        return QVariant();
    }

    LogsEvent* event = mEvents.at(index.row());
    if (role == Qt::DisplayRole){
        QStringList list;
        list << getCallerId( *event );
        list << event->time().toTimeSpec(Qt::LocalTime).toString();
        return QVariant(list);
    } else if (role == Qt::DecorationRole) {
        QList<QVariant> icons;
        getDecorationData(*event, icons);
        return QVariant(icons);
    } else if ( role == RoleDetailsModel ) {
        LOGS_QDEBUG( "logs [ENG]    LogsModel::data() RoleDetailsModel" )
        LogsDetailsModel* detailsModel = 
            new LogsDetailsModel( *mDbConnector, *event );
        QVariant var = qVariantFromValue( detailsModel );
        return var;
    }
    LogsModelItemContainer item(event);
    return doGetData(role, item);
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsMatchesModel* LogsModel::logsMatchesModel()
{
    LOGS_QDEBUG( "logs [ENG]    LogsModel::logsMatchesModel()" )
    return new LogsMatchesModel( *this, *mDbConnector );
}

// -----------------------------------------------------------------------------
// NOTE: documentation mentions that beginInsertRows should be called
// before adding data to the model. We are not conforming to that at
// the moment and still everything works fine. If there is problems
// in future, dbconnector should give prenotification about data addition.
// Same applies for dataUpdated and dataRemoved.
// -----------------------------------------------------------------------------
//
void LogsModel::dataAdded(QList<int> addedIndexes)
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsModel::dataAdded(), idxs:", addedIndexes );
    LOGS_QDEBUG_EVENT_ARR(mEvents)
    
    QList< QList<int> > sequences = findSequentialIndexes(addedIndexes);
    for ( int i = 0; i < sequences.count(); i++ ) {
        beginInsertRows(QModelIndex(), sequences.at(i).first(), sequences.at(i).last());
        endInsertRows();
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsModel::dataAdded()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsModel::dataUpdated(QList<int> updatedIndexes)
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsModel::dataUpdated(), idxs:", updatedIndexes );
    LOGS_QDEBUG_EVENT_ARR(mEvents)
    
    QList< QList<int> > sequences = findSequentialIndexes(updatedIndexes);
    for ( int i = 0; i < sequences.count(); i++ ) {
        QModelIndex top = index(sequences.at(i).first());
        QModelIndex bottom = index(sequences.at(i).last());
        emit dataChanged(top, bottom);
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsModel::dataUpdated()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsModel::dataRemoved(QList<int> removedIndexes)
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsModel::dataRemoved(), idxs:", removedIndexes );
    LOGS_QDEBUG_EVENT_ARR(mEvents)
    
    QList< QList<int> > sequences = findSequentialIndexes(removedIndexes);
    for ( int i = ( sequences.count() - 1 ); i >= 0; i-- ) {
        beginRemoveRows(QModelIndex(), sequences.at(i).first(), sequences.at(i).last());
        endRemoveRows();
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsModel::dataRemoved()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QList< QList<int> > LogsModel::findSequentialIndexes(const QList<int>& indexes)
{
    QList< QList<int> > sequences;
    QList<int> currSequence;
    int prevIndex = indexes.at(0) - 1;
    for ( int i = 0; i < indexes.count(); i++ ){
        int currIndex = indexes.at(i);
        if ( prevIndex+1 != currIndex ){
            
            sequences.append(currSequence);
            currSequence.clear();
        } 
        currSequence.append(currIndex);
        prevIndex = currIndex;
    }
    
    if ( !currSequence.isEmpty() ){
        // Add last sequence if such exist
        sequences.append(currSequence);
    }
    return sequences;
}

// -----------------------------------------------------------------------------
// Caller id:
// name
// or num
// or remote url
// or no num
// -----------------------------------------------------------------------------
//
QString LogsModel::getCallerId(const LogsEvent& event) const
{
    QString callerId(event.remoteParty());
    if ( callerId.length() == 0 ){
        callerId = event.number();
    }
    if ( callerId.length() == 0 && event.logsEventData() ){
        callerId = event.logsEventData()->remoteUrl();
    }
    if ( callerId.length() == 0 ){
        callerId = tr("No number");
    }
    int duplicates = event.duplicates();
    if ( duplicates > 0 && !event.isRead() ){
        callerId.append( "(" );
        callerId.append( QString::number(duplicates + 1) );
        callerId.append( ")");
    }
    return callerId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsModel::initIcons()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsModel::LogsModel()" )
    
    HbIcon* icon = new HbIcon(logsDialledVoiceCallIconId);    
    mIcons.insert(logsDialledVoiceCallIconId, icon);
    icon = new HbIcon(logsMissedVoiceCallIconId);    
    mIcons.insert(logsMissedVoiceCallIconId, icon);
    icon = new HbIcon(logsReceivedVoiceCallIconId);    
    mIcons.insert(logsReceivedVoiceCallIconId, icon);

    icon = new HbIcon(logsDialledVideoCallIconId);    
    mIcons.insert(logsDialledVideoCallIconId, icon);
    icon = new HbIcon(logsMissedVideoCallIconId);    
    mIcons.insert(logsMissedVideoCallIconId, icon);
    icon = new HbIcon(logsReceivedVideoCallIconId);    
    mIcons.insert(logsReceivedVideoCallIconId, icon);
    
    icon = new HbIcon(logsDialledVoipCallIconId);    
    mIcons.insert(logsDialledVoipCallIconId, icon);
    icon = new HbIcon(logsMissedVoipCallIconId);    
    mIcons.insert(logsMissedVoipCallIconId, icon);
    icon = new HbIcon(logsReceivedVoipCallIconId);    
    mIcons.insert(logsReceivedVoipCallIconId, icon);

    LOGS_QDEBUG( "logs [ENG] <- LogsModel::LogsModel()" )
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsModel::matchEventWithClearType(
        const LogsEvent& event, LogsModel::ClearType clearType)
{
    bool match( false );
    LogsEvent::LogsDirection dir = event.direction();
    switch (clearType){
        case LogsModel::TypeLogsClearAll:
            match = true;
            break;
        case LogsModel::TypeLogsClearReceived:
            match = ( dir == LogsEvent::DirIn );
            break;
        case LogsModel::TypeLogsClearCalled:
            match = ( dir == LogsEvent::DirOut );
            break;
        case LogsModel::TypeLogsClearMissed:
            match = ( dir == LogsEvent::DirMissed );
            break;
        default:
           break;
    }
    return match;
}

