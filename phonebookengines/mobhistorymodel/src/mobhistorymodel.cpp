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
#include <QSqlDatabase>
#include <QtAlgorithms>
#ifndef PBK_UNIT_TEST
#include <hbicon.h>
#endif
#include "mobhistorymodel_p.h"
#include "mobhistorymodel.h"

// Unnamed namespace for helper functions
namespace
{
    bool greaterThan(const TableRow& t1, const TableRow& t2)
    {
        return t1.timeStamp > t2.timeStamp;
    }
    
    bool lessThan(const TableRow& t1, const TableRow& t2)
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
    if (!index.isValid())
        return QVariant();
    
    int row = index.row();
    
    // check that row and column are Ok
    if (!validRowId(row))
        return QVariant();
    
    switch(role)
    {
        case Qt::DisplayRole:
            return displayRoleData(row);
        case Qt::DecorationRole:
            return decorationRoleData(row);
        case SeenStatusRole:
            return seenStatusRoleData(row);
        case DirectionRole:
            return directionRoleData(row);
        default:
            return QVariant();
    }
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
    if (order == Qt::AscendingOrder)
        qStableSort(d->m_table.begin(), d->m_table.end(), lessThan);
    else
        qStableSort(d->m_table.begin(), d->m_table.end(), greaterThan);
}

/*!
 * Clear all history from the database. If the history cached
 * is specific to one contact, only that history is cleared.
 * 
 */
void MobHistoryModel::clearHistory()
{

}

/*!
 * Create the model structure and cache history data from
 * conversations and call logs databases.
 *
 */
