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

void TestCntCollectionListModel::initTestCase()
{
    // clear all contacts/groups from database
    mManager = new QContactManager("symbian");
    QList<QContactLocalId> ids = mManager->contacts();
    mManager->removeContacts(&ids);
}

void TestCntCollectionListModel::createNoUserGroups()
{
    mModel = new CntCollectionListModel(mManager);
    QVERIFY(mModel->rowCount() == 1);
    delete mModel;
    mModel = 0;
}

void TestCntCollectionListModel::createWithUserGroups()
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
    
    QContact secondGroupContact;
    QContactName secondGroupContactName;
    secondGroupContactName.setFirst("very long name to make the for-loop break out");
    secondGroupContact.saveDetail(&secondGroupContactName);
    mManager->saveContact(&secondGroupContact);
    
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(firstGroup.id());
    relationship.setSecond(firstGroupContact.id());
    QContactRelationship relationship2;
    relationship2.setRelationshipType(QContactRelationship::HasMember);
    relationship2.setFirst(firstGroup.id());
    relationship2.setSecond(secondGroupContact.id());

    // save relationship
    mManager->saveRelationship(&relationship);
    mManager->saveRelationship(&relationship2);
    
    // also create an empty unnamed group
    QContact secondGroup;
    secondGroup.setType(QContactType::TypeGroup);
    mManager->saveContact(&secondGroup);
    
    mModel = new CntCollectionListModel(mManager);
    QVERIFY(mModel->rowCount() == 3);
}

void TestCntCollectionListModel::data()
{
    QVariant var;
    
    QModelIndex index = mModel->index(-1); // invalid index
    var = mModel->data(index, Qt::DisplayRole);
    QVERIFY(var.isNull());
    
    index = mModel->index(100); // invalid index
    var = mModel->data(index, Qt::DisplayRole);
    QVERIFY(var.isNull());

    index = mModel->index(1); // first user group
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
    QVERIFY(!mModel->removeRow(-1));
    QVERIFY(!mModel->removeRow(100));
    
    QVERIFY(mModel->removeRow(1));
    QVERIFY(mModel->rowCount() == 2);
}

void TestCntCollectionListModel::removeGroup()
{
    mModel->removeGroup(100);
    QVERIFY(mModel->rowCount() == 2);
    
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QList<QContactLocalId> groupContactIds = mManager->contacts(groupFilter);
    mModel->removeGroup(groupContactIds.at(1)); // first one has already been removed, take the second one
    QVERIFY(mModel->rowCount() == 1);
}

void TestCntCollectionListModel::cleanupTestCase()
{
    // clear all contacts/groups from database
    QList<QContactLocalId> ids = mManager->contacts();
    mManager->removeContacts(&ids);
    delete mManager;
}
