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
#include "t_noteeditor.h"
#include "cntdetailconst.h"
#include "cntdetailmodelitem.h"
#include "cntnoteeditormodel.h"
#include "cntnoteeditorviewitem.h"
#include <qcontact.h>
#include <qcontactnote.h>
#include <hbdataform.h>
#include <hbdataformmodelitem.h>
#include <hbabstractitemview.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

void T_NoteEditorTest::init()
    {
    mContact = new QContact();
    mForm = new HbDataForm();
    }

void T_NoteEditorTest::cleanup()
    {
    delete mForm;
    }
       
void T_NoteEditorTest::testNoteModelWithDefaultData()
    {
    CntNoteEditorModel* model = new CntNoteEditorModel( mContact );
    mForm->setModel( model );
    
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>(model->itemFromIndex( model->index(0,0) ));
    QVERIFY( item->type() == HbDataFormModelItem::CustomItemBase );
    QVERIFY( item->label() == QString("Note") );
    
    QContactNote note = item->detail();
    QVERIFY( note.note() == "" );
    
    delete model;
    }

void T_NoteEditorTest::testNoteModelWithData()
    {
    QContactNote* qNote = new QContactNote();
    qNote->setNote( "Ime parsaa" );
    mContact->saveDetail( qNote );
    
    CntNoteEditorModel* model = new CntNoteEditorModel( mContact );
    mForm->setModel( model );
        
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>(model->itemFromIndex( model->index(0,0) ));
    QVERIFY( item->type() == HbDataFormModelItem::CustomItemBase );
    QVERIFY( item->label() == QString("Note") );
        
    QContactNote note = item->detail();
    QVERIFY( note.note() == "Ime parsaa" );
    
    delete model;
    }
   
void T_NoteEditorTest::testNoteViewItem()
    {
    QContactNote* qNote = new QContactNote();
    qNote->setNote( "Ime parsaa" );
    mContact->saveDetail( qNote );
        
    CntNoteEditorModel* model = new CntNoteEditorModel( mContact );
    mForm->setModel( model, new CntNoteEditorViewItem() );
    
    CntNoteEditorViewItem* viewItem = static_cast<CntNoteEditorViewItem*>(mForm->itemByIndex( model->index(0,0) ));
    QVERIFY( viewItem->mEdit->text() == "Ime parsaa" );
    
    QSignalSpy spy( viewItem->mEdit, SIGNAL(textChanged(QString)) );
    viewItem->mEdit->setText( "diipadaapa" );
    QCOMPARE( spy.count(), 1 );
    
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>(model->itemFromIndex( model->index(0,0) ));
    QContactNote noteItem = item->detail();
    QVERIFY( noteItem.note() == "diipadaapa" );
    }

void T_NoteEditorTest::testNoteViewItemMaxLengths()
    {
    CntNoteEditorModel* model = new CntNoteEditorModel( mContact );
    mForm->setModel( model, new CntNoteEditorViewItem() );
        
    CntNoteEditorViewItem* viewItem = static_cast<CntNoteEditorViewItem*>(mForm->itemByIndex( model->index(0,0) ));
    QCOMPARE( viewItem->mEdit->maxLength(), CNT_NOTE_EDITOR_MAXLENGTH );
    }

void T_NoteEditorTest::testInsertAndSaveNote()
{
    CntNoteEditorModel* model = new CntNoteEditorModel( mContact );
    mForm->setModel( model, new CntNoteEditorViewItem() );
            
    model->insertDetailField();
    CntNoteEditorViewItem* viewItem = static_cast<CntNoteEditorViewItem*>( mForm->itemByIndex(model->index(0,0)) );
    viewItem->mEdit->setText( "Suut makiaksi" );
    model->saveContactDetails();
    
    QContact* contact = model->contact(); 
    QList<QContactNote> notes = contact->details<QContactNote>();
    QVERIFY( notes.size() == 1 ); // size should be 1 since the first (default) note is not saved becase it has no data in it.
    QVERIFY( notes.first().note() == "Suut makiaksi" ); 
}
// End of File
