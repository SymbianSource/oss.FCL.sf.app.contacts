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

#include <hblabel.h>
#include <hbgroupbox.h>
#include <hbaction.h>
#include <hbsearchpanel.h>
#include <hbtextitem.h>
#include <qtcontacts.h>
#include <QStringListModel>
#include <QGraphicsWidget>
#include <mobcntmodel.h>


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
    HbGroupBox *headingLabel = new HbGroupBox();
    HbLabel *label = new HbLabel(hbTrId("txt_phob_opt_delete_groups"));    
    headingLabel->setContentWidget(label);
    
    setHeadingWidget(headingLabel);
    
    mListView = new HbListView(this);
    
    
    mModel->initializeGroupsList();

    //Get the index of the contacts
    // Set the select option for those contacts in the selectionModel
    mListView->setModel(mModel);
    // set the listview to multiSelection mode, this will bring MarkAll functionality (from Orbit)
    mListView->setSelectionMode(HbAbstractItemView::MultiSelection);
    mListView->setFrictionEnabled(true);
    mListView->setScrollingStyle(HbScrollArea::PanOrFlick);
    
    setContentWidget(mListView);
    
    setTimeout(0);
    setModal(true);
    
    HbAction *mPrimaryAction = new HbAction(hbTrId("Delete selected"));
    setPrimaryAction(mPrimaryAction);
    
    HbAction *mSecondaryAction = new HbAction(hbTrId("txt_common_button_cancel"));
    setSecondaryAction(mSecondaryAction);
}

void CntGroupDeletePopup::deleteGroup()
{
    QModelIndexList indexes = mListView->selectionModel()->selection().indexes();
    QList<QContactLocalId>   selectionList;
    for (int i = 0; i < indexes.count(); i++)
    {
        QContact contact = mModel->contact(indexes[i]);
        QContactLocalId locId = contact.localId();
        selectionList.append(locId); 
    }
    
    QMap<int, QContactManager::Error> errors;
    bool result = mContactManager->removeContacts(&selectionList, &errors);
}
