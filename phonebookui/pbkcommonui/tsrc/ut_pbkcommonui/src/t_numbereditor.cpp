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
#include "t_numbereditor.h"
#include "cntdetailconst.h"
#include "cntdetailmodelitem.h"
#include "cntphonenumbermodel.h"
#include "cntphonenumberviewitem.h"

#include <hbdataform.h>
#include <hbcombobox.h>
#include <hblineedit.h>
#include <QtTest/QtTest>
#include <QTestEventList>

void T_NumberEditorTest::init()
    {
    mContact = new QContact();
    mForm = new HbDataForm();
    }

void T_NumberEditorTest::cleanup()
    {
    delete mForm;
    }
    
    // tests
void T_NumberEditorTest::testPhoneNumberModelWithDefaultData()
    {
    CntPhoneNumberModel* model = new CntPhoneNumberModel( mContact );
    // now there should be one default phonenumber with subtype Mobile.
    HbDataFormModelItem* root = model->invisibleRootItem();
    QVERIFY( root->childCount() == 1 );
    
    CntDetailModelItem* defaultItem = static_cast<CntDetailModelItem*>(root->childAt( 0 ));
    QContactDetail mobile = defaultItem->detail();
    QVERIFY( mobile.definitionName() == QContactPhoneNumber::DefinitionName );
    QContactPhoneNumber number = static_cast<QContactPhoneNumber>( mobile );
    QStringList subTypes = number.subTypes();
    QVERIFY( subTypes.count() == 1 );
    QVERIFY( subTypes.first() == QContactPhoneNumber::SubTypeMobile );
    
    delete model;
    }

void T_NumberEditorTest::testPhoneNumberModelWithData()
    {
    QContactPhoneNumber* number = new QContactPhoneNumber();
    number->setSubTypes( QContactPhoneNumber::SubTypeLandline );
    number->setNumber( "1234" );
    mContact->saveDetail( number );
    
    CntPhoneNumberModel* model = new CntPhoneNumberModel( mContact );
    HbDataFormModelItem* root = model->invisibleRootItem();
    QVERIFY( root->childCount() == 1 );
    CntDetailModelItem* defaultItem = static_cast<CntDetailModelItem*>(root->childAt( 0 ));
    QContactDetail mobile = defaultItem->detail();
    QVERIFY( mobile.definitionName() == QContactPhoneNumber::DefinitionName );
    QContactPhoneNumber nro = static_cast<QContactPhoneNumber>( mobile );
    QStringList subTypes = nro.subTypes();
    QVERIFY( subTypes.first() == QContactPhoneNumber::SubTypeLandline );
    QVERIFY( nro.number() == "1234" );
    
    delete model;
    }

void T_NumberEditorTest::testInsertPhoneNumber()
{
    QContactPhoneNumber* number = new QContactPhoneNumber();
    number->setSubTypes( QContactPhoneNumber::SubTypeLandline );
    number->setNumber( "1234" );
    mContact->saveDetail( number );
        
    CntPhoneNumberModel* model = new CntPhoneNumberModel( mContact );
    HbDataFormModelItem* root = model->invisibleRootItem();
    QVERIFY( root->childCount() == 1 );
    
    model->insertDetailField();
    QVERIFY( root->childCount() == 2 );
    
    model->saveContactDetails();
    QList<QContactPhoneNumber> nros = mContact->details<QContactPhoneNumber>();
    QVERIFY( nros.size() == 1 );
    
    delete model;
}

void T_NumberEditorTest::testPhoneNumberViewItem()
    {
    QContactPhoneNumber* number = new QContactPhoneNumber();
    number->setSubTypes( QContactPhoneNumber::SubTypeLandline );
    number->setNumber( "1234" );
    mContact->saveDetail( number );
        
    CntPhoneNumberModel* model = new CntPhoneNumberModel( mContact );
    CntPhoneNumberViewItem* item = new CntPhoneNumberViewItem();
    mForm->setModel( model, item );
    
    // select the first item from the view
    QModelIndex index = model->index(0, 0 );
    CntPhoneNumberViewItem* first = static_cast<CntPhoneNumberViewItem*>(mForm->itemByIndex( index ));
    
    // the given landline number should be selected in combobox
    int currentIndex = first->mBox->currentIndex();          
    QString text = first->mBox->itemText( currentIndex );
    QVERIFY( text == hbTrId("txt_phob_setlabel_val_phone") );
    
    // the given landline number should be in the text field
    QString value = first->mEdit->text();
    QVERIFY( value == "1234" );
    
    // changing the text should affect to current detail
    QSignalSpy editSpy( first->mEdit, SIGNAL(textChanged(QString)));
    first->mEdit->setText( "5678" );
    QCOMPARE( editSpy.count(), 1 );
    
    CntDetailModelItem* modelItem = static_cast<CntDetailModelItem*>(model->itemFromIndex( index ));
    QContactPhoneNumber testItem = modelItem->detail();
    QString n = testItem.number();
    QVERIFY( n == "5678" );
    
    // change the combo box index back to first one, and check that subtype is correct
    QSignalSpy boxSpy( first->mBox, SIGNAL(currentIndexChanged(int)) );
    // Since we are still on 'Landline' index, search for the 'Mobile (home)' and set it.
    QAbstractItemModel* itemModel = first->mBox->model();
    int selectedIndex = 0;
    for ( int i(0); i < itemModel->rowCount(); i++ )
        {
        QModelIndex modelIndex = itemModel->index(i, 0);
        if ( itemModel->data(modelIndex, DetailSubType ).toString() == QContactPhoneNumber::SubTypeMobile &&
             itemModel->data(modelIndex, DetailContext ).toString() == QContactDetail::ContextHome )
            {
            selectedIndex = i;
            break;
            }
        }
    first->mBox->setCurrentIndex( selectedIndex );
    QCOMPARE( boxSpy.count(), 1 );
    boxSpy.clear();
    // Now check that the model detail item contains the correct subtype
    testItem = modelItem->detail();
    QVERIFY( testItem.subTypes().isEmpty() == false );
    QVERIFY( testItem.subTypes().first() == QContactPhoneNumber::SubTypeMobile );
    QVERIFY( testItem.contexts().isEmpty() == false );
    QVERIFY( testItem.contexts().first() == QContactDetail::ContextHome );
    QVERIFY( first->mEdit->maxLength() == CNT_PHONENUMBER_EDITOR_MAXLENGTH );
    // test online account
    selectedIndex = 0;
    for ( int i(0); i < itemModel->rowCount(); i++ )
        {
        QModelIndex modelIndex = itemModel->index(i, 0);
        if ( itemModel->data(modelIndex, DetailSubType ).toString() == QContactOnlineAccount::SubTypeSip &&
             itemModel->data(modelIndex, DetailContext ).toString() == "" )
            {
            selectedIndex = i;
            break;
            }
        }
    first->mBox->setCurrentIndex( selectedIndex );
    QCOMPARE( boxSpy.count(), 1 );
    
    QContactOnlineAccount accountItem = modelItem->detail();
    QVERIFY( accountItem.subTypes().isEmpty() == false );
    QVERIFY( accountItem.subTypes().first() == QContactOnlineAccount::SubTypeSip );
    QVERIFY( accountItem.contexts().isEmpty() == true );
    QVERIFY( first->mEdit->maxLength() == CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH );
    }
