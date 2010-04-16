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
#include "t_nameeditor.h"
#include "cntdetailconst.h"
#include <QContact.h>
#include <QContactName.h>
#include <QContactNickName.h>
#include <hbdataform.h>
#include "cntnameeditormodel.h"
#include <QtTest/QtTest>

void T_NameEditorTest::init()
    {
    mContact = new QContact();
    mForm = new HbDataForm();
    }

void T_NameEditorTest::cleanup()
    {
    delete mForm;
    }
    
void T_NameEditorTest::testNameModelDefaultData()
    {
    CntNameEditorModel* model = new CntNameEditorModel( mContact );
    mForm->setModel( model );
    
    HbDataFormModelItem* root = model->invisibleRootItem();
    QCOMPARE( root->childCount(), 6 );
    
    for ( int i(0); i < root->childCount(); i++ )
        {
        HbDataFormModelItem* item = model->itemFromIndex( model->index(i,0) );
        QCOMPARE( item->type(), HbDataFormModelItem::TextItem );
        QVERIFY( item->contentWidgetData("text") == "" );
        }
    delete model;
    }

void T_NameEditorTest::testNameModelData()
    {
    QContactName* name = new QContactName();
    name->setFirst( "Juhapekka" );
    name->setLast( "Aaltonen" );
    name->setMiddle( "Antero" );
    name->setSuffix( "Jr" );
    name->setPrefix( "Mr");
    
    QContactNickname* nick = new QContactNickname();
    nick->setNickname( "SuperGeek" );
    mContact->saveDetail( nick );
    mContact->saveDetail( name );
    
    CntNameEditorModel* model = new CntNameEditorModel( mContact );
    mForm->setModel( model );
        
    HbDataFormModelItem* root = model->invisibleRootItem();
    QCOMPARE( root->childCount(), 6 );
    
    HbDataFormModelItem* firstname  = model->itemFromIndex( model->index(0,0) );
    QVERIFY(firstname->contentWidgetData("text") == "Juhapekka" );
    QVERIFY(firstname->contentWidgetData("maxLength") == CNT_FIRSTNAME_MAXLENGTH );
    
    HbDataFormModelItem* lastname   = model->itemFromIndex( model->index(1,0) );
    QVERIFY(lastname->contentWidgetData("text") == "Aaltonen" );
    QVERIFY(lastname->contentWidgetData("maxLength") == CNT_LASTNAME_MAXLENGTH );
    
    HbDataFormModelItem* middlename = model->itemFromIndex( model->index(2,0) );
    QVERIFY(middlename->contentWidgetData("text") == "Antero" );
    QVERIFY(middlename->contentWidgetData("maxLength") == CNT_MIDDLENAME_MAXLENGTH );
    
    HbDataFormModelItem* nickname   = model->itemFromIndex( model->index(3,0) );
    QVERIFY(nickname->contentWidgetData("text") == "SuperGeek" );
    QVERIFY(nickname->contentWidgetData("maxLength") == CNT_NICKNAME_MAXLENGTH );
    
    HbDataFormModelItem* prefix     = model->itemFromIndex( model->index(4,0) );
    QVERIFY(prefix->contentWidgetData("text") == "Mr" );
    QVERIFY(prefix->contentWidgetData("maxLength") == CNT_PREFIX_MAXLENGTH );
    
    HbDataFormModelItem* suffix     = model->itemFromIndex( model->index(5,0) );
    QVERIFY(suffix->contentWidgetData("text") == "Jr" );
    QVERIFY(suffix->contentWidgetData("maxLength") == CNT_SUFFIX_MAXLENGTH );
    
    model->saveContactDetails();
    delete model;
    }
// End of Filoe
