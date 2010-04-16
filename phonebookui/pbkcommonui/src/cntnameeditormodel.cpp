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
#include "cntnameeditormodel.h"
#include "cntdetailconst.h"
#include <qcontactname.h>
#include <qcontactnickname.h>

CntNameEditorModel::CntNameEditorModel(QContact* aContact) :
    CntDetailEditorModel(aContact)
{
    QList<QContactName> nameList = mContact->details<QContactName> ();
    if (nameList.isEmpty()) {
        QContactName emptyName;
        nameList.append(emptyName);
    }

    QList<QContactNickname> nickList = mContact->details<QContactNickname> ();
    if (nickList.isEmpty()) {
        QContactNickname nick;
        nickList.append(nick);
    }

    // Only one name and nickname is supported
    iName = nameList.first();
    iNick = nickList.first();

    HbDataFormModelItem::DataItemType text = HbDataFormModelItem::TextItem;
    HbDataFormModelItem* firstname = new HbDataFormModelItem(text, qtTrId("First name"));
    HbDataFormModelItem* lastname = new HbDataFormModelItem(text, qtTrId("Last name"));
    HbDataFormModelItem* middlename = new HbDataFormModelItem(text,qtTrId("Middle name"));
    HbDataFormModelItem* nickname = new HbDataFormModelItem(text, qtTrId("Nick name"));
    HbDataFormModelItem* prefix = new HbDataFormModelItem(text, qtTrId("Prefix"));
    HbDataFormModelItem* suffix = new HbDataFormModelItem(text, qtTrId("Suffix"));

    firstname->setContentWidgetData("text", iName.firstName());
    firstname->setContentWidgetData("maxLength", CNT_FIRSTNAME_MAXLENGTH);
    lastname->setContentWidgetData("text", iName.lastName());
    lastname->setContentWidgetData("maxLength", CNT_LASTNAME_MAXLENGTH);
    middlename->setContentWidgetData("text", iName.middleName());
    middlename->setContentWidgetData("maxLength", CNT_MIDDLENAME_MAXLENGTH);
    nickname->setContentWidgetData("text", iNick.nickname());
    nickname->setContentWidgetData("maxLength", CNT_NICKNAME_MAXLENGTH);
    prefix->setContentWidgetData("text", iName.prefix());
    prefix->setContentWidgetData("maxLength", CNT_PREFIX_MAXLENGTH);
    suffix->setContentWidgetData("text", iName.suffix());
    suffix->setContentWidgetData("maxLength", CNT_SUFFIX_MAXLENGTH);

    HbDataFormModelItem* root = invisibleRootItem();
    appendDataFormItem(firstname, root);
    appendDataFormItem(lastname, root);
    appendDataFormItem(middlename, root);
    appendDataFormItem(nickname, root);
    appendDataFormItem(prefix, root);
    appendDataFormItem(suffix, root);
}

CntNameEditorModel::~CntNameEditorModel()
{
}

void CntNameEditorModel::saveContactDetails()
{
    HbDataFormModelItem* root = invisibleRootItem();
    iName.setFirstName( root->childAt( 0 )->contentWidgetData("text").toString().trimmed() );
    iName.setLastName( root->childAt( 1 )->contentWidgetData("text").toString().trimmed() );
    iName.setMiddleName( root->childAt( 2 )->contentWidgetData("text").toString().trimmed() );
    iNick.setNickname( root->childAt( 3 )->contentWidgetData("text").toString().trimmed() );
    iName.setPrefix( root->childAt( 4 )->contentWidgetData("text").toString().trimmed() );
    iName.setSuffix( root->childAt( 5 )->contentWidgetData("text").toString().trimmed() );
    
    mContact->saveDetail( &iName );
    mContact->saveDetail( &iNick );
    
    // remove empty details
    if (iName.firstName().isEmpty() &&
            iName.lastName().isEmpty() &&
            iName.middleName().isEmpty() &&
            iName.prefix().isEmpty() &&
            iName.suffix().isEmpty())
            {
            mContact->removeDetail( &iName );
            }
    if(iNick.nickname().isEmpty())
        {
        mContact->removeDetail( &iNick );
        }
}

// End of File
