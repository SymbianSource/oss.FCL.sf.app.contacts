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
#include <cntabstractengine.h>
#include "cntglobal.h"
#include <hbaction.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <qtcontacts.h>
#include <hbmainwindow.h>

CntGroupDeletePopup::CntGroupDeletePopup(CntAbstractEngine *aEngine, QGraphicsItem *parent):
    HbSelectionDialog(parent),
    mContactManager( &aEngine->contactManager(SYMBIAN_BACKEND) ),
    mModel(NULL),
    mPrimaryAction(NULL)
{
    mModel = new CntGroupDeletePopupModel(aEngine, this);
}

CntGroupDeletePopup::~CntGroupDeletePopup()
{
    delete mModel;
    mModel = 0;
}

void CntGroupDeletePopup::populateListOfGroup()
{
    setHeadingText(hbTrId("txt_phob_opt_delete_groups"));

    setSelectionMode( HbAbstractItemView::MultiSelection );
    mModel->initializeGroupsList();
    setModel(mModel);
    clearActions(); 
    mPrimaryAction = new HbAction(hbTrId("txt_phob_button_delete_selected"), this);
    addAction(mPrimaryAction);
    mPrimaryAction->setEnabled(false);
    
    connect(this , SIGNAL(selectionChanged()), this, SLOT(checkPrimaryAction()));    
    QModelIndexList indexes = selectedModelIndexes();
        
    HbAction *secondaryAction = new HbAction(hbTrId("txt_common_button_cancel"), this);
    addAction(secondaryAction);
    
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

void CntGroupDeletePopup::checkPrimaryAction()
{
    QModelIndexList indexes = selectedModelIndexes();
    if (indexes.count())
    {
        mPrimaryAction->setEnabled(true);
    }
    else 
    {
        mPrimaryAction->setEnabled(false);
    }

}
