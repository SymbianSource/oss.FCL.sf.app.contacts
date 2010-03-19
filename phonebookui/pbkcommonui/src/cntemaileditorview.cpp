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

#include "cntemaileditorview.h"
#include "cnteditordataviewitem.h"
#include <hbinputstandardfilters.h>

CntEmailEditorView::CntEmailEditorView(CntViewManager *viewManager, QGraphicsItem *parent):
    CntBaseDetailEditorView(viewManager, parent)
{
    setHeader(hbTrId("Edit email details"));
}

CntEmailEditorView::~CntEmailEditorView()
{

}

void CntEmailEditorView::aboutToCloseView()
{
    for (int i = 0; i < mDetailList.count(); i++)
    {
        mContact->saveDetail(mDetailList.at(i));
        
        if (mDetailList.at(i)->value(QContactEmailAddress::FieldEmailAddress).isEmpty())
        {
            mContact->removeDetail(mDetailList.at(i));
        }
    }
    CntBaseDetailEditorView::aboutToCloseView();
}

QStandardItemModel *CntEmailEditorView::itemModel()
{
    QStandardItemModel *fieldModel = new QStandardItemModel();
    
    QString contextHome = QContactDetail::ContextHome;
    QString contextWork = QContactDetail::ContextWork;
    
    QString emailField = QContactEmailAddress::FieldEmailAddress;
    
    QStandardItem *email = new QStandardItem;
    email->setText(hbTrId("txt_phob_dblist_email"));
    email->setData(emailField, Qt::UserRole);
    email->setData(":/icons/qgn_prop_nrtyp_email.svg", Qt::UserRole+2);
    email->setData(CNT_EMAIL_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(email);
    QStandardItem *emailhome = new QStandardItem;
    emailhome->setText(hbTrId("txt_phob_dblist_email_home"));
    emailhome->setData(emailField, Qt::UserRole);
    emailhome->setData(contextHome, Qt::UserRole+1);
    emailhome->setData(":/icons/qgn_prop_nrtyp_email.svg", Qt::UserRole+2);
    emailhome->setData(CNT_EMAIL_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(emailhome);
    QStandardItem *emailwork = new QStandardItem;
    emailwork->setText(hbTrId("txt_phob_dblist_email_work"));
    emailwork->setData(emailField, Qt::UserRole);
    emailwork->setData(contextWork, Qt::UserRole+1);
    emailwork->setData(":/icons/qgn_prop_nrtyp_email.svg", Qt::UserRole+2);
    emailwork->setData(CNT_EMAIL_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(emailwork);
    
    return fieldModel;
}

void CntEmailEditorView::initializeForm()
{
    QContactEmailAddress *buttonEmail = new QContactEmailAddress();
    mDetailList.append(buttonEmail);
    CntEditorDataModelItem *buttonItem = new CntEditorDataModelItem(*buttonEmail, hbTrId("txt_phob_list_add_email_address"),
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+2));
    formModel()->appendDataFormItem(buttonItem, formModel()->invisibleRootItem());
    
    QList<QContactEmailAddress> details = mContact->details<QContactEmailAddress>();
    
    if (details.count() > 0)
    {
        for(int i = 0;i < details.count();i++)
        {
            QContactEmailAddress *email = new QContactEmailAddress(details.at(i));
            mDetailList.append(email);
            CntEditorDataModelItem *item = new CntEditorDataModelItem(*email, QContactEmailAddress::FieldEmailAddress, itemModel(), 
                    HbDataFormModelItem::CustomItemBase);
            item->setTextFilter(HbEmailAddressFilter::instance());
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

void CntEmailEditorView::addDetail()
{
    QContactEmailAddress *email = new QContactEmailAddress();
    mDetailList.append(email);
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*email, QContactEmailAddress::FieldEmailAddress, itemModel(), 
            HbDataFormModelItem::CustomItemBase);
    item->setTextFilter(HbEmailAddressFilter::instance());
    formModel()->insertDataFormItem(formModel()->rowCount() - 1, item, formModel()->invisibleRootItem());
    
    // set focus to the added field's lineedit
    static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->indexFromItem(item)))->focusLineEdit();
}

