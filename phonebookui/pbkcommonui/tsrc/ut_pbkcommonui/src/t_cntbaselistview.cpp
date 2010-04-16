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

#include "t_cntbaselistview.h"

#include <QtTest/QtTest>
#include <QObject>
#include <QGraphicsLinearLayout>

#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"

#include "hbstubs_helper.h"

void TestCntBaseListView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mBaseListView = 0;
}

void TestCntBaseListView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mBaseListView = new CntBaseListTestView(mViewManager, 0);
    mWindow->addView(mBaseListView);
    mWindow->setCurrentView(mBaseListView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mBaseListView != 0);
}

void TestCntBaseListView::setupView()
{
    mBaseListView->setupView();

    QVERIFY(mBaseListView->widget() != 0);
    QVERIFY(mBaseListView->listLayout() != 0);
}

void TestCntBaseListView::activateView()
{
    CntViewParameters params(CntViewParameters::noView);
    mBaseListView->activateView(params);
    QVERIFY(mBaseListView->listView()->model() != 0);
}

void TestCntBaseListView::onLongPress()
{
    // clear all contacts/groups from database
    QList<QContactLocalId> ids = mBaseListView->contactManager()->contacts();
    mBaseListView->contactManager()->removeContacts(&ids);
    
    QModelIndex emptyMyCardIndex = mBaseListView->contactModel()->index(0);
    
    HbStubHelper::reset();
    mBaseListView->onLongPressed(mBaseListView->listView()->itemByIndex(emptyMyCardIndex), QPointF());
    QVERIFY(HbStubHelper::widgetActionsCount() == 0);
    
    QContact noActionsContact;
    QContactName name;
    name.setFirst("firstname");
    noActionsContact.saveDetail(&name);
    mBaseListView->contactManager()->saveContact(&noActionsContact);
    
    QContact allActionsContact;
    QContactName otherName;
    otherName.setFirst("first");
    QContactPhoneNumber number;
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setContexts(QContactDetail::ContextHome);
    number.setNumber("123456");
    QContactEmailAddress email;
    email.setContexts(QContactDetail::ContextWork);
    email.setEmailAddress("email@mail.com");
    allActionsContact.saveDetail(&otherName);
    allActionsContact.saveDetail(&number);
    allActionsContact.saveDetail(&email);
    mBaseListView->contactManager()->saveContact(&allActionsContact);
    
    QContact allActionsPreferredNoContextsContact;
    QContactName thirdName;
    thirdName.setFirst("first");
    QContactPhoneNumber otherNumber;
    otherNumber.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    otherNumber.setNumber("123456");
    QContactEmailAddress otherEmail;
    otherEmail.setEmailAddress("email@mail.com");
    allActionsPreferredNoContextsContact.saveDetail(&thirdName);
    allActionsPreferredNoContextsContact.saveDetail(&otherNumber);
    allActionsPreferredNoContextsContact.saveDetail(&otherEmail);
    allActionsPreferredNoContextsContact.setPreferredDetail("call", otherNumber);
    allActionsPreferredNoContextsContact.setPreferredDetail("email", otherEmail);
    mBaseListView->contactManager()->saveContact(&allActionsPreferredNoContextsContact);
    
    QModelIndex noActionsContactIndex = mBaseListView->contactModel()->indexOfContact(noActionsContact);
    
    HbStubHelper::reset();
    mBaseListView->onLongPressed(mBaseListView->listView()->itemByIndex(noActionsContactIndex), QPointF());
    QVERIFY(HbStubHelper::widgetActionsCount() == 3);
    
    QModelIndex allActionsContactIndex = mBaseListView->contactModel()->indexOfContact(allActionsContact);
    
    HbStubHelper::reset();
    mBaseListView->onLongPressed(mBaseListView->listView()->itemByIndex(allActionsContactIndex), QPointF());
    QVERIFY(HbStubHelper::widgetActionsCount() == 6);
    
    QModelIndex allActionsPreferredNoContextsContactIndex = mBaseListView->contactModel()->indexOfContact(allActionsPreferredNoContextsContact);
    
    HbStubHelper::reset();
    mBaseListView->onLongPressed(mBaseListView->listView()->itemByIndex(allActionsPreferredNoContextsContactIndex), QPointF());
    QVERIFY(HbStubHelper::widgetActionsCount() == 6);
    
    mBaseListView->contactManager()->setSelfContactId(allActionsContact.localId());
    
    QModelIndex allActionsContactMyCardIndex = mBaseListView->contactModel()->indexOfContact(allActionsContact);
    
    HbStubHelper::reset();
    mBaseListView->onLongPressed(mBaseListView->listView()->itemByIndex(allActionsContactMyCardIndex), QPointF());
    QVERIFY(HbStubHelper::widgetActionsCount() == 3);
}

