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
#include <hbsearchpanel.h>
#include <hbtextitem.h>
#include <qtcontacts.h>
#include <QStringListModel>
#include <QGraphicsWidget>
#include <mobcntmodel.h>


CntGroupSelectionPopup::CntGroupSelectionPopup(QContactManager *manager, QContact *contact, QGraphicsItem *parent):
    HbDialog(parent),
    mListView(0),
    mSearchPanel(0),
    mEmptyListLabel(0),
    mContactManager(manager),
    mCntModel(0),
    mContact(contact),
    mContainerWidget(0),
    mContainerLayout(0)
{
    QList<QContactSortOrder> sortOrders;
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
    sortOrderFirstName.setCaseSensitivity(Qt::CaseInsensitive);
    sortOrders.append(sortOrderFirstName);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLast);
    sortOrderLastName.setCaseSensitivity(Qt::CaseInsensitive);
    sortOrders.append(sortOrderLastName);

    mCntModel = new MobCntModel(mContactManager, QContactFilter(), sortOrders);

    mSearchPanel = new HbSearchPanel();
    connect(mSearchPanel, SIGNAL(exitClicked()), this, SLOT(closeFind()));
    connect(mSearchPanel, SIGNAL(criteriaChanged(QString)), this, SLOT(setFilter(QString)));
    
    mContainerLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mContainerLayout->setContentsMargins(0, 0, 0, 0);
    mContainerLayout->setSpacing(0);
    
    mContainerWidget = new QGraphicsWidget;
 }

CntGroupSelectionPopup::~CntGroupSelectionPopup()
{
    delete mContainerWidget;
    mContainerWidget = 0;
    
    delete mEmptyListLabel;
    mEmptyListLabel = 0;

    delete mCntModel;
    mCntModel = 0;
}

void CntGroupSelectionPopup::populateListOfContact()
{
    QContactName groupContactName = mContact->detail( QContactName::DefinitionName );
    QString groupName(groupContactName.value( QContactName::FieldCustomLabel ));
    
    HbGroupBox *headingLabel = new HbGroupBox(this);
    headingLabel->setHeading(hbTrId("txt_phob_title_members_of_1_group").arg(groupName));    
    
    setHeadingWidget(headingLabel);
    
    mListView = new HbListView(this);
    QContactRelationshipFilter rFilter;
    
    QContactDetailFilter contactsFilter;
    contactsFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    contactsFilter.setValue(QString(QLatin1String(QContactType::TypeContact)));
    mCntModel->setFilterAndSortOrder(contactsFilter);
    mCntModel->showMyCard(false);
    
    rFilter.setRelationshipType(QContactRelationship::HasMember);
    rFilter.setRelatedContactRole(QContactRelationshipFilter::First);
    rFilter.setRelatedContactId(mContact->id());   
    QList<QContactLocalId> contactsLocalIdList = mContactManager->contactIds(rFilter);
    int countContacts = contactsLocalIdList.count();
    
    //Get the index of the contacts
    // Set the select option for those contacts in the selectionModel
    mListView->setModel(mCntModel);
    // set the listview to multiSelection mode, this will bring MarkAll functionality (from Orbit)
    mListView->setSelectionMode(HbAbstractItemView::MultiSelection);
    mListView->setFrictionEnabled(true);
    mListView->setScrollingStyle(HbScrollArea::PanOrFlick);
    
    for (int i=0; i < countContacts; i++ )
    {
        // get QContact from QContactId
        QContact contactInList = mContactManager->contact(contactsLocalIdList.at(i));
        QModelIndex contactIndex = mCntModel->indexOfContact(contactInList);
        mListView->selectionModel()->select(contactIndex, QItemSelectionModel::Select);
    }
    
    setTimeout(0);
    setModal(true);
     
    // Note that the layout takes ownership of the item(s) it contains.
    mContainerLayout->addItem(mListView);
    mContainerLayout->addItem(mSearchPanel);
    mContainerWidget->setLayout(mContainerLayout);

    setContentWidget(mContainerWidget);
    
    HbAction *mPrimaryAction = new HbAction(hbTrId("Save"));
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
        QModelIndex index = indexes[i];
        QContact contact = mCntModel->contact(indexes[i]);
        int id = contact.localId();
        int gpid = mContact->localId();
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
    rFilter.setRelatedContactRole(QContactRelationshipFilter::First);
    rFilter.setRelatedContactId(mContact->id());

    QList<QContactLocalId> contactsLocalIdList = mContactManager->contactIds(rFilter);
    
    // get the contact from  new selection model indexes
    // for each contact, check if contact in this new index is member of mContactsLocalIdList( as achieved in activateView)
    // if yes, then do nothing
    // if no, then add contact to the relationship

    QModelIndexList indexes = mListView->selectionModel()->selection().indexes();
    QList<QContactLocalId> selectionList;

    QList<QContactRelationship> removedRelationships;
    QList<QContactRelationship> addedRelationships;

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
            addedRelationships.append(relationship);
        }
    }
    
    // for each contact in mContactsLocalIdList, check if contact is also in new selection model
    // if yes, then do nothing
    // if no, then remove it from the relationship
    
    for (int j = 0; j < contactsLocalIdList.count(); j++)
    {
        QContactLocalId oldSelectedLocalContactId = contactsLocalIdList.at(j);
        if (!selectionList.contains(oldSelectedLocalContactId))
        {
            QContact contactInOldList = mContactManager->contact(contactsLocalIdList.at(j));          
            QContactRelationship relationship;
            relationship.setRelationshipType(QContactRelationship::HasMember);
            relationship.setFirst(mContact->id());
            relationship.setSecond(contactInOldList.id());
            removedRelationships.append(relationship);
        }
    }
    // save & remove relationships
    mContactManager->removeRelationships(removedRelationships);
    mContactManager->saveRelationships(&addedRelationships);
}

