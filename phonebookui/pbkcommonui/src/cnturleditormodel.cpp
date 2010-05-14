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
#include "cnturleditormodel.h"
#include "cntdetailmodelitem.h"

#include <qcontacturl.h>

CntUrlEditorModel::CntUrlEditorModel(QContact* aContact) :
    CntDetailEditorModel(aContact)
{
    QList<QContactUrl> urlList = mContact->details<QContactUrl>();
    if (urlList.isEmpty()) {
        QContactUrl url;
        url.setSubType(QContactUrl::SubTypeHomePage);
        urlList.append(url);
    }

    foreach( QContactUrl contactUrl, urlList ) {
        CntDetailModelItem* item = new CntDetailModelItem(contactUrl);
        appendDataFormItem(item, invisibleRootItem());
    }
}

CntUrlEditorModel::~CntUrlEditorModel()
{
}

void CntUrlEditorModel::insertDetailField()
{
    QContactUrl url;
    url.setSubType(QContactUrl::SubTypeHomePage);
        
    appendDataFormItem( new CntDetailModelItem(url), invisibleRootItem() );
}

void CntUrlEditorModel::saveContactDetails()
{
    HbDataFormModelItem* root = invisibleRootItem();

    int count(root->childCount());
    for (int i(0); i < count; i++) {
        CntDetailModelItem* detail = static_cast<CntDetailModelItem*> (root->childAt(i));
        QContactDetail url = detail->detail();
        mContact->saveDetail( &url );
        
        if ( url.value(QContactUrl::FieldUrl).isEmpty() )
        {
            mContact->removeDetail( &url );
        }
    }
}
// End of File

