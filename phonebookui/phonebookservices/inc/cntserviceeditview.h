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

#ifndef CNTSERVICEEDITVIEW_H
#define CNTSERVICEEDITVIEW_H

#include <QObject>
#include "cnteditview.h"

class CntServiceHandler;

class CntServiceEditView : public CntEditView
{

Q_OBJECT

public:

    CntServiceEditView(CntServiceHandler *aServiceHandler );
    ~CntServiceEditView();

    int viewId() const { return serviceEditView; }

private slots:
    void doContactUpdated(bool aSuccess);
    void doContactRemoved(bool aSuccess);
    void doChangesDiscarded();
    
//public slots:
//    void aboutToCloseView();
//    int handleExecutedCommand(QString aCommand, const QContact &aContact);
//    void discardAllChanges();

private:
    CntServiceHandler *mServiceHandler;
    
};

#endif //CNTSERVICEEDITVIEW_H
// EOF
