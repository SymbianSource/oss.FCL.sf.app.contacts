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

#ifndef CNTGROUPEDITORMODEL_H_
#define CNTGROUPEDITORMODEL_H_
#include "cntdetaileditormodel.h"
#include <qtcontacts.h>
#include "qtpbkglobal.h"

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntGroupEditorModel : public CntDetailEditorModel
    {
    Q_OBJECT
    
public:
    CntGroupEditorModel( QContact* aContact );
    ~CntGroupEditorModel();
    
    void saveContactDetails();
    
private:

    QContactName iGroupName;
    QContactPhoneNumber iGroupConfCallNumber;
    };
#endif /* CNTGROUPEDITORMODEL_H_ */
