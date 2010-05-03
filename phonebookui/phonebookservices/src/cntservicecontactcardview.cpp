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

#include "cntservicecontactcardview.h"
#include "cntservicehandler.h"
#include <QCoreApplication>

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntServiceContactCardView::CntServiceContactCardView(CntServiceHandler *aServiceHandler) : 
    CntContactCardView(),
    mServiceHandler(aServiceHandler)
{
    connect(this, SIGNAL(backPressed()), this, SLOT(doShowPreviousView()));
}

/*!
Destructor
*/
CntServiceContactCardView::~CntServiceContactCardView()
{

}

/*!
Close the view (quits the service as well)
*/
void CntServiceContactCardView::doShowPreviousView()
{
    int result = -2;
    connect(mServiceHandler, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    mServiceHandler->completeEdit(result);
}

// end of file
