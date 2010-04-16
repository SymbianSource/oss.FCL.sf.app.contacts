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

#include <QStringList>
#include <QtAlgorithms>
#include <HbGlobal>

#include "mobhistorymodel_p.h"
#include "mobhistorymodel.h"

// Unnamed namespace for helper functions
namespace
{
    bool greaterThan(const HistoryItem& t1, const HistoryItem& t2)
    {
        return t1.timeStamp > t2.timeStamp;
    }
    
    bool lessThan(const HistoryItem& t1, const HistoryItem& t2)
    {
        return t1.timeStamp < t2.timeStamp;
    }
}

Q_DECLARE_METATYPE(LogsEvent *)

/*!
 * Construct a new MobHistoryModel object to communicate 
 * with the conversations and logs databases.
 *
 * \param contactId History specific to this contact is cached. 
 * If no contact is specified all the call logs and conversation 
 * history from all contacts will be cached.
 */
MobHistoryModel::MobHistoryModel(QContactLocalId contactId,
                                 QContactManager* manager,
                                 QObject *parent)
    : QAbstractListModel(parent)
{
    d = new MobHistoryModelData(contactId, manager);
    
    // Check if the contact is my card
    if (d->m_contactId == d->m_contactManager->selfContactId()) {
        d->m_isMyCard = true;
    }
    else {
        d->m_isMyCard = false;
    }
    // Create the model structure and cache history data from the databases
    initializeModel();

}

MobHistoryModel::~MobHistoryModel()
{
}

/*!
 * Return the data to be used by the view or delegates for a particular
 * item and role.
 *
 * \param index The index of the item to return data about.
 * \param role The data should be relevant to this particular purpose.
 * \return QVariant The data for the specified index and role.
 */
QVariant MobHistoryModel::data(const QModelIndex& index, int role) const
{
    // Invalid index
    int row = index.row();
    
    if ( !validateRowIndex(row) )
        return QVariant();
    
    HistoryItem item = d->m_table.at(row);

    switch( role )
    {       
        case Qt::DisplayRole:
            return displayRoleData(item);
        case Qt::DecorationRole:
            return QVariant(item.iconPath);
        case SeenStatusRole:
            return QVariant(item.seenStatus);
        case DirectionRole:
            return QVariant(item.direction);
        case ItemTypeRole:
            return QVariant(item.msgType);
        case PhoneNumberRole:
            return QVariant(item.number);
        default:
            return QVariant();
    }
}

/*!
 * Return the data to be used by the view for a display role.
 *
 * \param column The column of the item to return data about.
 *  return QVariant List of strings to be displayed on the view.
 *  The stings can also be NULL
 *  index 0 Title of the conversation item.
 *  index 1 Body text
 *  index 2 Time stamp
 */
QVariant MobHistoryModel::displayRoleData(const HistoryItem& item) const
{
    QStringList list;
    
    list << item.title << item.message << item.timeStamp.toString();
    
    return QVariant(list);
}

/*!
 * Get the number of rows (conversations) in this model.
 *
 * \param parent Optional parent index value.
 * \return Number of rows in this model.
 */
int MobHistoryModel::rowCount(const QModelIndex& /*parent*/) const
{
    return d->m_table.size();
}

/*!
 * Sort list items on the model.
 *
 * \param column Column to be sorted. It is not used.
 * \param order Order to sort the list items.
 */
void MobHistoryModel::sort(int /*column*/, Qt::SortOrder order)
{
    if ( order == Qt::AscendingOrder )
        qStableSort(d->m_table.begin(), d->m_table.end(), lessThan);
    else
        qStableSort(d->m_table.begin(), d->m_table.end(), greaterThan);
}

/*!
 * Clear history from the database. If the history cached
 * is specific to one contact, only that history is cleared.
 * 
 */
void MobHistoryModel::clearHistory()
{
    if ( d->m_table.isEmpty() )
        return;
    
    // Call logs
    if ( !d->m_isMyCard && d->m_logsFilter ) {
        if ( !d->m_logsFilter->clearEvents() ) {
            // Operation not async
            int bottom = rowCount();
            d->m_table.clear();
            beginRemoveRows(QModelIndex(), rowCount(), bottom);
            endRemoveRows();
        }
    } else if ( d->m_logsModel ) {
        if ( !d->m_logsModel->clearList(LogsModel::TypeLogsClearAll) ) {
            // Operation not async
            int bottom = rowCount();
            d->m_table.clear();
            beginRemoveRows(QModelIndex(), rowCount(), bottom);
            endRemoveRows();
        }
    }
}

