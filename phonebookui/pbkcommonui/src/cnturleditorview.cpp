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

#include "cnturleditorview.h"
#include "cnteditordataviewitem.h"
#include <hbinputstandardfilters.h>

CntUrlEditorView::CntUrlEditorView(CntViewManager *viewManager, QGraphicsItem *parent):
    CntBaseDetailEditorView(viewManager, parent)
{
    setHeader(hbTrId("Edit URL details"));
}

CntUrlEditorView::~CntUrlEditorView()
{

}

void CntUrlEditorView::aboutToCloseView()
{
    for (int i = 0; i < mDetailList.count(); i++)
    {
        mContact->saveDetail(mDetailList.at(i));
        
        if (mDetailList.at(i)->value(QContactUrl::FieldUrl).isEmpty())
        {
            mContact->removeDetail(mDetailList.at(i));
        }
    }
    CntBaseDetailEditorView::aboutToCloseView();
}

QStandardItemModel *CntUrlEditorView::itemModel()
{
    QStandardItemModel *fieldModel = new QStandardItemModel();
    
    QString contextHome = QContactDetail::ContextHome;
    QString contextWork = QContactDetail::ContextWork;
    
    QString urlField = QContactUrl::FieldUrl;
    
    QStandardItem *url = new QStandardItem;
    url->setText(hbTrId("txt_phob_dblist_url"));
    url->setData(urlField, Qt::UserRole);
    url->setData(CNT_URL_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(url);
    QStandardItem *urlhome = new QStandardItem;
    urlhome->setText(hbTrId("txt_phob_dblist_url_home"));
    urlhome->setData(urlField, Qt::UserRole);
    urlhome->setData(contextHome, Qt::UserRole+1);
    urlhome->setData(CNT_URL_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(urlhome);
    QStandardItem *urlwork = new QStandardItem;
    urlwork->setText(hbTrId("txt_phob_dblist_url_work"));
    urlwork->setData(urlField, Qt::UserRole);
    urlwork->setData(contextWork, Qt::UserRole+1);
    urlwork->setData(CNT_URL_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(urlwork);
    
    return fieldModel;
}

void CntUrlEditorView::initializeForm()
{
    QContactUrl *buttonUrl = new QContactUrl();
    mDetailList.append(buttonUrl);
    CntEditorDataModelItem *buttonItem = new CntEditorDataModelItem(*buttonUrl, hbTrId("Add URL"),
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+2));
    formModel()->appendDataFormItem(buttonItem, formModel()->invisibleRootItem());
    
    QList<QContactUrl> details = mContact->details<QContactUrl>();
    
    if (details.count() > 0)
    {
        for(int i = 0;i < details.count();i++)
        {
            QContactUrl *url = new QContactUrl(details.at(i));
            mDetailList.append(url);
            CntEditorDataModelItem *item = new CntEditorDataModelItem(*url, QContactUrl::FieldUrl, itemModel(), 
                    HbDataFormModelItem::CustomItemBase);
            item->setTextFilter(HbUrlFilter::instance());
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

void CntUrlEditorView::addDetail()
{
    QContactUrl *url = new QContactUrl();
    mDetailList.append(url);
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*url, QContactUrl::FieldUrl, itemModel(), 
            HbDataFormModelItem::CustomItemBase);
    item->setTextFilter(HbEmailAddressFilter::instance());
    formModel()->insertDataFormItem(formModel()->rowCount() - 1, item, formModel()->invisibleRootItem());
    
    // set focus to the added field's lineedit
    static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->indexFromItem(item)))->focusLineEdit();
}

