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

#include <QObject>

#include "cntbaseview.h"
#include "cntviewparameters.h"

class CntViewManager;
class CntMainWindow;

class CntBaseTestView : public CntBaseView
{
    Q_OBJECT

public:
    CntBaseTestView(CntViewManager *viewManager, QGraphicsItem *parent = 0):
        CntBaseView(viewManager, parent) { }

    CntViewParameters::ViewId viewId() const { return CntViewParameters::noView; }

};

class TestCntBaseView : public QObject
{
     Q_OBJECT

private slots:

    void initTestCase();
    void createClasses();

    // main funcs
    void setupView();
    void viewManager();
    void contactModel();
    void contactManager();
    void commands();

    void addSoftkeyAction();

    void clearToolBar();
    void addToolBar();
    void setToolBarOrientation();

    void actions();
	
    void loadDocument();
    void findWidget();
    void findObject();
    
    void keyPressEvent();

    void cleanupTestCase();

private:
    CntBaseTestView *mBaseView;
    CntViewManager  *mViewManager;
    CntMainWindow   *mWindow;
 };
