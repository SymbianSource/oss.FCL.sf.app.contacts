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

#ifndef CNTVIEWMANAGER_H
#define CNTVIEWMANAGER_H

#include "cntviewparameters.h"
#include "qtpbkglobal.h"

#include "cntabstractviewmanager.h"
#include <QObject>

class CntMainWindow;
class CntBaseView;
class CntEditorFactory;

class QTPBK_EXPORT CntViewManager : public QObject
{
    Q_OBJECT

public:
    CntViewManager(CntMainWindow *mainWindow=0, CntViewParameters::ViewId defaultView = CntViewParameters::namesView);
    ~CntViewManager();
    
public:
    virtual void changeView( const CntViewParameters& aViewParams ) = 0;    // New, implemented in DefaultViewManager
    virtual void back(const CntViewParameters& aArgs) = 0;                  // New, implemented in DefaultViewManager
    virtual bool isDepracatedView( CntViewParameters::ViewId) = 0;          // New, implemented in DefaultViewManager
    
public:
    void setDefaultView(CntViewParameters::ViewId defaultView);
    CntMainWindow* mainWindow();

 /*
#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
*/
public:
    void removeDepracatedCurrentView(); // New
    void addViewToWindow(CntBaseView *view);
    void removeViewFromWindow(CntBaseView *view);
    virtual CntBaseView *getView(const CntViewParameters &aArgs);

#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    CntMainWindow             *mMainWindow;
    CntBaseView               *mDefaultView;   // own
    CntEditorFactory          *mEditorFactory; // own
    CntViewParameters::ViewId  mCurrentViewId;
};

#endif // CNTVIEWMANAGER_H
