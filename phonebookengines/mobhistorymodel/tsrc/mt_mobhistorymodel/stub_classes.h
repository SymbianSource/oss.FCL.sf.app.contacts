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
#ifndef STUB_CLASSES_H
#define STUB_CLASSES_H

#include <QString>
#include <QDateTime>
#include <QtTest/QtTest>
#include <QAbstractItemModel>

#include <conversationdefines.h>

#define OWN_ROWS 10
#define CC_ROWS 5
const QString own_msg = QString("Own card view message %1 added");
const QString cc_msg = QString("Contact message %1 added");

struct TableRow{
    int direction; // 0 - incoming, 1 - outgoing, 2 - missed
    QString brandedIcon;
    QString title;
    QString message;
    QDateTime timeStamp;
    QString mms_message;
    
    const TableRow& operator=(const TableRow& other)
    {        
        direction = other.direction;
        brandedIcon = other.brandedIcon;
        title = other.title;
        message = other.message;
        timeStamp = other.timeStamp;
        return *this;
    };
};

enum
{
   TimeStamp,
   Direction,
   BodyText
};

enum
{
    Incoming,
    Outgoing,
    Missed
};

class LogsEvent
{
public:
    enum LogsDirection
        {
        DirUndefined = -1, // undefined
        DirIn =  0,        // incoming
        DirOut = 1,        // outgoing
        DirMissed = 2      // missed
        };

public:
    LogsEvent();
    ~LogsEvent(){};
    QDateTime time() const{return m_time;}
    const QString& number() const{return m_num;}
    const QString& remoteParty() const{return m_remoteParty;}
    LogsDirection direction() const{return m_dir;}
    
private:
    QDateTime m_time;
    QString m_num;
    QString m_remoteParty;
    LogsDirection m_dir;
};

class LogsModel : public QAbstractItemModel
{
public:
    
    enum LogsModelType
    {
        RoleFullEvent,
        LogsFullModel
    };
    
public:
    LogsModel(LogsModelType /*modelType*/);
    ~LogsModel(){delete m_event;}
    
    QVariant data(const QModelIndex& index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex& /*parent*/ = QModelIndex()) const
        {return createIndex(row, column);}
    QModelIndex parent(const QModelIndex& /*index*/) const
        {return QModelIndex();}
    int rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const
        {return 1;}
    int columnCount(const QModelIndex& /*parent*/ = QModelIndex()) const
        {return 0;}
private:
    LogsEvent* m_event;
};

class LogsCustomFilter : public QAbstractItemModel
{    
public:
    LogsCustomFilter();
    ~LogsCustomFilter(){delete m_event;}
    void setContactId(int /*cntId*/){};
    void setSourceModel(QAbstractItemModel* /*srcModel*/){};
    
    QVariant data(const QModelIndex& index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex& /*parent*/ = QModelIndex()) const
        {return createIndex(row, column);}
    QModelIndex parent(const QModelIndex& /*index*/) const
        {return QModelIndex();}
    int rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const
        {return 1;}
    int columnCount(const QModelIndex& /*parent*/ = QModelIndex()) const
        {return 3;};
            
private:
    LogsEvent* m_event;
};


class ConversationsModel : public QAbstractItemModel
{
public:
    static ConversationsModel* instance();
    ConversationsModel(){};
    ~ConversationsModel(){};
    
    void refreshModel();
    QVariant data(const QModelIndex& index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex& /*parent*/ = QModelIndex()) const
        {return createIndex(row, column);};
    QModelIndex parent(const QModelIndex& /*index*/) const
        {return QModelIndex();}
    int rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const
        {return m_table.size();}
    int columnCount(const QModelIndex& /*parent*/ = QModelIndex()) const
        {return 0;}
            
private:
    QList<TableRow> m_table;
};

class MessageModel : public QAbstractItemModel
{
public:
    static MessageModel* instance();
    MessageModel(){};
    ~MessageModel(){};
    
    void fetchMessages(int cnt);
    QVariant data(const QModelIndex& index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex& /*parent*/ = QModelIndex()) const
        {return createIndex(row, column);};
    QModelIndex parent(const QModelIndex& /*index*/) const
        {return QModelIndex();};
    int rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const
        {return m_table.size();}
    int columnCount(const QModelIndex& /*parent*/ = QModelIndex()) const
        {return 0;}
            
private:
    QList<TableRow> m_table;
};

#endif
