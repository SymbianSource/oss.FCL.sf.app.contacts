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
#include "t_companyeditor.h"
#include "cntcompanyeditormodel.h"
#include "cntdetailconst.h"
#include <qcontact.h>
#include <qcontactorganization.h>

#include <hbdataform.h>
#include <hbdataformviewitem.h>
#include <QtTest/QtTest>

void T_CompanyEditorTest::init()
    {
    mContact = new QContact();
    mForm = new HbDataForm();
    }

void T_CompanyEditorTest::cleanup()
    {
    delete mForm;
    }
    
void T_CompanyEditorTest::testCompanyModelWithDefaultData()
    {
    CntCompanyEditorModel* model = new CntCompanyEditorModel( mContact );
    mForm->setModel( model );
    
    HbDataFormModelItem* company = model->itemFromIndex( model->index(0, 0) );
    QCOMPARE( company->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( company->label(), QString("Company") );
    QVERIFY( company->contentWidgetData("maxLength") == CNT_ORGANIZATION_MAXLENGTH );
    
    HbDataFormModelItem* jobTitle = model->itemFromIndex( model->index(1, 0) );
    QCOMPARE( jobTitle->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( jobTitle->label(), QString("Job title") );
    QVERIFY( jobTitle->contentWidgetData("maxLength") == CNT_JOBTITLE_MAXLENGTH );
    
    HbDataFormModelItem* department = model->itemFromIndex( model->index(2, 0) );
    QCOMPARE( department->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( department->label(), QString("Department") );
    QVERIFY( department->contentWidgetData("maxLength") == CNT_DEPARTMENT_MAXLENGTH );
    
    HbDataFormModelItem* assistant = model->itemFromIndex( model->index(3, 0) );
    QCOMPARE( assistant->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( assistant->label(), QString("Assistant name") );
    QVERIFY( assistant->contentWidgetData("maxLength") == CNT_ASSISTANT_MAXLENGTH );
    }

void T_CompanyEditorTest::testCompanyModelWithData()
    {
    QContactOrganization* org = new QContactOrganization();
    org->setName( "Nokia" );
    org->setTitle( "Software Developer" );
    org->setAssistantName( "No Assistant" );
            
    QStringList departments;
    departments << "Devices";
    org->setDepartment( departments );
            
    mContact->saveDetail( org );
    
    CntCompanyEditorModel* model = new CntCompanyEditorModel( mContact );
    mForm->setModel( model );
        
    HbDataFormModelItem* company = model->itemFromIndex( model->index(0, 0) );
    QCOMPARE( company->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( company->label(), QString("Company") );
    QVERIFY( company->contentWidgetData("text") == "Nokia" );
    
    HbDataFormModelItem* jobTitle = model->itemFromIndex( model->index(1, 0) );
    QCOMPARE( jobTitle->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( jobTitle->label(), QString("Job title") );
    QVERIFY( jobTitle->contentWidgetData("text") == "Software Developer" );
            
    HbDataFormModelItem* department = model->itemFromIndex( model->index(2, 0) );
    QCOMPARE( department->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( department->label(), QString("Department") );
    QVERIFY( department->contentWidgetData("text") == "Devices" );
            
    HbDataFormModelItem* assistant = model->itemFromIndex( model->index(3, 0) );
    QCOMPARE( assistant->type(), HbDataFormModelItem::TextItem );
    QCOMPARE( assistant->label(), QString("Assistant name") );
    QVERIFY( assistant->contentWidgetData("text") == "No Assistant" );
    
    model->saveContactDetails();
    
    QList<QContactOrganization> companies = mContact->details<QContactOrganization>();
    QCOMPARE( companies.size(), 1 );
    
    QContactOrganization nokia = companies.first();
    QVERIFY( nokia.name() == "Nokia" );
    QVERIFY( nokia.title() == "Software Developer" );
    QVERIFY( nokia.assistantName() == "No Assistant" );
    QCOMPARE( nokia.department().size(), 1 );
    
    delete model; 
    }
// End of File
