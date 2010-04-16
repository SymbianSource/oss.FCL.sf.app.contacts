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
#include <QObject>
#include <qtcontacts.h>
#include <hbtoolbar.h>

#include "cntmycardview.h"
#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"

#include "hbstubs_helper.h"

#include "t_cntmycardview.h"

void TestCntMyCardView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mMyCardView = 0;
}

void TestCntMyCardView::createClasses()
{
//    mWindow = new CntMainWindow(0, CntViewParameters::noView);
//    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
//    mMyCardView = new CntMyCardView(mViewManager);
//    mWindow->addView(mMyCardView);
//    mWindow->setCurrentView(mMyCardView);
//
//    // check that we have a view
//    QVERIFY(mWindow != 0);
//    QVERIFY(mViewManager != 0);
//    QVERIFY(mMyCardView != 0);
}

void TestCntMyCardView::activateView()
{
//    QList<QContactLocalId> ids = mMyCardView->contactManager()->contacts();
//    mMyCardView->contactManager()->removeContacts(&ids);
//    
//    CntViewParameters viewParameters(CntViewParameters::noView);
//    
//    mMyCardView->activateView(viewParameters);
//    QVERIFY(!mMyCardView->findWidget("cnt_button_choose")->isEnabled());
//    
//    mWindow->removeView(mMyCardView);
//    delete mMyCardView;
//    mMyCardView = 0;
//    
//    mMyCardView = new CntMyCardView(mViewManager);
//    mWindow->addView(mMyCardView);
//    mWindow->setCurrentView(mMyCardView);
//    
//    QContact contact;
//    QContactName name;
//    name.setFirst("first");
//    contact.saveDetail(&name);
//    mMyCardView->contactManager()->saveContact(&contact);
//    
//    mMyCardView->activateView(viewParameters);
//    QVERIFY(mMyCardView->findWidget("cnt_button_choose")->isEnabled());
}

void TestCntMyCardView::openNameEditor()
{
//    mMyCardView->openNameEditor();
//    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::editView);
//    QVERIFY(mMyCardView->contactManager()->selfContactId() != 0);
}

void TestCntMyCardView::openMyCardSelectionView()
{
//    mWindow->removeView(mWindow->currentView());
//    delete mMyCardView;
//    mMyCardView = 0;
//    
//    mMyCardView = new CntMyCardView(mViewManager);
//    mWindow->addView(mMyCardView);
//    mWindow->setCurrentView(mMyCardView);
//    
//    mMyCardView->openMyCardSelectionView();
//    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::myCardSelectionView);
}

void TestCntMyCardView::aboutToCloseView()
{
//    mWindow->removeView(mWindow->currentView());
//    delete mMyCardView;
//    mMyCardView = 0;
//    
//    mMyCardView = new CntMyCardView(mViewManager);
//    mWindow->addView(mMyCardView);
//    mWindow->setCurrentView(mMyCardView);
//    
//    mMyCardView->aboutToCloseView();
//    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::namesView);
}

void TestCntMyCardView::cleanupTestCase()
{
//    delete mViewManager;
//    mViewManager = 0;
//    mWindow->deleteLater();
}

// EOF
