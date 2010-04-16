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
#include "t_addresseditor.h"    
#include "cntdetailconst.h"
#include "cntaddressmodel.h"
#include "cntaddressviewitem.h"
#include <QContactAddress.h>
#include <hbdataformmodel.h>
#include <hbdataform.h>
#include <QtTest/QtTest>

void T_AddressEditorTest::init()
    {
    mContact = new QContact();
    mForm = new HbDataForm();
    }

void T_AddressEditorTest::cleanup()
    {
    delete mForm;
    }

void T_AddressEditorTest::testAddressModelWithDefaultData()
    {
    CntAddressModel* model = new CntAddressModel( mContact );
    CntAddressViewItem* viewItem = new CntAddressViewItem();
    mForm->setModel( model, viewItem );
    
    testItemLayout( model );
    delete model;
    }

void T_AddressEditorTest::testAddressModelWithData()
    {
    QContactAddress* address = new QContactAddress();
    address->setContexts( QContactAddress::ContextHome );
    address->setStreet( "Karakaari 7" );
    address->setPostcode( "02320" );
    address->setLocality("Espoo");
    address->setRegion( "Uudenmaanlääni" );
    address->setCountry( "Finland" );
    mContact->saveDetail( address );
        
    CntAddressModel* model = new CntAddressModel( mContact );
    CntAddressViewItem* viewItem = new CntAddressViewItem();
    mForm->setModel( model, viewItem );
    
    testItemLayout( model );
    
    HbDataFormModelItem* context = model->itemFromIndex( model->index(0, 0) );
    QCOMPARE( context->type(), HbDataFormModelItem::GroupItem );
    QCOMPARE( context->label(), QString("Address") );
    // WITH MAP LOCATION PICKER
//    QVERIFY(context->childAt( 1 )->contentWidgetData("text").toString() == QString("") );
//    QVERIFY(context->childAt( 1 )->contentWidgetData("maxLength").toInt() == CNT_STREET_MAXLENGTH );
//    QVERIFY(context->childAt( 2 )->contentWidgetData("text").toString() == QString("") );
//    QVERIFY(context->childAt( 2 )->contentWidgetData("maxLength").toInt() == CNT_POSTCODE_MAXLENGTH );
//    QVERIFY(context->childAt( 3 )->contentWidgetData("text").toString() == QString("") );
//    QVERIFY(context->childAt( 3 )->contentWidgetData("maxLength").toInt() == CNT_LOCALITY_MAXLENGTH );
//    QVERIFY(context->childAt( 4 )->contentWidgetData("text").toString() == QString("") );
//    QVERIFY(context->childAt( 4 )->contentWidgetData("maxLength").toInt() == CNT_REGION_MAXLENGTH );
//    QVERIFY(context->childAt( 5 )->contentWidgetData("text").toString() == QString("") );
//    QVERIFY(context->childAt( 5 )->contentWidgetData("maxLength").toInt() == CNT_COUNTRY_MAXLENGTH );
        
    QVERIFY(context->childAt( 0 )->contentWidgetData("text").toString() == QString("") );
    QVERIFY(context->childAt( 0 )->contentWidgetData("maxLength").toInt() == CNT_STREET_MAXLENGTH );
    QVERIFY(context->childAt( 1 )->contentWidgetData("text").toString() == QString("") );
    QVERIFY(context->childAt( 1 )->contentWidgetData("maxLength").toInt() == CNT_POSTCODE_MAXLENGTH );
    QVERIFY(context->childAt( 2 )->contentWidgetData("text").toString() == QString("") );
    QVERIFY(context->childAt( 2 )->contentWidgetData("maxLength").toInt() == CNT_LOCALITY_MAXLENGTH );
    QVERIFY(context->childAt( 3 )->contentWidgetData("text").toString() == QString("") );
    QVERIFY(context->childAt( 3 )->contentWidgetData("maxLength").toInt() == CNT_REGION_MAXLENGTH );
    QVERIFY(context->childAt( 4 )->contentWidgetData("text").toString() == QString("") );
    QVERIFY(context->childAt( 4 )->contentWidgetData("maxLength").toInt() == CNT_COUNTRY_MAXLENGTH );
        
    HbDataFormModelItem* contextHome = model->itemFromIndex( model->index(1, 0) );
    QCOMPARE( contextHome->type(), HbDataFormModelItem::GroupItem );
    QCOMPARE( contextHome->label(), QString("Address (home)") );
    // WITH MAP LOCATION PICKER
//    QVERIFY(contextHome->childAt( 1 )->contentWidgetData("text").toString() == QString("Karakaari 7") );
//    QVERIFY(contextHome->childAt( 2 )->contentWidgetData("text").toString() == QString("02320") );
//    QVERIFY(contextHome->childAt( 3 )->contentWidgetData("text").toString() == QString("Espoo") );
//    QVERIFY(contextHome->childAt( 4 )->contentWidgetData("text").toString() == QString("Uudenmaanlääni") );
//    QVERIFY(contextHome->childAt( 5 )->contentWidgetData("text").toString() == QString("Finland") );
    
    QVERIFY(contextHome->childAt( 0 )->contentWidgetData("text").toString() == QString("Karakaari 7") );
    QVERIFY(contextHome->childAt( 1 )->contentWidgetData("text").toString() == QString("02320") );
    QVERIFY(contextHome->childAt( 2 )->contentWidgetData("text").toString() == QString("Espoo") );
    QVERIFY(contextHome->childAt( 3 )->contentWidgetData("text").toString() == QString("Uudenmaanlääni") );
    QVERIFY(contextHome->childAt( 4 )->contentWidgetData("text").toString() == QString("Finland") );
        
    HbDataFormModelItem* contextWork = model->itemFromIndex( model->index(2, 0) );
    QCOMPARE( contextWork->type(), HbDataFormModelItem::GroupItem );
    QCOMPARE( contextWork->label(), QString("Address (work)") );
    // WITH MAP LOCATION PICKER
//    QVERIFY(contextWork->childAt( 1 )->contentWidgetData("text").toString() == QString("") );
//    QVERIFY(contextWork->childAt( 2 )->contentWidgetData("text").toString() == QString("") );
//    QVERIFY(contextWork->childAt( 3 )->contentWidgetData("text").toString() == QString("") );
//    QVERIFY(contextWork->childAt( 4 )->contentWidgetData("text").toString() == QString("") );
//    QVERIFY(contextWork->childAt( 5 )->contentWidgetData("text").toString() == QString("") );
    
    QVERIFY(contextWork->childAt( 0 )->contentWidgetData("text").toString() == QString("") );
    QVERIFY(contextWork->childAt( 1 )->contentWidgetData("text").toString() == QString("") );
    QVERIFY(contextWork->childAt( 2 )->contentWidgetData("text").toString() == QString("") );
    QVERIFY(contextWork->childAt( 3 )->contentWidgetData("text").toString() == QString("") );
    QVERIFY(contextWork->childAt( 4 )->contentWidgetData("text").toString() == QString("") );
    
    model->saveContactDetails();
    delete model;
    }

