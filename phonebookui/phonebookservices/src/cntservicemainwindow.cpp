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
#include "cntviewnavigator.h"

CntServiceMainWindow::CntServiceMainWindow(CntServiceHandler *handler, QWidget *parent):
    CntMainWindow(parent, noView),
    mServiceHandler(handler),
    mServiceViewManager(0)
{   
    CntViewNavigator* navigator = new CntViewNavigator( this );
    navigator->addException( serviceEditView, noView );

    mServiceViewManager = new CntServiceViewManager(this, mServiceHandler);
    mServiceViewManager->setViewNavigator( navigator );
}

CntServiceMainWindow::~CntServiceMainWindow()
{
    delete mServiceViewManager;
    mServiceViewManager = 0;
    delete mServiceHandler;
    mServiceHandler = 0;
}

// end of file
