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

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntServiceContactCardView::CntServiceContactCardView(CntServiceHandler *aServiceHandler, CntViewManager *viewManager, QGraphicsItem *parent) : 
    CntContactCardView(viewManager, parent),
    mServiceHandler(aServiceHandler)
{

}

/*!
Destructor
*/
CntServiceContactCardView::~CntServiceContactCardView()
{

}

/*!
Launch contact editor
*/
void CntServiceContactCardView::editContact()
{
    CntViewParameters viewParameters(CntViewParameters::serviceSubEditView);
    viewParameters.setSelectedContact(*mContact);   
    viewManager()->onActivateView(viewParameters);
}

/*!
Close the view (quits the service as well)
*/
void CntServiceContactCardView::aboutToCloseView()
{
    int result = -2;
    connect(mServiceHandler, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    mServiceHandler->completeEdit(result);
}

// end of file
