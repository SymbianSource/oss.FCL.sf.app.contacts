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

#include "cntaddresseditorview.h"
#include "cnteditordataviewitem.h"
#include <hbinputstandardfilters.h>

CntAddressEditorView::CntAddressEditorView(CntViewManager *viewManager, QGraphicsItem *parent):
    CntBaseDetailEditorView(viewManager, parent)
{
    setHeader(hbTrId("txt_phob_subtitle_edit_address_details"));
}

CntAddressEditorView::~CntAddressEditorView()
{

}

void CntAddressEditorView::aboutToCloseView()
{
    for (int i = 0; i < mDetailList.count(); i++)
    {
        mContact->saveDetail(mDetailList.at(i));
        
        if (mDetailList.at(i)->value(QContactAddress::FieldPostOfficeBox).isEmpty() &&
                mDetailList.at(i)->value(QContactAddress::FieldStreet).isEmpty() &&
                mDetailList.at(i)->value(QContactAddress::FieldPostcode).isEmpty() &&
                mDetailList.at(i)->value(QContactAddress::FieldLocality).isEmpty() &&
                mDetailList.at(i)->value(QContactAddress::FieldRegion).isEmpty() &&
                mDetailList.at(i)->value(QContactAddress::FieldCountry).isEmpty())
        {
            mContact->removeDetail(mDetailList.at(i));
        }
    }
    CntBaseDetailEditorView::aboutToCloseView();
}

