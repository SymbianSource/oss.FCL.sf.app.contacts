/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "stub_classes.h"

Q_DECLARE_METATYPE(LogsEvent *)

ConversationsModel* ConversationsModel::instance()
{
    static ConversationsModel* model = new ConversationsModel();
    return model;
}

void ConversationsModel::refreshModel()
{
    int rows = 7;
    
    for(int i=0; i<rows; i++) {
        TableRow row;
        row.timeStamp = QDateTime::currentDateTime();
        row.message = QString(cc_msg).arg(i);
        row.direction = Incoming;
        QTest::qWait(1000);
        m_table.append(row);
    }
    m_table[1].direction = Outgoing;
    beginInsertRows(QModelIndex(), 0, rows);
    endInsertRows();   
}

QVariant ConversationsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
         
    int row = index.row();
    if (row >= rowCount())
        return QVariant();
    
    switch(role)
    {
        case TimeStamp:
            return QVariant(m_table[row].timeStamp.toTime_t());
        case BodyText:
            return QVariant(m_table[row].message);
        case Direction:
            return QVariant(m_table[row].direction);
        default:
            return QVariant();
    }
    return QVariant();
}

MessageModel* MessageModel::instance()
{
    static MessageModel* messageModel = new MessageModel();
    return messageModel;
}

void MessageModel::fetchMessages(int /*cnt*/)
{
    int rows = 5;
    
    for(int i=0; i<rows; i++) {
        TableRow row;
        row.timeStamp = QDateTime::currentDateTime();
        row.message = QString(cc_msg).arg(i);
        row.direction = Incoming;
        QTest::qWait(1000);
        m_table.append(row);
    }
    m_table[1].direction = Outgoing;
    beginInsertRows(QModelIndex(), 0, rows);
    endInsertRows();
}


QVariant MessageModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
         
    int row = index.row();
    if (row >= rowCount())
        return QVariant();
    
    switch(role)
    {
        case TimeStamp:
            return QVariant(m_table[row].timeStamp.toTime_t());
        case BodyText:
            return QVariant(m_table[row].message);
        case Direction:
            return QVariant(m_table[row].direction);
        default:
            return QVariant();
    }
    return QVariant();
}

QVariant LogsModel::data(const QModelIndex& /*index*/, int /*role*/) const
{
    QVariant var = qVariantFromValue(m_event);
    return var;
}

QVariant LogsCustomFilter::data(const QModelIndex& /*index*/, int /*role*/) const
{
    QVariant var = qVariantFromValue(m_event);
    return var;
}

LogsEvent::LogsEvent()
{
    QDateTime m_time = QDateTime::currentDateTime();
    QString m_num = "1234567";
    QString m_remoteParty = "Caller's name";
    LogsDirection m_dir = DirIn;
}

LogsModel::LogsModel(LogsModelType /*modelType*/)
{
    m_event = new LogsEvent();
    beginInsertRows(QModelIndex(), 0, 1);
    endInsertRows();
}

LogsCustomFilter::LogsCustomFilter()
{
    m_event = new LogsEvent();
    beginInsertRows(QModelIndex(), 0, 1);
    endInsertRows();
}
