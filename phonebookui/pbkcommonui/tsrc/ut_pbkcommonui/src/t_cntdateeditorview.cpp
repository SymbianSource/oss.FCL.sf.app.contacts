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

#include "cntdateeditorview.h"
#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntdateeditorview.h"

void TestCntDateEditorView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mDateEditorView = 0;
}

void TestCntDateEditorView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mDateEditorView = new CntDateEditorView(mViewManager, 0);
    mWindow->addView(mDateEditorView);
    mWindow->setCurrentView(mDateEditorView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mDateEditorView != 0);
}

void TestCntDateEditorView::aboutToCloseView()
{
    QContact *contact= new QContact();
    QContactBirthday *bDay = new QContactBirthday();
    QContactAnniversary *anniversary = new QContactAnniversary();
    
    mDateEditorView->mDetailList.append(bDay);
    mDateEditorView->mDetailList.append(anniversary);
    mDateEditorView->mContact = contact;
    
    // empty detail isn't saved
    mDateEditorView->aboutToCloseView();
    QVERIFY(mDateEditorView->mContact->details<QContactBirthday>().count() == 0);
    QVERIFY(mDateEditorView->mContact->details<QContactAnniversary>().count() == 0);
    
    // Birthday
    bDay->setDate(QDate::currentDate());
    mDateEditorView->aboutToCloseView();
    QVERIFY(mDateEditorView->mContact->details<QContactBirthday>().count() == 1);
    QVERIFY(mDateEditorView->mContact->details<QContactAnniversary>().count() == 0);
    bDay->setDate(QDate());
    
    // Anniversary
    anniversary->setOriginalDate(QDate::currentDate());
    mDateEditorView->aboutToCloseView();
    QVERIFY(mDateEditorView->mContact->details<QContactBirthday>().count() == 0);
    QVERIFY(mDateEditorView->mContact->details<QContactAnniversary>().count() == 1);
    bDay->setDate(QDate());
    
    mWindow->removeView(mDateEditorView);
    delete mDateEditorView;
    mDateEditorView = 0;
}


void TestCntDateEditorView::initializeForm()
{
    mDateEditorView = new CntDateEditorView(mViewManager, 0);
    QContact *contact= new QContact;
    // create without any dates specified
    mDateEditorView->mContact = contact;
    mDateEditorView->initializeForm();
    
    //Check count is 2
    QVERIFY(mDateEditorView->formModel()->rowCount(QModelIndex()) == 2);
    delete mDateEditorView;
    mDateEditorView = 0;
}    
 
void TestCntDateEditorView::initializeFormData()   
{
    mDateEditorView = new CntDateEditorView(mViewManager, 0);
    QContact *contact = new QContact();
    QContactBirthday *bDay = new QContactBirthday();
    QContactAnniversary *anniversary = new QContactAnniversary();
    
    bDay->setDate(QDate::currentDate());
    anniversary->setOriginalDate(QDate::currentDate());
    
    contact->saveDetail(bDay);
    contact->saveDetail(anniversary);
    
    mDateEditorView->mContact = contact;
    
    mDateEditorView->initializeForm();
    QVERIFY(mDateEditorView->formModel()->rowCount(QModelIndex()) == 2);   
}

void TestCntDateEditorView::cleanupTestCase()
{
    mWindow->deleteLater();
    delete mViewManager;
    mViewManager = 0;
    delete mDateEditorView;
    mDateEditorView = 0;
}

// EOF
