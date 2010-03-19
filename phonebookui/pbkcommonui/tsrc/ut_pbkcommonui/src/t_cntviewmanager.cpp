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

#include "t_cntviewmanager.h"

#include <QtTest/QtTest>
#include <QObject>

#include "cntcollectionview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"
#include "cntbaseview.h"


void TestCntViewManager::initTestCase()
{
    QContactManager manager("symbian");
    QList<QContactLocalId> ids = manager.contacts();
    manager.removeContacts(&ids);
    
    mViewManager = 0;
    mMainWindow = 0;
}

void TestCntViewManager::createViewManager()
{
    mMainWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mMainWindow);

    QVERIFY(mViewManager->mDefaultView == mViewManager->getView(CntViewParameters::namesView));
    QVERIFY(mViewManager->mDefaultViewId == CntViewParameters::namesView);
    QVERIFY(mMainWindow->currentView() == mViewManager->mDefaultView);
}

void TestCntViewManager::mainWindow()
{
    CntMainWindow *win = 0;
    win = mViewManager->mainWindow();
    QVERIFY(win == mMainWindow);
}

void TestCntViewManager::setDefaultView()
{
    // this should do nothing, check that namesView is still there
    mViewManager->setDefaultView(CntViewParameters::noView);
    QVERIFY(mViewManager->mDefaultView == mViewManager->getView(CntViewParameters::namesView));
    QVERIFY(mViewManager->mDefaultViewId == CntViewParameters::namesView);
    QVERIFY(mMainWindow->currentView() == mViewManager->mDefaultView);
    
    // this shouldn't do any re-assigning either since namesView is the default view already
    mViewManager->setDefaultView(CntViewParameters::namesView);
    QVERIFY(mViewManager->mDefaultView == mViewManager->getView(CntViewParameters::namesView));
    QVERIFY(mViewManager->mDefaultViewId == CntViewParameters::namesView);
    QVERIFY(mMainWindow->currentView() == mViewManager->mDefaultView);

    mViewManager->setDefaultView(CntViewParameters::collectionView);
    QVERIFY(mViewManager->mDefaultView == mViewManager->getView(CntViewParameters::collectionView));
    QVERIFY(mViewManager->mDefaultViewId == CntViewParameters::collectionView);
    QVERIFY(mMainWindow->currentView() == mViewManager->mDefaultView);
}

void TestCntViewManager::setPreviousViewParameters()
{
    CntViewParameters viewParameters(CntViewParameters::namesView);
    mViewManager->setPreviousViewParameters(mViewManager->getView(CntViewParameters::collectionView), viewParameters);

    QVERIFY(mViewManager->previousViewParameters().nextViewId() == CntViewParameters::collectionView);
    QVERIFY(mViewManager->previousViewParameters().previousViewId() == viewParameters.nextViewId());

    CntViewParameters params(CntViewParameters::editView);
    mViewManager->setPreviousViewParameters(0, params);

    // verify that nothing was changed
    QVERIFY(mViewManager->previousViewParameters().nextViewId() == CntViewParameters::collectionView);
    QVERIFY(mViewManager->previousViewParameters().previousViewId() == viewParameters.nextViewId());
}

void TestCntViewManager::onActivateViewId()
{
    delete mViewManager;
    delete mMainWindow;
    mViewManager = 0;
    mMainWindow = 0;

    mMainWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mMainWindow, CntViewParameters::collectionView);

    QVERIFY(static_cast<CntBaseView *>(mMainWindow->currentView())->viewId() !=
            CntViewParameters::myCardView);

    mViewManager->onActivateView(static_cast<int>(CntViewParameters::myCardView));

    QVERIFY(static_cast<CntBaseView *>(mMainWindow->currentView())->viewId() ==
            CntViewParameters::myCardView);
}

void TestCntViewManager::onActivateViewParams()
{
    // activating an empty view does nothing, just here to verify it
    CntViewParameters viewParameters(CntViewParameters::noView);
    mViewManager->onActivateView(viewParameters);

    QVERIFY(static_cast<CntBaseView *>(mMainWindow->currentView())->viewId() !=
            CntViewParameters::myCardSelectionView);

    CntViewParameters viewParameters2(CntViewParameters::myCardSelectionView);
    mViewManager->onActivateView(viewParameters2);

    QVERIFY(static_cast<CntBaseView *>(mMainWindow->currentView())->viewId() ==
            CntViewParameters::myCardSelectionView);
}

