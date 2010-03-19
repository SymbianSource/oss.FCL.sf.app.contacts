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
#include <hbtoolbar.h>

#include "cntcollectionview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"

#include "hbstubs_helper.h"

#include "t_cntcollectionview.h"

void TestCntCollectionView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mCollectionView = 0;
}

void TestCntCollectionView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mCollectionView = new CntCollectionView(mViewManager);
    mWindow->addView(mCollectionView);
    mWindow->setCurrentView(mCollectionView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mCollectionView != 0);
}

void TestCntCollectionView::aboutToCloseView()
{
    mCollectionView->aboutToCloseView();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::namesView);
}

void TestCntCollectionView::handleExecutedCommand()
{
    delete mCollectionView;
    mCollectionView = 0;
    
    mCollectionView = new CntCollectionView(mViewManager, 0);
    mWindow->addView(mCollectionView);
    mWindow->setCurrentView(mCollectionView);
    
    QList<QContactLocalId> ids = mCollectionView->contactManager()->contacts();
    mCollectionView->contactManager()->removeContacts(&ids);
    
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mCollectionView->contactManager()->saveContact(&firstGroup);
    mCollectionView->setDataModel();
    
    mCollectionView->handleExecutedCommand("dummy", QContact());
    QVERIFY(mCollectionView->listView()->model()->rowCount() == 2);
    
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QList<QContactLocalId> groupContactIds = mCollectionView->contactManager()->contacts(groupFilter);
    mCollectionView->handleExecutedCommand("delete", mCollectionView->contactManager()->contact(groupContactIds.at(0)));
    QVERIFY(mCollectionView->listView()->model()->rowCount() == 1);
}

void TestCntCollectionView::addActionsToToolBar()
{   
    HbStubHelper::reset();
    mCollectionView->addActionsToToolBar();
    QVERIFY(HbStubHelper::widgetActionsCount() == 4);
}

void TestCntCollectionView::setDataModel()
{
    mWindow->removeView(mCollectionView);
    delete mCollectionView;
    mCollectionView = 0;
    
    mCollectionView = new CntCollectionView(mViewManager, 0);
    mWindow->addView(mCollectionView);
    mWindow->setCurrentView(mCollectionView);
    
    mCollectionView->setDataModel();
    QVERIFY(mCollectionView->listView()->model() != 0);
}

void TestCntCollectionView::onLongPressed()
{
    mWindow->removeView(mCollectionView);
    delete mCollectionView;
    mCollectionView = 0;
    
    mCollectionView = new CntCollectionView(mViewManager, 0);
    mWindow->addView(mCollectionView);
    mWindow->setCurrentView(mCollectionView);
    
    QList<QContactLocalId> ids = mCollectionView->contactManager()->contacts();
    mCollectionView->contactManager()->removeContacts(&ids);
    
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mCollectionView->contactManager()->saveContact(&firstGroup);
    mCollectionView->setDataModel();
    
    QModelIndex favIndex = mCollectionView->listView()->model()->index(0, 0);
    
    HbStubHelper::reset();
    mCollectionView->onLongPressed(mCollectionView->listView()->itemByIndex(favIndex), QPointF());
    QVERIFY(HbStubHelper::widgetActionsCount() == 2);
    
    QModelIndex userGroupIndex = mCollectionView->listView()->model()->index(1, 0);
    
    HbStubHelper::reset();
    mCollectionView->onLongPressed(mCollectionView->listView()->itemByIndex(userGroupIndex), QPointF());
    QVERIFY(HbStubHelper::widgetActionsCount() == 3);
}

void TestCntCollectionView::onListViewActivated()
{
    QModelIndex favIndex = mCollectionView->listView()->model()->index(0, 0);
    
    mCollectionView->onListViewActivated(favIndex);
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::collectionFavoritesView);
    
    delete mCollectionView;
    mCollectionView = 0;
    
    mCollectionView = new CntCollectionView(mViewManager, 0);
    mWindow->addView(mCollectionView);
    mWindow->setCurrentView(mCollectionView);
    
    mCollectionView->setDataModel();
    
    QModelIndex userGroupIndex = mCollectionView->listView()->model()->index(1, 0);
    
    mCollectionView->onListViewActivated(userGroupIndex);
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::groupActionsView);
}

void TestCntCollectionView::cleanupTestCase()
{
    delete mViewManager;
    mViewManager = 0;
    delete mWindow;
    mWindow = 0;
}

// EOF
