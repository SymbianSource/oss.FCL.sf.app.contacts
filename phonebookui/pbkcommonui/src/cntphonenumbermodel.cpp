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
#include "cntphonenumbermodel.h"
#include "cntdetailconst.h"
#include "cntdetailmodelitem.h"
#include <hbdataformmodelitem.h>
#include <QDebug>

CntPhoneNumberModel::CntPhoneNumberModel( QContact* aContact, QObject* aParent ) : 
CntDetailEditorModel( aContact )
    {
    setParent( aParent );
    
    QList<QContactDetail> all;
    foreach ( QContactDetail detail, mContact->details<QContactPhoneNumber>() )
        all.append( detail );
    
    foreach ( QContactDetail detail, mContact->details<QContactOnlineAccount>() )
        all.append( detail );
    
    // if there's no details, add 
    if ( all.isEmpty() )
        {
        QContactPhoneNumber number;
        number.setSubTypes( QContactPhoneNumber::SubTypeMobile );
        
        all.append( number );
        }
            
    HbDataFormModelItem* root = invisibleRootItem();
    // add all phone numbers & online accounts to model
    foreach ( QContactDetail contactDetail, all )
        {
        CntDetailModelItem* item = new CntDetailModelItem( contactDetail );
        appendDataFormItem(item, root );
        }
    }

void CntPhoneNumberModel::insertDetailField()
{
    QContactPhoneNumber number;
    number.setSubTypes( QContactPhoneNumber::SubTypeMobile );
    
    appendDataFormItem( new CntDetailModelItem(number), invisibleRootItem() );
}

void CntPhoneNumberModel::saveContactDetails()
{
    HbDataFormModelItem* root = invisibleRootItem();
    int count( root->childCount() );
    for ( int i = 0; i < count; i++ ) {
        CntDetailModelItem* item = static_cast<CntDetailModelItem*>( root->childAt(i) );
        QContactDetail detail = item->detail();
        
        if ( detail.definitionName() == QContactPhoneNumber::DefinitionName )
        {
            QContactPhoneNumber phonenumber = detail;
            if ( phonenumber.number().length() > 0 ) {
                mContact->saveDetail( &phonenumber );
            }
        }
        else if ( detail.definitionName() == QContactOnlineAccount::DefinitionName )
        {
            QContactOnlineAccount account = detail;
            if ( account.accountUri().length() > 0 ) {
                mContact->saveDetail( &account );
            }
        }
        else {
            /* should never be here */
            qWarning() << "Unknown: " << detail.definitionName(); 
        }
    }
}

CntPhoneNumberModel::~CntPhoneNumberModel()
    {
    }

// End of File
