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

#include <mobcntmodel.h>


#include "t_cntgroupselectionpopup.h"

void TestCntGroupSelectionPopup::init()
{
    // clear all contacts/groups from database
    mManager = new QContactManager("symbian");
    
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMapHandle;
    mManager->removeContacts(&ids,&errorMapHandle);
    }

void TestCntGroupSelectionPopup::populateListOfContact()
{
    // create and save contact to the group
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    mManager->saveContact(&firstContact);
    
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);
    
    mPopup = new CntGroupSelectionPopup(mManager, &firstGroup);
    mPopup->populateListOfContact();
    
    QVERIFY(mPopup->mListView != 0);
    QVERIFY(mPopup->mListView->selectionMode() == HbAbstractItemView::MultiSelection);
    deletePopup();
       
}

void TestCntGroupSelectionPopup::saveNewGroup()
{   
    // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    bool ret= mManager->saveContact(&firstGroup);
    
    // create and save contact to the group
    QContact firstContact;
    firstContact.setType(QContactType::TypeContact);
    ret= mManager->saveContact(&firstContact);
    
    // set selection model, and saveGroup 
    
    mPopup = new CntGroupSelectionPopup(mManager, &firstGroup);
    mPopup->populateListOfContact();
    
    QModelIndex index = mPopup->mCntModel->indexOfContact(firstContact);
    mPopup->mListView->selectionModel()->select(index, QItemSelectionModel::Select);

    mPopup->saveNewGroup();
    
    // Use relationship filter to get list of contacts in the relationship (if any)
    QContactRelationshipFilter filter;
    filter.setRelationshipType(QContactRelationship::HasMember);
    filter.setRelatedContactRole(QContactRelationshipFilter::First); 
    filter.setRelatedContactId(firstGroup.id());

    QList<QContactLocalId> mContactsList = mManager->contactIds(filter);
    int count = mContactsList.count();
    QVERIFY(count == 1);
    deletePopup();
}



void TestCntGroupSelectionPopup::saveOldGroup()
{   
    // create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);
    
    QContact firstGroupContact;
    QContactName firstGroupContactName;
    firstGroupContactName.setFirst("firstname");
    firstGroupContact.saveDetail(&firstGroupContactName);
    mManager->saveContact(&firstGroupContact);
    
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(firstGroup.id());
    relationship.setSecond(firstGroupContact.id());
    
    // save relationship
    mManager->saveRelationship(&relationship);
    
    QContact secondContact;
    secondContact.setType(QContactType::TypeContact);
    QContactName secondContactName;
    secondContactName.setFirst("Secondname");
    secondContact.saveDetail(&secondContactName);
    mManager->saveContact(&secondContact);
    
    // set selection model, and saveGroup 
    
    mPopup = new CntGroupSelectionPopup(mManager, &firstGroup);
    mPopup->populateListOfContact();
    
    QModelIndex index = mPopup->mCntModel->indexOfContact(secondContact);
    mPopup->mListView->selectionModel()->select(index, QItemSelectionModel::Select);

    mPopup->saveOldGroup();

    // Use relationship filter to get list of contacts in the relationship (if any)
    QContactRelationshipFilter filter;
    filter.setRelationshipType(QContactRelationship::HasMember);
    filter.setRelatedContactRole(QContactRelationshipFilter::First); 
    filter.setRelatedContactId(firstGroup.id());

    QList<QContactLocalId> mContactsList = mManager->contactIds(filter);
    int count = mContactsList.count();
    QVERIFY(count == 2);
    
    delete mPopup;
    mPopup = 0;
    mPopup = new CntGroupSelectionPopup(mManager, &firstGroup);
    mPopup->populateListOfContact();
    
    index = mPopup->mCntModel->indexOfContact(firstGroupContact);
    mPopup->mListView->selectionModel()->select(index, QItemSelectionModel::Deselect);
    
    mPopup->saveOldGroup();

    // Use relationship filter to get list of contacts in the relationship (if any)
    QContactRelationshipFilter filter2;
    filter2.setRelationshipType(QContactRelationship::HasMember);
    filter2.setRelatedContactRole(QContactRelationshipFilter::First); 
    filter2.setRelatedContactId(firstGroup.id());
    
    mContactsList.clear();
    mContactsList = mManager->contactIds(filter2);
    count = mContactsList.count();
    
    QVERIFY(count == 1);
    deletePopup();
}

void TestCntGroupSelectionPopup::closeFind()
    {
    // Create some group of contacts...
    QContact dummyGroup;
    dummyGroup.setType(QContactType::TypeGroup);
    QContactName dummyGroupName;
    dummyGroupName.setCustomLabel("groupname");
    dummyGroup.saveDetail(&dummyGroupName);
    mManager->saveContact(&dummyGroup);

    mPopup = new CntGroupSelectionPopup(mManager, &dummyGroup); 

    mPopup->closeFind();
    // Y need ? 
    //in src code its was mSearchPanel->deleteLater(); mSearchPanel = 0 ;
    // since its delete later one should not set it to 0
    //QVERIFY(!mPopup->mSearchPanel);
    QVERIFY(!mPopup->mEmptyListLabel);

    // Check that calling it for the second time 
    // doesn't break anything.
    mPopup->closeFind();
  //  QVERIFY(!mPopup->mSearchPanel);
    QVERIFY(!mPopup->mEmptyListLabel);
    deletePopup();
    }

void TestCntGroupSelectionPopup::setFilter()
    {
    // Create some group of contacts...
    QContact dummyGroup;
    dummyGroup.setType(QContactType::TypeGroup);
    QContactName dummyGroupName;
    dummyGroupName.setCustomLabel("groupname");
    dummyGroup.saveDetail(&dummyGroupName);
    mManager->saveContact(&dummyGroup);

    mPopup = new CntGroupSelectionPopup(mManager, &dummyGroup);
    QList<QContactLocalId> ids = mManager->contactIds();
    mManager->removeContacts(&ids);
 
    
    QContact contact1;
    QContactName name1;
    name1.setFirst("abc");
    contact1.saveDetail(&name1);
    
    QContact contact2;
    QContactName name2;
    name2.setFirst("acb");
    contact2.saveDetail(&name2);
    
    mManager->saveContact(&contact1);
    mManager->saveContact(&contact2);
    
    mPopup->populateListOfContact();
    
    mPopup->setFilter(name2.first());
    
    int count1 = mPopup->mCntModel->rowCount();
    
    QVERIFY(!mPopup->mEmptyListLabel);
    QVERIFY(mPopup->mCntModel->rowCount() == 1);
    
    mPopup->setFilter("abc");
    count1 = mPopup->mCntModel->rowCount();
    QVERIFY(!mPopup->mEmptyListLabel);
    QVERIFY(mPopup->mCntModel->rowCount() == 1);
    
    mPopup->setFilter("abcd");
    count1 = mPopup->mCntModel->rowCount();
    QVERIFY(mPopup->mEmptyListLabel);
    QVERIFY(mPopup->mCntModel->rowCount() == 0);
    
    mPopup->setFilter("abcde");
    count1 = mPopup->mCntModel->rowCount();
    QVERIFY(mPopup->mEmptyListLabel);
    QVERIFY(mPopup->mCntModel->rowCount() == 0);
    }

void TestCntGroupSelectionPopup::cleanup()
{
    QList<QContactLocalId> ids = mManager->contactIds();
    mManager->removeContacts(&ids);
    delete mManager;
    mManager = 0;
    }

void TestCntGroupSelectionPopup::deletePopup()
    {
    delete mPopup;
    mPopup = 0;
    }

// EOF
