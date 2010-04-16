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
#include "t_urleditor.h"
#include "cntdetailconst.h"
#include "cnturleditormodel.h"
#include "cnturleditorviewitem.h"
#include "cntdetailmodelitem.h"

#include <qcontacturl.h>
#include <hbdataform.h>
#include <hbcombobox.h>
#include <hblineedit.h>
#include <QtTest/QtTest>
#include <QTestEventList>

void T_UrlEditorTest::init()
    {
    mContact = new QContact();
    mForm = new HbDataForm();
    }

void T_UrlEditorTest::cleanup()
    {
    delete mForm;
    }
    
void T_UrlEditorTest::testUrlModelWithDefaultData()
    {
    CntUrlEditorModel* model = new CntUrlEditorModel( mContact );
    // now there should be one default phonenumber with subtype Mobile.
    HbDataFormModelItem* root = model->invisibleRootItem();
    QVERIFY( root->childCount() == 1 );
        
    CntDetailModelItem* defaultItem = static_cast<CntDetailModelItem*>(root->childAt( 0 ));
    QContactDetail mobile = defaultItem->detail();
    QVERIFY( mobile.definitionName() == QContactUrl::DefinitionName );
   
    QContactUrl url = static_cast<QContactUrl>( mobile );
    QVERIFY( url.url() == "" );
    delete model;
    }

void T_UrlEditorTest::testUrlModelWithData()
    {
    QContactUrl* url = new QContactUrl();
    url->setUrl( "www.nokia.com" );
    mContact->saveDetail( url );
       
    CntUrlEditorModel* model = new CntUrlEditorModel( mContact );
    HbDataFormModelItem* root = model->invisibleRootItem();
    QVERIFY( root->childCount() == 1 );
    
    CntDetailModelItem* defaultItem = static_cast<CntDetailModelItem*>(root->childAt( 0 ));
    QContactUrl urlAddress = defaultItem->detail();
    QVERIFY( urlAddress.definitionName() == QContactUrl::DefinitionName );
    QVERIFY( urlAddress.url() == "www.nokia.com" );
       
    delete model;
    }

void T_UrlEditorTest::testUrlViewItem()
    {
    QContactUrl* url = new QContactUrl();
    url->setContexts( QContactDetail::ContextWork );
    url->setUrl( "www.nokia.com" );
    mContact->saveDetail( url );
            
    CntUrlEditorModel* model = new CntUrlEditorModel( mContact );
    CntUrlEditorViewItem* item = new CntUrlEditorViewItem();
    mForm->setModel( model, item );
        
    // select the first item from the view
    QModelIndex index = model->index(0, 0 );
    CntUrlEditorViewItem* first = static_cast<CntUrlEditorViewItem*>(mForm->itemByIndex( index ));
        
    // the given landline number should be selected in combobox
    int currentIndex = first->mBox->currentIndex();          
    QString text = first->mBox->itemText( currentIndex );
    QVERIFY( text == "URL (work)" );
    
    QString value = first->mEdit->text();
    QVERIFY( value == "www.nokia.com" );
    
    // changing the text should affect to current detail
    QSignalSpy spy( first->mEdit, SIGNAL(textChanged(QString)));
    first->mEdit->setText( "www.apple.com (sorry)" );
    QCOMPARE( spy.count(), 1 );
        
    CntDetailModelItem* modelItem = static_cast<CntDetailModelItem*>(model->itemFromIndex( index ));
    QContactUrl testItem = modelItem->detail();
    QString n = testItem.url();
    QVERIFY( n == "www.apple.com (sorry)" );
    }

void T_UrlEditorTest::testUrlViewItemComboChange()
    {
    QContactUrl* url = new QContactUrl();
    url->setContexts( QContactDetail::ContextHome );
    url->setUrl( "www.nokia.com" );
    mContact->saveDetail( url );
               
    CntUrlEditorModel* model = new CntUrlEditorModel( mContact );
    CntUrlEditorViewItem* item = new CntUrlEditorViewItem();
           
    mForm->setModel( model, item );
           
    // select the first item from the view
    QModelIndex index = model->index(0, 0 );
    CntUrlEditorViewItem* first = static_cast<CntUrlEditorViewItem*>(mForm->itemByIndex( index ));

    QAbstractItemModel* boxModel = first->mBox->model();
    int selectedIndex = 0;
    for ( int i(0); i < boxModel->rowCount(); i++ )
        {
        QModelIndex modelIndex = boxModel->index( i, 0 );
        QString cntx = boxModel->data(modelIndex, DetailContext).toString();
        if ( cntx == QContactUrl::ContextWork )
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
    QContactUrl urlAddress = modelItem->detail();
    QVERIFY( urlAddress.contexts().isEmpty() == false );
    QVERIFY( urlAddress.contexts().first() == QContactUrl::ContextWork );
    }

void T_UrlEditorTest::testUrlViewItemLineEdit()
    {
    QContact* c = new QContact();
    QContactUrl* url = new QContactUrl();
    url->setContexts( QContactDetail::ContextHome );
    url->setUrl( "www.nokia.com" );
    c->saveDetail( url );
                   
    CntUrlEditorModel* model = new CntUrlEditorModel( c );
    CntUrlEditorViewItem* item = new CntUrlEditorViewItem();
               
    HbDataForm* form = new HbDataForm();
    form->setModel( model, item );
               
    // select the first item from the view
    QModelIndex index = model->index(0, 0 );
    CntUrlEditorViewItem* first = static_cast<CntUrlEditorViewItem*>(form->itemByIndex( index ));
    QVERIFY( first->mEdit->maxLength() == CNT_URL_EDITOR_MAXLENGTH );
    
    QSignalSpy spy( first->mEdit, SIGNAL(textChanged(QString)) );
    first->mEdit->setText( "www.phonebook.com" );
    QCOMPARE( spy.count(), 1 );
    
    CntDetailModelItem* modelItem = static_cast<CntDetailModelItem*>(model->itemFromIndex(index) );
    QContactUrl urlItem = modelItem->detail();
    QVERIFY( urlItem.url() == "www.phonebook.com" );
    }
// End of File
