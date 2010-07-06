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

#include "cntcollectionview.h"
#include "cntfetchcontactsview.h"
#include "cntgroupdeletepopup.h"
#include "cntcollectionlistmodel.h"
#include "cntextensionmanager.h"
#include "cntglobal.h"
#include "cntfavourite.h"
#include "cntdebug.h"

#include <cntuiextensionfactory.h>
#include <cntuigroupsupplier.h>
#include <hblabel.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbmenu.h>
#include <hbnotificationdialog.h>
#include <hbinputdialog.h>
#include <hbdialog.h>
#include <hbaction.h>
#include <hbview.h>
#include <hbframebackground.h>
#include <hbgroupbox.h>
#include <hbmessagebox.h>
#include <hbparameterlengthlimiter.h>

#include <QActionGroup>
#include <QList>

const char *CNT_COLLECTIONVIEW_XML = ":/xml/contacts_collections.docml";

/*!

*/
CntCollectionView::CntCollectionView(CntExtensionManager &extensionManager) :
    mExtensionManager(extensionManager),
    mView(NULL),
    mSoftkey(NULL),
    mViewManager(NULL),
    mModel(NULL),
    mListView(NULL),
    mNamesAction(NULL),
    mFindAction(NULL),
    mExtensionAction(NULL),
    mNewGroupAction(NULL),
    mDeleteGroupsAction(NULL),
    mHandledContact(NULL),
    mFetchView(NULL),
    mActionGroup(NULL)
{
    bool ok = false;
    mDocumentLoader.load(CNT_COLLECTIONVIEW_XML, &ok);

    if (ok)
    {
        mView = static_cast<HbView*>(mDocumentLoader.findWidget(QString("view")));
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_collections.docml");
    }
    
    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    
    // menu actions
    mNewGroupAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:newgroup"));
    connect(mNewGroupAction, SIGNAL(triggered()), this, SLOT(newGroup()));
    mDeleteGroupsAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:deletegroups"));
    connect(mDeleteGroupsAction, SIGNAL(triggered()), this, SLOT(deleteGroups()));
    
    // toolbar actions
    
    mNamesAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:names"));
    connect(mNamesAction, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    HbAction* groups = static_cast<HbAction*> (mDocumentLoader.findObject("cnt:groups"));
    
    mActionGroup = new QActionGroup(this);
    groups->setActionGroup(mActionGroup);
    mNamesAction->setActionGroup(mActionGroup);
    groups->setChecked(true);
    
    mFindAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:find"));
    mFindAction->setEnabled(false);
    mExtensionAction = static_cast<HbAction*> (mDocumentLoader.findObject("cnt:activity"));
}

/*!

*/
CntCollectionView::~CntCollectionView()
{
    mView->deleteLater();
    
    delete mHandledContact;
    mHandledContact = NULL;
    
    delete mFetchView;
    mFetchView = NULL;
}

/*!
Called when activating the view
*/
void CntCollectionView::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    CNT_ENTRY
    
    Q_UNUSED(aArgs)
    
    if (mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);   
    
    mViewManager = aMgr;

    // disable delete group(s) button if only favorites group is present
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QLatin1String(QContactType::TypeGroup));
    QList<QContactLocalId> groupContactIds = getContactManager()->contactIds(groupFilter);
    if (groupContactIds.count() < 2)
    {
        mDeleteGroupsAction->setEnabled(false);
    }
    
    // set up the list
    mListView = static_cast<HbListView*>(mDocumentLoader.findWidget(QString("listView")));
    mListView->setUniformItemSizes(true);
    
    connect(mListView, SIGNAL(activated(const QModelIndex&)), this,
        SLOT(openGroup(const QModelIndex&)));
    connect(mListView, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)), this,
        SLOT(showContextMenu(HbAbstractViewItem*,QPointF)));
    
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mListView->itemPrototypes().first()->setDefaultFrame(frame);
    
    mListView->listItemPrototype()->setGraphicsSize(HbListViewItem::LargeIcon);
    mListView->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);
    mModel = new CntCollectionListModel(getContactManager(), mExtensionManager, this);
    mListView->setModel(mModel);
    
    mFetchView = new CntFetchContacts(*mViewManager->contactManager( SYMBIAN_BACKEND ));
    connect(mFetchView, SIGNAL(clicked()), this, SLOT(handleNewGroupMembers()));
    
    CNT_EXIT
}

void CntCollectionView::deactivate()
{

}

/*!
Handle view switching request from an extension group item 
*/
void CntCollectionView::openView(CntViewParameters& viewParams)
{
    mViewManager->changeView(viewParams);
}

/*!
Go back to previous view
*/
void CntCollectionView::showPreviousView()
{
    CntViewParameters viewParameters;
    mViewManager->back(viewParameters);
}

