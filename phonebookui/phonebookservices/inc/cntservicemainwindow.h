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

#ifndef CNTSERVICEMAINWINDOW_H
#define CNTSERVICEMAINWINDOW_H

#include <QObject>
#include "cntmainwindow.h"

class CntServiceHandler;
class CntServiceViewManager;

class CntServiceMainWindow : public CntMainWindow
{
    Q_OBJECT

public:
    CntServiceMainWindow(CntServiceHandler *handler, QWidget *parent=0);
    ~CntServiceMainWindow();

private:
    CntServiceHandler       *mServiceHandler;
    CntServiceViewManager   *mServiceViewManager;
};

#endif // CNTSERVICEMAINWINDOW_H
