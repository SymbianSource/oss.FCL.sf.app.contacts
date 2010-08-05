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
#include <hblistviewitem.h>
#include <qtcontacts.h>
#include <hbmainwindow.h>

CntGroupDeletePopup::CntGroupDeletePopup(QContactManager *manager, QGraphicsItem *parent):
    HbSelectionDialog(parent),
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

    setSelectionMode( HbAbstractItemView::MultiSelection );
    mModel->initializeGroupsList();
    setModel(mModel);
    clearActions(); 
    HbAction *mPrimaryAction = new HbAction(hbTrId("txt_phob_button_delete_selected"), this);
    addAction(mPrimaryAction);
    
    HbAction *mSecondaryAction = new HbAction(hbTrId("txt_common_button_cancel"), this);
    addAction(mSecondaryAction);
    
    setTimeout(HbDialog::NoTimeout);
    setDismissPolicy(HbDialog::NoDismiss);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum );
    
    qreal heightToSet = mainWindow()->layoutRect().height();
    setMinimumHeight(heightToSet);
}

QList<QContactLocalId> CntGroupDeletePopup::deleteGroup() const
{

    QModelIndexList indexes = selectedModelIndexes();
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
