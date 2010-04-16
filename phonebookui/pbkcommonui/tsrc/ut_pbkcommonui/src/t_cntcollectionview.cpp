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
#include <hblistview.h>
#include <hbview.h>
#include <hbaction.h>

#include "cntcollectionview.h"
#include "cntcollectionlistmodel.h"
#include "cntmainwindow.h"

#include "hbstubs_helper.h"

#include "t_cntcollectionview.h"

void TestCntCollectionView::init()
{
    mViewManager = new TestViewManager();
    mCollectionView = new CntCollectionView();
    
    QVERIFY(mCollectionView != 0);
    
    CntViewParameters args( CntViewParameters::collectionView );
    mCollectionView->activate( mViewManager, args );
}

void TestCntCollectionView::cleanup()
{
    mCollectionView->deactivate();
    delete mViewManager;
    mViewManager = 0;
    delete mCollectionView;
    mCollectionView = 0;
}

void TestCntCollectionView::testActivate()
{
    // activate already called in init()
    QVERIFY(mCollectionView->mListView->model() != 0);
    QVERIFY(mCollectionView->mView != 0);
    QVERIFY(mCollectionView->mView->navigationAction() == mCollectionView->mSoftkey);
    QVERIFY(!mCollectionView->mDeleteGroupsAction->isEnabled());
    
    QContact group;
    group.setType(QContactType::TypeGroup);
    QContactName groupName;
    groupName.setCustomLabel("group");
    group.saveDetail(&groupName);
    mViewManager->contactManager("symbiam")->saveContact(&group);
    
    mCollectionView->mDeleteGroupsAction->setEnabled(true);
    CntViewParameters args( CntViewParameters::collectionView );
    mCollectionView->activate( mViewManager, args );
    
    QVERIFY(mCollectionView->mDeleteGroupsAction->isEnabled());
    QVERIFY(mCollectionView->mView->navigationAction() == mCollectionView->mSoftkey);
}

void TestCntCollectionView::testShowPreviousView()
{
    mCollectionView->showPreviousView();
}

void TestCntCollectionView::testOpenEmptyFavoritesGroup()
{
    QModelIndex favIndex = mCollectionView->mListView->model()->index(0, 0);
    
    mCollectionView->openGroup(favIndex);
}

void TestCntCollectionView::testOpenUserGroup()
{
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    
    mViewManager->contactManager("symbiam")->saveContact(&firstGroup);
    mCollectionView->refreshDataModel();
    
    QModelIndex userGroupIndex = mCollectionView->mListView->model()->index(1, 0);
    
    mCollectionView->openGroup(userGroupIndex);
}

void TestCntCollectionView::testShowContextMenu()
{
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    
    mViewManager->contactManager("symbiam")->saveContact(&firstGroup);
    mCollectionView->refreshDataModel();
    
    QModelIndex favIndex = mCollectionView->mListView->model()->index(0, 0);
    
    HbStubHelper::reset();
    mCollectionView->showContextMenu(mCollectionView->mListView->itemByIndex(favIndex), QPointF());
    QVERIFY(HbStubHelper::widgetActionsCount() == 1);
    
    QModelIndex userGroupIndex = mCollectionView->mListView->model()->index(1, 0);
    
    HbStubHelper::reset();
    mCollectionView->showContextMenu(mCollectionView->mListView->itemByIndex(userGroupIndex), QPointF());
    QVERIFY(HbStubHelper::widgetActionsCount() == 2);
}

void TestCntCollectionView::testRefreshDataModel()
{
    mCollectionView->refreshDataModel();
    QVERIFY(mCollectionView->mListView->model() == mCollectionView->mModel);
}

// EOF
