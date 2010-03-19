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

#include "cntviewmanager.h"
#include "cntmainwindow.h"

#include "t_cntfavoritesselectionview.h"
#include "cntfavoritesselectionview.h"

void TestCntFavoritesSelectionView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mSelectionView = 0;
}

void TestCntFavoritesSelectionView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mSelectionView = new CntFavoritesSelectionView(mViewManager, 0);
    mWindow->addView(mSelectionView);
    mWindow->setCurrentView(mSelectionView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mSelectionView != 0);
}

void TestCntFavoritesSelectionView::aboutToCloseView()
{
    mSelectionView->aboutToCloseView();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::collectionView);
}

void TestCntFavoritesSelectionView::cleanupTestCase()
{
    delete mViewManager;
    mViewManager = 0;
    delete mWindow;
    mWindow = 0;
}

// EOF