void CntAddressEditorView::initializeForm()
{    
    QList<QContactAddress> addressDetails = mContact->details<QContactAddress>();
    QContactAddress *address = 0;
    QContactAddress *addressHome = 0;
    QContactAddress *addressWork = 0;
    
    // we allow one address detail of each type: no context, home and work
    if (addressDetails.count() > 0)
    {
        for(int i = 0;i < addressDetails.count();i++)
        {
            QString context;
            if (addressDetails.at(i).contexts().count())
            {
                context = addressDetails.at(i).contexts().at(0);
            }
            
            if (context.isEmpty() && address == 0)
            {
                address = new QContactAddress(addressDetails.at(i));
                mDetailList.append(address);
            }
            else if (context == QContactDetail::ContextHome && addressHome == 0)
            {
                addressHome = new QContactAddress(addressDetails.at(i));
                mDetailList.append(addressHome);
            }
            else if (context == QContactDetail::ContextWork && addressWork == 0)
            {
                addressWork = new QContactAddress(addressDetails.at(i));
                mDetailList.append(addressWork);
            }
        }
    }
    else
    {
        address = new QContactAddress();
        mDetailList.append(address);
        
        addressHome = new QContactAddress();
        addressHome->setContexts(QContactDetail::ContextHome);
        mDetailList.append(addressHome);
        
        addressWork = new QContactAddress();
        addressWork->setContexts(QContactDetail::ContextWork);
        mDetailList.append(addressWork);
    }
    
    // check if any of the address details isn't set yet
    if (address == 0)
    {
        address = new QContactAddress();
        mDetailList.append(address);
    }
    if (addressHome == 0)
    {
        addressHome = new QContactAddress();
        addressHome->setContexts(QContactDetail::ContextHome);
        mDetailList.append(addressHome);
    }
    if (addressWork == 0)
    {
        addressWork = new QContactAddress();
        addressWork->setContexts(QContactDetail::ContextWork);
        mDetailList.append(addressWork);
    }

    HbDataFormModelItem *addressType = 
        formModel()->appendDataFormGroup(hbTrId("txt_phob_list_address"), formModel()->invisibleRootItem());

    //Details for addressDefault
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*address, QContactAddress::FieldStreet, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_street"));
    formModel()->appendDataFormItem(item, addressType);

    item = new CntEditorDataModelItem(*address, QContactAddress::FieldPostOfficeBox, CNT_POBOX_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("P.O. Box"));
    item->setTextFilter(HbPhoneNumberFilter::instance());
    formModel()->appendDataFormItem(item, addressType);

    item = new CntEditorDataModelItem(*address, QContactAddress::FieldPostcode, CNT_POBOX_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_post_code"));
    item->setTextFilter(HbPhoneNumberFilter::instance());
    formModel()->appendDataFormItem(item, addressType);

    item = new CntEditorDataModelItem(*address, QContactAddress::FieldLocality, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_city"));
    formModel()->appendDataFormItem(item, addressType);

    item = new CntEditorDataModelItem(*address, QContactAddress::FieldRegion, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_province"));
    formModel()->appendDataFormItem(item, addressType);

    item = new CntEditorDataModelItem(*address, QContactAddress::FieldCountry, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_country"));
    formModel()->appendDataFormItem(item, addressType);

    //Details for addressHome
    HbDataFormModelItem *addressHomeType = 
        formModel()->appendDataFormGroup(hbTrId("txt_phob_subhead_address_home"), formModel()->invisibleRootItem());
    
    item = new CntEditorDataModelItem(*addressHome, QContactAddress::FieldStreet, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("Street (home)"));
    formModel()->appendDataFormItem(item, addressHomeType);

    item = new CntEditorDataModelItem(*addressHome, QContactAddress::FieldPostOfficeBox, CNT_POBOX_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("P.O. Box (home)"));
    item->setTextFilter(HbPhoneNumberFilter::instance());
    formModel()->appendDataFormItem(item, addressHomeType);

    item = new CntEditorDataModelItem(*addressHome, QContactAddress::FieldPostcode, CNT_POBOX_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("Postal code (home)"));
    item->setTextFilter(HbPhoneNumberFilter::instance());
    formModel()->appendDataFormItem(item, addressHomeType);

    item = new CntEditorDataModelItem(*addressHome, QContactAddress::FieldLocality, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("City (home)"));
    formModel()->appendDataFormItem(item, addressHomeType);

    item = new CntEditorDataModelItem(*addressHome, QContactAddress::FieldRegion, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("Region (home)"));
    formModel()->appendDataFormItem(item, addressHomeType);

    item = new CntEditorDataModelItem(*addressHome, QContactAddress::FieldCountry, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("Country (home)"));
    formModel()->appendDataFormItem(item, addressHomeType);

    //Details for addressWork
    HbDataFormModelItem *addressWorkType = 
        formModel()->appendDataFormGroup(hbTrId("txt_phob_subhead_address_work"), formModel()->invisibleRootItem());

    item = new CntEditorDataModelItem(*addressWork, QContactAddress::FieldStreet, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("Street (work)"));
    formModel()->appendDataFormItem(item, addressWorkType);

    item = new CntEditorDataModelItem(*addressWork, QContactAddress::FieldPostOfficeBox, CNT_POBOX_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("P.O. Box (work)"));
    item->setTextFilter(HbPhoneNumberFilter::instance());
    formModel()->appendDataFormItem(item, addressWorkType);

    item = new CntEditorDataModelItem(*addressWork, QContactAddress::FieldPostcode, CNT_POBOX_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("Postal code (work)"));
    item->setTextFilter(HbPhoneNumberFilter::instance());
    formModel()->appendDataFormItem(item, addressWorkType);

    item = new CntEditorDataModelItem(*addressWork, QContactAddress::FieldLocality, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("City (work)"));
    formModel()->appendDataFormItem(item, addressWorkType);

    item = new CntEditorDataModelItem(*addressWork, QContactAddress::FieldRegion, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("Region (work)"));
    formModel()->appendDataFormItem(item, addressWorkType);

    item = new CntEditorDataModelItem(*addressWork, QContactAddress::FieldCountry, CNT_NAMES_EDITOR_MAXLENGTH,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("Country (work)"));
    formModel()->appendDataFormItem(item, addressWorkType);
    
    // Define which address type is expanded, by default normal address is opened
    if (mParamString.isEmpty())
    {
        dataForm()->setExpanded(formModel()->indexFromItem(addressType), true);
        // set focus to street detail's lineedit
        static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->indexFromItem(addressType->childAt(0))))->focusLineEdit();
    }
    else if (mParamString == QContactDetail::ContextHome)
    {
        dataForm()->setExpanded(formModel()->indexFromItem(addressHomeType), true);
        // set focus to street detail's lineedit
        static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->indexFromItem(addressHomeType->childAt(0))))->focusLineEdit();
    }
    else if (mParamString == QContactDetail::ContextWork)
    {
        dataForm()->setExpanded(formModel()->indexFromItem(addressWorkType), true);
        // set focus to street detail's lineedit
        static_cast<CntEditorDataViewItem*>(dataForm()->itemByIndex(formModel()->indexFromItem(addressWorkType->childAt(0))))->focusLineEdit();
    }
}

