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
#include "logscustomfilter.h"
#include "logsevent.h"
#include "logseventdata.h"
#include "logsdbconnector.h"
#include "logslogger.h"

Q_DECLARE_METATYPE(LogsEvent *)

// -----------------------------------------------------------------------------
// LogsCustomFilter::LogsCustomFilter
// -----------------------------------------------------------------------------
//
LogsCustomFilter::LogsCustomFilter() 
: QSortFilterProxyModel(),
  mContactId(0)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsCustomFilter::LogsCustomFilter()" )
    
    // Enable filter check for dataChanged phase
    setDynamicSortFilter( true );
    
    LOGS_QDEBUG( "logs [ENG] <- LogsCustomFilter::LogsCustomFilter()" )
}


// -----------------------------------------------------------------------------
// LogsCustomFilter::~LogsCustomFilter
// -----------------------------------------------------------------------------
//
LogsCustomFilter::~LogsCustomFilter()
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsCustomFilter::~LogsCustomFilter()" )
}

// -----------------------------------------------------------------------------
// LogsCustomFilter::setContactId
// -----------------------------------------------------------------------------
//
void LogsCustomFilter::setContactId(quint32 contactId)
{
    mContactId = contactId;
}

// -----------------------------------------------------------------------------
// LogsCustomFilter::clearEvents
// -----------------------------------------------------------------------------
//
bool LogsCustomFilter::clearEvents()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsCustomFilter::clearEvents()" )
    bool clearingStarted(false);
    LogsAbstractModel* model = qobject_cast<LogsAbstractModel*>( sourceModel() );
    if ( model && model->dbConnector() ){
        connect( model->dbConnector(), SIGNAL(clearingCompleted(int)), 
                 this, SIGNAL(clearingCompleted(int)) );
        QList<int> eventIds = getEventIds();
        if ( !eventIds.isEmpty() ){
            clearingStarted = model->dbConnector()->clearEvents(eventIds);
        }
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsCustomFilter::clearEvents()" )
    return clearingStarted;
}

// -----------------------------------------------------------------------------
// LogsCustomFilter::markEventsSeen
// -----------------------------------------------------------------------------
//
bool LogsCustomFilter::markEventsSeen()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsCustomFilter::markEventsSeen()" )
    bool markingStarted(false);
    LogsAbstractModel* model = qobject_cast<LogsAbstractModel*>( sourceModel() );
    if ( model && model->dbConnector() ){
        connect( model->dbConnector(), SIGNAL(markingCompleted(int)), 
                 this, SIGNAL(markingCompleted(int)) );
        QList<int> eventIds = getEventIds(true);
        if ( !eventIds.isEmpty() ){
            markingStarted = model->dbConnector()->markEventsSeen(eventIds);
        }
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsCustomFilter::markEventsSeen()" )
    return markingStarted;
}
    
// -----------------------------------------------------------------------------
// LogsCustomFilter::filterAcceptsRow
// From QSortFilterProxyModel
// -----------------------------------------------------------------------------
//
bool LogsCustomFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if ( !sourceModel() ){
        return false;
    }
    bool accept = false;
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    const LogsEvent *event = qVariantValue<LogsEvent *>( sourceModel()->data(
            index0, LogsModel::RoleFullEvent) );
    
    if ( event && mContactId && event->logsEventData() ){
        accept = ( event->logsEventData()->contactLocalId() == mContactId );
    }
    
    return accept;
}

// -----------------------------------------------------------------------------
// LogsCustomFilter::getEventIds
// -----------------------------------------------------------------------------
//
QList<int> LogsCustomFilter::getEventIds(bool onlyUnseen) const
{
    QList<int> ids;
    for ( int i = 0; i < rowCount(); i++ ){
        const LogsEvent* event = qVariantValue<LogsEvent *>( 
                data(index(i, 0), LogsModel::RoleFullEvent ) );
        if ( event ){
            if ( !onlyUnseen || ( !event->isRead() && !event->isSeenLocally() ) ){
                ids.append( event->logId() );
            }
        }
    }
    return ids;
}

// End of file

