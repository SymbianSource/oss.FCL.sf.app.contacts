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

#include "cntcompanyeditorview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntcompanyeditorview.h"

void TestCntCompanyEditorView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mCompanyEditorView = 0;
}

void TestCntCompanyEditorView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mCompanyEditorView = new CntCompanyEditorView(mViewManager, 0);
    mWindow->addView(mCompanyEditorView);
    mWindow->setCurrentView(mCompanyEditorView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mCompanyEditorView != 0);
}

void TestCntCompanyEditorView::aboutToCloseView()
{
    QContact *contact= new QContact();
    QContactOrganization *company = new QContactOrganization();
    mCompanyEditorView->mDetailList.append(company);
    mCompanyEditorView->mContact = contact;
    
    // empty detail isn't saved
    mCompanyEditorView->aboutToCloseView();
    QVERIFY(mCompanyEditorView->mContact->details<QContactOrganization>().count() == 0);
    
    // Company name
    company->setName("Company name");
    mCompanyEditorView->aboutToCloseView();
    QVERIFY(mCompanyEditorView->mContact->details<QContactOrganization>().count() == 1);
    company->setName("");
    
    // Title
    company->setTitle("Company Title");
    mCompanyEditorView->aboutToCloseView();
    QVERIFY(mCompanyEditorView->mContact->details<QContactOrganization>().count() == 1);
    company->setTitle("");
    
    // Department
    QStringList list;
    list << "Department";
    company->setDepartment(list);
    mCompanyEditorView->aboutToCloseView();
    QVERIFY(mCompanyEditorView->mContact->details<QContactOrganization>().count() == 1);
    company->setDepartment(QStringList());
    
    // Assistant name
    company->setAssistantName("Assistant");
    mCompanyEditorView->aboutToCloseView();
    QVERIFY(mCompanyEditorView->mContact->details<QContactOrganization>().count() == 1);
    
    mWindow->removeView(mCompanyEditorView);
    delete mCompanyEditorView;
    mCompanyEditorView = 0;
}


void TestCntCompanyEditorView::initializeForm()
{
    mCompanyEditorView = new CntCompanyEditorView(mViewManager, 0);
    QContact *contact= new QContact();
    // create without data
    mCompanyEditorView->mContact = contact;
    mCompanyEditorView->mParamString = "company";
    mCompanyEditorView->initializeForm();
    
    //Check count
    QVERIFY(mCompanyEditorView->formModel()->rowCount(QModelIndex()) == 4);
    delete mCompanyEditorView;
    mCompanyEditorView = 0;
}
 
void TestCntCompanyEditorView::initializeFormData()   
{
    mCompanyEditorView = new CntCompanyEditorView(mViewManager, 0);
    QContactOrganization *company = new QContactOrganization();
    company->setName("Company name");
    company->setTitle("Company Title");
    QStringList list;
    list << "Department";
    company->setDepartment(list);
    company->setAssistantName("Assistant");
    
    QContact *contact= new QContact();
    
    contact->saveDetail(company);
        
    mCompanyEditorView->mContact = contact;
    
    mCompanyEditorView->initializeForm();
    QVERIFY(mCompanyEditorView->formModel()->rowCount(QModelIndex()) == 4);
    
}

void TestCntCompanyEditorView::cleanupTestCase()
{
    delete mWindow;
    mWindow = 0;
    delete mViewManager;
    mViewManager = 0;
    delete mCompanyEditorView;
    mCompanyEditorView = 0;
}

// EOF
