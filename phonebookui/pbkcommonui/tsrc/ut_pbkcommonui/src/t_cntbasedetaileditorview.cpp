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

#include "t_cntbasedetaileditorview.h"

#include <QtTest/QtTest>
#include <QObject>
#include "hbstubs_helper.h"

#include "cntviewmanager.h"
#include "cntmainwindow.h"
#include <hbmenu.h>
#include <hbgroupbox.h>

void TestCntBaseDetailEditorView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mBaseDetailEditorView = 0;
}

void TestCntBaseDetailEditorView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mBaseDetailEditorView = new CntBaseDetailEditorTestView(mViewManager, 0);
    mWindow->addView(mBaseDetailEditorView);
    mWindow->setCurrentView(mBaseDetailEditorView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mBaseDetailEditorView != 0);
}

void TestCntBaseDetailEditorView::addMenuItems()
{
    HbStubHelper::reset();
    mBaseDetailEditorView->addMenuItems();
    
    QVERIFY(HbStubHelper::widgetActionsCount() == 1);
}
	
void TestCntBaseDetailEditorView::formModel()
{
    HbDataFormModel *model = mBaseDetailEditorView->formModel();
    QVERIFY(model == mBaseDetailEditorView->formModel());
}

void TestCntBaseDetailEditorView::dataForm()
{
    HbDataForm *dataForm = mBaseDetailEditorView->dataForm();
    QVERIFY(dataForm == mBaseDetailEditorView->dataForm());
    QVERIFY(mBaseDetailEditorView->dataForm()->model() != 0);
}

void TestCntBaseDetailEditorView::setHeader()
{
    mBaseDetailEditorView->setHeader("dummy");
    QVERIFY(mBaseDetailEditorView->header()->titleText() == "dummy");
}

void TestCntBaseDetailEditorView::header()
{
    HbGroupBox *heading = mBaseDetailEditorView->header();
    QVERIFY(heading == mBaseDetailEditorView->header());
}

	
void TestCntBaseDetailEditorView::activateView()
{
    CntViewParameters params(CntViewParameters::namesView, CntViewParameters::collectionView); // these don't matter..
	QContact contact;
    params.setSelectedContact(contact);
    params.setSelectedAction("dummy");

    mBaseDetailEditorView->activateView(params);
    QVERIFY(mBaseDetailEditorView->mContact != 0);
    QVERIFY(mBaseDetailEditorView->mParamString == "dummy");

    params.setSelectedAction("add");

    mBaseDetailEditorView->activateView(params);
    QVERIFY(mBaseDetailEditorView->mResult);
}

void TestCntBaseDetailEditorView::cleanupTestCase()
{
    delete mWindow;
    mWindow = 0;
    delete mViewManager;
    mViewManager = 0;
}

// EOF
