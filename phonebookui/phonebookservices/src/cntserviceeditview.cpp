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

#include "cntserviceeditview.h"
#include "cntservicehandler.h"
#include <QCoreApplication>

CntServiceEditView::CntServiceEditView(CntServiceHandler *aServiceHandler ) : CntEditView(),
mServiceHandler(aServiceHandler)
{   
    connect( this, SIGNAL(contactUpdated(bool)), this, SLOT(doContactUpdated(bool)) );
    connect( this, SIGNAL(contactRemoved(bool)), this, SLOT(doContactRemoved(bool)) );
    connect( this, SIGNAL(changesDiscarded()), this, SLOT(doChangesDiscarded()) );
}

CntServiceEditView::~CntServiceEditView()
{
}

void CntServiceEditView::doContactUpdated(bool aSuccess)
{
    connect(mServiceHandler, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    int retValue = aSuccess ? 1 : 0;
    mServiceHandler->completeEdit(retValue);
}

void CntServiceEditView::doContactRemoved(bool aSuccess)
{
    connect(mServiceHandler, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    int retValue = aSuccess ? -1 : 0;
    mServiceHandler->completeEdit(retValue);
}

void CntServiceEditView::doChangesDiscarded()
{
    connect(mServiceHandler, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    mServiceHandler->completeEdit(0);
}

//
///*!
//Saves the contact
//*/
//void CntServiceEditView::aboutToCloseView()
//{
//    int result = 0;
//    // save contact if there is one
//    if (contact() && !contact()->isEmpty())
//    {
//        bool isSaved = contactManager()->saveContact(contact());
//        if (isSaved)
//        {
//            result = 1;
//        }
//    }
//    
//    connect(mServiceHandler, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
//    mServiceHandler->completeEdit(result);
//}
//
///*!
//Cancel all changes made and return to the application editor was opened from
//*/
//void CntServiceEditView::discardAllChanges()
//{
//    connect(mServiceHandler, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
//    mServiceHandler->completeEdit(0);
//}
//
///*
//Handle signals emitted from CntCommands, only used for delete command for now.
//*/
//int CntServiceEditView::handleExecutedCommand(QString aCommand, const QContact &aContact)
//{
//    Q_UNUSED(aContact);
//    int result=-1;
//    if (aCommand == "delete")
//    {
//        connect(mServiceHandler, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
//        mServiceHandler->completeEdit(result);
//        result = 0;
//    }
//    return result;
//}

// EOF