/*!
 * Mark all the conversations in the view as seen.
 * 
 */
void MobHistoryModel::markAllAsSeen()
{
    if ( d->m_isMarkedAsSeen )
        return;
    
    // Call logs
    if ( !d->m_isMyCard && d->m_logsFilter ) {
        d->m_logsFilter->markEventsSeen();
    } else if ( d->m_logsModel ) {
        d->m_logsModel->markEventsSeen(LogsModel::TypeLogsClearMissed);
    }
    d->m_isMarkedAsSeen = true;
}

/*!
 * Sort items in the model and refresh the view
 * 
 */
void MobHistoryModel::sortAndRefresh(Qt::SortOrder order)
{
    sort(0, order);
    emit layoutChanged();
}

/*!
 * Create the model structure and cache history data from
 * conversations and call logs databases.
 *
 */
void MobHistoryModel::initializeModel()
{
    initializeLogsModel();
}

void MobHistoryModel::initializeLogsModel()
{
    //populate model with call events
    d->m_logsModel = new LogsModel(LogsModel::LogsFullModel);
    if (!d->m_isMyCard) {
        //do the filtering to get call events for the target contact
        d->m_logsFilter = new LogsCustomFilter;
        d->m_logsFilter->setContactId(d->m_contactId);
        d->m_logsFilter->setSourceModel(d->m_logsModel);
        d->m_AbstractLogsModel = d->m_logsFilter;
        
        connect(d->m_logsFilter, SIGNAL(clearingCompleted(int)), 
                    this, SLOT(clearedCallLogs(int)));
        connect(d->m_logsFilter, SIGNAL(markingCompleted(int)), 
                    this, SLOT(markingCompleted(int)));
    } else {
        //get all call events
        d->m_AbstractLogsModel = d->m_logsModel;
        
        connect(d->m_logsModel, SIGNAL(clearingCompleted(int)), 
                    this, SLOT(clearedCallLogs(int)));
        connect(d->m_logsModel, SIGNAL(markingCompleted(int)), 
                    this, SLOT(markingCompleted(int)));
    }
    
    //read first call events and start listening for more 
    for ( int i = 0; i < d->m_AbstractLogsModel->rowCount(); ++i ) {
        LogsEvent* event = qVariantValue<LogsEvent*>(
                d->m_AbstractLogsModel->data(d->m_AbstractLogsModel->index(i, 0), LogsModel::RoleFullEvent) );
        
        if ( event ) {
            HistoryItem item;
            readLogEvent(event, item);
            d->m_LogsMap.insert(i, d->m_table.size());
            d->m_table.append( item );
        }
    }
    
    connect(d->m_AbstractLogsModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), 
            this, SLOT(logsRowsInserted(const QModelIndex &, int, int)));
    connect(d->m_AbstractLogsModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), 
            this, SLOT(logsRowsRemoved(const QModelIndex &, int, int)));
    connect(d->m_AbstractLogsModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), 
                this, SLOT(logsDataChanged(const QModelIndex &, const QModelIndex &)));

}

/*!
 * Read call event into a history item
 *
 * \param event Call log event
 * \param item Conversation history item
 */
void MobHistoryModel::readLogEvent(LogsEvent* event, HistoryItem& item)
{
    QString bodyText;
    QString icon;
    QString title;
    
    if ( d->m_isMyCard ) {
        if ( event->remoteParty().length() > 0 ) {
            title = event->remoteParty();
        }
        else {
            title = event->number();
        }
    } else {
        if ( event->direction() == LogsEvent::DirIn ) {
            bodyText = QString("Received call");
            icon = QString("qtg_small_received");
        } else if ( event->direction() == LogsEvent::DirOut ) {
            bodyText = QString(hbTrId("txt_phob_list_dialled_call"));
            icon = QString("qtg_small_sent");
        } else if ( event->direction() == LogsEvent::DirMissed ) {
            bodyText = QString(hbTrId("txt_phob_list_missed_call"));
            icon = QString("qtg_small_missed_call");
            if ( !event->isRead() )
                item.seenStatus = Unseen;
        }
    }

    if ( event->direction() == LogsEvent::DirOut )
        item.direction = Outgoing;
    else
        item.direction = Incoming;
    
    item.message = bodyText;
    item.iconPath = icon;
    item.title = title;
    item.timeStamp = event->time();
    item.msgType = CallLog;
    item.number = event->number();
}

/*!
 * Slot used for receiving new rows from the LogsModel.
 *
 * \param parent Optional parent index value.
 * \param first The first row item to be received from the model.
 * \param last The last row item to be received from the model.
 */
