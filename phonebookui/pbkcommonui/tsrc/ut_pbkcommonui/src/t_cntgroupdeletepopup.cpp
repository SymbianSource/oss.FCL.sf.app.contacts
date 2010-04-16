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
#include "cntgroupdeletepopupmodel.h"

#include "t_cntgroupdeletepopup.h"

void TestCntGroupDeletePopup::init()
{
    // clear all contacts/groups from database
    mManager = new QContactManager("symbian");
    
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMapHandle;
    mManager->removeContacts(&ids,&errorMapHandle);
    }



void TestCntGroupDeletePopup::populateListOfGroup()
{
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("groupname");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);
    
    mPopup = new CntGroupDeletePopup(mManager);
    mPopup->populateListOfGroup();
    QVERIFY(mPopup->mListView != 0);
    QVERIFY(mPopup->mListView->selectionMode() == HbAbstractItemView::MultiSelection);

   }


void TestCntGroupDeletePopup::deleteGroup()
{
   // create a group
    QContact firstGroup;
    firstGroup.setType(QContactType::TypeGroup);
    QContactName firstGroupName;
    firstGroupName.setCustomLabel("first gp");
    firstGroup.saveDetail(&firstGroupName);
    mManager->saveContact(&firstGroup);

    mPopup = new CntGroupDeletePopup(mManager);
    mPopup->populateListOfGroup();
    CntGroupDeletePopupModel *model = new CntGroupDeletePopupModel(mManager);
    model->initializeGroupsList();
    QModelIndex index = model->index(0);
    mPopup->mListView->selectionModel()->select(index, QItemSelectionModel::Select);

    mPopup->deleteGroup();

    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));

    QList<QContactLocalId> groupContactIds = mManager->contactIds(groupFilter);
    int count = groupContactIds.count();
    QVERIFY(count == 0);
}



void TestCntGroupDeletePopup::cleanup()
{
    delete mPopup;
    mPopup = 0;
    QList<QContactLocalId> ids = mManager->contactIds();
    mManager->removeContacts(&ids);
    delete mManager;
    mManager = 0;
    }


// EOF
