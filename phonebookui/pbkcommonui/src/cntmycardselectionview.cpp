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

#include "cntmycardselectionview.h"
#include <QModelIndex>
#include <mobcntmodel.h>
#include <hblistview.h>
#include <qcontact.h>

CntMyCardSelectionView::CntMyCardSelectionView() : CntBaseSelectionView()
{
    connect(mListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(onListViewActivated(const QModelIndex&)));
}

CntMyCardSelectionView::~CntMyCardSelectionView()
{
}

/*!
Set index to mycard
*/
void CntMyCardSelectionView::onListViewActivated(const QModelIndex& index)
{
    if (index.isValid())
    {  
        QContact contact = mListModel->contact(index);
        QContactManager* mgr = mMgr->contactManager(SYMBIAN_BACKEND);
        mgr->setSelfContactId(contact.localId());
        
        removeFromGroup(&contact);
        
        CntViewParameters params;
        params.insert(EViewId, namesView);
        mMgr->changeView(params);
    }
}
    
void CntMyCardSelectionView::removeFromGroup(const QContact* aContact)
{
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QLatin1String(QContactType::TypeGroup));

    QContactManager* mgr = mMgr->contactManager(SYMBIAN_BACKEND);
    QList<QContactLocalId> groupContactIds = mgr->contactIds(groupFilter);
    if (!groupContactIds.isEmpty())
    {
        for(int i = 0;i < groupContactIds.count();i++)
        {
            QContact groupContact = mgr->contact(groupContactIds.at(i));
            QContactRelationship relationship;
            relationship.setRelationshipType(QContactRelationship::HasMember);
            relationship.setFirst(groupContact.id());
            relationship.setSecond(aContact->id());
            mgr->removeRelationship(relationship);  
         }
    }
}

