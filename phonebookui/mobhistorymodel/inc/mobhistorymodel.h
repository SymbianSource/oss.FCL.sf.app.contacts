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
#ifndef MOBHISTORYMODEL_H
#define MOBHISTORYMODEL_H

#include <QAbstractListModel>
#include <QSharedData>
#include <qtcontacts.h>

#include "mobhistorymodelglobal.h"

QTM_USE_NAMESPACE

class MobHistoryModelData;
class LogsEvent;
class HistoryItem;

/*!
 * MobHistoryModel is a model used to fetch conversation history
 * (SMS & call logs) from databases. It will 
 * cache content from the databases to be displayed on the screen
 * by the conversation view.
 * 
 * Note that that this is a prototype implementation and does
 * not yet support more advanced features.
 */
class MOBHISTORYMODEL_EXPORT MobHistoryModel : public QAbstractListModel
{    
    Q_OBJECT
    
public:
    enum ConversationDirection
        {
        Incoming = 0,
        Outgoing,
        Missed
        };
    
    enum ReadStatus
        {
        Unseen = 3,
        Seen
        };
    
    enum ItemType
        {
        CallLog = 5,
        Message
        };
    
    enum CustomRoles
        {
        SeenStatusRole = Qt::UserRole + 1,
        DirectionRole,
        ItemTypeRole,
        PhoneNumberRole
        };
    
public:
    MobHistoryModel(QContactLocalId contactId,
                    QContactManager* manager,
                    QObject *parent = 0);
    ~MobHistoryModel();
    
public: // from QAbstractTableModel/QAbstractItemModel
    QVariant data(const QModelIndex& index, int role) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    void sort(int column = 0, Qt::SortOrder order = Qt::AscendingOrder);
    void clearHistory();
    void markAllAsSeen();
    void sortAndRefresh(Qt::SortOrder order = Qt::AscendingOrder);
    
private:
    void initializeModel();
    QVariant displayRoleData(const HistoryItem& item) const;
    
    // Utility finctions
    void readLogEvent(LogsEvent* event, HistoryItem& item);
    void initializeLogsModel();
    bool validateRowIndex(const int index) const;
    QList< QList<int> > findIndices( const QList< int >& indices );

private slots:
    void logsRowsInserted(const QModelIndex& parent, int first, int last);
    void logsRowsRemoved(const QModelIndex& parent, int first, int last);
    void logsDataChanged(const QModelIndex& first, const QModelIndex& last);
    void clearedCallLogs(int err);
    void markingCompleted(int err);
    
private:
	QSharedDataPointer<MobHistoryModelData> d;
};
#endif