void MobHistoryModel::logsRowsInserted(const QModelIndex& /*parent*/, int first, int last)
{
    int oldRowCount = rowCount();
    
    for ( int i = first; i < d->m_AbstractLogsModel->rowCount() && i <= last; ++i ) {
        LogsEvent* event = qVariantValue<LogsEvent*>(
                d->m_AbstractLogsModel->data(d->m_AbstractLogsModel->index(i, 0), LogsModel::RoleFullEvent) );
        
        if ( event ) {
            HistoryItem item;
            readLogEvent(event, item);
            d->m_LogsMap.insert(i, d->m_table.size());
            d->m_table.append( item );
        }
    }
    
    sort();
    beginInsertRows(QModelIndex(), oldRowCount, rowCount());
    endInsertRows();
}

/*!
 * Slot used for receiving new rows from the LogsModel.
 *
 * \param parent Optional parent index value.
 * \param first The first row item to be received from the model.
 * \param last The last row item to be received from the model.
 */
void MobHistoryModel::logsRowsRemoved(const QModelIndex& /*parent*/, int first, int last)
{
    QList< int > indices;
    
    for ( int i = first; i <= last; ++i ) {
        int index = d->m_LogsMap.value(i);        
        d->m_table.removeAt(index);
        d->m_LogsMap.remove(i);        
        indices.append(index);
    }
    
    // Remove list items in batches
    if ( !indices.isEmpty() ) {
        QList< QList<int> > batches = findIndices(indices);
        foreach( QList<int> l, batches ) {
            beginRemoveRows(QModelIndex(), l.first(), l.last());
            endRemoveRows();
        }
    }
}

/*!
 * Update events from logs model. Events are
 * received as a batch
 *
 * \param first First updated model index
 * \param last Last updated model index
 */
void MobHistoryModel::logsDataChanged(const QModelIndex& first, const QModelIndex& last)
{
    int f = first.row();
    int l = last.row();
    QList< int > indices;
    
    for ( int i = f; i < d->m_AbstractLogsModel->rowCount() && i <= l; ++i ) {
        
        LogsEvent* event = qVariantValue<LogsEvent*>(
                d->m_AbstractLogsModel->data(d->m_AbstractLogsModel->index(i, 0), LogsModel::RoleFullEvent) );
        
        // Fetch index from the mapped logs model indices
        int index = d->m_LogsMap.value(i);
        
        if ( event && validateRowIndex(index) ) {
            HistoryItem item;
            readLogEvent(event, item);
            d->m_table.replace( index, item );
            indices.append(index);
        }
    }
    
    // Emit dataChanged signal only if there were updates
    if ( !indices.isEmpty() ) {
        QList< QList<int> > batches = findIndices(indices);
        foreach( QList<int> l, batches )
            emit dataChanged(index(l.first(), 0), index(l.last(), 0));
    }
}

/*!
 * Clear logs event slot received from logs model
 *
 * \param err Error of the clear logs request
 */
void MobHistoryModel::clearedCallLogs(int err)
{
    if ( err == 0 ) {
        int bottom = rowCount();
        d->m_table.clear();
        beginRemoveRows(QModelIndex(), rowCount(), bottom);
        endRemoveRows();
    }
}

/*!
 * Mark events as seen slot received from logs model
 *
 * \param err Error of the marking logs request
 */
void MobHistoryModel::markingCompleted(int err)
{
    if ( err == 0 ) {
        d->m_isMarkedAsSeen = true;
    }
}

/*!
 * Check whether an idex is out of bound of our list
 *
 * \param index Index to be validated
 */

bool MobHistoryModel::validateRowIndex( const int index) const
{
    return( index < rowCount() && index >= 0 );
}

/*!
 * Find the sequences of indices for the given indices from the private list
 * 
 * \param indices List of indices
 * \return sequences of indices
 */
QList< QList<int> > MobHistoryModel::findIndices( const QList< int >& indices )
{
    QList< QList<int> > sequences;
    QList<int> currSequence;
    int prevIndex = indices.at(0) - 1;
    
    for (int i = 0; i < indices.count(); i++)
    {
        int currIndex = indices.at(i);
        
        if (currIndex >= 0)
        {
            if (prevIndex+1 != currIndex)
            {
                sequences.append(currSequence);
                currSequence.clear();
            } 
            currSequence.append(currIndex);
            prevIndex = currIndex;
        }
    }
    
    if (!currSequence.isEmpty())
    {
        // Add last sequence if such exist
        sequences.append(currSequence);
    }
    
    return sequences;
}