void MobHistoryModel::initializeModel()
{
    //populate model with call events
    d->m_logsModel = new LogsModel(LogsModel::LogsFullModel);
    if (!d->m_isMyCard) {
        //do the filtering to get call events for the target contact
        d->m_logsFilter = new LogsCustomFilter;
        d->m_logsFilter->setContactId(d->m_contactId);
        d->m_logsFilter->setSourceModel(d->m_logsModel);
        d->m_AbstractLogsModel = d->m_logsFilter;
    } else {
        //get all call events
        d->m_AbstractLogsModel = d->m_logsModel;
    }
    
    //read first call events and start listening for more 
    readEvents(0, d->m_AbstractLogsModel->rowCount());
    connect(d->m_AbstractLogsModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), 
            this, SLOT(dataFromLogsModel(const QModelIndex &, int, int)));
    
    // Messages database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("c:\\private\\20024328\\conversations.db");
    if (!db.isOpen())
        db.open();
    
    // All messages history model
    if (d->m_isMyCard) {
        d->m_conversationsModel = ConversationsModel::instance();
        // Connect to the MessageModel's signal
        connect(d->m_conversationsModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                this, SLOT(dataFromConversationsModel(const QModelIndex&, int, int)));
        //d->m_conversationsModel->refreshModel();
        
    // Person-centric messages history model
    } else {
        d->m_messageModel = MessageModel::instance();
        // Connect to the MessageModel's signal
        connect(d->m_messageModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                this, SLOT(dataFromMessageModel(const QModelIndex&, int, int)));
        d->m_messageModel->fetchMessages(d->m_contactId);
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
QVariant MobHistoryModel::displayRoleData(int row) const
{
    TableRow tableRow = d->m_table.at(row);
    QStringList list;
    
    // Title
    QString title = tableRow.title;
    
    // Message
    QString msg = tableRow.message;
    
    // Time stamp string
    QString time = tableRow.timeStamp.toString();
    
    list << title << msg << time;
    
    return QVariant(list);
}

/*!
 * Return the data to be used by the view for a decoration role.
 *
 * \param row The row of the item to return data about.
 * \param column The column of the item to return data about.
 *  return QVariant Path of the icon
 */
QVariant MobHistoryModel::decorationRoleData(int row) const
{
    TableRow tableRow = d->m_table.at(row);
    return QVariant(tableRow.brandedIconPath);
}

/*!
 * Return the data to be used by the view for a seen status role.
 *
 * \param row The row of the item to return data about.
 * \param column The column of the item to return data about.
 *  return QVariant The status data for the specified index.
 */
QVariant MobHistoryModel::seenStatusRoleData(int row) const
{
    TableRow tableRow = d->m_table.at(row);
    return QVariant(tableRow.seenStatus);
}

/*!
 * Return the data to be used by the view for a direction role.
 *
 * \param row The row of the item to return data about.
 * \param column The column of the item to return data about.
 *  return QVariant The direction data for the specified index.
 */
QVariant MobHistoryModel::directionRoleData(int row) const
{
    TableRow tableRow = d->m_table.at(row);
    return QVariant(tableRow.direction);
}

/*!
 * Verify specified row id is valid.
 *
 * \param row A row number
 * \return bool indicating validity of row id
 */
bool MobHistoryModel::validRowId(int row) const
{
    return (row >= 0 && row < rowCount());
}

/*!
 * Adds call events to the combined (calls + messages) model
 *
 * \param first First new row in call's model
 * \param last Last new row in call's model
 */
void MobHistoryModel::readEvents(int first, int last)
{
    for (int i = first; i < d->m_AbstractLogsModel->rowCount() && i <= last; ++i) {
        LogsEvent* event = qVariantValue<LogsEvent*>(
                d->m_AbstractLogsModel->data(d->m_AbstractLogsModel->index(i, 0), LogsModel::RoleFullEvent) );
        
        if (event) {
            TableRow row;
            QString bodyText;
            QString title;
            if (d->m_isMyCard) {
                if (event->remoteParty().length() > 0) {
                    title = event->remoteParty();
                }
                else {
                    title = event->number();
                }
            }
            else {
                if (event->direction() == LogsEvent::DirIn) {
                    bodyText = QString("Incoming call");
                } else if (event->direction() == LogsEvent::DirOut) {
                    bodyText = QString("Outgoing call");
                } else if (event->direction() == LogsEvent::DirMissed) {
                    bodyText = QString("Missed call");
                    //if (!event->isRead())
                    //    row.seen_status = Unseen;
                }
            }

            row.direction = Incoming;
            if (event->direction() == LogsEvent::DirOut) {
                row.direction = Outgoing;
            }
            row.message = bodyText;
            row.title = title;
            row.timeStamp = event->time();
            
            QList<QVariant> icons = d->m_AbstractLogsModel->index(i, 0).data(Qt::DecorationRole).toList();
            if (!icons.isEmpty()) {
                row.brandedIconPath = qVariantValue<HbIcon>(icons.at(0)).iconName();
            }
            
            d->m_table.append(row);
        }
    }
    sort();
}

/*!
 * Slot used for receiving new rows from the MessageModel.
 *
 * \param parent Optional parent index value.
 * \param first The first row item to be received from the model.
 * \param last The last row item to be received from the model.
 */
void MobHistoryModel::dataFromMessageModel(const QModelIndex& /*parent*/, int first, int last)
{
    // Add conversation items received
    int oldRowCount = rowCount();
    
    for(int i = first; i<last; i++) {
        QModelIndex index = d->m_messageModel->index(i, 1); // column value not used in msg model
        
        // Create and populate a new row to add to table from the messages model
        TableRow row;
        
        row.timeStamp.setTime_t(d->m_messageModel->data(index, TimeStamp).toInt());
        
        row.message = d->m_messageModel->data(index, BodyText).toString();
        if (d->m_isMyCard)
            row.message.prepend("Me: ");
        
        if (d->m_messageModel->data(index, Direction).toInt() == 0)
            row.direction = Incoming;
        else
            row.direction = Outgoing;
        
        if (d->m_messageModel->data(index, UnReadStatus).toBool())
            row.seenStatus = Unseen;
        else
            row.seenStatus = Seen;
        
        // Insert the row to our data model table
        d->m_table.append(row);
    }
    sort();
    
    // The items are sorted base on time. All new items added to the table
    // will end up at the end of the list.
    beginInsertRows(QModelIndex(), oldRowCount, rowCount());
    endInsertRows();
}

/*!
 * Slot used for receiving new rows from the ConversationModel.
 *
 * \param parent Optional parent index value.
 * \param first The first row item to be received from the model.
 * \param last The last row item to be received from the model.
 */
void MobHistoryModel::dataFromConversationsModel(const QModelIndex& /*parent*/, int first, int last)
{
    // Add conversation items received
    int oldRowCount = rowCount();
    
    for(int i = first; i<last; i++) {
        QModelIndex index = d->m_conversationsModel->index(i, 1); // column value not used in msg model
        
        // Create and populate a new row to add to table from the messages model
        TableRow row;
        
        row.title = d->m_conversationsModel->data(index, FirstName).toString();
        row.title.append(" ");
        row.title.append(d->m_conversationsModel->data(index, LastName).toString());
        
        row.timeStamp.setTime_t(d->m_conversationsModel->data(index, TimeStamp).toInt());
        row.message = d->m_conversationsModel->data(index, BodyText).toString();
        
        if (d->m_conversationsModel->data(index, Direction).toInt() == 0)
            row.direction = Incoming;
        else
            row.direction = Outgoing;
        
        if (d->m_conversationsModel->data(index, UnReadStatus).toBool())
            row.seenStatus = Unseen;
        else
            row.seenStatus = Seen;
        
        // Insert the row to our data model table.
        d->m_table.append(row);
    }
    sort();

    // The items are sorted base on time. All new items added to the table
    // will end up at the end of the list.
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
void MobHistoryModel::dataFromLogsModel(const QModelIndex& /*parent*/, int first, int last)
{
    int oldRowCount = rowCount();
    readEvents(first, last);
    beginInsertRows(QModelIndex(), oldRowCount, rowCount());
    endInsertRows();
}
