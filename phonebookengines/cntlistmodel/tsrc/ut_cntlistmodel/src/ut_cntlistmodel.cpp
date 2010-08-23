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

#include "ut_cntlistmodel.h"
#include "cntlistmodel.h"
#include "cntlistmodel_p.h"

#include <hbnamespace.h>
#include <qtcontacts.h>
#include <QUrl>

void TestCntListModel::initTestCase()
{
    //let's have clean database before running tests 
    mManager = new QContactManager("symbian");
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMapInit;
    mManager->removeContacts(ids, &errorMapInit);
    mManager->removeContact(mManager->selfContactId());
}

void TestCntListModel::contactReady(int startRow, int endRow)
{
    QVERIFY(startRow == endRow);
    mDataReady = true;
}

void TestCntListModel::create()
{
    mCntModel = new CntListModel();
    QVERIFY(mCntModel != NULL);
    QVERIFY(mCntModel->rowCount() == 1);
    QVERIFY(mCntModel->d->m_cache);
    QVERIFY(mCntModel->d->m_ownedContactManager);
    QVERIFY(mCntModel->d->m_contactManager != NULL);
    
    delete mCntModel;
    mCntModel = NULL;
    
    mCntModel = new CntListModel(mManager);
    QVERIFY(mCntModel != NULL);
    QCOMPARE(mCntModel->rowCount(), 1);
    QVERIFY(mCntModel->rowCount() == 1);
    QVERIFY(mCntModel->d->m_cache);
    QVERIFY(!mCntModel->d->m_ownedContactManager);
    QVERIFY(mCntModel->d->m_contactManager != NULL);
}

void TestCntListModel::data()
{
    mModelListener = new ModelListener(this);
    mDataReady = false;

    //create and save contact
    QContact c;
    QContactName name;
    name.setFirstName("firstname");
    name.setLastName("lastname");
    c.saveDetail(&name);
    QContactPhoneNumber number;
    number.setNumber("1234567");
    number.setContexts(QContactDetail::ContextHome);
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    c.saveDetail(&number);
    QContactEmailAddress email;
    email.setEmailAddress("dummyemail");
    c.saveDetail(&email);
    QVERIFY(mManager->saveContact(&c));
    QTest::qWait(1000);
    
    //check invalid row and column
    QVariant ret;
    ret = mCntModel->data(QModelIndex(), Qt::UserRole);
    QVERIFY(ret.isNull());
    
    //check the saved contact's info
    QModelIndex modelIndex = mCntModel->indexOfContact(c);
    int row = modelIndex.row();
    QContact contactFromModel = mCntModel->contact(modelIndex);
    QVERIFY(c == contactFromModel);
    
    ret = mCntModel->data(modelIndex, Qt::UserRole); 
    QVERIFY(ret.isNull());
    
    ret = mCntModel->data(modelIndex, Qt::DisplayRole); 
    QVERIFY(ret.type() == QVariant::StringList);
    QStringList displayContent;
    displayContent = ret.toStringList();
    QVERIFY(displayContent.count() == 2);
    QVERIFY(displayContent.at(0) == "firstname lastname");
    // second string is only an empty placeholder, e.g. " ", until cache has fetched the value

    // wait for cache to signal that all contact info is ready
    while (!mDataReady) { QTest::qWait(200); QApplication::processEvents(); }
    mDataReady = false;
    ret = mCntModel->data(modelIndex, Qt::DisplayRole); 

    QVERIFY(ret.type() == QVariant::StringList);
    displayContent = ret.toStringList();
    QVERIFY(displayContent.count() == 2);
    QVERIFY(displayContent.at(0) == "firstname lastname");
    QVERIFY(displayContent.at(1) == "1234567");
   
    // check backgroundrole
    ret = mCntModel->data(modelIndex, Qt::BackgroundRole); 
    QVERIFY(ret.isNull());
    
    //check decoration role
    ret = mCntModel->data(modelIndex, Qt::DecorationRole); 
    QVERIFY(ret.type() == QVariant::List);
    
    ret = mCntModel->data(modelIndex, Hb::IndexFeedbackRole); 
    QVERIFY(ret.type() == QVariant::String);

    // add empty avatar and check decoration
    QContactAvatar avatar;
    c.saveDetail(&avatar);
    QVERIFY(mManager->saveContact(&c));
    QTest::qWait(1000);
    ret = mCntModel->data(modelIndex, Qt::DecorationRole); 
    QVERIFY(ret.type() == QVariant::List);
    
    // add data to the avatar and check decoration
    avatar.setImageUrl(QUrl("dummyimagepath"));
    c.saveDetail(&avatar);
    QVERIFY(mManager->saveContact(&c));
    QTest::qWait(1000);
    modelIndex = mCntModel->indexOfContact(c);
    ret = mCntModel->data(modelIndex, Qt::DecorationRole); 

    // wait for cache to signal that all contact info is ready
    while (!mDataReady) { QTest::qWait(200); QApplication::processEvents(); }
    mDataReady = false;
    ret = mCntModel->data(modelIndex, Qt::DecorationRole); 
    QVERIFY(ret.type() == QVariant::List);
    
    // check MyCard info from the model
    modelIndex = mCntModel->index(0, 0);
    ret = mCntModel->data(modelIndex, Qt::BackgroundRole);
    QVERIFY(!ret.isNull());
    
    // create and assign empty MyCard
    QContact myCard;
    QVERIFY(mManager->saveContact(&myCard));
    QTest::qWait(1000);
    mManager->setSelfContactId(myCard.localId());
    QTest::qWait(1000);
    
    // check that MyCard was really saved
    QCOMPARE(mCntModel->myCardId(), myCard.localId());
    
    // check MyCard info from the model
    myCard = mManager->contact(mManager->selfContactId());
    modelIndex = mCntModel->indexOfContact(myCard);
    ret = mCntModel->data(modelIndex, Qt::BackgroundRole);
    QVERIFY(!ret.isNull());
    
    ret = mCntModel->data(modelIndex, Qt::DisplayRole);
    QVERIFY(ret.type() == QVariant::StringList);
    displayContent = ret.toStringList();
    QVERIFY(displayContent.count() == 1); // "Unnamed"
    
    // add some content to MyCard
    myCard.saveDetail(&number);
    QVERIFY(mManager->saveContact(&myCard));
    QTest::qWait(1000);
    ret = mCntModel->data(modelIndex, Qt::DisplayRole);
    // wait for cache
    QTest::qWait(1000);
    ret = mCntModel->data(modelIndex, Qt::DisplayRole);
    QVERIFY(ret.type() == QVariant::StringList);
    displayContent = ret.toStringList();
    QVERIFY(displayContent.contains(hbTrId("txt_phob_list_unnamed")));
    
    ret = mCntModel->data(modelIndex, Hb::IndexFeedbackRole); 
    QVERIFY(ret.isNull());
}

