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

#ifndef MOBHISTORYMODELPRIVATE_H
#define MOBHISTORYMODELPRIVATE_H

#include <QSharedData>
#include <QMap>
#include <QDateTime>
#include <qtcontacts.h>
#include <hbicon.h>

#ifdef PBK_UNIT_TEST
#include "stub_classes.h"
#else
#include "logsevent.h"
#include "logsmodel.h"
#include "logscustomfilter.h"
#include "messagemodel.h"
#include "conversationsmodel.h"
#include "conversationdefines.h"

struct TableRow{
    int direction; // 0 - incoming, 1 - outgoing, 2 - missed
    int seenStatus;    // 3 = unread, 4 = read
    QString brandedIconPath;
    QString title;
    QString message;
    QDateTime timeStamp;
    QString mmsMessage;
    
    TableRow() :
        direction(0),
        seenStatus(4),
        brandedIconPath(QString()),
        title(QString()),
        message(QString()),
        timeStamp(QDateTime()),
        mmsMessage(QString())
    {};
    
    const TableRow& operator=(const TableRow& other)
    {        
        direction = other.direction;
        brandedIconPath = other.brandedIconPath;
        title = other.title;
        message = other.message;
        timeStamp = other.timeStamp;
        return *this;
    };
};
#endif

class MobHistoryModelData : public QSharedData
{
public:
    MobHistoryModelData(QContactLocalId contactId, QContactManager* manager)
        : QSharedData(),
          m_logsModel(0),
          m_logsFilter(0),
          m_AbstractLogsModel(0),
          m_messageModel(0),
          m_conversationsModel(0),
          m_contactId(contactId),
          m_contactManager(manager)
          {}
    ~MobHistoryModelData()
    {
        if (m_logsModel) {
            delete m_logsModel;
            m_logsModel = 0;
        }
        if (m_logsFilter) {
            delete m_logsFilter;
            m_logsFilter = 0;
        }
    };
    
public:
    LogsModel* m_logsModel;
    LogsCustomFilter* m_logsFilter;
    QAbstractItemModel* m_AbstractLogsModel;
	// Not own: Static object
    MessageModel* m_messageModel;
	// Not own: Static object
    ConversationsModel* m_conversationsModel;
    QContactLocalId m_contactId;
    QContactManager* m_contactManager;
    bool m_isMyCard;
    //this contains merged calls and messages history
    QList<TableRow> m_table;
};

#endif

