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

#include "cntnoteeditorview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntnoteeditorview.h"

void TestCntNoteEditorView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mNoteEditorView = 0;
}

void TestCntNoteEditorView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mNoteEditorView = new CntNoteEditorView(mViewManager, 0);
    mWindow->addView(mNoteEditorView);
    mWindow->setCurrentView(mNoteEditorView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mNoteEditorView != 0);
}

void TestCntNoteEditorView::aboutToCloseView()
{
    QContact *contact= new QContact();
    QContactNote *note = new QContactNote();
    mNoteEditorView->mDetailList.append(note);
    mNoteEditorView->mContact = contact;
    
    // empty detail isn't saved
    mNoteEditorView->aboutToCloseView();
    QVERIFY(mNoteEditorView->mContact->details<QContactNote>().count() == 0);
    
    // Note field
    note->setNote("Note");
    mNoteEditorView->aboutToCloseView();
    QVERIFY(mNoteEditorView->mContact->details<QContactNote>().count() == 1);
    note->setNote("");
    
    mWindow->removeView(mNoteEditorView);
    delete mNoteEditorView;
    mNoteEditorView = 0;
}


void TestCntNoteEditorView::initializeForm()
{
    mNoteEditorView = new CntNoteEditorView(mViewManager, 0);
    QContact *contact= new QContact;
    // create without data
    mNoteEditorView->mContact = contact;
    mNoteEditorView->initializeForm();
    
    //Check count
    QVERIFY(mNoteEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mNoteEditorView;
    mNoteEditorView = 0;
}

void TestCntNoteEditorView::initializeFormAdd()
{
    QContact *contact= new QContact;
    mNoteEditorView = new CntNoteEditorView(mViewManager, 0);
    mNoteEditorView->mContact = contact;
    
    QContactNote *note = new QContactNote();
    note->setNote("Note");
    contact->saveDetail(note);
    
    mNoteEditorView->mParamString = "add";
    mNoteEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mNoteEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mNoteEditorView;
    mNoteEditorView = 0;
}

void TestCntNoteEditorView::initializeFormFocusSecond()
{
    QContact *contact= new QContact;
    mNoteEditorView = new CntNoteEditorView(mViewManager, 0);
    mNoteEditorView->mContact = contact;
    
    QContactNote *note = new QContactNote();
    note->setNote("Note");
    contact->saveDetail(note);
    
    QContactNote *note2 = new QContactNote();
    note->setNote("Note2");
    contact->saveDetail(note2);
    
    mNoteEditorView->mParamString = "1"; // focus the second one, not verifiable tho
    mNoteEditorView->initializeForm();
    
    //Check count 
    QVERIFY(mNoteEditorView->formModel()->rowCount(QModelIndex()) == 3);
    delete mNoteEditorView;
    mNoteEditorView = 0;
}
 
void TestCntNoteEditorView::initializeFormData()   

{   // create with data
    mNoteEditorView = new CntNoteEditorView(mViewManager, 0);
    QContactNote *note = new QContactNote();
    note->setNote("Note");
    
    QContact *contact= new QContact;
    
    contact->saveDetail(note);
  
    
    mNoteEditorView->mContact = contact;
    
    mNoteEditorView->initializeForm();
    QVERIFY(mNoteEditorView->formModel()->rowCount(QModelIndex()) == 2);
    
}

void TestCntNoteEditorView::addDetail()
{
    mNoteEditorView->addDetail();
    QVERIFY(mNoteEditorView->formModel()->rowCount(QModelIndex()) == 3);
}

void TestCntNoteEditorView::cleanupTestCase()
{
    delete mWindow;
    mWindow = 0;
    delete mViewManager;
    mViewManager = 0;
    delete mNoteEditorView;
    mNoteEditorView = 0;
}

// EOF