void TestCntListModel::rowCount()
{
    // we should have 2 contacts in the model saved from the last test case
    QTest::qWait(5000);
    QCOMPARE(mCntModel->rowCount(), 2);
}

void TestCntListModel::contact()
{
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMapContact;
    mManager->removeContacts(ids,&errorMapContact);
    QTest::qWait(1000);
    
    QModelIndex modelIndex = mCntModel->index(0, 0);
    QContact empty = mCntModel->contact(modelIndex);
    //QVERIFY(empty.isEmpty());
    
    //create and save contact
    QContact c;
    QContactName name;
    name.setFirstName("firstname");
    name.setLastName("lastname");
    c.saveDetail(&name);
    QContactPhoneNumber number;
    number.setNumber("1234567");
    number.setContexts(QContactDetail::ContextHome);
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    c.saveDetail(&number);
    QContactEmailAddress email;
    email.setEmailAddress("dummyemail");
    c.saveDetail(&email);
    QVERIFY(mManager->saveContact(&c));
    QTest::qWait(1000);
    
    modelIndex = mCntModel->index(10, 0);
    c = mCntModel->contact(modelIndex);
    QVERIFY(c.isEmpty());
    
    modelIndex = mCntModel->index(1, 0);
    c = mCntModel->contact(modelIndex);
    QVERIFY(!c.isEmpty());
}

void TestCntListModel::indexOfContact()
{
    QModelIndex modelIndex = mCntModel->index(1, 0);
    QContact c = mCntModel->contact(modelIndex);
    
    QVERIFY(mCntModel->indexOfContact(c) == modelIndex);
}

void TestCntListModel::contactManager()
{
    QVERIFY(mManager == &(mCntModel->contactManager()));
}

void TestCntListModel::setFilter()
{
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMap;
    mManager->removeContacts(ids,&errorMap);
    QTest::qWait(1000);
    
    QContact c;
    QContactName name;
    name.setFirstName("firstname");
    name.setLastName("lastname");
    c.saveDetail(&name);
    QVERIFY(mManager->saveContact(&c));
    
    QContactUnionFilter unionFilter;

    QContactDetailFilter landlineFilter;
    landlineFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    landlineFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeLandline));
    unionFilter << landlineFilter;

    QContactDetailFilter mobileFilter;
    mobileFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    mobileFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeMobile));
    unionFilter << mobileFilter;
    
    mCntModel->setFilter(unionFilter);
    
    QModelIndex modelIndex = mCntModel->indexOfContact(c);
    QVERIFY(modelIndex.row() < 0);
    QVERIFY(mCntModel->d->m_filter == unionFilter);
    QVERIFY(mCntModel->d->m_sortOrders.count() == 3);
}

