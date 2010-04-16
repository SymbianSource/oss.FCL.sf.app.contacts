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
#include "cntgroupactionsview.h"
#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"
#include "hbstubs_helper.h"
#include <QGraphicsLinearLayout>
#include "t_cntgroupactionsview.h"

void TestCntGroupActionsView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mGroupActionsView = 0;
}

void TestCntGroupActionsView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mGroupActionsView = new CntGroupActionsView(mViewManager);
    mWindow->addView(mGroupActionsView);
    mWindow->setCurrentView(mGroupActionsView);
    
    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mGroupActionsView != 0);
}

void TestCntGroupActionsView::aboutToCloseView()
{
   
}

void TestCntGroupActionsView::editGroup()
{
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mGroupActionsView->contactManager()->saveContact(&firstGroup);
    
    mGroupActionsView->mGroupContact = &firstGroup;

    mGroupActionsView->editGroup();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::groupEditorView);
}


void TestCntGroupActionsView::addActionsToToolBar()
{
    HbStubHelper::reset();
    mGroupActionsView->addActionsToToolBar();
    QVERIFY(HbStubHelper::widgetActionsCount() == 0);
}

void TestCntGroupActionsView::addMenuItems()
{
    
    HbStubHelper::reset();
    mGroupActionsView->addMenuItems();
    
    QVERIFY(HbStubHelper::widgetActionsCount() == 1);
}


void TestCntGroupActionsView::activateView()
{
    //set view parameters and create contact
    CntViewParameters viewParameters(CntViewParameters::noView);
    QContact* contact = new QContact();
    contact->setType(QContactType::TypeGroup);
  
    viewParameters.setSelectedContact(*contact);
    mGroupActionsView->activateView(viewParameters);
    
    QVERIFY(mGroupActionsView->mGroupContact != 0);
    QVERIFY(mGroupActionsView->mDataContainer != 0);
    QVERIFY(mGroupActionsView->mScrollArea != 0);
    QVERIFY(mGroupActionsView->mContainerWidget != 0);
    QVERIFY(mGroupActionsView->mContainerLayout != 0);
   
    QVERIFY(mGroupActionsView->mContainerLayout->count() == 2);
       
    delete contact;
    
    CntViewParameters newViewParameters(CntViewParameters::noView);
    QContact* newContact = new QContact();
    newContact->setType(QContactType::TypeGroup);
    
    QContactPhoneNumber* number = new QContactPhoneNumber();
    number->setNumber("123456789");
    newContact->saveDetail(number);
    
    newViewParameters.setSelectedContact(*contact);
    mGroupActionsView->activateView(newViewParameters);
    
    QVERIFY(mGroupActionsView->mGroupContact != 0);
    QVERIFY(mGroupActionsView->mDataContainer != 0);
    QVERIFY(mGroupActionsView->mScrollArea != 0);
    QVERIFY(mGroupActionsView->mContainerWidget != 0);
    QVERIFY(mGroupActionsView->mContainerLayout != 0);
   
    QVERIFY(mGroupActionsView->mContainerLayout->count() == 3);
       
    delete newContact;

}
void TestCntGroupActionsView::cleanupTestCase()
{
    delete mViewManager;
    mViewManager = 0;
    mWindow ->deleteLater();
    //mWindow = 0;
    delete mGroupActionsView;
    mGroupActionsView = 0;
}

// EOF