void T_AddressEditorTest::testAddressCustomViewItem()
    {
    }

void T_AddressEditorTest::testItemLayout( CntAddressModel* aModel )
    {
    HbDataFormModelItem* noContext = aModel->itemFromIndex( aModel->index(0, 0) );
    QCOMPARE( noContext->type(), HbDataFormModelItem::GroupItem );
    QCOMPARE( noContext->label(), QString("Address") );
//    QCOMPARE( noContext->childCount(), 6 ); // WITH MAP LOCATION PICKER
//    QCOMPARE( noContext->childAt(0)->type(), HbDataFormModelItem::CustomItemBase );
//    QCOMPARE( noContext->childAt(1)->type(), HbDataFormModelItem::TextItem );
//    QCOMPARE( noContext->childAt(2)->type(), HbDataFormModelItem::TextItem );
//    QCOMPARE( noContext->childAt(3)->type(), HbDataFormModelItem::TextItem );
//    QCOMPARE( noContext->childAt(4)->type(), HbDataFormModelItem::TextItem );
//    QCOMPARE( noContext->childAt(5)->type(), HbDataFormModelItem::TextItem );
        
    QCOMPARE( noContext->childCount(), 5 );
    QCOMPARE( noContext->childAt(0)->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( noContext->childAt(1)->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( noContext->childAt(2)->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( noContext->childAt(3)->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( noContext->childAt(4)->type(), HbDataFormModelItem::TextItem );
        
    HbDataFormModelItem* contextHome = aModel->itemFromIndex( aModel->index(1, 0) );
    QCOMPARE( contextHome->type(), HbDataFormModelItem::GroupItem );
    QCOMPARE( contextHome->label(), QString("Address (home)") );
    
//    QCOMPARE( contextHome->childCount(), 6 ); // WITH MAP LOCATION PICKER
//    QCOMPARE( contextHome->childAt(0)->type(), HbDataFormModelItem::CustomItemBase );
//    QCOMPARE( contextHome->childAt(1)->type(), HbDataFormModelItem::TextItem );
//    QCOMPARE( contextHome->childAt(2)->type(), HbDataFormModelItem::TextItem );
//    QCOMPARE( contextHome->childAt(3)->type(), HbDataFormModelItem::TextItem );
//    QCOMPARE( contextHome->childAt(4)->type(), HbDataFormModelItem::TextItem );
//    QCOMPARE( contextHome->childAt(5)->type(), HbDataFormModelItem::TextItem );
    
    QCOMPARE( contextHome->childCount(), 5 );
    QCOMPARE( contextHome->childAt(0)->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( contextHome->childAt(1)->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( contextHome->childAt(2)->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( contextHome->childAt(3)->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( contextHome->childAt(4)->type(), HbDataFormModelItem::TextItem );
        
    HbDataFormModelItem* contextWork = aModel->itemFromIndex( aModel->index(2, 0) );
    QCOMPARE( contextWork->type(), HbDataFormModelItem::GroupItem );
    QCOMPARE( contextWork->label(), QString("Address (work)") );
//    QCOMPARE( contextWork->childCount(), 6 );// WITH MAP LOCATION PICKER
//    QCOMPARE( contextWork->childAt(0)->type(), HbDataFormModelItem::CustomItemBase );
//    QCOMPARE( contextWork->childAt(1)->type(), HbDataFormModelItem::TextItem );
//    QCOMPARE( contextWork->childAt(2)->type(), HbDataFormModelItem::TextItem );
//    QCOMPARE( contextWork->childAt(3)->type(), HbDataFormModelItem::TextItem );
//    QCOMPARE( contextWork->childAt(4)->type(), HbDataFormModelItem::TextItem );
//    QCOMPARE( contextWork->childAt(5)->type(), HbDataFormModelItem::TextItem );
    
    QCOMPARE( contextWork->childCount(), 5 );
    QCOMPARE( contextWork->childAt(0)->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( contextWork->childAt(1)->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( contextWork->childAt(2)->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( contextWork->childAt(3)->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( contextWork->childAt(4)->type(), HbDataFormModelItem::TextItem );

    }

// End of File
