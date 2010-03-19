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

#include "cntfamilydetaileditorview.h"
#include "cnteditordataviewitem.h"

CntFamilyDetailEditorView::CntFamilyDetailEditorView(CntViewManager *viewManager, QGraphicsItem *parent):
    CntBaseDetailEditorView(viewManager, parent)
{
    setHeader(hbTrId("txt_phob_subtitle_edit_family_details"));
}

CntFamilyDetailEditorView::~CntFamilyDetailEditorView()
{

}

void CntFamilyDetailEditorView::aboutToCloseView()
{
    for (int i = 0; i < mDetailList.count(); i++)
    {
        mContact->saveDetail(mDetailList.at(i));
        
        if (mDetailList.at(i)->value(QContactFamily::FieldSpouse).isEmpty() &&
                mDetailList.at(i)->value(QContactFamily::FieldChildren).isEmpty())
        {
            mContact->removeDetail(mDetailList.at(i));
        }
    }
    CntBaseDetailEditorView::aboutToCloseView();
}

void CntFamilyDetailEditorView::initializeForm()
{    
    QList<QContactFamily> familyDetails = mContact->details<QContactFamily>();
    QContactFamily *family = 0;
       
    if (familyDetails.count() > 0)
    {
        family = new QContactFamily(familyDetails.at(0));
    }
    else
    {
        family = new QContactFamily();
    }
    mDetailList.append(family);
    
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*family, QContactFamily::FieldSpouse, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_spouse"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    item = new CntEditorDataModelItem(*family, QContactFamily::FieldChildren, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_children"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    // set focus to the correct field
    if (mParamString == "spouse")
    {
        static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->index(0, 0, QModelIndex())))->focusLineEdit();
    }
    else
    {
        static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->index(1, 0, QModelIndex())))->focusLineEdit();
    }
}