/*!
Called after user clicked on the listview.
*/
void CntCollectionView::openGroup(const QModelIndex &index)
{
    if (mModel->isExtensionGroup(index))
    {
        CntViewParameters params = mModel->extensionGroupActivated(index.row());
        if (params.count())
        {
            mViewManager->changeView(params);
        }
    }
    else
    {
        int id = index.data(Qt::UserRole).toInt();
        int favoriteGrpId = CntFavourite::favouriteGroupId(mViewManager->contactManager(SYMBIAN_BACKEND));

        if (id == favoriteGrpId )
        {
            QContact favoriteGroup = getContactManager()->contact(favoriteGrpId);
            QContactRelationshipFilter rFilter;
            rFilter.setRelationshipType(QContactRelationship::HasMember);
            rFilter.setRelatedContactRole(QContactRelationship::First);
            rFilter.setRelatedContactId(favoriteGroup.id());
            // group members and their count
            QList<QContactLocalId> groupMemberIds = getContactManager()->contactIds(rFilter);

            if (groupMemberIds.isEmpty())
            {
                CntViewParameters viewParameters;
                viewParameters.insert(EViewId, collectionFavoritesView);
                QVariant var;
                var.setValue(favoriteGroup);
                viewParameters.insert(ESelectedGroupContact, var);
                mViewManager->changeView(viewParameters);
            }
            else
            {
                CntViewParameters viewParameters;
                viewParameters.insert(EViewId, FavoritesMemberView);
                QVariant var;
                var.setValue(favoriteGroup);
                viewParameters.insert(ESelectedGroupContact, var);
                mViewManager->changeView(viewParameters);
            }
        }
        else
        {
            QContact groupContact = getContactManager()->contact(id);

            CntViewParameters viewParameters;
            viewParameters.insert(EViewId, groupMemberView);
            QVariant var;
            var.setValue(groupContact);
            viewParameters.insert(ESelectedGroupContact, var);
            mViewManager->changeView(viewParameters);
        }
    }
}

void CntCollectionView::showContextMenu(HbAbstractViewItem *item, const QPointF &coords)
{
    if (mModel->isExtensionGroup(item->modelIndex()))
    {
        mModel->extensionGroupLongPressed(item->modelIndex().row(), coords, this);
    }
    else
    {
        int id = item->modelIndex().data(Qt::UserRole).toInt();
        QVariant data( item->modelIndex().row() );

        int favoriteGrpId = CntFavourite::favouriteGroupId(mViewManager->contactManager(SYMBIAN_BACKEND));
        
        HbMenu *menu = new HbMenu();
        menu->setAttribute(Qt::WA_DeleteOnClose);
        menu->setPreferredPos( coords );
        
        HbAction* openAction = menu->addAction(hbTrId("txt_common_menu_open"));
        openAction->setData( data );

        if (id != favoriteGrpId)
        {
            HbAction* deleteAction = menu->addAction(hbTrId("txt_phob_menu_delete_group"));
            deleteAction->setData( data );
        }
        menu->open(this, SLOT(handleMenu(HbAction*)));

    }
}

void CntCollectionView::handleMenu(HbAction* action)
{
    int row = action->data().toInt();
    HbMenu *menuItem = static_cast<HbMenu*>(sender());
    QModelIndex index = mModel->index(row, 0);
    
    int id = index.data(Qt::UserRole).toInt();
    
    if ( action == menuItem->actions().first() )
        {
        openGroup(index);
        }
    else if (action == menuItem->actions().at(1))
        {
        
        QContact groupContact = getContactManager()->contact(id);
        deleteGroup(groupContact);
        }
}



void CntCollectionView::newGroup()
{
    HbInputDialog *popup = new HbInputDialog();
    popup->setAttribute(Qt::WA_DeleteOnClose, true);
    
    HbLineEdit *lineEdit = popup->lineEdit();
    lineEdit->setInputMethodHints(Qt::ImhNoPredictiveText);
    
    popup->setPromptText(hbTrId("txt_phob_title_new_group_name"));
    popup->clearActions();
    HbAction* primaryAction = new HbAction(hbTrId("txt_phob_button_create"));
    popup->addAction(primaryAction);
    
    HbAction* secondaryAction = new HbAction(hbTrId("txt_common_button_cancel"));
    popup->addAction(secondaryAction);
           
    popup->setInputMode(HbInputDialog::TextInput);

    popup->open(this, SLOT(handleNewGroup(HbAction*)));
}

void CntCollectionView::handleNewGroup(HbAction* action)
{
    HbInputDialog *popup = static_cast<HbInputDialog*>(sender());
    
    if (popup && action == popup->actions().first())
    {
        QString text = popup->value().toString();
        
        mHandledContact = new QContact();
        mHandledContact->setType(QContactType::TypeGroup);
        
        QContactName groupName;
        groupName.setCustomLabel(text);
        
        mHandledContact->saveDetail(&groupName);
        getContactManager()->saveContact(mHandledContact);
        
        QContactDetailFilter filter;
        QList<QContactLocalId> contactsList = getContactManager()->contactIds(filter);
        QSet<QContactLocalId> contactsSet = contactsList.toSet();

        // Select some contact(s) to add to the group
        QString groupNameCreated(mHandledContact->displayLabel());
        mFetchView->setDetails(HbParameterLengthLimiter(hbTrId("txt_phob_title_members_of_1_group")).arg(groupNameCreated),
                               hbTrId("txt_common_button_save"));
        mFetchView->displayContacts(HbAbstractItemView::MultiSelection, contactsSet);
    }
}

