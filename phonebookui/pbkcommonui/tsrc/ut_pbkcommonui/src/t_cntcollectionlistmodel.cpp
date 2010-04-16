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

#include "cntcollectionlistmodel.h"
#include "t_cntcollectionlistmodel.h"

void TestCntCollectionListModel::init()
{    
    // clear all contacts/groups from database
    mManager = new QContactManager("symbian");
    QList<QContactLocalId> ids = mManager->contactIds();
    mManager->removeContacts(&ids);
  
//    mModel = new CntCollectionListModel(mManager);
//    QVERIFY(mModel->rowCount() == 1);
    
}
//
//void TestCntCollectionListModel::createNoUserGroups()
//{
//    mModel = new CntCollectionListModel(mManager);
//    QVERIFY(mModel->rowCount() == 1);
//    delete mModel;
//    mModel = 0;
//}

//void TestCntCollectionListModel::createWithUserGroups()
//{
//    // create a group and add two members in it
//    QContact firstGroup;
//    firstGroup.setType(QContactType::TypeGroup);
//    QContactName firstGroupName;
//    firstGroupName.setCustomLabel("groupname");
//    firstGroup.saveDetail(&firstGroupName);
//    mManager->saveContact(&firstGroup);
//    
//    QContact firstGroupContact;
//    QContactName firstGroupContactName;
//    firstGroupContactName.setFirst("firstname");
//    firstGroupContact.saveDetail(&firstGroupContactName);
//    mManager->saveContact(&firstGroupContact);
//    
//    QContact secondGroupContact;
//    QContactName secondGroupContactName;
//    secondGroupContactName.setFirst("very long name to make the for-loop break out");
//    secondGroupContact.saveDetail(&secondGroupContactName);
//    mManager->saveContact(&secondGroupContact);
//    
//    QContactRelationship relationship;
//    relationship.setRelationshipType(QContactRelationship::HasMember);
//    relationship.setFirst(firstGroup.id());
//    relationship.setSecond(firstGroupContact.id());
//    QContactRelationship relationship2;
//    relationship2.setRelationshipType(QContactRelationship::HasMember);
//    relationship2.setFirst(firstGroup.id());
//    relationship2.setSecond(secondGroupContact.id());
//
//    // save relationship
//    mManager->saveRelationship(&relationship);
//    mManager->saveRelationship(&relationship2);
//    
//    // also create an empty unnamed group
//    QContact secondGroup;
//    secondGroup.setType(QContactType::TypeGroup);
//    mManager->saveContact(&secondGroup);
//    
//    mModel = new CntCollectionListModel(mManager);
//    QVERIFY(mModel->rowCount() == 3);
//}

void TestCntCollectionListModel::data()
{
    mModel = new CntCollectionListModel(mManager);

    QVariant var;
    
    QModelIndex index = mModel->index(-1); // invalid index
    var = mModel->data(index, Qt::DisplayRole);
    QVERIFY(var.isNull());
    
    index = mModel->index(100); // invalid index
    var = mModel->data(index, Qt::DisplayRole);
    QVERIFY(var.isNull());
    
     //create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);

    index = mModel->index(0); // first user group
    var = mModel->data(index, Qt::DisplayRole);
    QVERIFY(var.canConvert<QStringList>());
    
    var = mModel->data(index, Qt::DecorationRole);
    QVERIFY(var.canConvert<QVariantList>());
    
    var = mModel->data(index, Qt::UserRole);
    QVERIFY(var.canConvert<int>());
    
    var = mModel->data(index, Qt::UserRole+1);
    QVERIFY(var.isNull());
}

void TestCntCollectionListModel::removeRows()
{

    // create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);

    // create a group and add two members in it
    QContact secondGroup;
    secondGroup.setType(QContactType::TypeGroup);
    QContactName secondGroupName;
    secondGroupName.setCustomLabel("grouptwo");
    secondGroup.saveDetail(&secondGroupName);
    mManager->saveContact(&secondGroup);
    
    mModel = new CntCollectionListModel(mManager);
    
    QVERIFY(!mModel->removeRow(-1));
    QVERIFY(!mModel->removeRow(100));
    
    QVERIFY(mModel->removeRow(1));
    QVERIFY(mModel->rowCount() == 2); // fav + one grp
}

void TestCntCollectionListModel::removeGroup()
{

    // create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);
    
    // create a group and add two members in it
    QContact secondGroup;
    secondGroup.setType(QContactType::TypeGroup);
    QContactName secondGroupName;
    secondGroupName.setCustomLabel("groupname");
    secondGroup.saveDetail(&secondGroupName);
    mManager->saveContact(&secondGroup);
    
    mModel = new CntCollectionListModel(mManager);
    
    mModel->removeGroup(100);
    QVERIFY(mModel->rowCount() == 3);
    
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QList<QContactLocalId> groupContactIds = mManager->contacts(groupFilter);
    mModel->removeGroup(groupContactIds.at(1)); // first one has already been removed, take the second one
    QVERIFY(mModel->rowCount() == 2);
}

void TestCntCollectionListModel::initializeStaticGroups()
    {
    mModel = new CntCollectionListModel(mManager);
    QVERIFY(mModel->rowCount() == 1);
    }

void TestCntCollectionListModel::isFavoriteGroupCreated()
    {
    mModel = new CntCollectionListModel(mManager);
    QVERIFY(mModel->isFavoriteGroupCreated() == true);
    
    // clear all contacts/groups from database. This will remove favorite grp also
    QList<QContactLocalId> ids = mManager->contactIds();
    mManager->removeContacts(&ids);
    QVERIFY(mModel->isFavoriteGroupCreated() == false);
    
    }
void TestCntCollectionListModel::favoriteGroupId()
    {
    mModel = new CntCollectionListModel(mManager);
    QContact contact;
    QList<QContactLocalId> ids = mManager->contactIds();
    QVERIFY(ids.count() == 1);
    for(int i = 0 ; i< ids.count();i++)
    {
    contact = mManager->contact( ids.at(i) );
    }
    int contactId = contact.localId();
    
    QVERIFY(mModel->favoriteGroupId() == contactId);
    }

void  TestCntCollectionListModel::rowCount()
    {
    mModel = new CntCollectionListModel(mManager);
    
    QVERIFY(mModel->rowCount() == 1);// fav static group
    
     
    // create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("Group1");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);
    int favGrpId = firstGroup.localId();
    
    mModel->initializeUserGroups();

    QVERIFY(mModel->rowCount() == 2);
    
    }
void  TestCntCollectionListModel::initializeUserGroups()
    {
    mModel = new CntCollectionListModel(mManager);
    
    QVERIFY(mModel->rowCount() == 1); // fav group is created
    
    // create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);
    mModel->initializeUserGroups();
    
    QVERIFY(mModel->rowCount() == 2); // fav + 'groupname' group is created

        
    }

void TestCntCollectionListModel::cleanup()
{
    delete mModel;
    mModel = 0;
    // clear all contacts/groups from database
    QList<QContactLocalId> ids = mManager->contactIds();
    mManager->removeContacts(&ids);
    delete mManager;
}
