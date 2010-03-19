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

#include "cntnoteeditorview.h"
#include "cnteditordataviewitem.h"

CntNoteEditorView::CntNoteEditorView(CntViewManager *viewManager, QGraphicsItem *parent):
    CntBaseDetailEditorView(viewManager, parent)
{
    setHeader(hbTrId("Edit note detail"));
}

CntNoteEditorView::~CntNoteEditorView()
{

}

void CntNoteEditorView::aboutToCloseView()
{
    for (int i = 0; i < mDetailList.count(); i++)
    {
        mContact->saveDetail(mDetailList.at(i));
        
        if (mDetailList.at(i)->value(QContactNote::FieldNote).isEmpty())
        {
            mContact->removeDetail(mDetailList.at(i));
        }
    }
    CntBaseDetailEditorView::aboutToCloseView();
}

void CntNoteEditorView::initializeForm()
{
    QContactNote *buttonNote = new QContactNote();
    mDetailList.append(buttonNote);
    CntEditorDataModelItem *buttonItem = new CntEditorDataModelItem(*buttonNote, hbTrId("Add note"),
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+2));
    formModel()->appendDataFormItem(buttonItem, formModel()->invisibleRootItem());
    
    QList<QContactNote> noteDetails = mContact->details<QContactNote>();
    
    if (noteDetails.count() > 0)
    {
        for(int i = 0;i < noteDetails.count();i++)
        {
            QContactNote *note = new QContactNote(noteDetails.at(i));
            mDetailList.append(note);
            CntEditorDataModelItem *item = new CntEditorDataModelItem(*note, QContactNote::FieldNote, CNT_NOTE_EDITOR_MAXLENGTH,
                    static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_note"));
            formModel()->insertDataFormItem(formModel()->rowCount() - 1, item, formModel()->invisibleRootItem());
            
            // check if this was the clicked item in edit view
            if (mParamString != "add" && mParamString.toInt() == i)
            {
                // set focus to the added field's lineedit
                static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->indexFromItem(item)))->focusLineEdit();
            }
        }
    }
    else
    {
        addDetail();
    }
}

void CntNoteEditorView::addDetail()
{
    QContactNote *note = new QContactNote();
    mDetailList.append(note);
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*note, QContactNote::FieldNote, CNT_NOTE_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_note"));
    formModel()->insertDataFormItem(formModel()->rowCount() - 1, item, formModel()->invisibleRootItem());
    
    // set focus to the added field's lineedit
    static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->indexFromItem(item)))->focusLineEdit();
}

