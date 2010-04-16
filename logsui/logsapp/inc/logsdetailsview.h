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
#ifndef LOGSDETAILSVIEW_H
#define LOGSDETAILSVIEW_H

#include "logsbaseview.h"

class HbListView;
class HbGroupBox;
class HbAction;
class LogsComponentRepository;
class HbAction;
class LogsAbstractViewManager;

/**
 * 
 */
class LogsDetailsView : public LogsBaseView
{
    Q_OBJECT
    friend class UT_LogsDetailsView;
    
public:
    explicit LogsDetailsView( LogsComponentRepository& repository,
                              LogsAbstractViewManager& viewManager );
    ~LogsDetailsView();
    
public: // From LogsBaseView
    
    virtual void activated(bool showDialer, QVariant args);
    virtual void deactivated();

public slots:
    
    void callKeyPressed();
    
private slots:
    
    void initiateVoiceCall();
    void initiateVideoCall();
    void addToContacts();
    void openContact();
    void contactActionCompleted(bool modified);
    void deleteEvent();//overrides base class implementation
    void copyNumberToClipboard();
    
    //from LogsBaseView
    virtual void handleBackSoftkey();
    
private: // from LogsBaseView
    
    virtual void initView();
    virtual QAbstractItemModel* model() const;
    virtual void updateWidgetsSizeAndLayout();
    
private:
    
    void initListWidget();
    void updateMenu();

private:
    
    HbGroupBox* mViewName; //not owned
    HbListView* mListView; //not owned
};

#endif // LOGSDETAILSVIEW_H
