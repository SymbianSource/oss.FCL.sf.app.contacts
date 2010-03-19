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

#include "cntgroupeditorview.h"
#include "cnteditordataviewitem.h"

//#include "cntgroupeditordataviewitem.h"
#include <hbinputstandardfilters.h>

CntGroupEditorView::CntGroupEditorView(CntViewManager *viewManager, QGraphicsItem *parent):
    CntBaseDetailEditorView(viewManager, parent)
{
    setHeader(hbTrId("txt_phob_subtitle_edit_group_details"));
}

CntGroupEditorView::~CntGroupEditorView()
{

}

void CntGroupEditorView::aboutToCloseView()
{
    for (int i = 0; i < mDetailList.count(); i++)
    {
        mContact->saveDetail(mDetailList.at(i));
    }
    contactManager()->saveContact(mContact);
    CntBaseDetailEditorView::aboutToCloseView();
}



void CntGroupEditorView::initializeForm()
{
    QList<QContactName> nameDetails = mContact->details<QContactName>();
    QList<QContactPhoneNumber> numberDetails = mContact->details<QContactPhoneNumber>();

    QContactName *groupName = 0;
    QContactPhoneNumber *groupConfCallNumber = 0;

    if (nameDetails.count() > 0)
    {
        groupName = new QContactName(nameDetails.at(0));
    }
    else
    {
        groupName = new QContactName();
    }
    mDetailList.append(groupName);

    if (numberDetails.count() > 0)
    {
        groupConfCallNumber = new QContactPhoneNumber(numberDetails.at(0));
    }
    else
    {
        groupConfCallNumber = new QContactPhoneNumber();
    }
    mDetailList.append(groupConfCallNumber);

    // name of Group data item
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*groupName, QContactName::FieldCustomLabel, CNT_NAMES_EDITOR_MAXLENGTH,
                static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1), hbTrId("txt_phob_formlabel_group_name"));
        formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    // Group Conference Call CntEditorDataModelItem *callItem
    item = new CntEditorDataModelItem(*groupConfCallNumber, QContactPhoneNumber::FieldNumber,CNT_PHONENUMBER_EDITOR_MAXLENGTH,
                static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+1),hbTrId("Conference Call number"));
    item->setTextFilter(HbPhoneNumberFilter::instance());

    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
}
