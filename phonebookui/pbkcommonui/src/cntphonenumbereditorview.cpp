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

#include "cntphonenumbereditorview.h"
#include "cnteditordataviewitem.h"
#include <hbinputstandardfilters.h>

CntPhoneNumberEditorView::CntPhoneNumberEditorView(CntViewManager *viewManager, QGraphicsItem *parent):
    CntBaseDetailEditorView(viewManager, parent)
{
    setHeader(hbTrId("Edit phone number details"));
}

CntPhoneNumberEditorView::~CntPhoneNumberEditorView()
{

}

void CntPhoneNumberEditorView::aboutToCloseView()
{
    for (int i = 0; i < mDetailList.count(); i++)
    {
        mContact->saveDetail(mDetailList.at(i));
        
        if (mDetailList.at(i)->value(QContactPhoneNumber::FieldNumber).isEmpty())
        {
            mContact->removeDetail(mDetailList.at(i));
        }
    }
    CntBaseDetailEditorView::aboutToCloseView();
}

QStandardItemModel *CntPhoneNumberEditorView::itemModel()
{
    QStandardItemModel *fieldModel = new QStandardItemModel();
    
    QString contextHome = QContactDetail::ContextHome;
    QString contextWork = QContactDetail::ContextWork;

    QString mobileSubType = QContactPhoneNumber::SubTypeMobile;
    QString landLineSubType = QContactPhoneNumber::SubTypeLandline;
    QString faxSubType = QContactPhoneNumber::SubTypeFacsimile;
    QString pagerSubType = QContactPhoneNumber::SubTypePager;
    QString carPhoneSubType = QContactPhoneNumber::SubTypeCar;
    QString dtmfSubType = QContactPhoneNumber::SubTypeDtmfMenu;
    QString assistantSubType = QContactPhoneNumber::SubTypeAssistant;
    
    QStandardItem *mobile = new QStandardItem;
    mobile->setText(hbTrId("txt_phob_dblist_mobile"));
    mobile->setData(mobileSubType, Qt::UserRole);
    mobile->setData(":/icons/qgn_prop_nrtyp_mobile.svg", Qt::UserRole+2);
    mobile->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(mobile);
    QStandardItem *mobilehome = new QStandardItem;
    mobilehome->setText(hbTrId("txt_phob_dblist_mobile_home"));
    mobilehome->setData(mobileSubType, Qt::UserRole);
    mobilehome->setData(contextHome, Qt::UserRole+1);
    mobilehome->setData(":/icons/qgn_prop_nrtyp_mobile.svg", Qt::UserRole+2);
    mobilehome->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(mobilehome);
    QStandardItem *mobilework = new QStandardItem;
    mobilework->setText(hbTrId("txt_phob_dblist_mobile_work"));
    mobilework->setData(mobileSubType, Qt::UserRole);
    mobilework->setData(contextWork, Qt::UserRole+1);
    mobilework->setData(":/icons/qgn_prop_nrtyp_mobile.svg", Qt::UserRole+2);
    mobilework->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(mobilework);

    QStandardItem *land = new QStandardItem;
    land->setText(hbTrId("Landline"));
    land->setData(landLineSubType, Qt::UserRole);
    land->setData(":/icons/qgn_prop_nrtyp_phone.svg", Qt::UserRole+2);
    land->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(land);
    QStandardItem *landhome = new QStandardItem;
    landhome->setText(hbTrId("Landline (home)"));
    landhome->setData(landLineSubType, Qt::UserRole);
    landhome->setData(contextHome, Qt::UserRole+1);
    landhome->setData(":/icons/qgn_prop_nrtyp_phone.svg", Qt::UserRole+2);
    landhome->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(landhome);
    QStandardItem *landwork = new QStandardItem;
    landwork->setText(hbTrId("Landline (work)"));
    landwork->setData(landLineSubType, Qt::UserRole);
    landwork->setData(contextWork, Qt::UserRole+1);
    landwork->setData(":/icons/qgn_prop_nrtyp_phone.svg", Qt::UserRole+2);
    landwork->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(landwork);
    
    QStandardItem *fax = new QStandardItem;
    fax->setText(hbTrId("txt_phob_dblist_fax"));
    fax->setData(faxSubType, Qt::UserRole);
    fax->setData(":/icons/qgn_prop_nrtyp_fax.svg", Qt::UserRole+2);
    fax->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(fax);
    QStandardItem *faxhome = new QStandardItem;
    faxhome->setText(hbTrId("txt_phob_dblist_fax_home"));
    faxhome->setData(faxSubType, Qt::UserRole);
    faxhome->setData(contextHome, Qt::UserRole+1);
    faxhome->setData(":/icons/qgn_prop_nrtyp_fax.svg", Qt::UserRole+2);
    faxhome->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(faxhome);
    QStandardItem *faxwork = new QStandardItem;
    faxwork->setText(hbTrId("txt_phob_dblist_fax_work"));
    faxwork->setData(faxSubType, Qt::UserRole);
    faxwork->setData(contextWork, Qt::UserRole+1);
    faxwork->setData(":/icons/qgn_prop_nrtyp_fax.svg", Qt::UserRole+2);
    faxwork->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(faxwork);
    
    QStandardItem *pager = new QStandardItem;
    pager->setText(hbTrId("Pager number"));
    pager->setData(pagerSubType, Qt::UserRole);
    pager->setData(":/icons/qgn_prop_nrtyp_pager.svg", Qt::UserRole+2);
    pager->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(pager);
    QStandardItem *carPhone = new QStandardItem;
    carPhone->setText(hbTrId("Car phone"));
    carPhone->setData(carPhoneSubType, Qt::UserRole);
    carPhone->setData(":/icons/qgn_prop_nrtyp_car.svg", Qt::UserRole+2);
    carPhone->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(carPhone);
    QStandardItem *assistant = new QStandardItem;
    assistant->setText(hbTrId("Assistant number"));
    assistant->setData(assistantSubType, Qt::UserRole);
    assistant->setData(":/icons/qgn_prop_nrtyp_assistant.svg", Qt::UserRole+2);
    assistant->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(assistant);
    QStandardItem *dtmf = new QStandardItem;
    dtmf->setText(hbTrId("DTMF"));
    dtmf->setData(dtmfSubType, Qt::UserRole);
    dtmf->setData(":/icons/qgn_prop_empty.svg", Qt::UserRole+2);
    dtmf->setData(CNT_DTMF_EDITOR_MAXLENGTH, Qt::UserRole+3);
    fieldModel->appendRow(dtmf);
    
    return fieldModel;
}

