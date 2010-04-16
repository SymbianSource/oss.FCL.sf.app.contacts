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
#ifndef LOGSCOMPONENTREPOSITORY_H
#define LOGSCOMPONENTREPOSITORY_H

#include "logsdefs.h"
#include <hbdocumentloader.h>

class LogsRecentCallsView;
class LogsDetailsView;
class LogsMatchesView;
class Dialpad;
class DialpadKeyHandler;
class LogsAbstractViewManager;
class LogsModel;

/**
 * 
 */
class LogsComponentRepository : public HbDocumentLoader   
{   
    friend class UT_LogsComponentRepository;
    
public:    

    explicit LogsComponentRepository(LogsAbstractViewManager& viewManager);
    ~LogsComponentRepository();

public:
    
    LogsRecentCallsView* recentCallsView();
    LogsDetailsView* detailsView();
    LogsMatchesView* matchesView();

    /**
     * Returns dialpad. Must be always a valid pointer.
     */
    Dialpad* dialpad();
    
    /**
     * Returns model. Must be always a valid pointer.
     */
    LogsModel*  model() const;
    
    /**
     * Sets object tree to a proper view object list
     */
    void setObjectTreeToView( LogsAppViewId viewId );
    
    bool loadSection(  LogsAppViewId viewId, const QString& sectionName );
    
private: 

    //from HbDocumentLoader
    QObject *createObject(const QString& type, const QString &name);

    void addToolbarToObjectList( QObjectList& list );
    
private:
    
    LogsAbstractViewManager& mViewManager;
    LogsRecentCallsView* mRecentCallsView;
    LogsDetailsView* mDetailsView;
    LogsMatchesView* mMatchesView;
    QObjectList mRecentViewComponents;
    QObjectList mDetailsViewComponents;
    QObjectList mMatchesViewComponents;
    
    Dialpad* mDialpad;
    DialpadKeyHandler* mDialpadKeyHandler;
    LogsModel*  mModel;
};

#endif // LOGSCOMPONENTREPOSITORY_H