void CntGroupSelectionPopup::closeFind()
{
    if (mSearchPanel)
    {
         mContainerLayout->removeItem(mEmptyListLabel);
         delete mEmptyListLabel;
         mEmptyListLabel = 0;

         mContainerLayout->removeItem(mSearchPanel);
         mContainerLayout->addItem(mListView);

         QContactDetailFilter filter;
         filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
         QString typeContact = QContactType::TypeContact;
         filter.setValue(typeContact);

         mSearchPanel->deleteLater();
     }
}

void CntGroupSelectionPopup::setFilter(const QString &filterString)
{
    QStringList searchList = filterString.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    // find matches and existing members
    QContactRelationshipFilter relationFilter;
    relationFilter.setRelationshipType(QContactRelationship::HasMember);
    relationFilter.setRelatedContactRole(QContactRelationshipFilter::First);
    relationFilter.setRelatedContactId(mContact->id());
        
    QContactDetailFilter detailfilter;
    detailfilter.setDetailDefinitionName(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel);
    detailfilter.setMatchFlags(QContactFilter::MatchStartsWith);
    detailfilter.setValue(searchList);
    
    QContactUnionFilter filter;
    filter.append(relationFilter);
    filter.append(detailfilter);
    
    mCntModel->setFilterAndSortOrder(filter);
    
    // select all contacts already in group relationship
    QList<QContactLocalId> contactsLocalIdList = mContactManager->contactIds(relationFilter);
    int countContacts = contactsLocalIdList.count();
    for (int i=0; i < countContacts; i++ )
    {
        // get QContact from QContactId
        QContact contactInList = mContactManager->contact(contactsLocalIdList.at(i));
        QModelIndex contactIndex = mCntModel->indexOfContact(contactInList);
        mListView->selectionModel()->select(contactIndex, QItemSelectionModel::Select);
    }

    if (!mCntModel->rowCount()) {
        if (mEmptyListLabel == 0) {
            mEmptyListLabel = new HbTextItem(hbTrId("(no matching contacts)"));
            mEmptyListLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
            mEmptyListLabel->setFontSpec(HbFontSpec(HbFontSpec::Primary));
            mEmptyListLabel->setAlignment(Qt::AlignCenter);
            mContainerLayout->insertItem(1, mEmptyListLabel);
        }
    }
    else {
        mContainerLayout->removeItem(mEmptyListLabel);
        delete mEmptyListLabel;
        mEmptyListLabel = 0;
    }
}