void CntPhoneNumberEditorView::initializeForm()
{
    QContactPhoneNumber *buttonNumber = new QContactPhoneNumber();
    mDetailList.append(buttonNumber);
    CntEditorDataModelItem *buttonItem = new CntEditorDataModelItem(*buttonNumber, hbTrId("txt_phob_button_add_number"),
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+2));
    formModel()->appendDataFormItem(buttonItem, formModel()->invisibleRootItem());
    
    QList<QContactPhoneNumber> details = mContact->details<QContactPhoneNumber>();
    
    if (details.count() > 0)
    {
        for(int i = 0;i < details.count();i++)
        {
            QContactPhoneNumber *number = new QContactPhoneNumber(details.at(i));
            mDetailList.append(number);
            CntEditorDataModelItem *item = new CntEditorDataModelItem(*number, QContactPhoneNumber::FieldNumber, itemModel(), 
                    HbDataFormModelItem::CustomItemBase);
            item->setTextFilter(HbPhoneNumberFilter::instance());
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

void CntPhoneNumberEditorView::addDetail()
{
    QContactPhoneNumber *number = new QContactPhoneNumber();
    number->setSubTypes(QContactPhoneNumber::SubTypeMobile);
    mDetailList.append(number);
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*number, QContactPhoneNumber::FieldNumber, itemModel(), 
            HbDataFormModelItem::CustomItemBase);
    item->setTextFilter(HbPhoneNumberFilter::instance());
    formModel()->insertDataFormItem(formModel()->rowCount() - 1, item, formModel()->invisibleRootItem());
    
    // set focus to the added field's lineedit
    static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->indexFromItem(item)))->focusLineEdit();
}