void TestCntBaseListView::onListViewActivated()
{    
    // clear all contacts/groups from database
    QList<QContactLocalId> ids = mBaseListView->contactManager()->contacts();
    mBaseListView->contactManager()->removeContacts(&ids);
    
    QModelIndex emptyMyCardIndex = mBaseListView->contactModel()->index(0);
    
    mBaseListView->onListViewActivated(emptyMyCardIndex);
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::myCardView);
    
    delete mBaseListView;
    mBaseListView = 0;
    
    mBaseListView = new CntBaseListTestView(mViewManager, 0);
    mWindow->addView(mBaseListView);
    mWindow->setCurrentView(mBaseListView);
    
    QContact noActionsContact;
    QContactName name;
    name.setFirst("firstname");
    noActionsContact.saveDetail(&name);
    mBaseListView->contactManager()->saveContact(&noActionsContact);
    
    QModelIndex noActionsContactIndex = mBaseListView->contactModel()->indexOfContact(noActionsContact);
    
    mBaseListView->onListViewActivated(noActionsContactIndex);
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::commLauncherView);

    delete mBaseListView;
    mBaseListView = 0;
    
    mBaseListView = new CntBaseListTestView(mViewManager, 0);
    mWindow->addView(mBaseListView);
    mWindow->setCurrentView(mBaseListView);
    
    mBaseListView->contactManager()->setSelfContactId(noActionsContact.localId());
    
    QModelIndex myCardContactIndex = mBaseListView->contactModel()->indexOfContact(noActionsContact);
    
    mBaseListView->onListViewActivated(myCardContactIndex);
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::commLauncherView);
}

void TestCntBaseListView::addItemsToLayout()
{
    mWindow->removeView(mBaseListView);
    delete mBaseListView;
    mBaseListView = 0;
    
    mBaseListView = new CntBaseListTestView(mViewManager, 0);
    mWindow->addView(mBaseListView);
    mWindow->setCurrentView(mBaseListView);

    mBaseListView->addItemsToLayout();
    QVERIFY(mBaseListView->widget() != 0);
    QVERIFY(mBaseListView->listLayout() != 0);
    QVERIFY(mBaseListView->listLayout()->count() == 1);
}

void TestCntBaseListView::setBannerName()
{
    mBaseListView->setBannerVisibility(false);
    mBaseListView->setBannerName("test");
	QVERIFY(mBaseListView->listLayout()->count() == 1);
	
    mBaseListView->setBannerVisibility(true);
    mBaseListView->setBannerName("test");
    QVERIFY(mBaseListView->banner()->titleText() == "test");
    QVERIFY(mBaseListView->listLayout()->count() == 2);
}

void TestCntBaseListView::bannerName()
{
    QVERIFY(mBaseListView->bannerName() == "test");
}

void TestCntBaseListView::banner()
{
    mWindow->removeView(mBaseListView);
    delete mBaseListView;
    mBaseListView = 0;
    
    mBaseListView = new CntBaseListTestView(mViewManager, 0);
    mWindow->addView(mBaseListView);
    mWindow->setCurrentView(mBaseListView);
    
    mBaseListView->mHasBanner = false;
    mBaseListView->banner();
    QVERIFY(mBaseListView->banner() == 0);

    mBaseListView->mHasBanner = true;
    mBaseListView->banner();
    QVERIFY(mBaseListView->banner() != 0);
}

void TestCntBaseListView::isBannerVisible()
{
    mBaseListView->setBannerVisibility(true);
    QVERIFY(mBaseListView->isBannerVisible());

    mBaseListView->setBannerVisibility(false);
    QVERIFY(!mBaseListView->isBannerVisible());
}


void TestCntBaseListView::listLayout()
{
    mBaseListView->listLayout();
    QVERIFY(mBaseListView->listLayout() != 0);
}

void TestCntBaseListView::listView()
{
    mBaseListView->listView();
    QVERIFY(mBaseListView->listView() != 0);
}

void TestCntBaseListView::setDataModel()
{
    mWindow->removeView(mBaseListView);
    delete mBaseListView;
    mBaseListView = 0;
    
    mBaseListView = new CntBaseListTestView(mViewManager, 0);
    mWindow->addView(mBaseListView);
    mWindow->setCurrentView(mBaseListView);

    mBaseListView->setDataModel();

    QVERIFY(mBaseListView->listView()->model() != 0);
}

void TestCntBaseListView::clearDataModelFilter()
{
    delete mBaseListView;
    mBaseListView = 0;
    
    mBaseListView = new CntBaseListTestView(mViewManager, 0);
    mWindow->addView(mBaseListView);
    mWindow->setCurrentView(mBaseListView);

    mBaseListView->clearDataModelFilter();

    QVERIFY(mBaseListView->listView()->model() != 0);
}

void TestCntBaseListView::cleanupTestCase()
{
delete mBaseListView;
mBaseListView = 0;
    delete mViewManager;
    mViewManager = 0;
    mWindow->deleteLater();
}

// EOF
