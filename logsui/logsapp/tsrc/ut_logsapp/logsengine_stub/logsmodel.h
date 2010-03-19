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
#ifndef LOGSMODEL_H
#define LOGSMODEL_H

#include <logsexport.h>
#include "logsabstractmodel.h"
#include "logsengdefs.h"
#include <QStringList>

class LogsMatchesModel;

/**
 * 
 */
class LogsModel : public LogsAbstractModel
{
public:

    enum ClearType {
                TypeLogsClearAll = 0,
                TypeLogsClearReceived,
                TypeLogsClearCalled,
                TypeLogsClearMissed
                };

    enum LogsModelType {
        LogsRecentModel,
        LogsFullModel
    };

public: // The exported API

    LogsModel(LogsModelType modelType = LogsRecentModel);
    ~LogsModel();
    
    bool clearList(LogsModel::ClearType cleartype);
    int clearMissedCallsCounter();
    LogsMatchesModel* logsMatchesModel();
    bool markEventsSeen(LogsModel::ClearType cleartype);

public: // From QAbstractItemModel
    
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
        
public: //data 
    
 

    void addTextData(QString text);
    static bool isMatchesModelCreated();
    static void setMatchesModelCreated(bool isCreated);
    
    QStringList mTextData;
    QList<QVariant> mIconData;
    
    LogsDbConnector* mDbConnector;
    bool mEmptyModel;
    bool mIsCleared;
    bool mTestEventsMarked;
    bool mTestIsMarkingNeeded;
    bool mMissedCallsCounterCleared;
};

#endif //LOGSMODEL_H
