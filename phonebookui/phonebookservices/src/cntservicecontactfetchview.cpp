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

#include "cntservicescontact.h"
#include "cntservicehandler.h"

#include <hbmenu.h>

CntServiceContactFetchView::CntServiceContactFetchView(CntServiceHandler *aServiceHandler, CntViewManager *aViewManager, QGraphicsItem *aParent):
    CntBaseSelectionView(aViewManager,aParent),
    mServiceHandler(aServiceHandler)
{

}

CntServiceContactFetchView::~CntServiceContactFetchView()
{
}

void CntServiceContactFetchView::addMenuItems()
{
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:cancel");
    actions()->addActionsToMenu(menu());
    
    connect(actions()->baseAction("cnt:cancel"), SIGNAL(triggered()),
            this, SLOT (cancelFetch()));
}

void CntServiceContactFetchView::cancelFetch()
{
    connect(mServiceHandler, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    CntServicesContactList serviceList;
    mServiceHandler->completeFetch(serviceList);
}

void CntServiceContactFetchView::aboutToCloseView()
{
    CntServicesContactList serviceList;
    QModelIndexList temp = selectionModel()->selection().indexes();
    for(int i = 0; i < temp.count(); i++ )
    {
        QContact contact = contactModel()->contact(temp.at(i));
        CntServicesContact servicesContact;

        //get the name
        servicesContact.mDisplayName = contactManager()->synthesizedDisplayLabel(contact);

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

    connect(mServiceHandler, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    mServiceHandler->completeFetch(serviceList);
}

void CntServiceContactFetchView::activateView(const CntViewParameters &aArgs)
{
    // Set action filter
    QMap<int,QVariant> map = aArgs.parameters();
    QString filter = map.value(CntViewParameters::Filter).toString();
    QString action = map.value(CntViewParameters::Action).toString();
    setActionFilter(action, filter);
    
    // Set title of the view.
    QString title = map.value(CntViewParameters::Title).toString();
    setTitle(title);
    
    CntBaseSelectionView::activateView(aArgs);
}

void CntServiceContactFetchView::setActionFilter(QString action, QString filter)
{
    Q_UNUSED(filter);
    
    if (action == KCntActionSms)
    {
        QContactActionFilter actionFilter;
        actionFilter.setActionName("message");
        contactModel()->setFilterAndSortOrder(actionFilter);
    }
    else if (action == KCntActionCall)
    {
        QContactActionFilter actionFilter;
        actionFilter.setActionName("call");
        contactModel()->setFilterAndSortOrder(actionFilter);
    }
    else if (action == KCntActionEmail)
    {
        QContactActionFilter actionFilter;
        actionFilter.setActionName("email");
        contactModel()->setFilterAndSortOrder(actionFilter);
    }
    else
    {
        QContactDetailFilter filter;
        filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
        QString typeContact = QContactType::TypeContact;
        filter.setValue(typeContact);
        contactModel()->setFilterAndSortOrder(filter);
    }

    // hide my card if it's not set
    if (contactManager()->selfContactId() == 0)
    {
        contactModel()->showMyCard(false);
    }
}

// EOF
