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

#include "cntgroupmemberview.h"
#include "cntviewmanager.h"
#include "cntmainwindow.h"

#include "hbstubs_helper.h"

#include "t_cntgroupmemberview.h"

void TestCntGroupMemberView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mGroupMemberView = 0;
    mGroupContact = 0;
    mContact = 0;
}

void TestCntGroupMemberView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntViewManager(mWindow, CntViewParameters::noView);
    mGroupMemberView = new CntGroupMemberView(mViewManager);
    mWindow->addView(mGroupMemberView);
    mWindow->setCurrentView(mGroupMemberView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mGroupMemberView != 0);
}

void TestCntGroupMemberView::aboutToCloseView()
{
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mGroupMemberView->contactManager()->saveContact(&firstGroup);
    
    mGroupMemberView->mGroupContact = &firstGroup;
    
    mGroupMemberView->aboutToCloseView();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::groupActionsView);
    
}

void TestCntGroupMemberView::deleteGroup()
{
    delete mContact;
    delete mGroupContact;
    
    mContact = 0;
    mGroupContact = 0;
    
    // create a group
    mGroupContact = new QContact();
    mGroupContact->setType(QContactType::TypeGroup);    
    mGroupMemberView->contactManager()->saveContact(mGroupContact);
    mGroupMemberView->mGroupContact = mGroupContact;
    
    mContact = new QContact();
    mContact->setType(QContactType::TypeGroup);    
    mGroupMemberView->contactManager()->saveContact(mContact);
    
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(mGroupContact->id());
    relationship.setSecond(mContact->id());
    mGroupMemberView->contactManager()->saveRelationship(&relationship);
    
    // delete the group
    mGroupMemberView->deleteGroup();
    
    // do a group filter and check if group is there
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QList<QContactLocalId> groupContactIds = mGroupMemberView->contactManager()->contacts(groupFilter);
    int count = groupContactIds.count();
      
    // check presence 
    QVERIFY(count >= 1);
    
    
}
void TestCntGroupMemberView::removeFromGroup()
{
       
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mGroupMemberView->contactManager()->saveContact(&firstGroup);
    
    // create and save contact to the group
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    mGroupMemberView->contactManager()->saveContact(&firstContact);
    
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(firstGroup.id());
    relationship.setSecond(firstContact.id());
    
    mGroupMemberView->contactManager()->saveRelationship(&relationship);
    
    // get index of the contcat to be removed
    //QModelIndex contactIndex = mGroupMemberView->listView()->model()->index(0, 0);
    QModelIndex index = mGroupMemberView->contactModel()->indexOfContact(firstContact);
    //remove from group
    mGroupMemberView->removeFromGroup(index);
    
    // retrieve contcats in the relationship and check if it is empty
    QList<QContactRelationship> retrieveList;
    retrieveList = mGroupMemberView->contactManager()->relationships(firstGroup.id(), QContactRelationshipFilter::First);
    QVERIFY(retrieveList.count() != 0);
    
}

void TestCntGroupMemberView::handleExecutedCommand()
{
    mGroupMemberView->handleExecutedCommand("delete", QContact());
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::collectionView);
}

void TestCntGroupMemberView::addActionsToToolBar()
{   
    HbStubHelper::reset();
    mGroupMemberView->addActionsToToolBar();
    QVERIFY(HbStubHelper::widgetActionsCount() == 2);
}


void TestCntGroupMemberView::onLongPressed()
{
        
    // create a contact 
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    mGroupMemberView->contactManager()->saveContact(&firstContact);
        
        
    QModelIndex index = mGroupMemberView->contactModel()->indexOfContact(firstContact);
    
    HbStubHelper::reset();
    //mGroupMemberView->onLongPressed(mGroupMemberView->listView()->itemByIndex(index), QPointF());
    //QVERIFY(HbStubHelper::widgetActionsCount() == 2);
        
}

void TestCntGroupMemberView::onListViewActivated()
{
    
    // create a contact 
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    mGroupMemberView->contactManager()->saveContact(&firstContact);
    
    QModelIndex index = mGroupMemberView->contactModel()->indexOfContact(firstContact);
    
    mGroupMemberView->onListViewActivated(index);
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::commLauncherView);
    
    
}

void TestCntGroupMemberView::viewDetailsOfGroupContact()
{
    
    // create a contact 
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    mGroupMemberView->contactManager()->saveContact(&firstContact);
    
    QModelIndex index = mGroupMemberView->contactModel()->indexOfContact(firstContact);
    
    mGroupMemberView->viewDetailsOfGroupContact(index);
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::commLauncherView);
       
}

void TestCntGroupMemberView::callNamesList()
{
    mGroupMemberView->callNamesList();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::namesView);
}


void TestCntGroupMemberView::editGroup()
{
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mGroupMemberView->contactManager()->saveContact(&firstGroup);
    
    mGroupMemberView->mGroupContact = &firstGroup;

    mGroupMemberView->editGroup();
    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::groupEditorView);
}

void TestCntGroupMemberView::activateView()
{
    CntViewParameters params(CntViewParameters::groupMemberView);
    
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mGroupMemberView->contactManager()->saveContact(&firstGroup);
    mGroupMemberView->mGroupContact = &firstGroup;
    
    
    // create and save contact to the group
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    mGroupMemberView->contactManager()->saveContact(&firstContact);
    
    // create another contact but dont save it to group
    QContact secondContact;
    secondContact.setType(QContactType::TypeContact);
    mGroupMemberView->contactManager()->saveContact(&secondContact);
    
    // save the group
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(firstGroup.id());
    relationship.setSecond(firstContact.id());
    
    mGroupMemberView->contactManager()->saveRelationship(&relationship);
    
    // set contact as the group id
    params.setSelectedContact(firstGroup);
    
    // call activate view 
    mGroupMemberView->activateView(params);
    
    // retrieve list of contacts in the relationship is done in activateView function
    // check if list contains 1 contact
    
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeContact)));

    QList<QContactLocalId> groupContactIds = mGroupMemberView->contactManager()->contacts(groupFilter);
    
       
    QVERIFY(groupContactIds.count() != 0);
    
    
}
void TestCntGroupMemberView::cleanupTestCase()
{
    delete mViewManager;
    mViewManager = 0;
    delete mWindow;
    mWindow = 0;
}

// EOF
