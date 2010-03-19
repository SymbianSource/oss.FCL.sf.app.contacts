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

#include "cntgroupselectionpopup.h"

#include <hblabel.h>
#include <hbgroupbox.h>
#include <hbaction.h>
#include <qtcontacts.h>
#include <QStringListModel>
#include <mobcntmodel.h>


CntGroupSelectionPopup::CntGroupSelectionPopup(QContactManager *manager,MobCntModel *model, QContact *contact, QGraphicsItem *parent):
    HbDialog(parent),
    mListView(0),
    mContactManager(manager),
    mCntModel(model),
    mContact(contact)
{
}

CntGroupSelectionPopup::~CntGroupSelectionPopup()
{

}

void CntGroupSelectionPopup::populateListOfContact()
{
    QContactName groupContactName = mContact->detail( QContactName::DefinitionName );
    QString groupName(groupContactName.value( QContactName::FieldCustomLabel ));
    
    HbGroupBox *headingLabel = new HbGroupBox();
    HbLabel *label = new HbLabel(hbTrId("txt_phob_title_members_of_1_group").arg(groupName));    
    headingLabel->setContentWidget(label);
    
    setHeadingWidget(headingLabel);
    
    mListView = new HbListView(this);
    QContactRelationshipFilter rFilter;
    
    QContactDetailFilter contactsFilter;
    contactsFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    contactsFilter.setValue(QString(QLatin1String(QContactType::TypeContact)));
    mCntModel->setFilterAndSortOrder(contactsFilter);
    mCntModel->showMyCard(false);
    
    rFilter.setRelationshipType(QContactRelationship::HasMember);
    rFilter.setRole(QContactRelationshipFilter::Second); // should be second here
    rFilter.setOtherParticipantId(mContact->id());   
    QList<QContactLocalId> contactsLocalIdList = mContactManager->contacts(rFilter);
    int countContacts = contactsLocalIdList.count();
    
    //Get the index of the contacts
    // Set the select option for those contacts in the selectionModel
    mListView->setModel(mCntModel);
    // set the listview to multiSelection mode, this will bring MarkAll functionality (from Orbit)
    mListView->setSelectionMode(HbAbstractItemView::MultiSelection);
    
    
    for (int i=0; i < countContacts; i++ )
    {
        // get QContact from QContactId
        QContact contactInList = mContactManager->contact(contactsLocalIdList.at(i));
        QModelIndex contactIndex = mCntModel->indexOfContact(contactInList);
        mListView->selectionModel()->select(contactIndex, QItemSelectionModel::Select);
    }
    setContentWidget(mListView);
       
    setTimeout(0);
    setModal(true);
     
    HbAction *mPrimaryAction = new HbAction(hbTrId("Save"));
    setPrimaryAction(mPrimaryAction);
    
    HbAction *mSecondaryAction = new HbAction(hbTrId("Cancel"));
    setSecondaryAction(mSecondaryAction);
}

void CntGroupSelectionPopup::populateListOfGroup()
{
    QContactName groupContactName = mContact->detail( QContactName::DefinitionName );
    QString groupName(groupContactName.value( QContactName::FieldCustomLabel ));
    
    HbGroupBox *headingLabel = new HbGroupBox();
    HbLabel *label = new HbLabel(hbTrId("txt_phob_opt_delete_groups"));    
    headingLabel->setContentWidget(label);
    
    setHeadingWidget(headingLabel);
    
    mListView = new HbListView(this);
    QContactRelationshipFilter rFilter;
    
    QContactDetailFilter contactsFilter;
    contactsFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    contactsFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));
    mCntModel->setFilterAndSortOrder(contactsFilter);
    mCntModel->showMyCard(false);
    
    //Get the index of the contacts
    // Set the select option for those contacts in the selectionModel
    mListView->setModel(mCntModel);
    // set the listview to multiSelection mode, this will bring MarkAll functionality (from Orbit)
    mListView->setSelectionMode(HbAbstractItemView::MultiSelection);
    
    setContentWidget(mListView);
    
    setTimeout(0);
    setModal(true);
    
    HbAction *mPrimaryAction = new HbAction(hbTrId("Delete selected"));
    setPrimaryAction(mPrimaryAction);
    
    HbAction *mSecondaryAction = new HbAction(hbTrId("txt_common_button_cancel"));
    setSecondaryAction(mSecondaryAction);
}


void CntGroupSelectionPopup::saveNewGroup()
{
    // Save the relationship from the selection model of the member selection list
    QModelIndexList indexes = mListView->selectionModel()->selection().indexes();
    for (int i = 0; i < indexes.count(); i++)
    {
        QContact contact = mCntModel->contact(indexes[i]);
        QContactRelationship relationship;
        relationship.setRelationshipType(QContactRelationship::HasMember);
        relationship.setFirst(mContact->id());
        relationship.setSecond(contact.id());
        
        // save relationship
        mContactManager->saveRelationship(&relationship);
    }
}

void CntGroupSelectionPopup::saveOldGroup()
{
    // Use relationship filter to get list of contacts in the relationship (if any)
    QContactRelationshipFilter rFilter;
    rFilter.setRelationshipType(QContactRelationship::HasMember);
    rFilter.setRole(QContactRelationshipFilter::Second); // should be second here
    rFilter.setOtherParticipantId(mContact->id());

    QList<QContactLocalId> contactsLocalIdList = mContactManager->contacts(rFilter);
    
    // get the contact from  new selection model indexes
    // for each contact, check if contact in this new index is member of mContactsLocalIdList( as achieved in activateView)
    // if yes, then do nothing
    // if no, then add contact to the relationship

    QModelIndexList indexes = mListView->selectionModel()->selection().indexes();
    QList<QContactLocalId>   selectionList;
    for (int i = 0; i < indexes.count(); i++)
    {
        QContact contact = mCntModel->contact(indexes[i]);
        QContactLocalId locId = contact.localId();
        selectionList.append(locId);       
        
        if (!contactsLocalIdList.contains(locId))
        {
            // new contact added to the group
            QContactRelationship relationship;
            relationship.setRelationshipType(QContactRelationship::HasMember);
            relationship.setFirst(mContact->id());
            relationship.setSecond(contact.id());
            // save relationship
            mContactManager->saveRelationship(&relationship);
        }
    }
    
    // for each contact in mContactsLocalIdList, check if contact is also in new selection model
    // if yes, then do nothing
    // if no, then remove it from the relationship
    
    for (int x=0; x < contactsLocalIdList.count(); x++)
    {
        QContactLocalId oldSelectedLocalContactId = contactsLocalIdList.at(x);
        if (!selectionList.contains(oldSelectedLocalContactId))
        {
            QContact contactInOldList = mContactManager->contact(contactsLocalIdList.at(x));          
            QContactRelationship relationship;
            relationship.setRelationshipType(QContactRelationship::HasMember);
            relationship.setFirst(mContact->id());
            relationship.setSecond(contactInOldList.id());
            mContactManager->removeRelationship(relationship);
        }
    }
}

void CntGroupSelectionPopup::deleteGroup()
{
    QModelIndexList indexes = mListView->selectionModel()->selection().indexes();
    QList<QContactLocalId>   selectionList;
    for (int i = 0; i < indexes.count(); i++)
    {
        QContact contact = mCntModel->contact(indexes[i]);
        QContactLocalId locId = contact.localId();
        selectionList.append(locId); 
    }
    mContactManager->removeContacts(&selectionList);
}
