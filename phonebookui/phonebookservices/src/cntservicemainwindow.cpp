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

#include "cntservicemainwindow.h"
#include "cntservicehandler.h"
#include "cntserviceviewmanager.h"

CntServiceMainWindow::CntServiceMainWindow(CntServiceHandler *handler, QWidget *parent):
    CntMainWindow(parent,CntViewParameters::noView),
    mServiceHandler(handler),
    mServiceViewManager(0)
{   
    mServiceViewManager = new CntServiceViewManager(this, CntViewParameters::noView, mServiceHandler);
}

CntServiceMainWindow::~CntServiceMainWindow()
{
    delete mServiceViewManager;
    mServiceViewManager = 0;
    delete mServiceHandler;
    mServiceHandler = 0;
}

// end of file
