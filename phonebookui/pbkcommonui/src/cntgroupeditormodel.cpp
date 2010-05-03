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
#include "cntgroupeditormodel.h"
#include "cntdetailconst.h"
#include <qcontactname.h>
#include <qcontactphonenumber.h>

CntGroupEditorModel::CntGroupEditorModel(QContact* aContact) :
    CntDetailEditorModel(aContact)
{
    QList<QContactName> nameDetails = mContact->details<QContactName>();
    if (nameDetails.isEmpty())
    {
        QContactName emptyName;
        nameDetails.append(emptyName);
    }
    
    QList<QContactPhoneNumber> numberDetails = mContact->details<QContactPhoneNumber>();
    if (numberDetails.isEmpty())
    {
        QContactPhoneNumber emptyNumber;
        numberDetails.append(emptyNumber);
    }
    
    iGroupName = nameDetails.first();
    iGroupConfCallNumber = numberDetails.first();
    
    HbDataFormModelItem::DataItemType text = HbDataFormModelItem::TextItem;
    HbDataFormModelItem* groupname = new HbDataFormModelItem(text, hbTrId("txt_phob_formlabel_group_name"));
    HbDataFormModelItem* conferenceNumber = new HbDataFormModelItem(text, hbTrId("Conference Call number"));
    
    groupname->setContentWidgetData("text", iGroupName.customLabel());
    conferenceNumber->setContentWidgetData("text", iGroupConfCallNumber.number());
    
    HbDataFormModelItem* root = invisibleRootItem();
    appendDataFormItem(groupname, root);
    appendDataFormItem(conferenceNumber, root);
}

CntGroupEditorModel::~CntGroupEditorModel()
{
}

void CntGroupEditorModel::saveContactDetails()
{
    HbDataFormModelItem* root = invisibleRootItem();
    
    iGroupName.setCustomLabel(root->childAt( 0 )->contentWidgetData("text").toString().trimmed());
    iGroupConfCallNumber.setNumber(root->childAt( 1 )->contentWidgetData("text").toString().trimmed());
    
    mContact->saveDetail( &iGroupName );
    mContact->saveDetail( &iGroupConfCallNumber );
    
    // remove empty details
    if (iGroupName.customLabel().isEmpty())
    {
        mContact->removeDetail( &iGroupName );
    }

    if(iGroupConfCallNumber.number().isEmpty())
    {
    mContact->removeDetail( &iGroupConfCallNumber );
    }
}

// End of File
