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

#include "cntnameseditorview.h"
#include "cnteditordataviewitem.h"

CntNamesEditorView::CntNamesEditorView(CntViewManager *viewManager, QGraphicsItem *parent):
    CntBaseDetailEditorView(viewManager, parent)
{
    setHeader(hbTrId("Edit name details"));
}

CntNamesEditorView::~CntNamesEditorView()
{

}

void CntNamesEditorView::aboutToCloseView()
{
    for (int i = 0; i < mDetailList.count(); i++)
    {
        mContact->saveDetail(mDetailList.at(i));
        if (mDetailList.at(i)->definitionName() == QContactNickname::DefinitionName && 
                mDetailList.at(i)->value(QContactNickname::FieldNickname).isEmpty())
        {
            mContact->removeDetail(mDetailList.at(i));
        }
        if (mDetailList.at(i)->definitionName() == QContactName::DefinitionName && 
                mDetailList.at(i)->value(QContactName::FieldFirst).isEmpty() &&
                mDetailList.at(i)->value(QContactName::FieldLast).isEmpty() &&
                mDetailList.at(i)->value(QContactName::FieldMiddle).isEmpty() &&
                mDetailList.at(i)->value(QContactName::FieldPrefix).isEmpty() &&
                mDetailList.at(i)->value(QContactName::FieldSuffix).isEmpty())
        {
            mContact->removeDetail(mDetailList.at(i));
        }
    }
    CntBaseDetailEditorView::aboutToCloseView();
}

void CntNamesEditorView::initializeForm()
{    
    QList<QContactName> nameDetails = mContact->details<QContactName>();
    QList<QContactNickname> nickDetails = mContact->details<QContactNickname>();
    QContactName *name = 0;
    QContactNickname *nick = 0;
    
    if (nameDetails.count() > 0)
    {
        name = new QContactName(nameDetails.at(0));
    }
    else
    {
        name = new QContactName();
    }
    mDetailList.append(name);
    
    if (nickDetails.count() > 0)
    {
        nick = new QContactNickname(nickDetails.at(0));
    }
    else
    {
        nick = new QContactNickname();
    }
    mDetailList.append(nick);
    
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*name, QContactName::FieldFirst, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_first_name"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    item = new CntEditorDataModelItem(*name, QContactName::FieldLast, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_last_name"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    item = new CntEditorDataModelItem(*name, QContactName::FieldMiddle, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_middle_name"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    item = new CntEditorDataModelItem(*nick, QContactNickname::FieldNickname, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_nick_name"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    item = new CntEditorDataModelItem(*name, QContactName::FieldPrefix, CNT_PREFIXSUFFIX_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_prefix"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    item = new CntEditorDataModelItem(*name, QContactName::FieldSuffix, CNT_PREFIXSUFFIX_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("Suffix"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());

    // set focus to first name field's lineedit
    static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->index(0, 0, QModelIndex())))->focusLineEdit();
}

