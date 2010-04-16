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

#include "cntemaileditorview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntemaileditorview.h"

void TestCntEmailEditorView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mEmailEditorView = 0;
}

void TestCntEmailEditorView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mEmailEditorView = new CntEmailEditorView(mViewManager, 0);
    mWindow->addView(mEmailEditorView);
    mWindow->setCurrentView(mEmailEditorView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mEmailEditorView != 0);
}

void TestCntEmailEditorView::aboutToCloseView()
{
    QContact *contact= new QContact();
    QContactEmailAddress *email = new QContactEmailAddress();
    mEmailEditorView->mDetailList.append(email);
    mEmailEditorView->mContact = contact;
    
    // empty email not saved
    mEmailEditorView->aboutToCloseView();
    QVERIFY(mEmailEditorView->mContact->details<QContactEmailAddress>().count() == 0);
    
    // one address
    email->setEmailAddress("dummy@dummy.fi");
    mEmailEditorView->aboutToCloseView();
    QVERIFY(mEmailEditorView->mContact->details<QContactEmailAddress>().count() == 1);
    
    mWindow->removeView(mEmailEditorView);
    delete mEmailEditorView;
    mEmailEditorView = 0;
}

void TestCntEmailEditorView::itemModel()
{
    mEmailEditorView = new CntEmailEditorView(mViewManager, 0);
    QStandardItemModel *itemModel = new QStandardItemModel();
    QContact *contact= new QContact;
       
    mEmailEditorView->mContact = contact;
    itemModel = mEmailEditorView->itemModel();
    QVERIFY(itemModel->rowCount(QModelIndex()) == 3 );
}

void TestCntEmailEditorView::initializeForm()
{
    mEmailEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mEmailEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mEmailEditorView;
    mEmailEditorView = 0;
}

void TestCntEmailEditorView::initializeFormAdd()
{
    QContact *contact= new QContact;
    mEmailEditorView = new CntEmailEditorView(mViewManager, 0);
    mEmailEditorView->mContact = contact;
    
    QContactEmailAddress *email = new QContactEmailAddress();
    email->setEmailAddress("dummy@something");
    contact->saveDetail(email);
    
    mEmailEditorView->mParamString = "add";
    mEmailEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mEmailEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mEmailEditorView;
    mEmailEditorView = 0;
}

void TestCntEmailEditorView::initializeFormFocusSecond()
{
    QContact *contact= new QContact;
    mEmailEditorView = new CntEmailEditorView(mViewManager, 0);
    mEmailEditorView->mContact = contact;
    
    QContactEmailAddress *email = new QContactEmailAddress();
    email->setEmailAddress("dummy@something");
    contact->saveDetail(email);
    
    QContactEmailAddress *email2 = new QContactEmailAddress();
    email2->setEmailAddress("dummy2@something");
    contact->saveDetail(email2);
    
    mEmailEditorView->mParamString = "1"; // focus the second one, not verifiable tho
    mEmailEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mEmailEditorView->formModel()->rowCount(QModelIndex()) == 3);
    delete mEmailEditorView;
    mEmailEditorView = 0;
}
 
void TestCntEmailEditorView::initializeFormData()   
{
    QContact *contact= new QContact;
    mEmailEditorView = new CntEmailEditorView(mViewManager, 0);
    mEmailEditorView->mContact = contact;
    
    QContactEmailAddress *email = new QContactEmailAddress();
    email->setEmailAddress("dummy@something");

    contact->saveDetail(email);
   
    mEmailEditorView->initializeForm();
    QVERIFY(mEmailEditorView->formModel()->rowCount(QModelIndex()) == 2);
    
}

void TestCntEmailEditorView::addDetail()
{
    mEmailEditorView->addDetail();
    QVERIFY(mEmailEditorView->formModel()->rowCount(QModelIndex()) == 3);
}


void TestCntEmailEditorView::cleanupTestCase()
{
    mWindow->deleteLater();
    delete mViewManager;
    mViewManager = 0;
    delete mEmailEditorView;
    mEmailEditorView = 0;
}

// EOF
