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

#include "cntbaseview.h"
#include "cntservicehandler.h"

#include "cntservicecontactfetchview.h"
#include "cntservicecontactselectionview.h"
#include "cntserviceeditview.h"
#include "cntservicecontactcardview.h"
#include "cntservicesubeditview.h"
#include "cntserviceassigncontactcardview.h"

/*!
Constructor
*/
CntServiceViewManager::CntServiceViewManager(CntMainWindow *mainWindow, CntViewParameters::ViewId defaultView, CntServiceHandler *aHandler):
    CntViewManager(mainWindow, defaultView),
    mServiceHandler(aHandler)
{
    connect(mServiceHandler, SIGNAL(launchFetch(const QString&, const QString&, const QString&)), 
            this, SLOT(launchFetch(const QString&, const QString&, const QString&)));
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

/*!
Launch fetch service view
*/
void CntServiceViewManager::launchFetch(const QString &title, const QString &action, const QString &filter)
{
    CntBaseView *view = getView(CntViewParameters::serviceContactFetchView);
    if (view)
    {
        //add view to main window
        addViewToWindow(view);
        view->setTitle(title);
        static_cast<CntServiceContactFetchView*>(view)->setActionFilter(action, filter);
        CntViewParameters viewParameters(CntViewParameters::noView);
        view->activateView(viewParameters);
    }
}

/*!
Launch editor service view
*/
void CntServiceViewManager::launchEditor(QContact contact)
{
    CntBaseView *view = getView(CntViewParameters::serviceEditView);
    if (view)
    {
        //add view to main window
        addViewToWindow(view);
        CntViewParameters viewParameters(CntViewParameters::noView);
        viewParameters.setSelectedContact(contact);   
        view->activateView(viewParameters);
    }
}

/*!
Launch contact selection service view (update existing contact with detail)
*/
void CntServiceViewManager::launchContactSelection(QContactDetail detail)
{
    CntBaseView *view = getView(CntViewParameters::serviceContactSelectionView);
    if (view)
    {
        //add view to main window
        addViewToWindow(view);
        CntViewParameters viewParameters(CntViewParameters::noView);
        viewParameters.setSelectedDetail(detail);   
        view->activateView(viewParameters);
    }
}

/*!
Launch contact card service view
*/
void CntServiceViewManager::launchContactCard(QContact contact)
{
    CntBaseView *view = getView(CntViewParameters::serviceContactCardView);
    if (view)
    {
        //add view to main window
        addViewToWindow(view);
        CntViewParameters viewParameters(CntViewParameters::noView);
        viewParameters.setSelectedContact(contact);   
        view->activateView(viewParameters);
    }
}

/*!
Launch assign (temporary) contact card service view
*/
void CntServiceViewManager::launchAssignContactCard(QContact contact, QContactDetail detail)
{
    CntBaseView *view = getView(CntViewParameters::serviceAssignContactCardView);
    if (view)
    {
        //add view to main window
        addViewToWindow(view);
        CntViewParameters viewParameters(CntViewParameters::noView);
        viewParameters.setSelectedContact(contact);   
        viewParameters.setSelectedDetail(detail);
        view->activateView(viewParameters);
    }
}

/*!
Create a view based on ID. \Return pointer to new object if success, 0 if not.
*/
CntBaseView *CntServiceViewManager::getView(CntViewParameters::ViewId id)
{
    CntBaseView* view(0);

    switch (id)
    {
    // contact fetch service view (fetching contacts from for example messaging)
    case CntViewParameters::serviceContactFetchView:
        {
        view = new CntServiceContactFetchView(mServiceHandler, this);
        break;
        }
    // contact selection service view (selecting contact to edit when updating existing contact)
    case CntViewParameters::serviceContactSelectionView:
        {
        view = new CntServiceContactSelectionView(mServiceHandler, this);
        break;
        }
    // communication launcher service view
    case CntViewParameters::serviceContactCardView:
        {
        view = new CntServiceContactCardView(mServiceHandler, this);
        break;
        }
    // communication launcher service view
    case CntViewParameters::serviceAssignContactCardView:
        {
        view = new CntServiceAssignContactCardView(mServiceHandler, this);
        break;
        }
    // contact edit service view (editing a contact from outside phonebook app)
    case CntViewParameters::serviceEditView:
        {
        view = new CntServiceEditView(mServiceHandler, this);
        break;
        }
    // edit view when editor opened from comm laucher service view
    case CntViewParameters::serviceSubEditView:
        {
        view = new CntServiceSubEditView(mServiceHandler, this);
        break;
        }
    default:
        {
        view = CntViewManager::getView(id);
        break;
        }
    }

    return view;
}

// end of file
