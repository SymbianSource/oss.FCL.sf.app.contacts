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

#include "cntservicecontactselectionview.h"

#include "cntservicehandler.h"

CntServiceContactSelectionView::CntServiceContactSelectionView(CntServiceHandler *aServiceHandler, CntViewManager *aViewManager, 
        QGraphicsItem *aParent, HbAbstractItemView::SelectionMode newMode):
    CntBaseSelectionView(aViewManager, aParent, newMode),
    mServiceHandler(aServiceHandler)
{

}

CntServiceContactSelectionView::~CntServiceContactSelectionView()
{

}

void CntServiceContactSelectionView::onListViewActivated(const QModelIndex &aIndex)
{
    CntViewParameters viewParameters(CntViewParameters::serviceEditView);
    QContact contact = contactModel()->contact(aIndex);
    contact.saveDetail(&mDetail);
    viewParameters.setSelectedContact(contact);   
    viewManager()->onActivateView(viewParameters);
}

void CntServiceContactSelectionView::aboutToCloseView()
{
    connect(mServiceHandler, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    mServiceHandler->completeEdit(0);
}

void CntServiceContactSelectionView::activateView(const CntViewParameters &viewParameters)
{
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    QString typeContact = QContactType::TypeContact;
    filter.setValue(typeContact);
    contactModel()->setFilterAndSortOrder(filter);
    // hide my card if it's not set
    if (contactManager()->selfContactId() == 0)
    {
        contactModel()->showMyCard(false);
    }
    mDetail = viewParameters.selectedDetail();
}

// EOF
