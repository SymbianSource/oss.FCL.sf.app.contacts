/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mt_mobhistorymodel.h"
#include "mobhistorymodel.h"
#include "stub_classes.h"

void TestMobHistoryModel::initTestCase()
{
}

void TestMobHistoryModel::cleanupTestCase()
{
}

void TestMobHistoryModel::testRoles()
{
    // Contact centric history data
    QContactManager* cm = new QContactManager("symbian");
    QContact c;
    QVERIFY(cm->saveContact(&c));
    MobHistoryModel* model = new MobHistoryModel(c.localId(), cm);
    QVERIFY(model != 0);
    QVERIFY(model->rowCount() == FILTERED_EVENTS);
    
    // Branded icon path
    int index = 1;
    QModelIndex modelIndex = model->index(index, 0);
    QVERIFY(modelIndex.isValid());
    QVariant var = modelIndex.data(Qt::DecorationRole);
    QVERIFY(var.type() == QVariant::String);
    
    // Direction
    modelIndex = model->index(index, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(DirectionRole);
    QVERIFY(var.type() == QVariant::Int);
    QVERIFY(var.toInt() == Incoming ||
            var.toInt() == Outgoing ||
            var.toInt() == Missed);
    
    // Display role
    modelIndex = model->index(index, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(Qt::DisplayRole);
    QString str = QString(cc_msg).arg(index);
    QVERIFY(var.toStringList().at(0) == "");
    QDateTime t = constDateTime.addSecs(5*index);
    str = t.toString();
    QVERIFY(var.toStringList().at(2) == str);
    
    index = 3;
    modelIndex = model->index(index, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(Qt::DisplayRole);
    str = QString(cc_msg).arg(index);
    QVERIFY(var.type() == QVariant::StringList);
    QVERIFY(var.toStringList().at(0) == "");
    t = constDateTime.addSecs(5*index);
    str = t.toString();
    QVERIFY(var.toStringList().at(2) == str);
    
    // decoration role
    modelIndex = model->index(index, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(Qt::DecorationRole);
    QVERIFY(var.type() == QVariant::String);
    QVERIFY(var.toString() == QString("qtg_small_missed_call") || 
            var.toString() == QString("qtg_small_sent") ||
            var.toString() == QString("qtg_small_received"));
    
    // SeenStatusRole
    index = 1;
    modelIndex = model->index(index, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(SeenStatusRole);
    QVERIFY(var.type() == QVariant::Int);
    QVERIFY(var.toInt() == Unseen ||
            var.toInt() == Seen);
    
    // ItemTypeRole
    index = 1;
    modelIndex = model->index(index, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(ItemTypeRole);
    QVERIFY(var.type() == QVariant::Int);
    QVERIFY(var.toInt() == CallLog ||
            var.toInt() == Message);
    
    // PhoneNumberRole
    modelIndex = model->index(index, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(PhoneNumberRole);
    QVERIFY(var.type() == QVariant::String);
    QVERIFY(var.toString() == "1234567");
    
    // Invalid role
    index = 1;
    modelIndex = model->index(index, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(1000);
    QVERIFY(var.type() == QVariant::Invalid);
    
    // Invalid indices
    modelIndex = model->index(-1, 0);
    QVERIFY(!modelIndex.isValid());
    var = modelIndex.data(Qt::DisplayRole);
    QVERIFY(var.type() == QVariant::Invalid);
    
    var = model->data(modelIndex, Qt::DisplayRole);
    QVERIFY(var.type() == QVariant::Invalid);
    
    modelIndex = model->index(100, 0);
    QVERIFY(!modelIndex.isValid());
    var = modelIndex.data(Qt::DisplayRole);
    QVERIFY(var.type() == QVariant::Invalid);
    
    var = model->data(modelIndex, Qt::DisplayRole);
    QVERIFY(var.type() == QVariant::Invalid);
    
    // Test all conversations
    QVERIFY(cm->setSelfContactId(c.localId()));
    delete model;
    model = new MobHistoryModel(c.localId(), cm);
    QVERIFY(model != 0);
    QVERIFY(model->rowCount() == ALL_EVENTS);
    
    cm->removeContact(c.localId());
    
    delete cm;
    delete model;
}

void TestMobHistoryModel::testSorting()
{
    QContactManager* cm = new QContactManager("symbian");
    QContact c;
    QVERIFY(cm->saveContact(&c));
    MobHistoryModel* model = new MobHistoryModel(c.localId(), cm);
    QVERIFY(model != 0);
    QVERIFY(model->rowCount() == FILTERED_EVENTS);
    
    // Sort ascending
    QSignalSpy spy( model, SIGNAL( layoutChanged()));
    model->sortAndRefresh(Qt::AscendingOrder);
    
    QModelIndex modelIndex = model->index(1, 0);
    QVERIFY(modelIndex.isValid());
    QVariant var = modelIndex.data(Qt::DisplayRole);
    QDateTime dt1 = QDateTime::fromString(var.toStringList().at(2));
    
    modelIndex = model->index(2, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(Qt::DisplayRole);
    QDateTime dt2 = QDateTime::fromString(var.toStringList().at(2));
    QVERIFY(dt1 < dt2);    
    QVERIFY(spy.count() == 1);
    
    // Sort descending
    QSignalSpy spy1( model, SIGNAL( layoutChanged()));
    model->sortAndRefresh(Qt::DescendingOrder);
    
    modelIndex = model->index(1, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(Qt::DisplayRole);
    dt1 = QDateTime::fromString(var.toStringList().at(2));
    
    modelIndex = model->index(2, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(Qt::DisplayRole);
    dt2 = QDateTime::fromString(var.toStringList().at(2));
    QVERIFY(dt1 > dt2);
    QVERIFY(spy1.count() == 1);
    
    cm->removeContact(c.localId()); 
    delete model;
    delete cm;
}

void TestMobHistoryModel::testClear()
{
    QContactManager* cm = new QContactManager("symbian");
    QContact c;
    QVERIFY(cm->saveContact(&c));
    MobHistoryModel* model = new MobHistoryModel(c.localId(), cm);
    QVERIFY(model != 0);
    QVERIFY(model->rowCount() == FILTERED_EVENTS);
    
    // Clear history
    QSignalSpy spy( model, SIGNAL( rowsRemoved(const QModelIndex &, int, int)));
    int beforeClear = model->rowCount();
    model->clearHistory();
    QTest::qWait(1000); // wait clearing completion
    QVERIFY( model->rowCount() == 0 );
    QVERIFY(spy.count() == 1);
    
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(2).type() == QVariant::Int);
    QVERIFY(arguments.at(2).toInt() == beforeClear);
    
    // Clear already empty history view
    QSignalSpy spy1( model, SIGNAL( rowsRemoved(const QModelIndex &, int, int)));
    model->clearHistory();
    QTest::qWait(1000); // wait clearing completion
    QVERIFY(spy1.count() == 0);
    
    cm->removeContact(c.localId());
    delete model;
    delete cm;
}

void TestMobHistoryModel::testMarkSeen()
{
    QContactManager* cm = new QContactManager("symbian");
    QContact c;
    QVERIFY(cm->saveContact(&c));
    MobHistoryModel* model = new MobHistoryModel(c.localId(), cm);
    QVERIFY(model != 0);
    QVERIFY(model->rowCount() == FILTERED_EVENTS);
    
    // Mark history as seen
    QSignalSpy spy( model, SIGNAL( layoutChanged()));
    model->markAllAsSeen();
    QTest::qWait(1000); // wait marking completion
    QVERIFY(spy.count() == 1);
    
    // Mark already marked history as seen
    QSignalSpy spy1( model, SIGNAL( layoutChanged()));
    model->markAllAsSeen();
    QTest::qWait(1000); // wait marking completion
    QVERIFY(spy1.count() == 0);
    
    cm->removeContact(c.localId());
    delete model;
    delete cm;
}

QTEST_MAIN(TestMobHistoryModel);

