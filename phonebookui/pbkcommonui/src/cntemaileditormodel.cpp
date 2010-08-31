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
#include "cntemaileditormodel.h"
#include "cntdetailmodelitem.h"
#include "cntdetailorderinghelper.h"
#include <qcontactemailaddress.h>

CntEmailEditorModel::CntEmailEditorModel( QContact* aContact ) :
CntDetailEditorModel( aContact )
    {
    mAddressList = CntDetailOrderingHelper::getOrderedEmailAccounts(*mContact);
    if ( mAddressList.isEmpty() )
        {
        QContactEmailAddress newAddr;
        mAddressList.append( newAddr );
        }
    
    foreach ( QContactEmailAddress email, mAddressList )
        {
        CntDetailModelItem* item = new CntDetailModelItem( email );
        appendDataFormItem( item, invisibleRootItem() );
        }
    }

CntEmailEditorModel::~CntEmailEditorModel()
{
}

void CntEmailEditorModel::insertDetailField()
{
    // don't put this one in mAddressList since that list is used
    // to check if there's new items
    QContactEmailAddress newAddr;
    CntDetailModelItem* item = new CntDetailModelItem( newAddr );
    appendDataFormItem( item, invisibleRootItem() );
}

void CntEmailEditorModel::saveContactDetails()
{
    HbDataFormModelItem* root = invisibleRootItem();
    int count( root->childCount() );
    for ( int i(0); i < count; i++ ) {
        CntDetailModelItem* item = static_cast<CntDetailModelItem*>( root->childAt(i) );
        QContactEmailAddress address = item->detail();
        if ( !mAddressList.contains(address) && !address.emailAddress().isEmpty() )
        {
            mContact->saveDetail( &address );
            // this is not currently necessary since view is closed after this call.
            mAddressList.append( address );
        }
        if ( address.value(QContactEmailAddress::FieldEmailAddress).isEmpty() ) {
            mContact->removeDetail( &address );
        }
    }
}

QContactDetail CntEmailEditorModel::detail() const
{
    QListIterator<QContactEmailAddress> addrList(mContact->details<QContactEmailAddress>());
    addrList.toBack(); // go through backwards, so the newest item will be returned
    while ( addrList.hasPrevious() )
    {
        QContactEmailAddress address = addrList.previous();
        if ( !address.value(QContactEmailAddress::FieldEmailAddress).isEmpty() )
        {
            return address;
        }
    }
    return QContactEmailAddress(); // return empty address if none found
}

// End of File
