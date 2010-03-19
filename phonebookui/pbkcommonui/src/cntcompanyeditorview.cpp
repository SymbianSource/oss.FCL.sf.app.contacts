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

#include "cntcompanyeditorview.h"
#include "cnteditordataviewitem.h"

CntCompanyEditorView::CntCompanyEditorView(CntViewManager *viewManager, QGraphicsItem *parent):
    CntBaseDetailEditorView(viewManager, parent)
{
    setHeader(hbTrId("Edit company details"));
}

CntCompanyEditorView::~CntCompanyEditorView()
{

}

void CntCompanyEditorView::aboutToCloseView()
{
    for (int i = 0; i < mDetailList.count(); i++)
    {
        mContact->saveDetail(mDetailList.at(i));
        
        if (mDetailList.at(i)->value(QContactOrganization::FieldName).isEmpty() &&
                mDetailList.at(i)->value(QContactOrganization::FieldTitle).isEmpty() &&
                mDetailList.at(i)->value(QContactOrganization::FieldDepartment).isEmpty() &&
                mDetailList.at(i)->value(QContactOrganization::FieldAssistantName).isEmpty())
        {
            mContact->removeDetail(mDetailList.at(i));
        }
    }
    CntBaseDetailEditorView::aboutToCloseView();
}

void CntCompanyEditorView::initializeForm()
{    
    QList<QContactOrganization> companyDetails = mContact->details<QContactOrganization>();
    QContactOrganization *company = 0;
       
    if (companyDetails.count() > 0)
    {
        company = new QContactOrganization(companyDetails.at(0));
    }
    else
    {
        company = new QContactOrganization();
    }
    mDetailList.append(company);
    
    
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*company, QContactOrganization::FieldName, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_company"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    item = new CntEditorDataModelItem(*company, QContactOrganization::FieldTitle, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_job_title"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    item = new CntEditorDataModelItem(*company, QContactOrganization::FieldDepartment, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_department"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    item = new CntEditorDataModelItem(*company, QContactOrganization::FieldAssistantName, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_assistant"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    // set focus to the correct field
    if (mParamString == "company")
    {
        static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->index(0, 0, QModelIndex())))->focusLineEdit();
    }
    else
    {
        static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->index(3, 0, QModelIndex())))->focusLineEdit();
    }
}

