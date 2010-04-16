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
#include <qcontactmanager.h>

QTM_USE_NAMESPACE

#ifdef PBK_UNIT_TEST
#include "stub_classes.h"
#else
#include <logsevent.h>
#include <logsmodel.h>
#include <logscustomfilter.h>

struct HistoryItem {
    int direction; // 0 - incoming, 1 - outgoing, 2 - missed
    int seenStatus;    // 3 = unseen, 4 = seen
    int msgType;    // 5 = call log, 6 = message
    QString number; 
    QString iconPath;
    QString title;
    QString message;
    QDateTime timeStamp;
    QString mmsMessage;
    
    HistoryItem() :
        direction(0),
        seenStatus(4),
        iconPath(QString()),
        title(QString()),
        message(QString()),
        timeStamp(QDateTime()),
        mmsMessage(QString())
    {};
    
    inline const HistoryItem& operator=(const HistoryItem& other)
    {        
        direction = other.direction;
        seenStatus = other.seenStatus;
        msgType = other.msgType;
        number = other.number;
        iconPath = other.iconPath;
        title = other.title;
        message = other.message;
        timeStamp = other.timeStamp;
        return *this;
    }
    
    inline bool operator==(const HistoryItem& other) const
    { return timeStamp == other.timeStamp; }
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
          m_contactId(contactId),
          m_contactManager(manager),
          m_isMarkedAsSeen(false)
          {}
    ~MobHistoryModelData()
    {
        if (m_logsModel) {
            delete m_logsModel;
            m_logsModel = NULL;
        }
        if (m_logsFilter) {
            delete m_logsFilter;
            m_logsFilter = NULL;
        }
    };
    
public:
    LogsModel* m_logsModel;
    LogsCustomFilter* m_logsFilter;
    QAbstractItemModel* m_AbstractLogsModel;
	QContactLocalId m_contactId;
    QContactManager* m_contactManager;
    bool m_isMyCard;
    bool m_isMarkedAsSeen;
    //this contains merged calls and messages history
    QList<HistoryItem> m_table;
    QMap<int,int> m_LogsMap;
};

#endif

