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

#include "cntservicesubeditview.h"

#include "cntservicehandler.h"

CntServiceSubEditView::CntServiceSubEditView(CntServiceHandler *aServiceHandler, CntViewManager *aViewManager, QGraphicsItem *aParent) : 
    CntEditView(aViewManager, aParent),
    mServiceHandler(aServiceHandler)
{   

}

CntServiceSubEditView::~CntServiceSubEditView()
{

}

/*!
Saves the contact
*/
void CntServiceSubEditView::aboutToCloseView()
{
    // save contact if there is one
    if (contact())
    {
        contactManager()->saveContact(contact());
    }
    
    CntViewParameters viewParameters(CntViewParameters::serviceContactCardView);
    viewParameters.setSelectedContact(*contact());   
    viewManager()->onActivateView(viewParameters);
}

/*!
Cancel all changes made and return to comm launcher view
*/
void CntServiceSubEditView::discardAllChanges()
{
    QContact oldContact = contactManager()->contact(contact()->localId());
    CntViewParameters viewParameters(CntViewParameters::serviceContactCardView);
    viewParameters.setSelectedContact(oldContact);   
    viewManager()->onActivateView(viewParameters);
}

/*
Handle signals emitted from CntCommands, only used for delete command for now.
*/
int CntServiceSubEditView::handleExecutedCommand(QString aCommand, const QContact &aContact)
{
    Q_UNUSED(aContact);
    int result = -1;
    if (aCommand == "delete")
    {
        connect(mServiceHandler, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        mServiceHandler->completeEdit(result);
        result = 0;
    }
    return result;
}

// EOF
