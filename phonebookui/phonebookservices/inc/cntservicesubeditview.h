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

#ifndef CNTSERVICESUBEDITVIEW_H
#define CNTSERVICESUBEDITVIEW_H

#include <QObject>
#include "cnteditview.h"

class CntServiceHandler;

class CntServiceSubEditView : public CntEditView
{

Q_OBJECT

public:

    CntServiceSubEditView( CntServiceHandler *aServiceHandler );
    ~CntServiceSubEditView();

    int viewId() const { return serviceSubEditView; }

//public slots:
//    void aboutToCloseView();
//    int handleExecutedCommand(QString aCommand, const QContact &aContact);
//    void discardAllChanges();

private:
    CntServiceHandler *mServiceHandler;
    
};

#endif //CNTSERVICESUBEDITVIEW_H
// EOF
