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
#include "t_familyeditor.h"
#include "cntfamilyeditormodel.h"
#include "cntdetailconst.h"

#include <hbdataform.h>
#include <hbdataformmodelitem.h>
#include <QContactFamily.h>
#include <QtTest/QtTest>

void T_FamilyEditorTest::init()
    {
    mContact = new QContact();
    mForm = new HbDataForm();
    }

void T_FamilyEditorTest::cleanup()
    {
    delete mForm;
    }

 void T_FamilyEditorTest::testFamilyModelWithDefaultData()
     {
     CntFamilyEditorModel* model = new CntFamilyEditorModel( mContact );
     mForm->setModel( model );
     
     HbDataFormModelItem* spouse = model->itemFromIndex( model->index(0,0) ); // spouse
     QVERIFY( spouse->type() == HbDataFormModelItem::TextItem );
     QVERIFY( spouse->label() == "Spouse" );
     QVERIFY( spouse->contentWidgetData("text") == "" );
     QVERIFY( spouse->contentWidgetData("maxLength") == CNT_SPOUSE_MAXLENGTH );
     
     HbDataFormModelItem* children = model->itemFromIndex( model->index(1,0) ); // children
     QVERIFY( children->type() == HbDataFormModelItem::TextItem );
     QVERIFY( children->label() == "Children" );
     QVERIFY( children->contentWidgetData("text") == "" );
     QVERIFY( children->contentWidgetData("maxLength") == CNT_CHILDREN_MAXLENGTH );
     }
 
 void T_FamilyEditorTest::testFamilyModelWithData()
     {
     QContactFamily* family = new QContactFamily();
     family->setSpouse( "MySpouse" );
     
     QStringList childrenList;
     childrenList << "ScreamingBaby" << "CryBaby" << "WiningBaby";
     family->setChildren( childrenList );
     
     mContact->saveDetail( family );
     
     CntFamilyEditorModel* model = new CntFamilyEditorModel( mContact );
     mForm->setModel( model );
          
     HbDataFormModelItem* spouse = model->itemFromIndex( model->index(0,0) ); // spouse
     QVERIFY( spouse->type() == HbDataFormModelItem::TextItem );
     QVERIFY( spouse->label() == "Spouse" );
     QVERIFY( spouse->contentWidgetData("text") == "MySpouse" );
     QVERIFY( spouse->contentWidgetData("maxLength") == CNT_SPOUSE_MAXLENGTH );
          
     HbDataFormModelItem* children = model->itemFromIndex( model->index(1,0) ); // children
     QVERIFY( children->type() == HbDataFormModelItem::TextItem );
     QVERIFY( children->label() == "Children" );
     QVERIFY( children->contentWidgetData("text") == "ScreamingBaby, CryBaby, WiningBaby" );
     QVERIFY( children->contentWidgetData("maxLength") == CNT_CHILDREN_MAXLENGTH );
     
     model->saveContactDetails();
     
     delete model;
     }
     
 // End of File
