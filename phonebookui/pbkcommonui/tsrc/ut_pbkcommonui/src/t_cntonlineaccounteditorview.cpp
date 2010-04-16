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

#include "cntonlineaccounteditorview.h"
#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntonlineaccounteditorview.h"

void TestCntOnlineAccountEditorView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mOnlineAccountEditorView = 0;
}

void TestCntOnlineAccountEditorView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mOnlineAccountEditorView = new CntOnlineAccountEditorView(mViewManager, 0);
    mWindow->addView(mOnlineAccountEditorView);
    mWindow->setCurrentView(mOnlineAccountEditorView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mOnlineAccountEditorView != 0);
}

void TestCntOnlineAccountEditorView::aboutToCloseView()
{
    QContact *contact= new QContact();
    QContactOnlineAccount *onlineAccount = new QContactOnlineAccount();
    mOnlineAccountEditorView->mDetailList.append(onlineAccount);
    mOnlineAccountEditorView->mContact = contact;
    
    // empty detail isn't saved
    mOnlineAccountEditorView->aboutToCloseView();
    QVERIFY(mOnlineAccountEditorView->mContact->details<QContactOnlineAccount>().count() == 0);
    
    // Online Account name set
    onlineAccount->setAccountUri("Test Account");
    mOnlineAccountEditorView->aboutToCloseView();
    QVERIFY(mOnlineAccountEditorView->mContact->details<QContactOnlineAccount>().count() == 1);
    onlineAccount->setAccountUri("");
    
    mWindow->removeView(mOnlineAccountEditorView);
    delete mOnlineAccountEditorView;
    mOnlineAccountEditorView = 0;
}

void TestCntOnlineAccountEditorView::itemModel()
{
    mOnlineAccountEditorView = new CntOnlineAccountEditorView(mViewManager, 0);
    QStandardItemModel *itemModel = new QStandardItemModel();
    QContact *contact= new QContact;
       
    mOnlineAccountEditorView->mContact = contact;
    itemModel = mOnlineAccountEditorView->itemModel();
    QVERIFY(itemModel->rowCount(QModelIndex()) == 5 );
}

void TestCntOnlineAccountEditorView::initializeForm()
{
    mOnlineAccountEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mOnlineAccountEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mOnlineAccountEditorView;
    mOnlineAccountEditorView = 0;
}

void TestCntOnlineAccountEditorView::initializeFormAdd()
{
    QContact *contact= new QContact;
    mOnlineAccountEditorView = new CntOnlineAccountEditorView(mViewManager, 0);
    mOnlineAccountEditorView->mContact = contact;
    
    QContactOnlineAccount *onlineAccount = new QContactOnlineAccount();
    onlineAccount->setAccountUri("Test Account");
    onlineAccount->setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
    contact->saveDetail(onlineAccount);
    
    mOnlineAccountEditorView->mParamString = "add";
    mOnlineAccountEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mOnlineAccountEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mOnlineAccountEditorView;
    mOnlineAccountEditorView = 0;
}

void TestCntOnlineAccountEditorView::initializeFormFocusSecond()
{
    QContact *contact= new QContact;
    mOnlineAccountEditorView = new CntOnlineAccountEditorView(mViewManager, 0);
    mOnlineAccountEditorView->mContact = contact;
    
    QContactOnlineAccount *onlineAccount = new QContactOnlineAccount();
    onlineAccount->setAccountUri("Test Account");
    onlineAccount->setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
    contact->saveDetail(onlineAccount);
    
    QContactOnlineAccount *onlineAccount2 = new QContactOnlineAccount();
    onlineAccount2->setAccountUri("Test Account 2");
    onlineAccount2->setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
    contact->saveDetail(onlineAccount2);
    
    mOnlineAccountEditorView->mParamString = "1"; // focus the second one, not verifiable tho
    mOnlineAccountEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mOnlineAccountEditorView->formModel()->rowCount(QModelIndex()) == 3);
    delete mOnlineAccountEditorView;
    mOnlineAccountEditorView = 0;
}
 
void TestCntOnlineAccountEditorView::initializeFormData()   

{   // create with onlineaccount address
    QContact *contact= new QContact;
    mOnlineAccountEditorView = new CntOnlineAccountEditorView(mViewManager, 0);
    mOnlineAccountEditorView->mContact = contact;
    
    QContactOnlineAccount *onlineAccount = new QContactOnlineAccount();
    onlineAccount->setAccountUri("Test Account");
    onlineAccount->setSubTypes(QContactOnlineAccount::SubTypeSipVoip);

    contact->saveDetail(onlineAccount);
   
    mOnlineAccountEditorView->initializeForm();
    QVERIFY(mOnlineAccountEditorView->formModel()->rowCount(QModelIndex()) == 2);
    
}

void TestCntOnlineAccountEditorView::addDetail()
{
    mOnlineAccountEditorView->addDetail();
    QVERIFY(mOnlineAccountEditorView->formModel()->rowCount(QModelIndex()) == 3);
}


void TestCntOnlineAccountEditorView::cleanupTestCase()
{
    mWindow->deleteLater();
    delete mViewManager;
    mViewManager = 0;
    delete mOnlineAccountEditorView;
    mOnlineAccountEditorView = 0;
}

// EOF
