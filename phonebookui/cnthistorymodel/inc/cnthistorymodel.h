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
#ifndef CNTHISTORYMODEL_H
#define CNTHISTORYMODEL_H

#include <QAbstractListModel>
#include <QSharedData>
#include <qmobilityglobal.h>
#include <qcontactid.h>

#include "cnthistorymodelglobal.h"

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntHistoryModelData;
class LogsEvent;
class HistoryItem;
class MsgItem;

/*!
 * CntHistoryModel is a model used to fetch conversation history
 * (SMS & call logs) from databases. It will 
 * cache content from the databases to be displayed on the screen
 * by the conversation view.
 * 
 * Note that that this is a prototype implementation and does
 * not yet support more advanced features.
 */
class CNTHISTORYMODEL_EXPORT CntHistoryModel : public QAbstractListModel
{    
    Q_OBJECT
    
public:
    enum Attributes
        {
        Incoming = 1,
        Outgoing = 2,
        Unseen = 4,
        Seen = 8,
        Attachment = 16
        };
    
    enum ItemType
        {
        CallLog = 32,
        Message = 64
        };
    enum Icon
        {
        MissedCall = 128,
        ReceivedCall = 256,
        DialledCall = 512
        };
    
    enum CustomRoles
        {
        FlagsRole = Qt::UserRole + 1,
        PhoneNumberRole
        };
    
public:
    CntHistoryModel(QContactLocalId contactId,
                    QContactManager* manager,
                    QObject *parent = 0);
    ~CntHistoryModel();
    
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
    QVariant decorationRoleData(const HistoryItem& item) const;
    QVariant backgroundRoleData(const HistoryItem& item) const;
    
    // Utility finctions
    void readLogEvent(LogsEvent* event, HistoryItem& item);
    void readMsgEvent(MsgItem& event, HistoryItem& item);
    void initializeLogsModel();
    void initializeMsgModel();
    bool validateRowIndex(const int index) const;
    QList< QList<int> > findIndices( const QList< int >& indices );

private slots:
    // Logs model slots
    void logsRowsInserted(const QModelIndex& parent, int first, int last);
    void logsRowsRemoved(const QModelIndex& parent, int first, int last);
    void logsDataChanged(const QModelIndex& first, const QModelIndex& last);
    void handleLogsReset();
    
    // Messaging model slots
    void messagesReady(QList<MsgItem>& msgs);
    void messageAdded(MsgItem& msg);
    void messageChanged(MsgItem& msg);
    void messageDeleted(MsgItem& msg);
    
private:
	QSharedDataPointer<CntHistoryModelData> d;
    
    // Testing related friend definitions    
    friend class TestCntHistoryModel;
};
#endif
