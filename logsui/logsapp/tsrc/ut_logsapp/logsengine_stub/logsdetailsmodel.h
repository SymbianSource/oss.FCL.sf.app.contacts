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

#ifndef LOGSDETAILSMODEL_H
#define LOGSDETAILSMODEL_H

#include <logsexport.h>
#include <logsabstractmodel.h>
#include "logsengdefs.h"

class LogsEvent;
class LogsDbConnector;
class HbIcon;

/**
 * Model for log event details.
 *
 */
class LogsDetailsModel : public LogsAbstractModel
{
    


public:
    
    explicit LogsDetailsModel();
    
public: // The exported API
  
    ~LogsDetailsModel();
    void clearEvent();
    void getNumberToClipboard();
    
public: // From QAbstractItemModel
    
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;
private:
    
private: //testing purpose data 

    LogsEvent* mEvent;
    int mDetailItemsCount;
    QMap<int, HbIcon*> mDetailIcons;
    QMap<int, QStringList> mDetailTexts;
    static QString mLastCallName; //last called exported function name
    
    friend class UT_LogsDetailsView;
};

#endif //LOGSDETAILSMODEL_H
