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

#include "cntservicehandler.h"

#include "cntservicecontactfetchview.h"
#include "cntservicecontactselectionview.h"
#include "cntserviceeditview.h"
#include "cntservicecontactcardview.h"
#include "cntservicesubeditview.h"
#include "cntserviceassigncontactcardview.h"
#include "cntserviceviewfactory.h"

#include <hbabstractitemview.h>
/*!
Constructor
*/
CntServiceViewManager::CntServiceViewManager(HbMainWindow *mainWindow, CntServiceHandler *aHandler):
    CntDefaultViewManager(mainWindow),
    mServiceHandler(aHandler)
{
    
    setViewFactory( new CntServiceViewFactory(mServiceHandler) );
    connect(mServiceHandler, SIGNAL(launchFetch(const QString&, const QString&, const QString&)), 
            this, SLOT(handleFetch(const QString&, const QString&, const QString&)));
    
    // Handles signal with mode selected.
    connect(mServiceHandler, SIGNAL(launchFetchVerified(const QString&, const QString&, const QString&, const QString&)), 
            this, SLOT(launchFetch(const QString&, const QString&, const QString&, const QString&)));
    
    connect(mServiceHandler, SIGNAL(launchEditor(QContact)), this, SLOT(launchEditor(QContact)));
    connect(mServiceHandler, SIGNAL(launchContactSelection(QContactDetail)), this, SLOT(launchContactSelection(QContactDetail)));
    connect(mServiceHandler, SIGNAL(launchContactCard(QContact)), this, SLOT(launchContactCard(QContact)));
    connect(mServiceHandler, SIGNAL(launchAssignContactCard(QContact, QContactDetail)), this, 
            SLOT(launchAssignContactCard(QContact, QContactDetail)));
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

/*!
Launch fetch service view.
*/
void CntServiceViewManager::handleFetch(const QString &title, const QString &action, const QString &filter)
{
    launchFetch(title,action,filter);
}

/*!
Launch fetch service view. Uses a mode to determine fetching type.
*/
void CntServiceViewManager::launchFetch(const QString &title, const QString &action, const QString &filter, const QString &mode)
{
    CntViewParameters params;
    params.insert(EViewId, serviceContactFetchView);
    params.insert(CntServiceHandler::EAction, action);
    params.insert(CntServiceHandler::EFilter, filter);
    params.insert(CntServiceHandler::ETitle, title);

    if (!mode.compare(KCntSingleSelectionMode, Qt::CaseSensitive)) {
        params.insert(ESelectionMode, HbAbstractItemView::SingleSelection);
    }
    else {
        params.insert(ESelectionMode, HbAbstractItemView::MultiSelection);
    }
    changeView(params);
}

/*!
Launch editor service view
*/
void CntServiceViewManager::launchEditor(QContact contact)
{
    CntViewParameters params;
    params.insert(EViewId, serviceEditView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedContact, var);
    changeView(params);
}

/*!
Launch contact selection service view (update existing contact with detail)
*/
void CntServiceViewManager::launchContactSelection(QContactDetail detail)
{
    CntViewParameters params;
    params.insert(EViewId, serviceContactSelectionView);
    QVariant var;
    var.setValue(detail);
    params.insert(ESelectedDetail, var);
    changeView(params);
}

/*!
Launch contact card service view
*/
void CntServiceViewManager::launchContactCard(QContact contact)
{
    CntViewParameters params;
    params.insert(EViewId, serviceContactCardView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedContact, var);
    changeView(params);
}

/*!
Launch assign (temporary) contact card service view
*/
void CntServiceViewManager::launchAssignContactCard(QContact contact, QContactDetail detail)
{
    CntViewParameters params;
    params.insert(EViewId, serviceAssignContactCardView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedContact, var);
    QVariant varDetail;
    varDetail.setValue(detail);
    params.insert(ESelectedDetail, varDetail);
    changeView(params);
}

// end of file
