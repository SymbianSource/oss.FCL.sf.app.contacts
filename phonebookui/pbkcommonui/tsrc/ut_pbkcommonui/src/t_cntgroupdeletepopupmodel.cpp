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

#include "cntgroupdeletepopupmodel.h"
#include "t_cntgroupdeletepopupmodel.h"

void TestCntGroupDeletePopupModel::init()
{
    // clear all contacts/groups from database
    mManager = new QContactManager("symbian");
    QList<QContactLocalId> ids = mManager->contactIds();
    mManager->removeContacts(&ids);
  
    mModel = new CntGroupDeletePopupModel(mManager);
    QVERIFY(mModel->rowCount() == 0);
   
    
}

void TestCntGroupDeletePopupModel::cleanup()
{
    delete mModel;
    mModel = 0;
    // clear all contacts/groups from database
    QList<QContactLocalId> ids = mManager->contactIds();
    mManager->removeContacts(&ids);

    delete mManager;
}

void TestCntGroupDeletePopupModel::initializeGroupsList()
{
    // create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("Favorites");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);

    QContact secondGroup;
    secondGroup.setType(QContactType::TypeGroup);
    QContactName secondGroupName;
    secondGroupName.setCustomLabel("groupOne");
    secondGroup.saveDetail(&secondGroupName);
    mManager->saveContact(&secondGroup);
    
    QContact thirdGroup;
    thirdGroup.setType(QContactType::TypeGroup);
    QContactName thirdGroupName;
    thirdGroupName.setCustomLabel("groupTwo");
    thirdGroup.saveDetail(&thirdGroupName);
    mManager->saveContact(&thirdGroup);
    
    mModel->isFavoriteGroupCreated();
    mModel->initializeGroupsList();
   
    QVERIFY(mModel->rowCount() == 2); //fav should not be listed
}

void TestCntGroupDeletePopupModel::data()
{
    QVariant var;
    
    QModelIndex index = mModel->index(-1); // invalid index
    var = mModel->data(index, Qt::DisplayRole);
    QVERIFY(var.isNull());
    
    index = mModel->index(100); // invalid index
    var = mModel->data(index, Qt::DisplayRole);
    QVERIFY(var.isNull());

    QContact secondGroup;
    secondGroup.setType(QContactType::TypeGroup);
    QContactName secondGroupName;
    secondGroupName.setCustomLabel("groupOne");
    secondGroup.saveDetail(&secondGroupName);
    mManager->saveContact(&secondGroup);
    
    mModel->isFavoriteGroupCreated();
    mModel->initializeGroupsList();
    
    index = mModel->index(0); // first user group
    var = mModel->data(index, Qt::DisplayRole);
    QVERIFY(var.canConvert<QStringList>());
     
    var = mModel->data(index, Qt::UserRole);
    QVERIFY(var.canConvert<int>());
    
    var = mModel->data(index, Qt::UserRole+1);
    QVERIFY(var.isNull());
}

void TestCntGroupDeletePopupModel::isFavoriteGroupCreated()
{
    QVERIFY(mModel->isFavoriteGroupCreated() == false);

    // create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("Favorites");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);
    
    mModel->isFavoriteGroupCreated();

    QVERIFY(mModel->isFavoriteGroupCreated() == true);
}

void TestCntGroupDeletePopupModel::favoriteGroupId()
{
    QVERIFY(mModel->favoriteGroupId() == -1);
    
    // create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("Favorites");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);
            
    mModel->isFavoriteGroupCreated();
    mModel->initializeGroupsList();
    
    int favGrpId = firstGroup.localId();

    QVERIFY(mModel->favoriteGroupId() == favGrpId);
}

void TestCntGroupDeletePopupModel::rowCount()
{
    QVERIFY(mModel->rowCount() == 0);
    
    // create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("Group1");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);
    int favGrpId = firstGroup.localId();
    
    mModel->isFavoriteGroupCreated();
    mModel->initializeGroupsList();

    QVERIFY(mModel->rowCount() == 1);
}
void TestCntGroupDeletePopupModel::contact()
{
    
    // create a group and add two members in it
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("GroupOne");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);
    
    int groupId = firstGroup.localId();
    
    mModel->isFavoriteGroupCreated();
    mModel->initializeGroupsList();
    QModelIndex index = mModel->index(0);
    QContact group = mModel->contact(index);
    
    QVERIFY(group.localId() == groupId);
}

