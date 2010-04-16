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

#include <QtTest/QtTest>
#include <QGraphicsLinearLayout>
#include <qtcontacts.h>

#include "cnthistoryview.h"
#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"
#include "mobhistorymodel.h"

#include "t_cntcommhistoryview.h"


void TestCntCommHistoryView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mHistoryView = 0;
}

void TestCntCommHistoryView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mHistoryView = new CntHistoryView(mViewManager,0);
    
    QVERIFY(mHistoryView->findWidget("content") != 0);
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mHistoryView != 0);
}

void TestCntCommHistoryView::aboutToCloseView()
{
    mWindow = new CntMainWindow(0, CntViewParameters::commLauncherView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::commLauncherView);
    mHistoryView = new CntHistoryView(mViewManager,0);

    mWindow->addView(mHistoryView);
    mWindow->setCurrentView(mHistoryView);
    
    QContact *c = new QContact;
    mHistoryView->mContact = c;
    mHistoryView->aboutToCloseView();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::commLauncherView);
    //delete c;

    mWindow->deleteLater();
    mViewManager = 0;
    delete mHistoryView;
    mHistoryView = 0;
}

void TestCntCommHistoryView::activateView()
{
    //create comm history view
    createClasses();

    //create my card
    QContact c;
    QContactName name;
    name.setFirst("first");
    c.saveDetail(&name);
    mHistoryView->contactManager()->saveContact(&c);
    QVERIFY(c.localId() > 0);
    mHistoryView->contactManager()->setSelfContactId(c.localId());
    
    //activate view
    CntViewParameters viewParameters(CntViewParameters::noView);
    viewParameters.setSelectedContact(c);
    mHistoryView->activateView(viewParameters);
    
    QVERIFY(mHistoryView->mHistoryListView != 0);
    QVERIFY(mHistoryView->mHistoryModel != 0);
    QVERIFY(mHistoryView->mContact != 0);
     
    //heading + list
    QVERIFY(mHistoryView->findWidget("content")->layout()->count() == 2);
    
    //check number of events in the list (3 events)
    QVERIFY(mHistoryView->mHistoryModel->rowCount() == 3);
    
    mHistoryView->updateScrollingPosition();
    
    //delete my card
    mHistoryView->contactManager()->removeContact(c.localId());
}

void TestCntCommHistoryView::cleanupTestCase()
{
    mWindow->deleteLater();
    delete mViewManager;
    mViewManager = 0;
    delete mHistoryView;
    mHistoryView = 0;
}


