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

#include "cntphonenumbereditorview.h"
#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntphonenumbereditorview.h"

void TestCntPhoneNumberEditorView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mPhoneNumberEditorView = 0;
}

void TestCntPhoneNumberEditorView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mPhoneNumberEditorView = new CntPhoneNumberEditorView(mViewManager, 0);
    mWindow->addView(mPhoneNumberEditorView);
    mWindow->setCurrentView(mPhoneNumberEditorView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mPhoneNumberEditorView != 0);
}

void TestCntPhoneNumberEditorView::aboutToCloseView()
{
    QContact *contact= new QContact();
    QContactPhoneNumber *number = new QContactPhoneNumber();
    mPhoneNumberEditorView->mDetailList.append(number);
    mPhoneNumberEditorView->mContact = contact;
    
    // empty detail isn't saved
    mPhoneNumberEditorView->aboutToCloseView();
    QVERIFY(mPhoneNumberEditorView->mContact->details<QContactPhoneNumber>().count() == 0);
    
    // Phone Number field set
    number->setNumber("123456789");
    mPhoneNumberEditorView->aboutToCloseView();
    QVERIFY(mPhoneNumberEditorView->mContact->details<QContactPhoneNumber>().count() == 1);
    number->setNumber("");
    
    mWindow->removeView(mPhoneNumberEditorView);
    delete mPhoneNumberEditorView;
    mPhoneNumberEditorView = 0;
}

void TestCntPhoneNumberEditorView::itemModel()
{
    mPhoneNumberEditorView = new CntPhoneNumberEditorView(mViewManager, 0);
    QStandardItemModel *itemModel = new QStandardItemModel();
    QContact *contact= new QContact();
       
    mPhoneNumberEditorView->mContact = contact;
    itemModel = mPhoneNumberEditorView->itemModel();
    QVERIFY(itemModel->rowCount(QModelIndex()) == 13 );
}

void TestCntPhoneNumberEditorView::initializeForm()
{
    mPhoneNumberEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mPhoneNumberEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mPhoneNumberEditorView;
    mPhoneNumberEditorView = 0;
}

void TestCntPhoneNumberEditorView::initializeFormAdd()
{
    QContact *contact= new QContact;
    mPhoneNumberEditorView = new CntPhoneNumberEditorView(mViewManager, 0);
    mPhoneNumberEditorView->mContact = contact;
    
    QContactPhoneNumber *number = new QContactPhoneNumber();
    number->setNumber("123456789");
    number->setSubTypes(QContactPhoneNumber::SubTypeMobile);
    contact->saveDetail(number);
    
    mPhoneNumberEditorView->mParamString = "add";
    mPhoneNumberEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mPhoneNumberEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mPhoneNumberEditorView;
    mPhoneNumberEditorView = 0;
}

void TestCntPhoneNumberEditorView::initializeFormFocusSecond()
{
    QContact *contact= new QContact;
    mPhoneNumberEditorView = new CntPhoneNumberEditorView(mViewManager, 0);
    mPhoneNumberEditorView->mContact = contact;
    
    QContactPhoneNumber *number = new QContactPhoneNumber();
    number->setNumber("123456789");
    number->setSubTypes(QContactPhoneNumber::SubTypeMobile);
    contact->saveDetail(number);
    
    QContactPhoneNumber *number2 = new QContactPhoneNumber();
    number2->setNumber("987654321");
    number2->setSubTypes(QContactPhoneNumber::SubTypeMobile);
    contact->saveDetail(number2);
    
    mPhoneNumberEditorView->mParamString = "1"; // focus the second one, not verifiable tho
    mPhoneNumberEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mPhoneNumberEditorView->formModel()->rowCount(QModelIndex()) == 3);
    delete mPhoneNumberEditorView;
    mPhoneNumberEditorView = 0;
}
 
void TestCntPhoneNumberEditorView::initializeFormData()   
{
    QContact *contact= new QContact;
    mPhoneNumberEditorView = new CntPhoneNumberEditorView(mViewManager, 0);
    mPhoneNumberEditorView->mContact = contact;
    
    QContactPhoneNumber *number = new QContactPhoneNumber();
    number->setNumber("123456789");
    number->setSubTypes(QContactPhoneNumber::SubTypeMobile);

    contact->saveDetail(number);
   
    mPhoneNumberEditorView->initializeForm();
    QVERIFY(mPhoneNumberEditorView->formModel()->rowCount(QModelIndex()) == 2);
}

void TestCntPhoneNumberEditorView::addDetail()
{
    mPhoneNumberEditorView->addDetail();
    QVERIFY(mPhoneNumberEditorView->formModel()->rowCount(QModelIndex()) == 3);
}

void TestCntPhoneNumberEditorView::cleanupTestCase()
{
    mWindow->deleteLater();
    delete mViewManager;
    mViewManager = 0;
    delete mPhoneNumberEditorView;
    mPhoneNumberEditorView = 0;
}

// EOF
