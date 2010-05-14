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

#ifndef QMOBCNTEMAILACTION_H
#define QMOBCNTEMAILACTION_H

#include "mobcntaction.h"

class MobCntEmailAction : public MobCntAction
{
    Q_OBJECT

public:
    MobCntEmailAction();
    ~MobCntEmailAction();
    
    QContactFilter contactFilter(const QVariant& value) const;
    bool isDetailSupported(const QContactDetail &detail, const QContact &contact = QContact()) const;
    QList<QContactDetail> supportedDetails(const QContact& contact) const;
    MobCntEmailAction* clone() const;
    void performAction();
};

#endif
