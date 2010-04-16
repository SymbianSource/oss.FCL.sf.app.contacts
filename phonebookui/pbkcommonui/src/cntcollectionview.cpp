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
#include "cntgroupselectionpopup.h"
#include "cntgroupdeletepopup.h"
#include "cntcollectionlistmodel.h"
#include "qtpbkglobal.h"

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

const char *CNT_COLLECTIONVIEW_XML = ":/xml/contacts_collections.docml";

/*!

*/
CntCollectionView::CntCollectionView() :
    mView(0),
    mSoftkey(0),
    mViewManager(0),
    mModel(0),
    mListView(0),
    mNamesAction(0),
    mRefreshAction(0),
    mNewGroupAction(0),
    mDeleteGroupsAction(0)
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
    mDeleteGroupsAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:deletegroups"));
    mDeleteGroupsAction->setParent(mView);
    connect(mDeleteGroupsAction, SIGNAL(triggered()), this, SLOT(deleteGroups()));
    
    // toolbar actions
    mNamesAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:names"));
    mNamesAction->setParent(mView);
    connect(mNamesAction, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    mRefreshAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:refresh"));
    mRefreshAction->setParent(mView);
    connect(mRefreshAction, SIGNAL(triggered()), this, SLOT(refreshDataModel()));
    mNewGroupAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:newgroup"));
    mNewGroupAction->setParent(mView);
    connect(mNewGroupAction, SIGNAL(triggered()), this, SLOT(newGroup()));
}

/*!

*/
CntCollectionView::~CntCollectionView()
{
    mView->deleteLater();
}

/*!
Called when activating the view
*/
void CntCollectionView::activate( CntAbstractViewManager* aMgr, const CntViewParameters& aArgs )
{
    Q_UNUSED(aArgs)
    
    if (mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);   
    
    mViewManager = aMgr;

    // disable delete group(s) button if only favorites group is present
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QLatin1String(QContactType::TypeGroup));
    QList<QContactLocalId> groupContactIds = mViewManager->contactManager(SYMBIAN_BACKEND)->contactIds(groupFilter);
    if (groupContactIds.count() < 2)
    {
        mDeleteGroupsAction->setEnabled(false);
    }
    
    // set up the list
    mListView = static_cast<HbListView*>(mDocumentLoader.findWidget(QString("listView")));
    
    connect(mListView, SIGNAL(activated(const QModelIndex&)), this,
        SLOT(openGroup(const QModelIndex&)));
    connect(mListView, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)), this,
        SLOT(showContextMenu(HbAbstractViewItem*,QPointF)));
    
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mListView->itemPrototypes().first()->setDefaultFrame(frame);
    
    mListView->listItemPrototype()->setGraphicsSize(HbListViewItem::LargeIcon);
    
    mModel = new CntCollectionListModel(mViewManager->contactManager(SYMBIAN_BACKEND), this);
    mListView->setModel(mModel);
}

void CntCollectionView::deactivate()
{

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
    int id = index.data(Qt::UserRole).toInt();
    int favoriteGrpId = mModel->favoriteGroupId();

    if (id == favoriteGrpId )
    {
        QContact favoriteGroup = mViewManager->contactManager(SYMBIAN_BACKEND)->contact(favoriteGrpId);
        QContactRelationshipFilter rFilter;
        rFilter.setRelationshipType(QContactRelationship::HasMember);
        rFilter.setRelatedContactRole(QContactRelationshipFilter::First);
        rFilter.setRelatedContactId(favoriteGroup.id());
        // group members and their count
        QList<QContactLocalId> groupMemberIds = mViewManager->contactManager(SYMBIAN_BACKEND)->contactIds(rFilter);
        
        if (groupMemberIds.isEmpty())
        {
            CntViewParameters viewParameters(CntViewParameters::collectionFavoritesView);
            viewParameters.setSelectedContact(favoriteGroup);
            mViewManager->changeView(viewParameters);
        }
        else 
        {
            CntViewParameters viewParameters(CntViewParameters::FavoritesMemberView);
            viewParameters.setSelectedContact(favoriteGroup);
            mViewManager->changeView(viewParameters);
        }
    }
    else
    {
        QContact groupContact = mViewManager->contactManager(SYMBIAN_BACKEND)->contact(id);
        CntViewParameters viewParameters(CntViewParameters::groupMemberView);
        viewParameters.setSelectedContact(groupContact);
        mViewManager->changeView(viewParameters);
    }
}

void CntCollectionView::showContextMenu(HbAbstractViewItem *item, const QPointF &coords)
{
    int id = item->modelIndex().data(Qt::UserRole).toInt();
    int favoriteGrpId = mModel->favoriteGroupId();
    
    HbMenu *menu = new HbMenu();
    HbAction *openAction = 0;
    HbAction *deleteAction = 0;
    
    openAction = menu->addAction(hbTrId("txt_common_menu_open"));
    
    if (id != favoriteGrpId)
    {
        deleteAction = menu->addAction(hbTrId("txt_phob_menu_delete_group"));
    }
    
    HbAction *selectedAction = menu->exec(coords);

    if (selectedAction)
    {
        if (selectedAction == openAction)
        {
            openGroup(item->modelIndex());
        }
        else if (selectedAction == deleteAction)
        {
            QContact groupContact = mViewManager->contactManager(SYMBIAN_BACKEND)->contact(id);
            deleteGroup(groupContact);
        }
    }
    menu->deleteLater();
}

