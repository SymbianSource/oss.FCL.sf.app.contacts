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
#ifndef LOGVIEWMANAGER_H
#define LOGVIEWMANAGER_H


#include <QObject>
#include <QList>
#include <logsservices.h>
#include "logsabstractviewmanager.h"


class HbMainWindow;
class LogsComponentRepository;
class LogsServiceHandler;
class LogsMainWindow;
class HbView;
class LogsBaseView;

/**
 * 
 */
class LogsViewManager : public QObject, public LogsAbstractViewManager
{
    Q_OBJECT
    
    friend class UT_LogsViewManager;

public:
  
    /**
     * Constructor
     * @param mainWindow
     * @param service
     */
    LogsViewManager( LogsMainWindow& mainWindow, LogsServiceHandler& service );
    ~LogsViewManager();

public slots:

    void changeRecentView(LogsServices::LogsView view, bool showDialpad);
    void proceedExit();
    void appFocusGained();
    
public: // From LogsAbstractViewManager
    
    virtual bool activateView(LogsAppViewId viewId);
    virtual bool activateView(LogsAppViewId viewId, bool showDialpad, QVariant args);
    virtual bool activatePreviousView();
    virtual HbMainWindow& mainWindow();
    virtual void exitApplication();
    
private slots:

    void handleOrientationChanged();
    
private:
    
    void initViews();
    bool doActivateView(LogsAppViewId viewId, bool showDialpad, QVariant args);
    
private: //data 
    
    LogsMainWindow& mMainWindow;
    LogsServiceHandler& mService;
    LogsComponentRepository* mComponentsRepository;
    QList<LogsBaseView*> mViewStack;
    
};

#endif //LOGVIEWMANAGER_H
