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
#include <qtcontacts.h>

#ifdef PBK_UNIT_TEST
#include "stub_classes.h"
#else
#include <logsevent.h>
#include <logsmodel.h>
#include <logscustomfilter.h>
#endif
#include <msghistory.h>
#include <msgitem.h>
#include <hbextendedlocale.h>

QTM_USE_NAMESPACE

// Constants
#define MISSED_CALL_ICON "qtg_small_missed_call"
#define DAILED_CALL_ICON "qtg_small_sent"
#define RECEIVED_CALL_ICON "qtg_small_received"
#define MESSAGE_ICON "qtg_small_message"

class HistoryItem
{   
public:   
    HistoryItem() : flags(0)
    {};
    
    inline const HistoryItem& operator=(const HistoryItem& other)
    {
        flags = other.flags;
        number = other.number;
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
    int flags;
    QString number;
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
          m_isMyCard(false),
          m_isMarkedAsSeen(false),
          m_initLogs(false),
          m_extendedLocale(HbExtendedLocale::system())
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
    bool m_initLogs;
    //this contains merged calls and messages history
    QList<HItemPointer> m_List;
    QMap<int, HItemPointer> m_logsMap;
    QMap<int, HItemPointer> m_msgMap;
    const HbExtendedLocale m_extendedLocale;
};

#endif

