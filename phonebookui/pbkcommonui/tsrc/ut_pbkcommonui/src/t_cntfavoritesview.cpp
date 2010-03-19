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
#include <QGraphicsLinearLayout>
#include <hbtoolbar.h>

#include "cntfavoritesview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"
#include "t_cntfavoritesview.h"

#include "hbstubs_helper.h"

void TestCntFavoritesView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mFavoritesView = 0;
}

void TestCntFavoritesView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mFavoritesView = new CntFavoritesView(mViewManager, 0);
    mWindow->addView(mFavoritesView);
    mWindow->setCurrentView(mFavoritesView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mFavoritesView != 0);
}

void TestCntFavoritesView::openFetch()
{
    mFavoritesView->openFetch();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() ==
            CntViewParameters::collectionFavoritesSelectionView);
}

void TestCntFavoritesView::openNamesList()
{
    delete mFavoritesView;
    mFavoritesView = 0;
    
    mFavoritesView = new CntFavoritesView(mViewManager, 0);
    mWindow->addView(mFavoritesView);
    mWindow->setCurrentView(mFavoritesView);

    mFavoritesView->openNamesList();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() ==
            CntViewParameters::namesView);
}

void TestCntFavoritesView::aboutToCloseView()
{
    delete mFavoritesView;
    mFavoritesView = 0;
    
    mFavoritesView = new CntFavoritesView(mViewManager, 0);
    mWindow->addView(mFavoritesView);
    mWindow->setCurrentView(mFavoritesView);

    mFavoritesView->aboutToCloseView();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() ==
            CntViewParameters::collectionView);
}

void TestCntFavoritesView::addActionsToToolBar()
{
    HbStubHelper::reset();
    mFavoritesView->addActionsToToolBar();
    QVERIFY(HbStubHelper::widgetActionsCount() == 3);
}

void TestCntFavoritesView::cleanupTestCase()
{
    delete mViewManager;
    mViewManager = 0;
    delete mWindow;
    mWindow = 0;
}

// EOF