void CntCollectionView::newGroup()
{
    QString mTextOfNewItem("");
    
    HbInputDialog popup;
    
    HbGroupBox *headingLabel = new HbGroupBox(&popup);  
    headingLabel->setHeading("txt_phob_title_new_group_name");
    popup.setHeadingWidget(headingLabel);
    popup.setPrimaryAction(new HbAction(hbTrId("txt_phob_button_create"),&popup));
    popup.setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"),&popup));
    popup.setInputMode(HbInputDialog::TextInput);
    popup.setPromptText("");
    popup.setBackgroundFaded(true);
    HbAction* action = popup.exec();
    QString text = popup.value().toString();
    
    if (action == popup.primaryAction())
    {
        mTextOfNewItem = text;
        
        QContact groupContact;
        groupContact.setType(QContactType::TypeGroup);
        
        QContactName groupName;
        groupName.setCustomLabel(mTextOfNewItem);
        
        groupContact.saveDetail(&groupName);
        mViewManager->contactManager(SYMBIAN_BACKEND)->saveContact(&groupContact);
        
        // call a dialog to display the contacts
        
        CntGroupSelectionPopup *groupSelectionPopup = 
            new CntGroupSelectionPopup(mViewManager->contactManager(SYMBIAN_BACKEND), &groupContact);
        groupSelectionPopup->populateListOfContact();
        HbAction* action = groupSelectionPopup->exec();
        if (action == groupSelectionPopup->primaryAction())
        {
            groupSelectionPopup->saveNewGroup();
            CntViewParameters viewParameters(CntViewParameters::groupMemberView);
            viewParameters.setSelectedContact(groupContact);
            mViewManager->changeView(viewParameters);
            delete groupSelectionPopup;
        }
        else if (action == groupSelectionPopup->secondaryAction())
        {
            delete groupSelectionPopup;
            QString groupNameCreated(groupName.customLabel());
            HbNotificationDialog::launchDialog(hbTrId("txt_phob_dpophead_new_group_1_created").arg(groupNameCreated));
            //refresh the page 
            refreshDataModel();
            mDeleteGroupsAction->setEnabled(true);
        }
        
    }
}

void CntCollectionView::refreshDataModel()
{
    mListView->setModel(0);
    delete mModel;
    mModel = 0;
    mModel = new CntCollectionListModel(mViewManager->contactManager(SYMBIAN_BACKEND), this);
    mListView->setModel(mModel);
}

void CntCollectionView::reorderGroup()
{
    // wait for specs
}

void CntCollectionView::deleteGroup(QContact group)
{
    QString name = group.displayLabel();

    HbMessageBox *note = new HbMessageBox(hbTrId("txt_phob_info_delete_1").arg(name), HbMessageBox::MessageTypeQuestion);
    note->setPrimaryAction(new HbAction(hbTrId("txt_phob_button_delete"), note));
    note->setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"), note));
    HbAction *selected = note->exec();
    if (selected == note->primaryAction())
    {
        mViewManager->contactManager(SYMBIAN_BACKEND)->removeContact(group.localId());
        mModel->removeGroup(group.localId());
        
        // disable delete group(s) button if only favorites group is present
        QContactDetailFilter groupFilter;
        groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
        groupFilter.setValue(QLatin1String(QContactType::TypeGroup));
        QList<QContactLocalId> groupContactIds = mViewManager->contactManager(SYMBIAN_BACKEND)->contactIds(groupFilter);
        if (groupContactIds.count() < 2)
        {
            mDeleteGroupsAction->setEnabled(false);
        }
    }
    delete note;
}

void CntCollectionView::deleteGroups()
{
    // save the group here
    CntGroupDeletePopup *groupDeletePopup = new CntGroupDeletePopup(mViewManager->contactManager(SYMBIAN_BACKEND));
    
    groupDeletePopup->populateListOfGroup();
    HbAction* action = groupDeletePopup->exec();
    if (action == groupDeletePopup->primaryAction())
    {   
        groupDeletePopup->deleteGroup();
    }
    
    delete groupDeletePopup;
    //refresh the page 
    refreshDataModel();
    
    // disable delete group(s) button if only favorites group is present
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QLatin1String(QContactType::TypeGroup));
    QList<QContactLocalId> groupContactIds = mViewManager->contactManager(SYMBIAN_BACKEND)->contactIds(groupFilter);
    if (groupContactIds.count() < 2)
    {
        mDeleteGroupsAction->setEnabled(false);
    }
}

void CntCollectionView::disconnectAll()
{
    // wait for specs
}

// EOF
