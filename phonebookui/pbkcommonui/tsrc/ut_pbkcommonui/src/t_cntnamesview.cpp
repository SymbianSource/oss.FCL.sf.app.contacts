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
#include <hbtoolbar.h>

#include "cntnamesview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntnamesview.h"

#include "hbstubs_helper.h"

void TestCntNamesView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mNamesView = 0;
}

void TestCntNamesView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mNamesView = new CntNamesView(mViewManager, 0);
    mWindow->addView(mNamesView);
    mWindow->setCurrentView(mNamesView);

    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mNamesView != 0);
}

void TestCntNamesView::addActionsToToolBar()
{
    HbStubHelper::reset();
    mNamesView->addActionsToToolBar();
    QVERIFY(HbStubHelper::widgetActionsCount() == 4);
}

void TestCntNamesView::addMenuItems()
{
    HbStubHelper::reset();
    mNamesView->addMenuItems();
    QVERIFY(HbStubHelper::widgetActionsCount() == 2);
}

void TestCntNamesView::openCollections()
{
    mNamesView->openCollections();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::collectionView);
}

void TestCntNamesView::handleExecutedCommand()
{
    cleanupTestCase();
    createClasses();
    
    QContact contact;
    HbStubHelper::reset();
    mNamesView->handleExecutedCommand("dummy", contact);
    QVERIFY(!HbStubHelper::notificationDialogOpened());
    
    HbStubHelper::reset();
    mNamesView->handleExecutedCommand("delete", contact);
    QVERIFY(HbStubHelper::notificationDialogOpened());
}

void TestCntNamesView::activateView()
{
    cleanupTestCase();
    createClasses();
    
    QList<QContactLocalId> ids = mNamesView->contactManager()->contacts();
    mNamesView->contactManager()->removeContacts(&ids);
    
    QContact contact;
    QContactName name;
    name.setFirst("first");
    contact.saveDetail(&name);
    mNamesView->contactManager()->saveContact(&contact);
    
    HbStubHelper::reset();
    CntViewParameters viewParameters(CntViewParameters::noView);
    mNamesView->activateView(viewParameters);
    QVERIFY(mNamesView->mSoftKeyBackAction == mWindow->softKeyAction(Hb::SecondarySoftKey));
    QVERIFY(!HbStubHelper::notificationDialogOpened());
    
    cleanupTestCase();
    createClasses();
    
    HbStubHelper::reset();
    viewParameters.setSelectedAction("delete");
    mNamesView->activateView(viewParameters);
    QVERIFY(HbStubHelper::notificationDialogOpened());
    
    cleanupTestCase();
    createClasses();
    
    HbStubHelper::reset();
    viewParameters.setSelectedAction("failed");
    mNamesView->activateView(viewParameters);
    QVERIFY(HbStubHelper::notificationDialogOpened());
    
    cleanupTestCase();
    createClasses();
    
    HbStubHelper::reset();
    viewParameters.setSelectedAction("save");
    viewParameters.setSelectedContact(contact);
    mNamesView->setDataModel(); // required for the list to have a model...
    mNamesView->activateView(viewParameters);
    QVERIFY(HbStubHelper::notificationDialogOpened());
}

void TestCntNamesView::showFind()
{
    cleanupTestCase();
    createClasses();
    
    mNamesView->showFind();
    QVERIFY(mNamesView->mSearchPanel);
    QVERIFY(mNamesView->banner()->isVisible());
    QVERIFY(!mNamesView->contactModel()->myCardStatus());
    QVERIFY(!mNamesView->toolBar()->isVisible());
    
    // calling it a second time shouldn't break it
    mNamesView->showFind();
    QVERIFY(mNamesView->mSearchPanel);
    QVERIFY(mNamesView->banner()->isVisible());
    QVERIFY(!mNamesView->contactModel()->myCardStatus());
    QVERIFY(!mNamesView->toolBar()->isVisible());
}

void TestCntNamesView::closeFind()
{
    cleanupTestCase();
    createClasses();
    
    mNamesView->showFind(); // make find pane visible first
    
    mNamesView->closeFind();
    QVERIFY(!mNamesView->mSearchPanel);
    QVERIFY(!mNamesView->mEmptyListLabel);
    QVERIFY(!mNamesView->banner()->isVisible());
    QVERIFY(mNamesView->contactModel()->myCardStatus());
    QVERIFY(mNamesView->toolBar()->isVisible());
    
    // calling it a second time shouldn't break it deAc
    mNamesView->closeFind();
    QVERIFY(!mNamesView->mSearchPanel);
    QVERIFY(!mNamesView->mEmptyListLabel);
    QVERIFY(!mNamesView->banner()->isVisible());
    QVERIFY(mNamesView->contactModel()->myCardStatus());
    QVERIFY(mNamesView->toolBar()->isVisible());
}

void TestCntNamesView::deActivateView()
{
    cleanupTestCase();
    createClasses();
    
    mNamesView->showFind(); // make find pane visible first
    
    mNamesView->deActivateView();
    QVERIFY(!mNamesView->mSearchPanel);
    QVERIFY(!mNamesView->mEmptyListLabel);
    QVERIFY(!mNamesView->banner()->isVisible());
    QVERIFY(mNamesView->contactModel()->myCardStatus());
    QVERIFY(mNamesView->toolBar()->isVisible());
}

void TestCntNamesView::setFilter()
{
    cleanupTestCase();
    createClasses();
    
    QList<QContactLocalId> ids = mNamesView->contactManager()->contacts();
    mNamesView->contactManager()->removeContacts(&ids);
    
    QContact contact1;
    QContactName name1;
    name1.setFirst("abc");
    contact1.saveDetail(&name1);
    
    QContact contact2;
    QContactName name2;
    name2.setFirst("acb");
    contact2.saveDetail(&name2);
    
    mNamesView->contactManager()->saveContact(&contact1);
    mNamesView->contactManager()->saveContact(&contact2);
    
    mNamesView->showFind(); // calls setFilter(QString())
    QVERIFY(!mNamesView->mEmptyListLabel);
    QVERIFY(mNamesView->contactModel()->rowCount() == 2);
    
    mNamesView->setFilter("abc");
    QVERIFY(!mNamesView->mEmptyListLabel);
    QVERIFY(mNamesView->contactModel()->rowCount() == 1);
    
    mNamesView->setFilter("abcd");
    QVERIFY(mNamesView->mEmptyListLabel);
    QVERIFY(mNamesView->contactModel()->rowCount() == 0);
    
    mNamesView->setFilter("abcde");
    QVERIFY(mNamesView->mEmptyListLabel);
    QVERIFY(mNamesView->contactModel()->rowCount() == 0);
}

void TestCntNamesView::cleanupTestCase()
{
    delete mViewManager;
    mViewManager = 0;
    delete mWindow;
    mWindow = 0;
}

// EOF
