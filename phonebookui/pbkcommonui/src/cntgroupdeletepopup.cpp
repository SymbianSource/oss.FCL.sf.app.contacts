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

#include "cntgroupdeletepopup.h"

#include <hbgroupbox.h>
#include <hbaction.h>
#include <hblistview.h>
#include <qtcontacts.h>

CntGroupDeletePopup::CntGroupDeletePopup(QContactManager *manager, QGraphicsItem *parent):
    HbDialog(parent),
    mListView(0),
    mContactManager(manager),
    mModel(0)
{
    mModel = new CntGroupDeletePopupModel(mContactManager, this);
}

CntGroupDeletePopup::~CntGroupDeletePopup()
{
    delete mModel;
    mModel = 0;
}

void CntGroupDeletePopup::populateListOfGroup()
{
    HbGroupBox *headingLabel = new HbGroupBox(this);   
    headingLabel->setHeading(hbTrId("txt_phob_opt_delete_groups"));
    
    setHeadingWidget(headingLabel);
    
    mListView = new HbListView(this);
    
    mModel->initializeGroupsList();

    //Get the index of the contacts
    // Set the select option for those contacts in the selectionModel
    mListView->setModel(mModel);
    // set the listview to multiSelection mode, this will bring MarkAll functionality (from Orbit)
    mListView->setSelectionMode(HbAbstractItemView::MultiSelection);
    mListView->setFrictionEnabled(true);
    mListView->setScrollingStyle(HbScrollArea::PanWithFollowOn);
    
    setContentWidget(mListView);
    
    setTimeout(HbDialog::NoTimeout);
    setDismissPolicy(HbDialog::NoDismiss);
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    
    HbAction *mPrimaryAction = new HbAction(hbTrId("txt_phob_button_delete_selected"), this);
    addAction(mPrimaryAction);
    
    HbAction *mSecondaryAction = new HbAction(hbTrId("txt_common_button_cancel"), this);
    addAction(mSecondaryAction);
}

QList<QContactLocalId> CntGroupDeletePopup::deleteGroup() const
{
    QModelIndexList indexes = mListView->selectionModel()->selection().indexes();
    QList<QContactLocalId> selectionList;
    for (int i = 0; i < indexes.count(); i++)
    {
        QContact contact = mModel->contact(indexes[i]);
        QContactLocalId locId = contact.localId();
        selectionList.append(locId); 
    }
    
    QMap<int, QContactManager::Error> errors;
    mContactManager->removeContacts(selectionList, &errors);
    
    return selectionList;
}
