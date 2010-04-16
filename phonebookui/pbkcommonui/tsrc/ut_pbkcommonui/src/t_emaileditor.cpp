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
#include "t_emaileditor.h"
#include "cntdetailconst.h"
#include "cntemaileditormodel.h"
#include "cntemaileditorviewitem.h"
#include "cntdetailmodelitem.h"

#include <qcontactemailaddress.h>
#include <hbdataform.h>
#include <hbcombobox.h>
#include <hblineedit.h>
#include <QtTest/QtTest>
#include <QTestEventList>

void T_EmailEditorTest::init()
    {
    mContact = new QContact();
    mForm = new HbDataForm();
    }

void T_EmailEditorTest::cleanup()
    {
    delete mForm;
    }
    
void T_EmailEditorTest::testEmailModelWithDefaultData()
    {
    CntEmailEditorModel* model = new CntEmailEditorModel( mContact );
    // now there should be one default phonenumber with subtype Mobile.
    HbDataFormModelItem* root = model->invisibleRootItem();
    QVERIFY( root->childCount() == 1 );
        
   CntDetailModelItem* defaultItem = static_cast<CntDetailModelItem*>(root->childAt( 0 ));
   QContactDetail mobile = defaultItem->detail();
   QVERIFY( mobile.definitionName() == QContactEmailAddress::DefinitionName );
   
   QContactEmailAddress address = static_cast<QContactEmailAddress>( mobile );
   QVERIFY( address.emailAddress() == "" );
   
   delete model;
   }

void T_EmailEditorTest::testEmailModelWithData()
    {
    QContactEmailAddress* address = new QContactEmailAddress();
    address->setEmailAddress( "phonebook@nokia.com" );
    mContact->saveDetail( address );
       
    CntEmailEditorModel* model = new CntEmailEditorModel( mContact );
    HbDataFormModelItem* root = model->invisibleRootItem();
    QVERIFY( root->childCount() == 1 );
    
    CntDetailModelItem* defaultItem = static_cast<CntDetailModelItem*>(root->childAt( 0 ));
    QContactDetail emailAddress = defaultItem->detail();
    QVERIFY( emailAddress.definitionName() == QContactEmailAddress::DefinitionName );
    QContactEmailAddress email = emailAddress;
    
    QVERIFY( email.emailAddress() == "phonebook@nokia.com" );
    
    model->saveContactDetails();
    
    delete model;
    }

void T_EmailEditorTest::testInsertModelItem()
{
    CntEmailEditorModel* model = new CntEmailEditorModel( mContact );
    HbDataFormModelItem* root = model->invisibleRootItem();
    QVERIFY( root->childCount() == 1 ); // the default one
    
    model->insertDetailField();
    
    QVERIFY ( root->childCount() == 2 );
    
    delete model;
}

void T_EmailEditorTest::testEmailViewItem()
    {
    QContactEmailAddress* address = new QContactEmailAddress();
    address->setContexts( QContactDetail::ContextWork );
    address->setEmailAddress( "phonebook@nokia.com" );
    mContact->saveDetail( address );
            
    CntEmailEditorModel* model = new CntEmailEditorModel( mContact );
    CntEmailEditorViewItem* item = new CntEmailEditorViewItem();
        
    mForm->setModel( model, item );
        
    // select the first item from the view
    QModelIndex index = model->index(0, 0 );
    CntEmailEditorViewItem* first = static_cast<CntEmailEditorViewItem*>(mForm->itemByIndex( index ));
        
    // the given landline number should be selected in combobox
    int currentIndex = first->mBox->currentIndex();          
    QString text = first->mBox->itemText( currentIndex );
    QVERIFY( text == "Email (work)" );
    
    QString value = first->mEdit->text();
    QVERIFY( value == "phonebook@nokia.com" );
    
    // changing the text should affect to current detail
    QSignalSpy spy( first->mEdit, SIGNAL(textChanged(QString)));
    first->mEdit->setText( "nokia@phonebook.com" );
    QCOMPARE( spy.count(), 1 );
        
    CntDetailModelItem* modelItem = static_cast<CntDetailModelItem*>(model->itemFromIndex( index ));
    QContactEmailAddress testItem = modelItem->detail();
    QString n = testItem.emailAddress();
    QVERIFY( n == "nokia@phonebook.com" );
    }

void T_EmailEditorTest::testEmailViewItemComboChange()
    {
    QContactEmailAddress* address = new QContactEmailAddress();
    address->setContexts( QContactDetail::ContextHome );
    address->setEmailAddress( "phonebook@nokia.com" );
    mContact->saveDetail( address );
               
    CntEmailEditorModel* model = new CntEmailEditorModel( mContact );
    CntEmailEditorViewItem* item = new CntEmailEditorViewItem();
           
    mForm->setModel( model, item );
           
    // select the first item from the view
    QModelIndex index = model->index(0, 0 );
    CntEmailEditorViewItem* first = static_cast<CntEmailEditorViewItem*>(mForm->itemByIndex( index ));

    QAbstractItemModel* boxModel = first->mBox->model();
    int selectedIndex = 0;
    for ( int i(0); i < boxModel->rowCount(); i++ )
        {
        QModelIndex modelIndex = boxModel->index( i, 0 );
        QString cntx = boxModel->data(modelIndex, DetailContext).toString();
        if ( cntx == QContactEmailAddress::ContextWork )
            {
            selectedIndex = i;
            break;
            }
        }
    
    QSignalSpy boxSpy( first->mBox, SIGNAL(currentIndexChanged(int)) );
    first->mBox->setCurrentIndex( selectedIndex );
    QCOMPARE( boxSpy.count(), 1 ); // check that index was really changed
    
    index = model->index(0, 0 );
    CntDetailModelItem* modelItem = static_cast<CntDetailModelItem*>(model->itemFromIndex(index) );
    QContactEmailAddress email = modelItem->detail();
    QVERIFY( email.contexts().isEmpty() == false );
    QVERIFY( email.contexts().first() == QContactEmailAddress::ContextWork );
    }

void T_EmailEditorTest::testEmailViewItemLineEdit()
    {
    QContactEmailAddress* address = new QContactEmailAddress();
    address->setContexts( QContactDetail::ContextHome );
    address->setEmailAddress( "phonebook@nokia.com" );
    mContact->saveDetail( address );
                   
    CntEmailEditorModel* model = new CntEmailEditorModel( mContact );
    CntEmailEditorViewItem* item = new CntEmailEditorViewItem();
               
    mForm->setModel( model, item );
               
    // select the first item from the view
    QModelIndex index = model->index(0, 0 );
    CntEmailEditorViewItem* first = static_cast<CntEmailEditorViewItem*>(mForm->itemByIndex( index ));
    QVERIFY( first->mEdit->maxLength() == CNT_EMAIL_EDITOR_MAXLENGTH );
    
    // change the line edit text
    QSignalSpy editSpy( first->mEdit, SIGNAL(textChanged(QString)) );
    first->mEdit->setText( "nokia@phonebook.com" );
    QCOMPARE( editSpy.count(), 1 );
    
    // check that model also contains the updated text
    CntDetailModelItem* modelItem = static_cast<CntDetailModelItem*>(model->itemFromIndex( index ));
    QContactEmailAddress a = modelItem->detail();
    QVERIFY( a.emailAddress() == "nokia@phonebook.com" );
    }
// End of File
