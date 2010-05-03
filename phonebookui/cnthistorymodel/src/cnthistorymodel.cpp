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
#include <msgitem.h>

#include "cnthistorymodel_p.h"
#include "cnthistorymodel.h"

// Unnamed namespace for helper functions
namespace
{
    bool greaterThan(const HItemPointer& t1, const HItemPointer& t2)
    {
        return t1.data()->timeStamp > t2.data()->timeStamp;
    }
    
    bool lessThan(const HItemPointer& t1, const HItemPointer& t2)
    {
        return t1.data()->timeStamp < t2.data()->timeStamp;
    }
}

Q_DECLARE_METATYPE(LogsEvent *)

/*!
 * Construct a new CntHistoryModel object to communicate 
 * with the conversations and logs databases.
 *
 * \param contactId History specific to this contact is cached. 
 * If no contact is specified all the call logs and conversation 
 * history from all contacts will be cached.
 */
CntHistoryModel::CntHistoryModel(QContactLocalId contactId,
                                 QContactManager* manager,
                                 QObject *parent)
    : QAbstractListModel(parent)
{
    d = new CntHistoryModelData(contactId, manager);
    
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

CntHistoryModel::~CntHistoryModel()
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
QVariant CntHistoryModel::data(const QModelIndex& index, int role) const
{
    // Invalid index
    int row = index.row();
    
    if ( !validateRowIndex(row) )
        return QVariant();
    
    HItemPointer p = d->m_List.at(row);
    if ( p.isNull() )
        return QVariant();
    
    switch( role )
    {       
        case Qt::DisplayRole:
            return displayRoleData(*p);
        case Qt::DecorationRole:
            return QVariant((*p).iconPath);
        case SeenStatusRole:
            return QVariant((*p).seenStatus);
        case DirectionRole:
            return QVariant((*p).direction);
        case ItemTypeRole:
            return QVariant((*p).msgType);
        case PhoneNumberRole:
            return QVariant((*p).number);
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
QVariant CntHistoryModel::displayRoleData(const HistoryItem& item) const
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
int CntHistoryModel::rowCount(const QModelIndex& /*parent*/) const
{
    return d->m_List.size();
}

/*!
 * Sort list items on the model.
 *
 * \param column Column to be sorted. It is not used.
 * \param order Order to sort the list items.
 */
void CntHistoryModel::sort(int /*column*/, Qt::SortOrder order)
{
    if ( order == Qt::AscendingOrder )
        qStableSort(d->m_List.begin(), d->m_List.end(), lessThan);
    else
        qStableSort(d->m_List.begin(), d->m_List.end(), greaterThan);
}

/*!
 * Clear history from the database. If the history cached
 * is specific to one contact, only that history is cleared.
 * 
 */
void CntHistoryModel::clearHistory()
{
    if ( d->m_List.isEmpty() )
        return;
    
    // Call logs
    if ( !d->m_isMyCard && d->m_logsFilter ) {
        if ( !d->m_logsFilter->clearEvents() ) {
            // Operation not async
            clearedCallLogs( 0 );
        }
    } else if ( d->m_logsModel ) {
        if ( !d->m_logsModel->clearList(LogsModel::TypeLogsClearAll) ) {
            // Operation not async
            clearedCallLogs( 0 );
        }
    }
    
    // Messages
    d->m_msgHistory->clearMessages( (int)d->m_contactId );    
}

/*!
 * Mark all the conversations in the view as seen.
 * 
 */
void CntHistoryModel::markAllAsSeen()
{
    if ( d->m_isMarkedAsSeen )
        return;
    
    // Call logs
    if ( !d->m_isMyCard && d->m_logsFilter ) {
        d->m_logsFilter->markEventsSeen();
    } else if ( d->m_logsModel ) {
        d->m_logsModel->markEventsSeen(LogsModel::TypeLogsClearMissed);
    }
    
    // Messages
    d->m_msgHistory->markRead( (int)d->m_contactId );
    
    d->m_isMarkedAsSeen = true;
}

/*!
 * Sort items in the model and refresh the view
 * 
 */
void CntHistoryModel::sortAndRefresh(Qt::SortOrder order)
{
    sort(0, order);
    emit layoutChanged();
}

/*!
 * Create the model structure and cache history data from
 * conversations and call logs databases.
 *
 */
void CntHistoryModel::initializeModel()
{
    initializeLogsModel();
    initializeMsgModel();
}

void CntHistoryModel::initializeMsgModel()
{
    MsgHistory* m = new MsgHistory();
    
    // Connect to signals emitted by msg model
    connect(m, SIGNAL(messagesReady(QList<MsgItem>& )), this, SLOT(messagesReady(QList<MsgItem>& )));
    connect(m, SIGNAL(messageAdded(MsgItem& )), this, SLOT(messageAdded(MsgItem& )));
    connect(m, SIGNAL(messageChanged(MsgItem& )), this, SLOT(messageChanged(MsgItem& )));
    connect(m, SIGNAL(messageDeleted(MsgItem& )), this, SLOT(messageDeleted(MsgItem& )));
    
    // Contact centric
    if( !d->m_isMyCard ) {
        // Subscribe to get new messages
        // received from this contact
        m->subscribe(d->m_contactId);
        
        // Initial fetch of all messages
        m->getMessages(d->m_contactId);
    }
    
    d->m_msgHistory = m;
}

void CntHistoryModel::initializeLogsModel()
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
            HItemPointer item = HItemPointer(new HistoryItem());
            readLogEvent(event, *item);
            d->m_logsMap.insert(i, item);
            d->m_List.append( item );
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
void CntHistoryModel::readLogEvent(LogsEvent* event, HistoryItem& item)
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
            bodyText = hbTrId("txt_phob_list_received");
            icon = QString("qtg_small_received");
        } else if ( event->direction() == LogsEvent::DirOut ) {
            bodyText = hbTrId("txt_phob_list_dialled_call");
            icon = QString("qtg_small_sent");
        } else if ( event->direction() == LogsEvent::DirMissed ) {
            bodyText = hbTrId("txt_phob_list_missed_call");
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
void CntHistoryModel::logsRowsInserted(const QModelIndex& /*parent*/, int first, int last)
{
    int oldRowCount = rowCount();
    
    for ( int i = first; i < d->m_AbstractLogsModel->rowCount() && i <= last; ++i ) {
        LogsEvent* event = qVariantValue<LogsEvent*>(
                d->m_AbstractLogsModel->data(d->m_AbstractLogsModel->index(i, 0), LogsModel::RoleFullEvent) );
        
        if ( event ) {
            HItemPointer item(new HistoryItem());
            readLogEvent(event, *item);
            d->m_logsMap.insert(i, item);
            d->m_List.append( item );
        }
    }
    
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
void CntHistoryModel::logsRowsRemoved(const QModelIndex& /*parent*/, int first, int last)
{
    QList< int > indices;
    
    for ( int i = first; i <= last; ++i ) {
        HItemPointer item = d->m_logsMap.value( i );
        int index = d->m_List.indexOf( item );
        d->m_List.removeOne( item );
        d->m_logsMap.remove( i );        
        indices.append( index );
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
void CntHistoryModel::logsDataChanged(const QModelIndex& first, const QModelIndex& last)
{
    int f = first.row();
    int l = last.row();
    QList< int > indices;
    
    for ( int i = f; i < d->m_AbstractLogsModel->rowCount() && i <= l; ++i ) {
        
        LogsEvent* event = qVariantValue<LogsEvent*>(
                d->m_AbstractLogsModel->data(d->m_AbstractLogsModel->index(i, 0), LogsModel::RoleFullEvent) );
        
        // Fetch item from the mapped logs model items
        HItemPointer item = d->m_logsMap.value( i );
        
        // Found item in the logs map
        if ( !item.isNull() ) {
            int index = d->m_List.indexOf( item );
            readLogEvent( event, *item );
            indices.append( index );
        }
    }
    
    // Emit dataChanged signal only if there were updates
    if ( !indices.isEmpty() ) {
        QList< QList<int> > batches = findIndices( indices );
        foreach( QList<int> l, batches )
            emit dataChanged( index(l.first(), 0), index(l.last(), 0) );
    }
}

/*!
 * Clear logs event slot received from logs model
 *
 * \param err Error of the clear logs request
 */
void CntHistoryModel::clearedCallLogs(int err)
{
    if ( err != 0 ) {
        return;
    }
    
    QList< int > indices;
    foreach( HItemPointer p, d->m_logsMap.values() ) {
        d->m_List.removeOne( p );
    }
    d->m_logsMap.clear();
    
    // Remove list items in batches
    if ( !indices.isEmpty() ) {
        QList< QList<int> > batches = findIndices(indices);
        foreach( QList<int> l, batches ) {
            beginRemoveRows( QModelIndex(), l.first(), l.last() );
            endRemoveRows();
        }
    }
}

/*!
 * Mark events as seen slot received from logs model
 *
 * \param err Error of the marking logs request
 */
void CntHistoryModel::markingCompleted(int err)
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

bool CntHistoryModel::validateRowIndex( const int index) const
{
    return( index < rowCount() && index >= 0 );
}

/*!
 * Find the sequences of indices for the given indices from the private list
 * 
 * \param indices List of indices
 * \return sequences of indices
 */
QList< QList<int> > CntHistoryModel::findIndices( const QList< int >& indices )
{
    QList< QList<int> > sequences;
    QList<int> currSequence;
    int prevIndex = indices.at(0) - 1;
    
    for (int i = 0; i < indices.count(); i++)
    {
        int currIndex = indices.at(i);
        
        if (currIndex >= 0)
        {
            if ( prevIndex+1 != currIndex && !currSequence.isEmpty() )
            {
                sequences.append( currSequence );
                currSequence.clear();
            } 
            currSequence.append( currIndex );
            prevIndex = currIndex;
        }
    }
    
    if (!currSequence.isEmpty())
    {
        // Add last sequence if such exist
        sequences.append( currSequence );
    }
    
    return sequences;
}

/*!
 * Read message event into a history item
 *
 * \param event Message event
 * \param item Conversation history item
 */
void CntHistoryModel::readMsgEvent(MsgItem& event, HistoryItem& item)
{
    // Msg direction
    if ( event.direction() == MsgItem::MsgDirectionIncoming )
        item.direction = CntHistoryModel::Incoming;
    if ( event.direction() == MsgItem::MsgDirectionOutgoing )
        item.direction = CntHistoryModel::Outgoing;
    
    // Read status
    /* TODO: This API available in the next release
	 *if ( event.isAttributeSet(MsgItem::MsgAttributeUnread) )
        item.seenStatus = CntHistoryModel::Unseen;
    else
        item.seenStatus = CntHistoryModel::Seen;*/
    
    item.msgType = CntHistoryModel::Message;
    item.number = event.phoneNumber(); 
    item.iconPath = QString("qtg_small_message");
    item.message = event.body();
    item.timeStamp = event.timeStamp();
}

/*!
 * Slot to receive new messages for the first time
 * from the messages model
 *
 * \param event Message event
 * \param item Conversation history item
 */
void CntHistoryModel::messagesReady(QList<MsgItem>& msgs)
{
    int oldRowCount = rowCount();
    
    foreach( MsgItem m, msgs ) {
        // Create a new hst item
        HItemPointer item(new HistoryItem());
        
        // Parse the MsgItem and add data into hst item
        readMsgEvent( m, *item );
        
        // Map the hst item to a MsgItem in the msgModel
        d->m_msgMap.insert( m.id(), item );
        
        // Append the hst item to our list
        d->m_List.append( item );
    }
    
    beginInsertRows(QModelIndex(), oldRowCount, rowCount());
    endInsertRows();
    
    // After all messagas are fetched sort them and
    // refresh the UI.
    sortAndRefresh();
}

/*!
 * Slot to receive new messages from the messages model
 *
 * \param event Message event
 * \param item Conversation history item
 */
void CntHistoryModel::messageAdded(MsgItem& msg)
{
    int oldRowCount = rowCount();
    
    // Create a new hst item
    HItemPointer item(new HistoryItem());
    
    // Parse the MsgItem and add data into hst item
    readMsgEvent( msg, *item );
    
    // Map the hst item to a MsgItem in the msgModel
    d->m_msgMap.insert( msg.id(), item );
    
    // Append the hst item to our list
    d->m_List.append( item );
    
    beginInsertRows( QModelIndex(), oldRowCount, rowCount() );
    endInsertRows();
}

/*!
 * Slot to update a message from the messages model
 *
 * \param event Message event
 * \param item Conversation history item
 */
void CntHistoryModel::messageChanged(MsgItem& msg)
{
    // Fetch the hst item that maps to this MsgItem
    HItemPointer p = d->m_msgMap.value( msg.id() );
    
    // No item was found.
    if ( p.isNull() )
        return;

    // Parse the MsgItem and add data into hst item
    readMsgEvent(msg, *p);
    
    // Get the index of the the hst item in the list
    int pIndex = d->m_List.indexOf( p );
    
    emit dataChanged(index(pIndex, 0), index(pIndex, 0));
}

/*!
 * Slot to delete a message from the messages model
 *
 * \param event Message event
 * \param item Conversation history item
 */
void CntHistoryModel::messageDeleted(MsgItem& msg)
{
    // Fetch the hst item that maps to this MsgItem
    HItemPointer p = d->m_msgMap.value( msg.id() );
    
    // No item was found.
    if ( p.isNull() )
        return;
    
    // Remove the item in stored containers
    int count = d->m_msgMap.remove( msg.id() );
    int index = d->m_List.indexOf( p );
    bool removed = d->m_List.removeOne( p );
    
    if ( removed && count > 0 ) {
        beginRemoveRows(QModelIndex(), index, index);
        endRemoveRows();
    }
}
