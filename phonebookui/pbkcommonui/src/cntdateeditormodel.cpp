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
#include "cntdateeditormodel.h"
#include "cntdetailmodelitem.h"
#include <hbdataformmodelitem.h>
#include <qcontactbirthday.h>
#include <qcontactanniversary.h>

CntDateEditorModel::CntDateEditorModel( QContact* aContact ) :
CntDetailEditorModel( aContact )
    {
    QList<QContactBirthday> bdList = mContact->details<QContactBirthday>();
    if ( bdList.isEmpty() )
        {
        QContactBirthday birthday;
        bdList << birthday;
        }
    
    QList<QContactAnniversary> anniversaryList = mContact->details<QContactAnniversary>();
    if ( anniversaryList.isEmpty() )
        {
        QContactAnniversary anniversary;
        anniversaryList << anniversary;
        }
        
    mBirthday = bdList.first();
    mAnniversary = anniversaryList.first();
    
    HbDataFormModelItem* root = invisibleRootItem();
    appendDataFormItem( new CntDetailModelItem( mBirthday, qtTrId("Birthday")), root );
    appendDataFormItem( new CntDetailModelItem( mAnniversary, qtTrId("Anniversary")), root );
    }

CntDateEditorModel::~CntDateEditorModel()
    {
    }

void CntDateEditorModel::saveContactDetails()
{
    HbDataFormModelItem* root = invisibleRootItem();
    // Birthday 
    CntDetailModelItem* birthday = static_cast<CntDetailModelItem*>( root->childAt(0) );
    mBirthday = birthday->detail();
    if ( mBirthday.date().isValid() )
        mContact->saveDetail( &mBirthday );
    
    // Anniversary
    CntDetailModelItem* anniversary = static_cast<CntDetailModelItem*>( root->childAt(1) );
    mAnniversary = anniversary->detail();
    if ( mAnniversary.originalDate().isValid() )
        mContact->saveDetail( &mAnniversary );
    
}
// End of File
