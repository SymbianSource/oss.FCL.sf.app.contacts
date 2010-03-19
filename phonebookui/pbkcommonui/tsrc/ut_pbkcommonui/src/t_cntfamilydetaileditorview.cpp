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

#include "cntfamilydetaileditorview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntfamilydetaileditorview.h"

void TestCntFamilyDetailEditorView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mFamilyDetailEditorView = 0;
}

void TestCntFamilyDetailEditorView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mFamilyDetailEditorView = new CntFamilyDetailEditorView(mViewManager, 0);
    mWindow->addView(mFamilyDetailEditorView);
    mWindow->setCurrentView(mFamilyDetailEditorView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mFamilyDetailEditorView != 0);
}

void TestCntFamilyDetailEditorView::aboutToCloseView()
{
    QContact *contact= new QContact();
    QContactFamily *family = new QContactFamily();
    mFamilyDetailEditorView->mDetailList.append(family);
    mFamilyDetailEditorView->mContact = contact;
    
    // empty address isn't saved
    mFamilyDetailEditorView->aboutToCloseView();
    QVERIFY(mFamilyDetailEditorView->mContact->details<QContactFamily>().count() == 0);
    // Spouse
    family->setSpouse("Spouse");
    mFamilyDetailEditorView->aboutToCloseView();
    QVERIFY(mFamilyDetailEditorView->mContact->details<QContactFamily>().count() == 1);
    family->setSpouse("");
    // Children
    family->setChildren(QStringList("Children"));
    mFamilyDetailEditorView->aboutToCloseView();
    QVERIFY(mFamilyDetailEditorView->mContact->details<QContactFamily>().count() == 1);
    
    mWindow->removeView(mFamilyDetailEditorView);
    delete mFamilyDetailEditorView;
    mFamilyDetailEditorView = 0;
}


void TestCntFamilyDetailEditorView::initializeForm()
{
    mFamilyDetailEditorView = new CntFamilyDetailEditorView(mViewManager, 0);
    
    QContact *contact= new QContact();
    mFamilyDetailEditorView->mContact = contact;
    mFamilyDetailEditorView->mParamString = "spouse";
    mFamilyDetailEditorView->initializeForm();
    
    //Check count
    QVERIFY(mFamilyDetailEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mFamilyDetailEditorView;
    mFamilyDetailEditorView = 0;
}    
 
void TestCntFamilyDetailEditorView::initializeFormData()   
{   
    mFamilyDetailEditorView = new CntFamilyDetailEditorView(mViewManager, 0);
    QContactFamily *family = new QContactFamily();
     
    family->setSpouse("Spouse");
    family->setChildren(QStringList("children"));
    
    QContact *contact= new QContact();
    
    contact->saveDetail(family);
    mFamilyDetailEditorView->mContact = contact;
    
    mFamilyDetailEditorView->initializeForm();
    QVERIFY(mFamilyDetailEditorView->formModel()->rowCount(QModelIndex()) == 2);
    
}

void TestCntFamilyDetailEditorView::cleanupTestCase()
{
    delete mWindow;
    mWindow = 0;
    delete mViewManager;
    mViewManager = 0;
    delete mFamilyDetailEditorView;
    mFamilyDetailEditorView = 0;
}

// EOF
