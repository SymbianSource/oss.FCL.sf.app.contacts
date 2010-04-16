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
#include "t_dateeditor.h"
#include "cntdetailconst.h"
#include "cntdetailmodelitem.h"
#include "cntdateeditormodel.h"
#include "cntdateeditorviewitem.h"

#include <qcontact.h>
#include <qcontactbirthday.h>
#include <qcontactanniversary.h>

#include <hbdataform.h>
#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>
#include <QtTest/QtTest>

void T_DateEditorTest::init()
{
    mContact = new QContact();
    mForm = new HbDataForm();
    mLocale = QLocale::system();
}

void T_DateEditorTest::cleanup()
{
    mForm->deleteLater();
}

// tests
void T_DateEditorTest::testDateModelWithDefaultData()
{
    CntDateEditorModel* model = new CntDateEditorModel(mContact);
    CntDateEditorViewItem* view = new CntDateEditorViewItem();

    mForm->setModel(model, view);

    HbDataFormModelItem* bdItem = model->itemFromIndex(model->index(0, 0));
    QVERIFY( bdItem->type() == HbDataFormModelItem::CustomItemBase );
    QVERIFY( bdItem->label() == "Birthday" );
    
    HbDataFormModelItem* anniversaryItem = model->itemFromIndex( model->index(1,0) );
    QVERIFY( anniversaryItem->type() == HbDataFormModelItem::CustomItemBase );
    QVERIFY( anniversaryItem->label() == "Anniversary" );
    }

void T_DateEditorTest::testDateModelWithData()
    {
    QContactBirthday* birthday = new QContactBirthday();
    birthday->setDate( QDate(1976,10,13) );
    mContact->saveDetail( birthday );
    
    QContactAnniversary* anniversary = new QContactAnniversary();
    anniversary->setOriginalDate( QDate(1977,3,27) );
    mContact->saveDetail( anniversary );
    
    CntDateEditorModel* model = new CntDateEditorModel( mContact );
    CntDateEditorViewItem* view = new CntDateEditorViewItem();
    mForm->setModel( model, view );
        
    CntDetailModelItem* bdItem = static_cast<CntDetailModelItem*>(model->itemFromIndex( model->index(0,0) ));
    QVERIFY( bdItem->type() == HbDataFormModelItem::CustomItemBase );
    QVERIFY( bdItem->label() == "Birthday" );
    QContactBirthday b = bdItem->detail();
    QVERIFY( b.date() == QDate(1976,10,13) );
    
    CntDetailModelItem* anniversaryItem = static_cast<CntDetailModelItem*>(model->itemFromIndex( model->index(1,0) ));
    QVERIFY( anniversaryItem->type() == HbDataFormModelItem::CustomItemBase );
    QVERIFY( anniversaryItem->label() == "Anniversary" );
    QContactAnniversary a = anniversaryItem->detail();
    QVERIFY( a.originalDate() == QDate(1977,3,27) );
    }

// this doesn't test any swahili, it was impossible to change the system
// locale. Should be tested perhaps with default locale?
void T_DateEditorTest::testDatelViewItem_Swahili()
    {
    QContactBirthday* birthday = new QContactBirthday();
    birthday->setDate( QDate(1976,10,13) );
    mContact->saveDetail( birthday );
       
    QContactAnniversary* anniversary = new QContactAnniversary();
    anniversary->setOriginalDate( QDate(1977,3,27) );
    mContact->saveDetail( anniversary );
       
    CntDateEditorModel* model = new CntDateEditorModel( mContact );
    CntDateEditorViewItem* view = new CntDateEditorViewItem();
    mForm->setModel( model, view );
    
    CntDateEditorViewItem* birthdayItem = static_cast<CntDateEditorViewItem*>(mForm->itemByIndex( model->index(0,0)));
    QVERIFY( birthdayItem->mButton->text() == mLocale.toString(QDate(1976,10,13)) );
    
    CntDateEditorViewItem* anniversaryItem = static_cast<CntDateEditorViewItem*>(mForm->itemByIndex( model->index(1,0)));
    QVERIFY( anniversaryItem->mButton->text() == mLocale.toString(QDate(1977,3,27)) );
    }

void T_DateEditorTest::testDatelViewItem_English()
    {
    QContactBirthday* birthday = new QContactBirthday();
    birthday->setDate( QDate(1976,10,13) );
    mContact->saveDetail( birthday );
       
    QContactAnniversary* anniversary = new QContactAnniversary();
    anniversary->setOriginalDate( QDate(1977,3,27) );
    mContact->saveDetail( anniversary );
       
    CntDateEditorModel* model = new CntDateEditorModel( mContact );
    CntDateEditorViewItem* view = new CntDateEditorViewItem();
    mForm->setModel( model, view );
    
    // Note that default locale is now Swahili (see init())
    CntDateEditorViewItem* birthdayItem = static_cast<CntDateEditorViewItem*>(mForm->itemByIndex( model->index(0,0)));
    QString txt = birthdayItem->mButton->text();
    QVERIFY( birthdayItem->mButton->text() == mLocale.toString(QDate(1976,10,13)) );
    
    CntDateEditorViewItem* anniversaryItem = static_cast<CntDateEditorViewItem*>(mForm->itemByIndex( model->index(1,0)));
    QVERIFY( anniversaryItem->mButton->text() == mLocale.toString(QDate(1977,3,27)) );
    
    model->saveContactDetails();
    
    QList<QContactBirthday> bds = mContact->details<QContactBirthday>();
    QCOMPARE( bds.size(), 1 );
    QContactBirthday b = bds.first();
    QCOMPARE( b.date(), QDate(1976, 10,13) );
    
    QList<QContactAnniversary> as = mContact->details<QContactAnniversary>();
    QCOMPARE( as.size(), 1 );
    QContactAnniversary a = as.first();
    QCOMPARE( a.originalDate(), QDate(1977,3,27) );
    
    delete model;
    }

// End of File
