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

#include "cntnameseditorview.h"
#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntnameseditorview.h"

void TestCntNamesEditorView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mNamesEditorView = 0;
}

void TestCntNamesEditorView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mNamesEditorView = new CntNamesEditorView(mViewManager, 0);
    mWindow->addView(mNamesEditorView);
    mWindow->setCurrentView(mNamesEditorView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mNamesEditorView != 0);
}

void TestCntNamesEditorView::aboutToCloseView()
{
    QContact *contact= new QContact();
    QContactName *name = new QContactName();
    QContactNickname *nickName = new QContactNickname();
    
    mNamesEditorView->mDetailList.append(name);
    mNamesEditorView->mDetailList.append(nickName);
    mNamesEditorView->mContact = contact;
    
    // empty detail isn't saved
    mNamesEditorView->aboutToCloseView();
    QVERIFY(mNamesEditorView->mContact->details<QContactName>().count() == 0);
    
    // First name
    name->setFirst("First");
    mNamesEditorView->aboutToCloseView();
    QVERIFY(mNamesEditorView->mContact->details<QContactName>().count() == 1);
    name->setFirst("");
    
    // Last name
    name->setLast("Last");
    mNamesEditorView->aboutToCloseView();
    QVERIFY(mNamesEditorView->mContact->details<QContactName>().count() == 1);
    name->setLast("");
    
    // Middle name
    name->setMiddle("Middle");
    mNamesEditorView->aboutToCloseView();
    QVERIFY(mNamesEditorView->mContact->details<QContactName>().count() == 1);
    name->setMiddle("");
    
    // Name Prefix 
    name->setPrefix("Prefix");
    mNamesEditorView->aboutToCloseView();
    QVERIFY(mNamesEditorView->mContact->details<QContactName>().count() == 1);
    name->setPrefix("");
    
    // Name Suffix
    name->setSuffix("Suffix");
    mNamesEditorView->aboutToCloseView();
    QVERIFY(mNamesEditorView->mContact->details<QContactName>().count() == 1);
    name->setSuffix("");
    
    // NickName
    nickName->setNickname("Nickname");
    mNamesEditorView->aboutToCloseView();
    QVERIFY(mNamesEditorView->mContact->details<QContactNickname>().count() == 1);
    nickName->setNickname("");
    
    mWindow->removeView(mNamesEditorView);
    delete mNamesEditorView;
    mNamesEditorView = 0;
}

void TestCntNamesEditorView::initializeForm()
{
    mNamesEditorView = new CntNamesEditorView(mViewManager, 0);
    QContact *contact= new QContact;
    // create without name and nickname
    mNamesEditorView->mContact = contact;
    mNamesEditorView->initializeForm();
    
    //Check count is 6
    QVERIFY(mNamesEditorView->formModel()->rowCount(QModelIndex()) == 6);
    delete mNamesEditorView;
    mNamesEditorView = 0;
}    
 
void TestCntNamesEditorView::initializeFormData()   
{
    mNamesEditorView = new CntNamesEditorView(mViewManager, 0);
    QContactName *name = new QContactName();
    QContactNickname *nickName = new QContactNickname(); 

    name->setFirst("First");
    name->setLast("Last");
    name->setMiddle("Middle");
    name->setPrefix("Prefix");
    name->setSuffix("Suffix");
    nickName->setNickname("Nick");
    
    QContact *contact= new QContact;
    
    contact->saveDetail(name);
    contact->saveDetail(nickName);
    
    mNamesEditorView->mContact = contact;
    
    mNamesEditorView->initializeForm();
    QVERIFY(mNamesEditorView->formModel()->rowCount(QModelIndex()) == 6);
    
}

void TestCntNamesEditorView::cleanupTestCase()
{
    mWindow->deleteLater();
    delete mViewManager;
    mViewManager = 0;
    delete mNamesEditorView;
    mNamesEditorView = 0;
}

// EOF
