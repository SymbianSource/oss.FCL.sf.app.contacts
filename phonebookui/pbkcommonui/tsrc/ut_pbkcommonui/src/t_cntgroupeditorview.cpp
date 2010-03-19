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

#include "cntgroupeditorview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntgroupeditorview.h"

void TestCntGroupEditorView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mGroupEditorView = 0;
}

void TestCntGroupEditorView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mGroupEditorView = new CntGroupEditorView(mViewManager, 0);
    mWindow->addView(mGroupEditorView);
    mWindow->setCurrentView(mGroupEditorView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mGroupEditorView != 0);
}

void TestCntGroupEditorView::aboutToCloseView()
{
    QContact *contact= new QContact();
    QContactName *groupName = new QContactName();
    QContactPhoneNumber *confCallNumber = new QContactPhoneNumber();
    
    mGroupEditorView->mDetailList.append(groupName);
    mGroupEditorView->mDetailList.append(confCallNumber);
    mGroupEditorView->mContact = contact;
    
    // empty detail isn't saved
    mGroupEditorView->aboutToCloseView();
    qDebug() << mGroupEditorView->mContact->details<QContactName>().count();
    QVERIFY(mGroupEditorView->mContact->details<QContactName>().count() == 1);
    
    
    // Group name
    groupName->setCustomLabel("Group Name");
    mGroupEditorView->aboutToCloseView();
    QVERIFY(mGroupEditorView->mContact->details<QContactName>().count() == 1);
    groupName->setCustomLabel("");
    
    // Conference Call number
    confCallNumber->setNumber("0501234567");
    mGroupEditorView->aboutToCloseView();
    QVERIFY(mGroupEditorView->mContact->details<QContactPhoneNumber>().count() == 1);
    confCallNumber->setNumber("");
    
    delete mGroupEditorView;
    mGroupEditorView = 0;
}

void TestCntGroupEditorView::initializeForm()
{
    mGroupEditorView = new CntGroupEditorView(mViewManager, 0);
    QContact *contact= new QContact;
    // create without group name and conference call number
    mGroupEditorView->mContact = contact;
    mGroupEditorView->initializeForm();
    
    //Check count is 2
    QVERIFY(mGroupEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mGroupEditorView;
    mGroupEditorView = 0;
}    
 
void TestCntGroupEditorView::initializeFormData()   
{
    mGroupEditorView = new CntGroupEditorView(mViewManager, 0);
    QContactName *groupName = new QContactName();
    QContactPhoneNumber *confCallNumber = new QContactPhoneNumber();

    groupName->setCustomLabel("Group Name");
    confCallNumber->setNumber("0501234567");
        
    QContact *contact= new QContact;
    
    contact->saveDetail(groupName);
    contact->saveDetail(confCallNumber);
    
    mGroupEditorView->mContact = contact;
    
    mGroupEditorView->initializeForm();
    QVERIFY(mGroupEditorView->formModel()->rowCount(QModelIndex()) == 2);
    
}

void TestCntGroupEditorView::cleanupTestCase()
{
    delete mWindow;
    mWindow = 0;
    delete mViewManager;
    mViewManager = 0;
    delete mGroupEditorView;
    mGroupEditorView = 0;
}

// EOF