void TestCntListModel::myCard()
{
    delete mCntModel;
    mCntModel = 0;
    
    mCntModel = new CntListModel(mManager);
    
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMap;
    mManager->removeContacts(ids,&errorMap);
    QTest::qWait(1000);
    
    QContact c;
    QContactName name;
    name.setFirstName("firstname");
    name.setLastName("lastname");
    c.saveDetail(&name);
    QVERIFY(mManager->saveContact(&c));
    
    QSignalSpy spy(mCntModel, SIGNAL(modelReset()));
    
    QVERIFY(mCntModel->myCardStatus());
    
    mCntModel->showMyCard(false);
    QVERIFY(!mCntModel->myCardStatus());
    QCOMPARE(spy.count(), 2);
    
    mCntModel->showMyCard(true);
    QVERIFY(mCntModel->myCardStatus());
    QCOMPARE(spy.count(), 4);
    
    mManager->setSelfContactId(c.localId());
    QTest::qWait(1000);
    spy.clear();
    
    mCntModel->showMyCard(false);
    QVERIFY(!mCntModel->myCardStatus());
    QCOMPARE(spy.count(), 2);
    
    mCntModel->showMyCard(true);
    QVERIFY(mCntModel->myCardStatus());
    QCOMPARE(spy.count(), 4);
    mCntModel->showMyCard(true);
    QVERIFY(mCntModel->myCardStatus());
    QCOMPARE(spy.count(), 4);
}

void TestCntListModel::rowId()
{
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMap;
    mManager->removeContacts(ids,&errorMap);
    QTest::qWait(1000);
    
    QContact c;
    QContactName name;
    name.setFirstName("firstname");
    name.setLastName("lastname");
    c.saveDetail(&name);
    QVERIFY(mManager->saveContact(&c));
    
    int row = mCntModel->rowId(c.localId());
    QVERIFY(row > 0);
    QVERIFY(mCntModel->validRowId(row));
    QVERIFY(!mCntModel->validRowId(-100));
    QVERIFY(!mCntModel->validRowId(100));
}

void TestCntListModel::dataForDisplayRole()
{
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMap;
    mManager->removeContacts(ids,&errorMap);
    QTest::qWait(1000);
    
    QContact c;
    QContactName name;
    name.setFirstName("firstname");
    name.setLastName("lastname");
    c.saveDetail(&name);
    QVERIFY(mManager->saveContact(&c));
    
    int row = mCntModel->rowId(c.localId());
    QVariant var = mCntModel->dataForRole(row, Qt::DisplayRole);
    QVERIFY(var.type() == QVariant::StringList);
    
    var = mCntModel->dataForRole(0, Qt::DisplayRole);
    QVERIFY(var.type() == QVariant::StringList);
}

void TestCntListModel::handleAdded()
{
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMap;
    mManager->removeContacts(ids,&errorMap);
    QTest::qWait(1000);

    QSignalSpy spy(mCntModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)));
    
    QContact c;
    QContactName name;
    name.setFirstName("firstname");
    name.setLastName("lastname");
    c.saveDetail(&name);
    QVERIFY(mManager->saveContact(&c));

    QCOMPARE(spy.count(), 1);
    
    QList<QContactLocalId> emptyList;
    mCntModel->handleAdded(emptyList);
    QCOMPARE(spy.count(), 1);
}

void TestCntListModel::handleChanged()
{
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMap;
    mManager->removeContacts(ids,&errorMap);
    QTest::qWait(1000);
    
    QContact c;
    QContactName name;
    name.setFirstName("firstname");
    name.setLastName("lastname");
    c.saveDetail(&name);
    QVERIFY(mManager->saveContact(&c));
    
    QSignalSpy spy(mCntModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex&)));

    name.setMiddleName("mid");
    c.saveDetail(&name);
    QVERIFY(mManager->saveContact(&c));

    QCOMPARE(spy.count(), 1);
    
    QList<QContactLocalId> emptyList;
    mCntModel->handleChanged(emptyList);
    QCOMPARE(spy.count(), 1);
}

void TestCntListModel::handleRemoved()
{
    QSignalSpy spy(mCntModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)));
    
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMap;
    mManager->removeContacts(ids,&errorMap);
    QTest::qWait(1000);
    
    QCOMPARE(spy.count(), 1);
    
    QList<QContactLocalId> emptyList;
    mCntModel->handleRemoved(emptyList);
    QCOMPARE(spy.count(), 1);
}

void TestCntListModel::handleMyCardChanged()
{
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMap;
    mManager->removeContacts(ids,&errorMap);
    QTest::qWait(1000);
    
    QContact c;
    QContactName name;
    name.setFirstName("firstname");
    name.setLastName("lastname");
    c.saveDetail(&name);
    QVERIFY(mManager->saveContact(&c));
    
    mCntModel->handleMyCardChanged(0, c.localId());
    QVERIFY(mCntModel->d->m_myCardId == c.localId());
}

void TestCntListModel::cleanupTestCase()
{
    mCntModel->d->m_cache->onShutdown();
    delete mCntModel;
    mCntModel = 0;
    
    //let's have clean database after running tests
    QList<QContactLocalId> ids = mManager->contactIds();
    QMap<int, QContactManager::Error> errorMap;
    mManager->removeContacts(ids, &errorMap);
    delete mManager;
    mManager = 0;
    delete mModelListener;
    mModelListener = 0;
}


ModelListener::ModelListener(TestCntListModel* parent)
    : mParent(parent)
{
    connect(mParent->mCntModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
}

void ModelListener::onDataChanged(QModelIndex start, QModelIndex end)
{
    mParent->contactReady(start.row(), end.row());
}

