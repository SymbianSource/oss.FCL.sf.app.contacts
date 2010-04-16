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

#ifndef CNTPDETAILMODELITEM_H_
#define CNTDETAILMODELITEM_H_

#include <hbdataformmodelitem.h>
#include <qtcontacts.h>
#include "qtpbkglobal.h"

QTM_BEGIN_NAMESPACE
class QContactDetail;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntDetailModelItem : public HbDataFormModelItem
    {
public:
    CntDetailModelItem( QContactDetail aDetail );
    CntDetailModelItem( QContactDetail aDetail, QString aLabel );
    ~CntDetailModelItem();
    
public:
    void setDetail( QContactDetail aDetail );
    QContactDetail detail();
    
private:
    QContactDetail mDetail;
    };
#endif /* CNTDETAILMODELITEM_H_ */
