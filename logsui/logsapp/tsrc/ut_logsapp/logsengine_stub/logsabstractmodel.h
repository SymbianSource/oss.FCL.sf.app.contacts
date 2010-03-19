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

#ifndef LOGSABSTRACTMODEL_H
#define LOGSABSTRACTMODEL_H

#include <QAbstractListModel>
#include <logsexport.h>
#include "logsengdefs.h"

class LogsEvent;
class LogsDbConnector;
class LogsModelItemContainer;

/**
 * Abstract logs model.
 *
 */
class LogsAbstractModel : public QAbstractListModel 
{
    
public:
    
    /**
     * Additional data role types.
     */
    enum LogsModelRole {
           RoleFullEvent = Qt::UserRole + 1, // LogsEvent
           RoleCall,                     // LogsCall
           RoleMessage,                  // LogsMessage
           RoleContact,                  // LogsContact
           RoleDetailsModel,             // LogsDetailsModel
           RoleBaseLast                  // Child classes may define roles above this val
    };

public:
    
    ~LogsAbstractModel();
    
public:
    
    static QString directionIconName(const LogsEvent& event);
    static QString typeIconName(const LogsEvent& event);
    
protected:

    QVariant doGetData(int role, LogsModelItemContainer& item) const;
    
    virtual QVariant createCall(LogsModelItemContainer& item) const;
    virtual QVariant createMessage(LogsModelItemContainer& item) const;
    virtual QVariant createContact(LogsModelItemContainer& item) const;
       
    LogsAbstractModel();
 
protected: //data 
    
    LogsDbConnector* mDbConnector;
    
private:
    
    friend class UT_LogsModel;
    friend class UT_LogsDetailsModel;
    friend class UT_LogsFilter;
    friend class UT_LogsCustomFilter;
    
};


/**
 * Specialized model may contain other model items than log events.
 */
class LogsModelItemContainer {
public:
    LogsModelItemContainer(LogsEvent* event = 0);
    virtual ~LogsModelItemContainer();
    LogsEvent* event();

private:
    LogsEvent* mEvent; // not owned
};

#endif //LOGSABSTRACTMODEL_H
