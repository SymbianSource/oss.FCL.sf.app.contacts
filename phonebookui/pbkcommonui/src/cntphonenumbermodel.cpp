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
#include "cntdetailorderinghelper.h"
#include <hbdataformmodelitem.h>
#include <QDebug>

CntPhoneNumberModel::CntPhoneNumberModel( QContact* aContact, QObject* aParent ) : 
CntDetailEditorModel( aContact )
    {
    setParent( aParent );
    
    QList<QContactDetail> all;
    foreach ( QContactDetail detail, CntDetailOrderingHelper::getOrderedSupportedPhoneNumbers(*mContact) )
    {
        all.append( detail );
        mNumberList.append( detail );
    }
    
    foreach ( QContactDetail detail, CntDetailOrderingHelper::getOrderedSupportedOnlineAccounts(*mContact) )
    {
        all.append( detail );
        mNumberList.append( detail );
    }
    
    // if there's no details, add 
    if ( all.isEmpty() )
        {
        mMobileTemplate.setSubTypes( QContactPhoneNumber::SubTypeMobile );
        all.append( mMobileTemplate );

        mLandlineTemplate.setSubTypes( QContactPhoneNumber::SubTypeLandline );
        all.append( mLandlineTemplate );
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
            QContactPhoneNumber number = detail;
            if ( !mNumberList.contains(detail) )
            {
                mContact->saveDetail( &detail );
            }
            
            if ( detail.value(QContactPhoneNumber::FieldNumber).isEmpty() )
            {
                // this won't change the pointer value if the detail does not exists in contact.
                // But, this is the situation in wk20...
                mContact->removeDetail( &detail );
            }
        }
        
        if ( detail.definitionName() == QContactOnlineAccount::DefinitionName )
        {
            QContactOnlineAccount account = detail;
            if ( !mNumberList.contains(detail) && !account.accountUri().isEmpty() )
            {
                mContact->saveDetail( &detail );
            }
            
            if ( account.accountUri().isEmpty() )
            {
                mContact->removeDetail( &detail );
            }
        }
    }
}

CntPhoneNumberModel::~CntPhoneNumberModel()
    {
    }

QContactDetail CntPhoneNumberModel::detail() const 
{
    QListIterator<QContactPhoneNumber> numberList(mContact->details<QContactPhoneNumber>());
    numberList.toBack(); // go through backwards, so the newest item will be returned
    while ( numberList.hasPrevious() )
    {
        QContactPhoneNumber number = numberList.previous();
        if ( !number.value(QContactPhoneNumber::FieldNumber).isEmpty() )
        {
            return number;
        }
    }
    return QContactPhoneNumber(); // return empty address if none found
}

// End of File
