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

#ifndef CNTMODELPROVIDER_H
#define CNTMODELPROVIDER_H

#include <QObject>
#include <mobcntmodel.h>
#include "qtpbkglobal.h"


QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

// CLASS DECLARATION
class QTPBK_EXPORT CntModelProvider : public QObject
{
    Q_OBJECT

public:
    static CntModelProvider* instance();
    void release();
    int referenceCount();

public:
    MobCntModel   *contactModel();
    QContactManager *contactManager();
    QContactManager *contactSimManager();
    ~CntModelProvider();

private:
    CntModelProvider();

private:
    // counter for handles to this object
    int             mRefCount;
    // Own: Pointer to the model object and managers
    MobCntModel     *mContactModel;
    QContactManager *mContactManager;
    QContactManager *mContactSimManager;
};

#endif      // CNTMODELPROVIDER_H

// End of File
