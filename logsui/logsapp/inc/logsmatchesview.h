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
#ifndef LOGSMATCHESVIEW_H
#define LOGSMATCHESVIEW_H

#include "logsbaseview.h"

class HbListView;
class HbAction;
class LogsComponentRepository;
class LogsAbstractViewManager;
class LogsMatchesModel;

/**
 * 
 */
class LogsMatchesView : public LogsBaseView
{
    Q_OBJECT
    friend class UT_LogsMatchesView;
    
public:
    explicit LogsMatchesView( LogsComponentRepository& repository,
                              LogsAbstractViewManager& viewManager );
    ~LogsMatchesView();

public: // From LogsBaseView
    
    virtual void activated(bool showDialer, QVariant args);
    virtual void deactivated();
 
public slots:
    
    void callKeyPressed();
  
    
protected slots: //from LogsBaseView
    
    virtual void dialpadEditorTextChanged();
    virtual void dialpadClosed();
    virtual void updateWidgetsSizeAndLayout();

private: //from LogsBaseView
    
    virtual void initView();
    virtual QAbstractItemModel* model() const;
    
private:
    
    void initListWidget();
    void updateModel(LogsMatchesModel* model);
            
private:
    
    HbListView* mListView;    //not owned
    LogsMatchesModel* mModel; //owned
    QString mLayoutSectionName;
};

#endif // LOGSMATCHESVIEW_H
