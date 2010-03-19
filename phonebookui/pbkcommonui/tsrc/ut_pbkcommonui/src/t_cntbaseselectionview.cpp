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
#include <hblistview.h>
#include <QGraphicsLinearLayout>

#include "cntviewmanager.h"
#include "cntmainwindow.h"

#include "t_cntbaseselectionview.h"

void TestCntBaseSelectionView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mSelectionView = 0;
}

void TestCntBaseSelectionView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mSelectionView = new CntBaseSelectionTestView(mViewManager, 0);
    mWindow->addView(mSelectionView);
    mWindow->setCurrentView(mSelectionView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mSelectionView != 0);
}

void TestCntBaseSelectionView::setupView()
{
    mSelectionView->setupView();

    QVERIFY(mSelectionView->listLayout() != 0);
    QVERIFY(mSelectionView->listView()->selectionMode() == HbAbstractItemView::MultiSelection);
}

void TestCntBaseSelectionView::addItemsToLayout()
{
    mWindow->removeView(mSelectionView);
    delete mSelectionView;
    mSelectionView = 0;
    
    mSelectionView = new CntBaseSelectionTestView(mViewManager, 0);
    mWindow->addView(mSelectionView);
    mWindow->setCurrentView(mSelectionView);

    mSelectionView->addItemsToLayout();
    QVERIFY(mSelectionView->widget() != 0);
    QVERIFY(mSelectionView->listLayout() != 0);
    QVERIFY(mSelectionView->listLayout()->count() == 1);
}

void TestCntBaseSelectionView::listLayout()
{
    mSelectionView->listLayout();
    QVERIFY(mSelectionView->listLayout() != 0);
}

void TestCntBaseSelectionView::listView()
{
    mSelectionView->listView();
    QVERIFY(mSelectionView->listView() != 0);
}

void TestCntBaseSelectionView::activateView()
{
    CntViewParameters params(CntViewParameters::noView);
    mSelectionView->activateView(params);
    QVERIFY(mSelectionView->listView()->model() != 0);
}

void TestCntBaseSelectionView::selectionModel()
{
    mWindow->removeView(mSelectionView);
    delete mSelectionView;
    mSelectionView = 0;
    
    mSelectionView = new CntBaseSelectionTestView(mViewManager, 0);
    mWindow->addView(mSelectionView);
    mWindow->setCurrentView(mSelectionView);
    
    CntViewParameters params(CntViewParameters::noView);
    mSelectionView->activateView(params);
    
    mSelectionView->selectionModel();
    QVERIFY(mSelectionView->listView() != 0);
    QVERIFY(mSelectionView->selectionModel() != 0);
}

void TestCntBaseSelectionView::cleanupTestCase()
{
    delete mWindow;
    mWindow = 0;
    delete mViewManager;
    mViewManager = 0;
}

// EOF
