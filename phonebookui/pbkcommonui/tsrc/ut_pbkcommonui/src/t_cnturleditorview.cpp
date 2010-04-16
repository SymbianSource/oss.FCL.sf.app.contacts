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

#include "cnturleditorview.h"
#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"
#include "t_cnturleditorview.h"

void TestCntUrlEditorView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mUrlEditorView = 0;
}

void TestCntUrlEditorView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mUrlEditorView = new CntUrlEditorView(mViewManager, 0);
    mWindow->addView(mUrlEditorView);
    mWindow->setCurrentView(mUrlEditorView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mUrlEditorView != 0);
}

void TestCntUrlEditorView::aboutToCloseView()
{
    QContact *contact= new QContact();
    QContactUrl *url = new QContactUrl();
    mUrlEditorView->mDetailList.append(url);
    mUrlEditorView->mContact = contact;
    
    // empty detail isn't saved
    mUrlEditorView->aboutToCloseView();
    QVERIFY(mUrlEditorView->mContact->details<QContactUrl>().count() == 0);
    
    // Url field set
    url->setUrl("http://www.testurl.net");
    mUrlEditorView->aboutToCloseView();
    QVERIFY(mUrlEditorView->mContact->details<QContactUrl>().count() == 1);
    url->setUrl("");
    
    mWindow->removeView(mUrlEditorView);
    delete mUrlEditorView;
    mUrlEditorView = 0;
}

void TestCntUrlEditorView::itemModel()
{
    mUrlEditorView = new CntUrlEditorView(mViewManager, 0);
    QStandardItemModel *itemModel = new QStandardItemModel();
    QContact *contact= new QContact;
       
    mUrlEditorView->mContact = contact;
    itemModel = mUrlEditorView->itemModel();
    QVERIFY(itemModel->rowCount(QModelIndex()) == 3 );
}

void TestCntUrlEditorView::initializeForm()
{
    mUrlEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mUrlEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mUrlEditorView;
    mUrlEditorView = 0;
}

void TestCntUrlEditorView::initializeFormAdd()
{
    QContact *contact= new QContact;
    mUrlEditorView = new CntUrlEditorView(mViewManager, 0);
    mUrlEditorView->mContact = contact;
    
    QContactUrl *url = new QContactUrl();
    url->setUrl("http://www.testurl.net");
    contact->saveDetail(url);
    
    mUrlEditorView->mParamString = "add";
    mUrlEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mUrlEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mUrlEditorView;
    mUrlEditorView = 0;
}

void TestCntUrlEditorView::initializeFormFocusSecond()
{
    QContact *contact= new QContact;
    mUrlEditorView = new CntUrlEditorView(mViewManager, 0);
    mUrlEditorView->mContact = contact;
    
    QContactUrl *url = new QContactUrl();
    url->setUrl("http://www.testurl.net");
    contact->saveDetail(url);
    
    QContactUrl *url2 = new QContactUrl();
    url2->setUrl("http://www.testurl2.com");
    contact->saveDetail(url2);
    
    mUrlEditorView->mParamString = "1"; // focus the second one, not verifiable tho
    mUrlEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mUrlEditorView->formModel()->rowCount(QModelIndex()) == 3);
    delete mUrlEditorView;
    mUrlEditorView = 0;
}
 
void TestCntUrlEditorView::initializeFormData()   
{
    QContact *contact= new QContact;
    mUrlEditorView = new CntUrlEditorView(mViewManager, 0);
    mUrlEditorView->mContact = contact;
    
    QContactUrl *url = new QContactUrl();
    url->setUrl("http://www.testurl.net");

    contact->saveDetail(url);
   
    mUrlEditorView->initializeForm();
    QVERIFY(mUrlEditorView->formModel()->rowCount(QModelIndex()) == 2);
}

void TestCntUrlEditorView::addDetail()
{
    mUrlEditorView->addDetail();
    QVERIFY(mUrlEditorView->formModel()->rowCount(QModelIndex()) == 3);
}

void TestCntUrlEditorView::cleanupTestCase()
{
    mWindow->deleteLater();
    delete mViewManager;
    mViewManager = 0;
    delete mUrlEditorView;
    mUrlEditorView = 0;
}

// EOF
