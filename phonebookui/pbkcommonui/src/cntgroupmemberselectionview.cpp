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

#include "cntgroupmemberselectionview.h"
#include <QGraphicsLinearLayout>
#include <hbnotificationdialog.h>
#include <hbgroupbox.h>
#include <hblistview.h>

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntGroupMemberSelectionView::CntGroupMemberSelectionView(CntViewManager *viewManager, QGraphicsItem *parent)
    : CntBaseSelectionView(viewManager, parent),
    mContact(0),
    mBanner(0),
    mCountContacts(0),
    mActionNewGroup(false)
{

}

/*!
Destructor
*/
CntGroupMemberSelectionView::~CntGroupMemberSelectionView()
{
    delete mContact;
}

/*!
Set selection to a group
*/
void CntGroupMemberSelectionView::aboutToCloseView()
{
    CntViewParameters viewParameters(CntViewParameters::collectionView);
    viewParameters.setSelectedContact(*mContact);
    viewManager()->onActivateView(viewParameters);

}

void CntGroupMemberSelectionView::saveGroup()
{
    // get the contact from  new selection model indexes
    // for each contact, check if contact in this new index is member of mContactsLocalIdList( as achieved in activateView)
    // if yes, then do nothing
    // if no, then add contact to the relationship
    
    QModelIndexList indexes = selectionModel()->selection().indexes();
    // if yes, then do nothing
    // if no, then remove it from the relationship
    
    /*
     * Code below to be refactored to improve performance
     */
    
    for (int i = 0; i < indexes.count(); i++)
    {
        QContact contact = contactModel()->contact(indexes[i]);
        QContactLocalId locId = contact.localId();
        qDebug()<< locId;
        mSelectionList.append(locId);       
        
        if (mContactsLocalIdList.contains(locId))
        {
            // user has not deselected an old member of the group 
            // dont do anything
        }
        else
        {
            // new contact added to the group
            QContactRelationship relationship;
            relationship.setRelationshipType(QContactRelationship::HasMember);
            relationship.setFirst(mContact->id());
            relationship.setSecond(contact.id());
            // save relationship
            contactManager()->saveRelationship(&relationship);
        }
    }
    
    // for each contact in mContactsLocalIdList, check if contact is also in new selection model
    // if yes, then do nothing
    // if no, then remove it from the relationship
        
    for (int x=0; x < mContactsLocalIdList.count(); x++)
    {
       QContactLocalId oldSelectedLocalContactId = mContactsLocalIdList.at(x);
       qDebug()<< oldSelectedLocalContactId;
       if (mSelectionList.contains(oldSelectedLocalContactId))
       {
           // user has not deselected an old member of the group 
           // dont do anything
       }
       else
       {
           QContact contactInOldList = contactManager()->contact(mContactsLocalIdList.at(x));          
           QContactRelationship relationship;
           relationship.setRelationshipType(QContactRelationship::HasMember);
           relationship.setFirst(mContact->id());
           relationship.setSecond(contactInOldList.id());
           contactManager()->removeRelationship(relationship);
       }
   }
    
    CntViewParameters viewParameters(CntViewParameters::groupActionsView);
    viewParameters.setSelectedContact(*mContact);
    viewManager()->onActivateView(viewParameters);
}

void CntGroupMemberSelectionView::markUnmarkAll()
{    
    if(mCountContacts < contactModel()->rowCount())
    {
        listView()->selectAll(); // mark all
    }
    else
    {
        listView()->clearSelection(); // unmark all only when everything is already marked
    }
    
}

void CntGroupMemberSelectionView::OnCancel()
{
   QString groupNameCreated(mGroupContactName.value( QContactName::FieldCustomLabel ));
   if(mActionNewGroup)
   {
       HbNotificationDialog::launchDialog(qtTrId("New group %1 created").arg(groupNameCreated));
   }
   CntViewParameters viewParameters(CntViewParameters::collectionView);
   viewManager()->onActivateView(viewParameters);
}
    
void CntGroupMemberSelectionView::openGroupNameEditor()
{
     CntViewParameters viewParameters(CntViewParameters::groupEditorView);
     viewParameters.setSelectedContact(*mContact);
     viewManager()->onActivateView(viewParameters);
}

void CntGroupMemberSelectionView::activateView(const CntViewParameters &viewParameters)
{
    QContact contact = viewParameters.selectedContact();
    mContact = new QContact(contact);

    mGroupContactName = mContact->detail( QContactName::DefinitionName );
    QString groupName(mGroupContactName.value( QContactName::FieldCustomLabel ));
    
    mBanner = new HbGroupBox(this);
    listLayout()->insertItem(0, mBanner);
    
    if (viewParameters.selectedAction() == "NewGroup")
    {
        mActionNewGroup = true; // to enable the Cancel notification only for New Group ->Cancel
    }
 
    // create a filter to show contacts of typeContact
    QContactDetailFilter contactsFilter;
    contactsFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    contactsFilter.setValue(QString(QLatin1String(QContactType::TypeContact)));
    
    contactModel()->setFilterAndSortOrder(contactsFilter);
    contactModel()->showMyCard(false);

    // Use relationship filter to get list of contacts in the relationship (if any)
    QContactRelationshipFilter rFilter;
    rFilter.setRelationshipType(QContactRelationship::HasMember);
    rFilter.setRole(QContactRelationshipFilter::Second); // should be second here
    rFilter.setOtherParticipantId(mContact->id());

    mContactsLocalIdList = contactManager()->contacts(rFilter);
    mCountContacts = mContactsLocalIdList.count();
    
    // set the model to the list (needed before next step)
    CntBaseSelectionView::activateView(viewParameters);
    
    //Get the index of the contacts
    // Set the select option for those contacts in the selectionModel
    
    for (int i=0; i < mCountContacts; i++ )
    {
        // get QContact from QContactId
        QContact contactInList = contactManager()->contact(mContactsLocalIdList.at(i));
        QContactLocalId contactInListLocId = contactInList.localId();
               
        QModelIndex contactIndex = contactModel()->indexOfContact(contactInList);
        selectionModel()->select(contactIndex, QItemSelectionModel::Select);
    }
    
    QString temp;
    temp.setNum(mCountContacts);
    mBanner->setHeading(qtTrId("(%1) members selected").arg(temp));
    
    connect(selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(updateTitle(const QItemSelection&, const QItemSelection&)));
}

void CntGroupMemberSelectionView::updateTitle(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (selected.count())
    {
        mCountContacts += selected.indexes().count();
        QString temp;
        temp.setNum(mCountContacts);
        mBanner->setHeading(qtTrId("(%1) members selected").arg(temp));
    }
    if (deselected.count())
    {
        mCountContacts -= deselected.indexes().count();
        QString temp;
        temp.setNum(mCountContacts);
        mBanner->setHeading(qtTrId("(%1) members selected").arg(temp));
    }
}

void CntGroupMemberSelectionView::addActionsToToolBar()
{
    //Add Action to the toolbar
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:save") << actions()->baseAction("cnt:markUnmarkAll") << actions()->baseAction("cnt:cancel");

    actions()->addActionsToToolBar(toolBar());

    connect(actions()->baseAction("cnt:save"), SIGNAL(triggered()),
            this, SLOT(saveGroup()));
    connect(actions()->baseAction("cnt:markUnmarkAll"), SIGNAL(triggered()),
            this, SLOT(markUnmarkAll()));
    connect(actions()->baseAction("cnt:cancel"), SIGNAL(triggered()),
            this, SLOT(OnCancel()));
}