void CntCollectionView::handleNewGroupMembers()
{
    mSelectedContactsSet = mFetchView->getSelectedContacts();
    if ( !mFetchView->wasCanceled() && mSelectedContactsSet.size() ) {
        saveNewGroup(mHandledContact);

        CntViewParameters viewParameters;
        viewParameters.insert(EViewId, groupMemberView);
        QVariant var;
        var.setValue(*mHandledContact);
        viewParameters.insert(ESelectedGroupContact, var);
        mViewManager->changeView(viewParameters);
    }
    
    QString groupNameCreated(mHandledContact->displayLabel());
    HbNotificationDialog::launchDialog(HbParameterLengthLimiter(hbTrId("txt_phob_dpophead_new_group_1_created")).arg(groupNameCreated));

    // Refresh the page 
    refreshDataModel();
    mDeleteGroupsAction->setEnabled(true);

    delete mHandledContact;
    mHandledContact = NULL;
}

void CntCollectionView::refreshDataModel()
{
    mListView->setModel(0);
    delete mModel;
    mModel = 0;
    mModel = new CntCollectionListModel(getContactManager(), mExtensionManager, this);
    mListView->setModel(mModel);
}

void CntCollectionView::deleteGroup(QContact group)
{
    mHandledContact = new QContact(group);
    QString name = mHandledContact->displayLabel();

    HbLabel *headingLabel = new HbLabel();
    headingLabel->setPlainText(HbParameterLengthLimiter(hbTrId("txt_phob_dialog_delete_1_group")).arg(name));
          
    HbMessageBox::question(hbTrId("txt_phob_dialog_only_group_will_be_removed_contac")
            , this, SLOT(handleDeleteGroup(HbAction*)),
                hbTrId("txt_common_button_delete"), hbTrId("txt_common_button_cancel"), headingLabel);
}

void CntCollectionView::handleDeleteGroup(HbAction* action)
{
    HbMessageBox *note = static_cast<HbMessageBox*>(sender());
    
    if (note && action == note->actions().first())
    {
        getContactManager()->removeContact(mHandledContact->localId());
        mModel->removeGroup(mHandledContact->localId());
        
        // disable delete group(s) button if only favorites group is present
        QContactDetailFilter groupFilter;
        groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
        groupFilter.setValue(QLatin1String(QContactType::TypeGroup));
        QList<QContactLocalId> groupContactIds = getContactManager()->contactIds(groupFilter);
        if (groupContactIds.count() < 2)
        {
            mDeleteGroupsAction->setEnabled(false);
        }
    }
    
    delete mHandledContact;
    mHandledContact = NULL;
}

void CntCollectionView::deleteGroups()
{
    // save the group here
    CntGroupDeletePopup *groupDeletePopup = new CntGroupDeletePopup(getContactManager());
    
    groupDeletePopup->populateListOfGroup();
    groupDeletePopup->open(this, SLOT(handleDeleteGroups(HbAction*)));

}

void CntCollectionView::handleDeleteGroups(HbAction* action)
{
    CntGroupDeletePopup *groupDeletePopup = static_cast<CntGroupDeletePopup*>(sender());
    
    if (groupDeletePopup && action == groupDeletePopup->actions().first())
    {
        QList<QContactLocalId> deletedList = groupDeletePopup->deleteGroup();
        foreach (QContactLocalId id, deletedList)
        {
            mModel->removeGroup(id);
        }
        
        // disable delete group(s) button if only favorites group is present
        QContactDetailFilter groupFilter;
        groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
        groupFilter.setValue(QLatin1String(QContactType::TypeGroup));
        QList<QContactLocalId> groupContactIds = getContactManager()->contactIds(groupFilter);
        if (groupContactIds.count() < 2)
        {
            mDeleteGroupsAction->setEnabled(false);
        }
    }
}

QContactManager* CntCollectionView::getContactManager()
{
    if (!mViewManager) return NULL;

    return mViewManager->contactManager(SYMBIAN_BACKEND);
}

void CntCollectionView::saveNewGroup(QContact* aContact)
{
    if (!aContact) return;
    
    // Save the relationship from the selection model of the member selection list
    QList<QContactLocalId> selectedList = mSelectedContactsSet.toList();
    for (int i = 0; i < selectedList.size(); i++ ) {
        QContact contact = getContactManager()->contact(selectedList.at(i));
        QContactRelationship relationship;
        relationship.setRelationshipType(QContactRelationship::HasMember);
        relationship.setFirst(aContact->id());
        relationship.setSecond(contact.id());

        // Save relationship
        getContactManager()->saveRelationship(&relationship);
    }
}

// EOF
