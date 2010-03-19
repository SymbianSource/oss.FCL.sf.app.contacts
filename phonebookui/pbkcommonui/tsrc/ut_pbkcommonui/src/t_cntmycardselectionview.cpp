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

#include "t_cntmycardselectionview.h"
#include "cntmycardselectionview.h"

#include <QtTest/QtTest>
#include <QObject>

#include "cntviewmanager.h"
#include "cntmainwindow.h"
#include "cntcollectionview.h"

void TestCntMyCardSelectionView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mMyCardSelectionView = 0;
}

void TestCntMyCardSelectionView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mMyCardSelectionView = new CntMyCardSelectionView(mViewManager, 0);
    mWindow->addView(mMyCardSelectionView);
    mWindow->setCurrentView(mMyCardSelectionView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mMyCardSelectionView != 0);
}

void TestCntMyCardSelectionView::aboutToCloseView()
{
    mViewManager->previousViewParameters().setNextViewId(CntViewParameters::collectionView);
    mMyCardSelectionView->aboutToCloseView();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::collectionView); 
}

void TestCntMyCardSelectionView::onListViewActivated()
{
    delete mMyCardSelectionView;
    mMyCardSelectionView = 0;
    
    mMyCardSelectionView = new CntMyCardSelectionView(mViewManager, 0);
    mWindow->addView(mMyCardSelectionView);
    mWindow->setCurrentView(mMyCardSelectionView);
    
    QList<QContactLocalId> ids = mMyCardSelectionView->contactManager()->contacts();
    mMyCardSelectionView->contactManager()->removeContacts(&ids);
    
    QContact contact;
    QContactName name;
    name.setFirst("first");
    contact.saveDetail(&name);
    mMyCardSelectionView->contactManager()->saveContact(&contact);
    
    QModelIndex empty = mMyCardSelectionView->contactModel()->index(100);
    
    mMyCardSelectionView->onListViewActivated(empty);
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::myCardSelectionView);
    
    QModelIndex notEmpty = mMyCardSelectionView->contactModel()->indexOfContact(contact);
    
    mMyCardSelectionView->onListViewActivated(notEmpty);
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::namesView); 
    QVERIFY(mMyCardSelectionView->contactManager()->selfContactId() == contact.localId());
}

void TestCntMyCardSelectionView::cleanupTestCase()
{
    delete mViewManager;
    mViewManager = 0;
    delete mWindow;
    mWindow = 0;
}

// EOF
