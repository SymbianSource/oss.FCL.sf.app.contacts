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

#ifndef CNTDATEEDITORMODEL_H_
#define CNTDATEEDITORMODEL_H_

#include "cntdetaileditormodel.h"
#include <qtcontacts.h>
#include "qtpbkglobal.h"

QTM_BEGIN_NAMESPACE
class QContact;
class QContactAnniversary;
class QContactBirthday;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntDateEditorModel : public CntDetailEditorModel
    {
    Q_OBJECT
    
public:
    CntDateEditorModel( QContact* aContact );
    ~CntDateEditorModel();
    
    void saveContactDetails();
    
private:
    QContactAnniversary mAnniversary;
    QContactBirthday mBirthday;
    };
#endif /* CNTDATEEDITORMODEL_H_ */
