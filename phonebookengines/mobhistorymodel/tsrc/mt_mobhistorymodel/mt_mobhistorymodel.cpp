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
#include <qcontactmanager.h>

#include "mt_mobhistorymodel.h"
#include "mobhistorymodel.h"
#include "stub_classes.h"

void TestMobHistoryModel::initTestCase()
{
}

void TestMobHistoryModel::cleanupTestCase()
{
}

void TestMobHistoryModel::testModel()
{
    // Contact centric history data
    QContactManager* cm = new QContactManager("symbian");
    QContact c;
    QVERIFY(cm->saveContact(&c));
    MobHistoryModel* model = new MobHistoryModel(c.localId(), cm);
    QVERIFY(model != 0);
    QVERIFY(model->rowCount() == 6);
    
    // Branded icon path
    QModelIndex modelIndex = model->index(1, 0);
    QVERIFY(modelIndex.isValid());
    QVariant var = modelIndex.data(Qt::DecorationRole);
    QVERIFY(var.type() == QVariant::String);
    
    // Message direction
    modelIndex = model->index(1, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(Qt::UserRole);
    QVERIFY(var.type() == QVariant::Int);
    
    // Message
    modelIndex = model->index(1, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(Qt::DisplayRole);
    QString str = QString(cc_msg).arg(0);
    QVERIFY(var.type() == QVariant::StringList);
    QVERIFY(var.toStringList().at(1) == str);
    
    modelIndex = model->index(3, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(Qt::DisplayRole);
    str = QString(cc_msg).arg(2);
    QVERIFY(var.type() == QVariant::StringList);
    QVERIFY(var.toStringList().at(1) == str);
    
    // Invalid role
    modelIndex = model->index(1, 0);
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
    QVERIFY(model->rowCount() == 8);
    
    cm->removeContact(c.localId());
    
    delete cm;
}

void TestMobHistoryModel::testSorting()
{
    QContactManager* cm = new QContactManager("symbian");
    QContact c;
    QVERIFY(cm->saveContact(&c));
    MobHistoryModel* model = new MobHistoryModel(c.localId(), cm);
    QVERIFY(model != 0);
    QVERIFY(model->rowCount() == 6);
    
    // Sort ascending
    model->sort(0, Qt::AscendingOrder);
    
    QModelIndex modelIndex = model->index(1, 0);
    QVERIFY(modelIndex.isValid());
    QVariant var = modelIndex.data(Qt::DisplayRole);
    QDateTime dt1 = QDateTime::fromString(var.toStringList().at(2));
    
    modelIndex = model->index(2, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(Qt::DisplayRole);
    QDateTime dt2 = QDateTime::fromString(var.toStringList().at(2));
    QVERIFY(dt1 < dt2);   
    
    // Sort descending
    model->sort(0, Qt::DescendingOrder);
    
    modelIndex = model->index(1, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(Qt::DisplayRole);
    dt1 = QDateTime::fromString(var.toStringList().at(2));
    
    modelIndex = model->index(2, 0);
    QVERIFY(modelIndex.isValid());
    var = modelIndex.data(Qt::DisplayRole);
    dt2 = QDateTime::fromString(var.toStringList().at(2));
    QVERIFY(dt1 > dt2);
    
    cm->removeContact(c.localId());
    
    delete model;
    delete cm;
}

QTEST_MAIN(TestMobHistoryModel);

