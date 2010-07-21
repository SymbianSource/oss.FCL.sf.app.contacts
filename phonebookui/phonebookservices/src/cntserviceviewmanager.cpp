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

#include "cntserviceviewmanager.h"
#include "cntserviceviewfactory.h"

/*!
Constructor
*/
CntServiceViewManager::CntServiceViewManager( HbMainWindow *mainWindow,
    CntAbstractServiceProvider& aProvider ):
    CntDefaultViewManager(mainWindow)
    {
    // ownership of the factory is given to the base class.
    setViewFactory( new CntServiceViewFactory( aProvider ) );
    }

/*!
Destructor
*/
CntServiceViewManager::~CntServiceViewManager()
{
/*Remove all views*/
}

void CntServiceViewManager::closeApp()
{
    // quit happens when returnValueDelivered signal is returned
    // from service handler, see constructor.
}

// end of file
