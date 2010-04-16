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
#include <hblistview.h>

#include "cntgroupmemberselectionview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"

#include "hbstubs_helper.h"

#include "t_cntgroupmemberselectionview.h"

void TestCntGroupMemberSelectionView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mGroupMemberSelectionView = 0;
}

void TestCntGroupMemberSelectionView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mGroupMemberSelectionView = new CntGroupMemberSelectionView(mViewManager);
    mWindow->addView(mGroupMemberSelectionView);
    mWindow->setCurrentView(mGroupMemberSelectionView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mGroupMemberSelectionView != 0);
}

void TestCntGroupMemberSelectionView::aboutToCloseView()
{
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mGroupMemberSelectionView->contactManager()->saveContact(&firstGroup);
    
    mGroupMemberSelectionView->mContact = &firstGroup;
    
    mGroupMemberSelectionView->aboutToCloseView();
    //QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::collectionView);
}

void TestCntGroupMemberSelectionView::saveGroup()
{
    mGroupMemberSelectionView->listView()->setModel(mGroupMemberSelectionView->contactModel());
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mGroupMemberSelectionView->contactManager()->saveContact(&firstGroup);
    
    // create and save contact to the group
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    mGroupMemberSelectionView->contactManager()->saveContact(&firstContact);
    
    // set selection model, and saveGroup 
    
    QModelIndex index = mGroupMemberSelectionView->contactModel()->indexOfContact(firstContact);
    mGroupMemberSelectionView->selectionModel()->select(index, QItemSelectionModel::Select);
    
    mGroupMemberSelectionView->mContact = &firstGroup;
    // Now that there is a contact in selection Model, call save Group and then check if it is part of the relationship
    mGroupMemberSelectionView->saveGroup();
    
    // Use relationship filter to get list of contacts in the relationship (if any)
    QContactRelationshipFilter filter;
    filter.setRelationshipType(QContactRelationship::HasMember);
    filter.setRole(QContactRelationshipFilter::First); 
    filter.setOtherParticipantId(firstContact.id());

    QList<QContactLocalId> mContactsList = mGroupMemberSelectionView->contactManager()->contacts(filter);
    int count = mContactsList.count();
        
    QVERIFY(count == 0);
    
    
    
}



void TestCntGroupMemberSelectionView::addActionsToToolBar()
{   
    HbStubHelper::reset();
    mGroupMemberSelectionView->addActionsToToolBar();
    QVERIFY(HbStubHelper::widgetActionsCount() == 3);
}


void TestCntGroupMemberSelectionView::OnCancel()
{
    mGroupMemberSelectionView->OnCancel();
    //QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::collectionView);
}


void TestCntGroupMemberSelectionView::openGroupNameEditor()
{
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mGroupMemberSelectionView->contactManager()->saveContact(&firstGroup);
    mGroupMemberSelectionView->mContact = &firstGroup;
    
    
    mGroupMemberSelectionView->openGroupNameEditor();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::groupEditorView);
}

void TestCntGroupMemberSelectionView::activateView()
{
    CntViewParameters params(CntViewParameters::groupMemberSelectionView);
    
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mGroupMemberSelectionView->contactManager()->saveContact(&firstGroup);
    
    // create and save contact to the group
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    mGroupMemberSelectionView->contactManager()->saveContact(&firstContact);
    
    // save the group
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(firstGroup.id());
    relationship.setSecond(firstContact.id());
    
    mGroupMemberSelectionView->contactManager()->saveRelationship(&relationship);
    
    // set contact as the group id
    params.setSelectedContact(firstGroup);
    
    // call activate view 
    mGroupMemberSelectionView->activateView(params);
    
    QVERIFY(mGroupMemberSelectionView->selectionModel()->hasSelection() == true);
    
}
void TestCntGroupMemberSelectionView::cleanupTestCase()
{
    delete mViewManager;
    mViewManager = 0;
    mWindow->deleteLater();
}
