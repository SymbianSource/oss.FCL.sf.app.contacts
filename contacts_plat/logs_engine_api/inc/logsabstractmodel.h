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

class LogsEvent;
class LogsDbConnector;
class LogsModelItemContainer;
class HbIcon;

/**
 * Abstract logs model.
 *
 */
class LogsAbstractModel : public QAbstractListModel 
{
    Q_OBJECT
    
public:
    
    /**
     * Additional data role types.
     */
    enum LogsModelRole {
           RoleFullEvent = Qt::UserRole + 1, // LogsEvent
           RoleCall,                         // LogsCall
           RoleMessage,                      // LogsMessage
           RoleContact,                      // LogsContact
           RoleDetailsModel,                 // LogsDetailsModel
           RoleBaseLast                      // Child classes may define roles above this val
    };

public:
    
    ~LogsAbstractModel();
    
public:
    
    static QString directionIconName(const LogsEvent& event);
    static QString typeIconName(const LogsEvent& event);
        
    /**
     * Fills iconList based on the event.
     * @param event, 
     * @param iconList, on return contains list of icons 
     */
    void getDecorationData(const LogsEvent& event, QList<QVariant>& iconList) const;
    
    LogsDbConnector* dbConnector();
    
protected:

    QVariant doGetData(int role, const LogsModelItemContainer& item) const;
    
    virtual QVariant createCall(const LogsModelItemContainer& item) const;
    virtual QVariant createMessage(const LogsModelItemContainer& item) const;
    virtual QVariant createContact(const LogsModelItemContainer& item) const;
      
    explicit LogsAbstractModel();
    
protected: //data 
    
    LogsDbConnector* mDbConnector;
    QMap<QString, HbIcon*> mIcons;
    
private:  // Testing related friend definitions
    
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
    LogsEvent* event() const;

protected:
    LogsEvent* mEvent; // not owned
};

#endif //LOGSABSTRACTMODEL_H
