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

#include "cntservicecontactfetchview.h"

#include <cntservicescontact.h>
#include <cntlistmodel.h>
#include "cntserviceviewparams.h"

#include <hbmenu.h>
#include <hbview.h>
#include <hblistview.h>
#include <hbdocumentloader.h>
#include <hbaction.h>

#include <QCoreApplication>

CntServiceContactFetchView::CntServiceContactFetchView( CntAbstractServiceProvider& aServiceProvider ):
CntBaseSelectionView(),
mProvider( aServiceProvider )
    {
    HbAction* cancel = static_cast<HbAction*>( mDocument->findObject( "cnt:cancel" ) );
    mView->menu()->addAction( cancel );
    
    connect(cancel,  SIGNAL(triggered()), this, SLOT(cancelFetch()) );
    connect( this, SIGNAL(viewClosed()), this, SLOT(closeFetchView()) );
    connect( this, SIGNAL(viewOpened(CntAbstractViewManager*, const CntViewParameters)), this, SLOT(aboutToOpenView(CntAbstractViewManager*, const CntViewParameters)) );
}

CntServiceContactFetchView::~CntServiceContactFetchView()
{
}


void CntServiceContactFetchView::cancelFetch()
    {
    CntServicesContactList serviceList;
    QVariant variant;
    variant.setValue(serviceList);
    mProvider.CompleteServiceAndCloseApp(variant);
    }



void CntServiceContactFetchView::closeFetchView()
{
    CntServicesContactList serviceList;
    QContactManager* mgr = mMgr->contactManager(SYMBIAN_BACKEND);
    QModelIndexList temp = mListView->selectionModel()->selection().indexes();
    for(int i = 0; i < temp.count(); i++ )
    {
        QContact contact = mListModel->contact(temp.at(i));
        CntServicesContact servicesContact;

        //get the name
        servicesContact.mDisplayName = contact.displayLabel();

        //get the phonenumber
        QList<QContactPhoneNumber> phonenumbers = contact.details<QContactPhoneNumber>();
        if(phonenumbers.count() > 0)
        {
            servicesContact.mPhoneNumber = phonenumbers.first().number();
        }
        else
        {
            servicesContact.mPhoneNumber = "";
        }

        //get first email address
        QList<QContactEmailAddress> emailAddresses = contact.details<QContactEmailAddress>();
        if(emailAddresses.count() > 0)
        {
            servicesContact.mEmailAddress = emailAddresses.first().emailAddress();
        }
        //contact id
        servicesContact.mContactId = contact.localId();

        //append it to the list
        serviceList.append(servicesContact);
    }

    QVariant variant;
    variant.setValue(serviceList);
    mProvider.CompleteServiceAndCloseApp(variant);
}

void CntServiceContactFetchView::aboutToOpenView(CntAbstractViewManager* aMgr, const CntViewParameters aArgs)
{
    mMgr = aMgr;
    
    // Set title of the view.
    QString title = aArgs.value(KCntServiceViewParamTitle).toString();
    mView->setTitle(title);
    
    // Set action filter
    QString action = aArgs.value(ESelectedAction).toString();
    // ESelectedAction is defined in cntviewparams.h

    // Has never been implemented.
    //QString filterStr = aArgs.value(KCntServiceViewParamFilter).toString();
    // KCntServiceViewParamFilter is defined in cntserviceviewparams.h

    if (action == KCntActionSms)
        {
            QContactActionFilter actionFilter;
            actionFilter.setActionName("message");
            mListModel->setFilter(actionFilter);
        }
        else if (action == KCntActionCall)
        {
            QContactActionFilter actionFilter;
            actionFilter.setActionName("call");
            mListModel->setFilter(actionFilter);
        }
        else if (action == KCntActionEmail)
        {
            QContactActionFilter actionFilter;
            actionFilter.setActionName("email");
            mListModel->setFilter(actionFilter);
        }
        else
        {
            QContactDetailFilter filter;
            filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
            QString typeContact = QContactType::TypeContact;
            filter.setValue(typeContact);
            mListModel->setFilter(filter);
        }

        // hide my card if it's not set
        if ( mListModel->myCardId() == 0 )
        {
            mListModel->showMyCard( false );
        }
}

// EOF
