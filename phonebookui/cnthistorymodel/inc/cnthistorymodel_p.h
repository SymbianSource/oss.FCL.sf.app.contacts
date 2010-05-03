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

#ifndef CNTHISTORYMODELPRIVATE_H
#define CNTHISTORYMODELPRIVATE_H

#include <QSharedData>
#include <QSharedPointer>
#include <QMap>
#include <QDateTime>
#include <qcontactmanager.h>

QTM_USE_NAMESPACE

#include <logsevent.h>
#include <logsmodel.h>
#include <logscustomfilter.h>
#include <msghistory.h>

class HistoryItem 
{   
public:   
    HistoryItem() :
        seenStatus(4)
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
    {
         return timeStamp == other.timeStamp;
    }
    
public:
    int direction; // 0 - incoming, 1 - outgoing, 2 - missed
    int seenStatus;    // 3 = unseen, 4 = seen
    int msgType;    // 5 = call log, 6 = message
    QString number; 
    QString iconPath;
    QString title;
    QString message;
    QDateTime timeStamp;
};

typedef QSharedPointer<HistoryItem> HItemPointer;

class CntHistoryModelData : public QSharedData
{
public:
    CntHistoryModelData(QContactLocalId contactId, QContactManager* manager)
        : QSharedData(),
          m_logsModel(NULL),
          m_logsFilter(NULL),
          m_AbstractLogsModel(NULL),
          m_msgHistory(NULL),
          m_contactId(contactId),
          m_contactManager(manager),
          m_isMarkedAsSeen(false)
          {}
    ~CntHistoryModelData()
    {
        if (m_logsModel) {
            delete m_logsModel;
            m_logsModel = NULL;
        }
        if (m_logsFilter) {
            delete m_logsFilter;
            m_logsFilter = NULL;
        }
        if (m_msgHistory) {
            delete m_msgHistory;
            m_msgHistory = NULL;
        }
    };
    
public:
    LogsModel* m_logsModel;
    LogsCustomFilter* m_logsFilter;
    QAbstractItemModel* m_AbstractLogsModel;
    MsgHistory* m_msgHistory;
	QContactLocalId m_contactId;
    QContactManager* m_contactManager;
    bool m_isMyCard;
    bool m_isMarkedAsSeen;
    //this contains merged calls and messages history
    QList<HItemPointer> m_List;
    QMap<int, HItemPointer> m_logsMap;
    QMap<int, HItemPointer> m_msgMap;
};

#endif