void TestCntViewManager::onActivatePreviousView()
{
    QVERIFY(static_cast<CntBaseView *>(mMainWindow->currentView())->viewId() !=
            CntViewParameters::myCardView);

    mViewManager->onActivatePreviousView();

    QVERIFY(static_cast<CntBaseView *>(mMainWindow->currentView())->viewId() ==
            CntViewParameters::myCardView);
}

void TestCntViewManager::addViewToWindow()
{
    delete mViewManager;
    delete mMainWindow;
    mViewManager = 0;
    mMainWindow = 0;

    mMainWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mMainWindow, CntViewParameters::noView);

    mViewManager->addViewToWindow(0);
    QVERIFY(mMainWindow->currentView() == 0);

    CntBaseView *view = new CntCollectionView(mViewManager);

    mViewManager->addViewToWindow(view);

    QVERIFY(static_cast<CntBaseView *>(mMainWindow->currentView()) == view);
}

void TestCntViewManager::removeViewFromWindow()
{
    // removing a null view doesn't change the viewCount
    mViewManager->removeViewFromWindow(mViewManager->getView(CntViewParameters::noView));
    QVERIFY(mMainWindow->views().count() == 1);
    
    mViewManager->removeViewFromWindow(static_cast<CntBaseView *>(mMainWindow->currentView()));
    QVERIFY(mMainWindow->views().count() == 0);
}

void TestCntViewManager::getView()
{
    delete mViewManager;
    delete mMainWindow;
    mViewManager = 0;
    mMainWindow = 0;

    mMainWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mMainWindow, CntViewParameters::noView);

    CntBaseView *view = mViewManager->getView(CntViewParameters::namesView);
    QVERIFY(view->viewId() == CntViewParameters::namesView);
    delete view;
    view = 0;

    view = mViewManager->getView(CntViewParameters::collectionView);
    QVERIFY(view->viewId() == CntViewParameters::collectionView);
    delete view;
    view = 0;

    view = mViewManager->getView(CntViewParameters::collectionFavoritesView);
    QVERIFY(view->viewId() == CntViewParameters::collectionFavoritesView);
    delete view;
    view = 0;

    view = mViewManager->getView(CntViewParameters::collectionFavoritesSelectionView);
    QVERIFY(view->viewId() == CntViewParameters::collectionFavoritesSelectionView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::commLauncherView);
    QVERIFY(view->viewId() == CntViewParameters::commLauncherView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::emailEditorView);
    QVERIFY(view->viewId() == CntViewParameters::emailEditorView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::namesEditorView);
    QVERIFY(view->viewId() == CntViewParameters::namesEditorView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::urlEditorView);
    QVERIFY(view->viewId() == CntViewParameters::urlEditorView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::companyEditorView);
    QVERIFY(view->viewId() == CntViewParameters::companyEditorView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::phoneNumberEditorView);
    QVERIFY(view->viewId() == CntViewParameters::phoneNumberEditorView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::onlineAccountEditorView);
    QVERIFY(view->viewId() == CntViewParameters::onlineAccountEditorView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::noteEditorView);
    QVERIFY(view->viewId() == CntViewParameters::noteEditorView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::familyDetailEditorView);
    QVERIFY(view->viewId() == CntViewParameters::familyDetailEditorView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::addressEditorView);
    QVERIFY(view->viewId() == CntViewParameters::addressEditorView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::dateEditorView);
    QVERIFY(view->viewId() == CntViewParameters::dateEditorView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::imageEditorView);
    QVERIFY(view->viewId() == CntViewParameters::imageEditorView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::editView);
    QVERIFY(view->viewId() == CntViewParameters::editView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::myCardSelectionView);
    QVERIFY(view->viewId() == CntViewParameters::myCardSelectionView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::myCardView);
    QVERIFY(view->viewId() == CntViewParameters::myCardView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::groupEditorView);
    QVERIFY(view->viewId() == CntViewParameters::groupEditorView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::groupMemberView);
    QVERIFY(view->viewId() == CntViewParameters::groupMemberView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::groupActionsView);
    QVERIFY(view->viewId() == CntViewParameters::groupActionsView);
    delete view;
    view = 0;
    
    view = mViewManager->getView(CntViewParameters::historyView);
    QVERIFY(view->viewId() == CntViewParameters::historyView);
    delete view;
    view = 0;

    view = mViewManager->getView(CntViewParameters::noView);
    QVERIFY(view == 0);

    mViewManager->setDefaultView(CntViewParameters::namesView);
    view = mViewManager->getView(CntViewParameters::namesView);
    QVERIFY(view == mViewManager->getView(CntViewParameters::namesView));
}

void TestCntViewManager::cleanupTestCase()
{
    delete mViewManager;
    delete mMainWindow;
}

//EOF
