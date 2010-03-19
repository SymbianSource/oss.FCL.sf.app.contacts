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

#include "cntdateeditorview.h"

CntDateEditorView::CntDateEditorView(CntViewManager *viewManager, QGraphicsItem *parent):
    CntBaseDetailEditorView(viewManager, parent)
{
    setHeader(hbTrId("txt_phob_subtitle_edit_date"));
}

CntDateEditorView::~CntDateEditorView()
{

}

void CntDateEditorView::aboutToCloseView()
{
    for (int i = 0; i < mDetailList.count(); i++)
    {
        mContact->saveDetail(mDetailList.at(i));
        
        if (mDetailList.at(i)->definitionName() == QContactBirthday::DefinitionName &&
                mDetailList.at(i)->variantValue(QContactBirthday::FieldBirthday).toString().isEmpty())
        {
            mContact->removeDetail(mDetailList.at(i));
        }
        
        if (mDetailList.at(i)->definitionName() == QContactAnniversary::DefinitionName &&
                mDetailList.at(i)->variantValue(QContactAnniversary::FieldOriginalDate).toString().isEmpty())
        {
            mContact->removeDetail(mDetailList.at(i));
        }
    }
    CntBaseDetailEditorView::aboutToCloseView();
}

void CntDateEditorView::initializeForm()
{    
    QList<QContactBirthday> birthdayDetails = mContact->details<QContactBirthday>();
    QList<QContactAnniversary> anniversaryDetails = mContact->details<QContactAnniversary>();
    
    QContactBirthday *birthday = 0;
    QContactAnniversary *anniversary = 0;
    
    if (birthdayDetails.count() > 0)
    {
        birthday = new QContactBirthday(birthdayDetails.at(0));
    }
    else
    {
        birthday = new QContactBirthday();
    }
    mDetailList.append(birthday);
   
    if (anniversaryDetails.count() > 0)
    {
        anniversary = new QContactAnniversary(anniversaryDetails.at(0));
    }
    else
    {
        anniversary = new QContactAnniversary();
    }
    mDetailList.append(anniversary);
    
    
    CntEditorDataModelItem *item = new CntEditorDataModelItem(*birthday, QContactBirthday::FieldBirthday, 0,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+3), hbTrId("txt_phob_formlabel_birthday"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
    
    item = new CntEditorDataModelItem(*anniversary, QContactAnniversary::FieldOriginalDate, 0,
            static_cast<HbDataFormModelItem::DataItemType>(HbDataFormModelItem::CustomItemBase+3), hbTrId("txt_phob_formlabel_anniversary"));
    formModel()->appendDataFormItem(item, formModel()->invisibleRootItem());
}

