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
    Q_OBJECT

public:
    
    explicit LogsDetailsModel( LogsDbConnector& dbConnector, LogsEvent& event );
    
public: // The exported API
  
    LOGSENGINE_EXPORT ~LogsDetailsModel();

    LOGSENGINE_EXPORT void clearEvent();
    LOGSENGINE_EXPORT void getNumberToClipboard();
    
public: // From QAbstractItemModel
    
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

protected: // From LogsAbstractModel
    
    virtual QVariant createContact(const LogsModelItemContainer& item) const;
    
private slots:

    void contactActionCompleted(bool modified);
    
private:
    
    void getDecorationData(int row, QList<QVariant>& iconList) const;
    void getDisplayData(int row, QStringList& texts) const;
    QString getCallerId(const LogsEvent& event) const;
    QString getHeaderData(const LogsEvent& event) const;
    QString getRemoteUri(const LogsEvent& event) const;
    QString getLocalUri(const LogsEvent& event) const;
    bool isAddress(QString value) const;
    bool isOutgoingCall() const;
    QString getHeaderValue(QString value,bool isremote) const;
    void initContent();
    void initTexts();
    void initIcons();
    
private: //data 
    
    LogsEvent* mEvent;
    int mDetailItemsCount;
    QMap<int, HbIcon*> mDetailIcons;
    QMap<int, QStringList> mDetailTexts;
    
private:
        
    friend class UT_LogsModel;
    friend class UT_LogsDetailsModel;
    
    
};

#endif //LOGSDETAILSMODEL_H
