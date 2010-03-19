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

#include <QObject>

class CntMainWindow;
class CntBaseView;

class QTPBK_EXPORT CntViewManager : public QObject
{
    Q_OBJECT

public:
    CntViewManager(CntMainWindow *mainWindow=0, CntViewParameters::ViewId defaultView = CntViewParameters::namesView);
    ~CntViewManager();

public slots:
    void onActivateView(int aViewId);
    void onActivateView(const CntViewParameters &viewParameters);
    void onActivatePreviousView();

public:
    void setDefaultView(CntViewParameters::ViewId defaultView);
    void setPreviousViewParameters(const CntBaseView *currentView, const CntViewParameters &previousViewParameters);
    CntMainWindow* mainWindow();
    CntViewParameters &previousViewParameters();

#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    void addViewToWindow(CntBaseView *view);
    void removeViewFromWindow(CntBaseView *view);
    virtual CntBaseView *getView(CntViewParameters::ViewId id);

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    CntMainWindow             *mMainWindow;
    CntBaseView               *mDefaultView;
    CntViewParameters::ViewId  mDefaultViewId;
    CntViewParameters          mPreviousViewParameters;
};

#endif // CNTVIEWMANAGER_H
