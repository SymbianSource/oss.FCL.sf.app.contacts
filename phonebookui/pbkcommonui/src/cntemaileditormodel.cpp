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
#include <qcontactemailaddress.h>

CntEmailEditorModel::CntEmailEditorModel( QContact* aContact ) :
CntDetailEditorModel( aContact )
    {
    QList<QContactEmailAddress> addr = mContact->details<QContactEmailAddress>();
    if ( addr.isEmpty() )
        {
        QContactEmailAddress newAddr;
        addr.append( newAddr );
        }
    
    foreach ( QContactEmailAddress email, addr )
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
        QString email = address.emailAddress();
        if ( email.length() > 0 ) {
            mContact->saveDetail( &address );
        }
    }
}

// End of File
