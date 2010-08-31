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

#include "cnthistorymodel_p.h"
#include "cnthistorymodel.h"

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
    : QAbstractListModel(parent),
      d_ptr(new CntHistoryModelPrivate(contactId, manager))
{
    Q_D(CntHistoryModel);
    d->q_ptr = this;
}

CntHistoryModel::~CntHistoryModel()
{
    Q_D(CntHistoryModel);
    delete d;
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
    Q_D(const CntHistoryModel);
    return d->data(index, role);
}

/*!
 * Get the number of rows (conversations) in this model.
 *
 * \param parent Optional parent index value.
 * \return Number of rows in this model.
 */
int CntHistoryModel::rowCount(const QModelIndex& parent) const
{
    Q_D(const CntHistoryModel);
    return d->rowCount(parent);
}

/*!
 * Sort list items on the model.
 *
 * \param column Column to be sorted. It is not used.
 * \param order Order to sort the list items.
 */
void CntHistoryModel::sort(int column, Qt::SortOrder order)
{
    Q_D(CntHistoryModel);
    d->sort(column, order);
}

/*!
 * Clear history from the database. If the history cached
 * is specific to one contact, only that history is cleared.
 * 
 */
void CntHistoryModel::clearHistory()
{    
    Q_D(CntHistoryModel);    
    d->clearHistory();
}

/*!
 * Mark all the conversations in the view as seen.
 * 
 */
void CntHistoryModel::markAllAsSeen()
{
    Q_D(CntHistoryModel);
    d->markAllAsSeen();
}

/*!
 * Sort items in the model and refresh the view
 * 
 */
void CntHistoryModel::sortAndRefresh(Qt::SortOrder order)
{
    Q_D(CntHistoryModel);
    d->sort(order);
    beginInsertRows(QModelIndex(), 0, rowCount());
    endInsertRows();
}

void CntHistoryModel::doBeginInsertRows(const QModelIndex &parent, int first, int last)
{
    beginInsertRows(parent, first, last);
}

void CntHistoryModel::doEndRemoveRows()
{
    endRemoveRows();
}

void CntHistoryModel::doBeginRemoveRows(const QModelIndex &parent, int first, int last)
{
    beginRemoveRows(parent, first, last);
}

void CntHistoryModel::doEndInsertRows()
{
    endInsertRows();
}

void CntHistoryModel::doDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    emit dataChanged(topLeft, bottomRight);
}
