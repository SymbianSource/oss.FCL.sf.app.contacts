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

#include "cntonlineaccounteditorview.h"
#include "cnteditordataviewitem.h"
#include <hbinputstandardfilters.h>

CntOnlineAccountEditorView::CntOnlineAccountEditorView(CntViewManager *viewManager, QGraphicsItem *parent):
    CntBaseDetailEditorView(viewManager, parent)
{
    setHeader(hbTrId("Edit account details"));
}

CntOnlineAccountEditorView::~CntOnlineAccountEditorView()
{

}

void CntOnlineAccountEditorView::aboutToCloseView()
{
    for (int i = 0; i < mDetailList.count(); i++)
    {
        mContact->saveDetail(mDetailList.at(i));
        
        if (mDetailList.at(i)->value(QContactOnlineAccount::FieldAccountUri).isEmpty())
        {
            mContact->removeDetail(mDetailList.at(i));
        }
    }
    CntBaseDetailEditorView::aboutToCloseView();
}

QStandardItemModel *CntOnlineAccountEditorView::itemModel()
{
    QStandardItemModel *fieldModel = new QStandardItemModel();
    
    QString contextHome = QContactDetail::ContextHome;
    QString contextWork = QContactDetail::ContextWork;

    QString internetSubType = QContactOnlineAccount::SubTypeSipVoip;
    QString sipSubType = QContactOnlineAccount::SubTypeSip;
    QString shareVideoSubType = QContactOnlineAccount::SubTypeVideoShare;
    
    QStandardItem *internet = new QStandardItem;
    internet->setText(hbTrId("Internet call"));
    internet->setData(internetSubType, Qt::UserRole);
    internet->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(internet);
    QStandardItem *internethome = new QStandardItem;
    internethome->setText(hbTrId("Internet call (home)"));
    internethome->setData(internetSubType, Qt::UserRole);
    internethome->setData(contextHome, Qt::UserRole+1);
    internethome->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(internethome);
    QStandardItem *internetwork = new QStandardItem;
    internetwork->setText(hbTrId("Internet call (work)"));
    internetwork->setData(internetSubType, Qt::UserRole);
    internetwork->setData(contextWork, Qt::UserRole+1);
    internetwork->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(internetwork);
    
    QStandardItem *sip = new QStandardItem;
    sip->setText(hbTrId("SIP"));
    sip->setData(sipSubType, Qt::UserRole);
    sip->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(sip);
    QStandardItem *shareVideo = new QStandardItem;
    shareVideo->setText(hbTrId("Share Video"));
    shareVideo->setData(shareVideoSubType, Qt::UserRole);
    shareVideo->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(shareVideo);
    
    return fieldModel;
}

void CntOnlineAccountEditorView::initializeForm()
{
    QContactOnlineAccount *buttonAccount = new QContactOnlineAccount();
    mDetailList.append(buttonAccount);
    CntEditorDataModelItem *buttonItem = new CntEditorDataModelItem(*buttonAccount, hbTrId("txt_phob_button_add_account"),
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+2));
    formModel()->appendDataFormItem(buttonItem, formModel()->invisibleRootItem());
    
    QList<QContactOnlineAccount> details = mContact->details<QContactOnlineAccount>();
    
    if (details.count() > 0)
    {
        for(int i = 0;i < details.count();i++)
        {
            QContactOnlineAccount *account = new QContactOnlineAccount(details.at(i));
            mDetailList.append(account);
            CntEditorDataModelItem *item = new CntEditorDataModelItem(*account, QContactOnlineAccount::FieldAccountUri, itemModel(), 
                    HbDataFormModelItem::CustomItemBase);
            item->setTextFilter(HbInputLowerCaseFilter::instance());
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

void CntOnlineAccountEditorView::addDetail()
{
    QContactOnlineAccount *account = new QContactOnlineAccount();
    account->setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
    mDetailList.append(account);
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*account, QContactOnlineAccount::FieldAccountUri, itemModel(), 
            HbDataFormModelItem::CustomItemBase);
    item->setTextFilter(HbInputLowerCaseFilter::instance());
    formModel()->insertDataFormItem(formModel()->rowCount() - 1, item, formModel()->invisibleRootItem());
    
    // set focus to the added field's lineedit
    static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->indexFromItem(item)))->focusLineEdit();
}

