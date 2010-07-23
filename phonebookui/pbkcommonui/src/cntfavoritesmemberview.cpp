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

#include "cntfavoritesmemberview.h"
#include "cntfetchcontactpopup.h"
#include "cntglobal.h"

#include <hblistview.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbindexfeedback.h>
#include <hbscrollbar.h>
#include <hbview.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbparameterlengthlimiter.h>
#include <cntlistmodel.h>
#include <hbframebackground.h>
#include <xqservicerequest.h>

const char *CNT_FAVORITESMEMBERVIEW_XML = ":/xml/contacts_favmember.docml";

CntFavoritesMemberView::CntFavoritesMemberView() :
    mContact(NULL),
    mModel(NULL),
    mFavoriteListView(NULL), 
    mViewManager(NULL),
    mFetchView(NULL)
{
    bool ok = false;
    mDocumentLoader.load(CNT_FAVORITESMEMBERVIEW_XML, &ok);
  
    if (ok)
    {
        mView = static_cast<HbView*>(mDocumentLoader.findWidget(QString("view")));
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_favmember.docml");
    }
    
    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    
    // menu actions
    mManageFavoritesAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:manageFavorite"));
    connect(mManageFavoritesAction, SIGNAL(triggered()), this, SLOT(manageFavorites()));
}

/*!
Destructor
*/
CntFavoritesMemberView::~CntFavoritesMemberView()
{
    mView->deleteLater();

    delete mManageFavoritesAction; 
    mManageFavoritesAction = 0;

    delete mContact;
    mContact = 0;

    delete mModel;
    mModel = 0;
}

void CntFavoritesMemberView::showPreviousView()
{
    CntViewParameters viewParameters;
    mViewManager->back(viewParameters);
}

void CntFavoritesMemberView::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    if (mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);   
    
    mContact = new QContact(aArgs.value(ESelectedGroupContact).value<QContact>());
    mViewManager = aMgr;

    mFavoriteListView = static_cast<HbListView*> (mDocumentLoader.findWidget("listView"));
    mFavoriteListView->setUniformItemSizes(true);
    mFavoriteListView->setFrictionEnabled(true);
    mFavoriteListView->setScrollingStyle(HbScrollArea::PanWithFollowOn);
    mFavoriteListView->verticalScrollBar()->setInteractive(true);
    connect(mFavoriteListView, SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)),
                      this,  SLOT(onLongPressed(HbAbstractViewItem *, const QPointF &)));

    HbIndexFeedback *indexFeedback = new HbIndexFeedback(mView);
    indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
    indexFeedback->setItemView(mFavoriteListView);
    
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    
    mFavoriteListView->itemPrototypes().first()->setDefaultFrame(frame);
    mFavoriteListView->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);
    mFavoriteListView->listItemPrototype()->setGraphicsSize(HbListViewItem::Thumbnail);

    if (!mModel)
    {
        createModel();
    }

    connect(mFavoriteListView, SIGNAL(activated (const QModelIndex&)), this,
            SLOT(openContact(const QModelIndex&)));
}

void CntFavoritesMemberView::deactivate()
{
    
}

void CntFavoritesMemberView::manageFavorites()
{
    QContactRelationshipFilter membersFilter;
    membersFilter.setRelationshipType(QContactRelationship::HasMember);
    membersFilter.setRelatedContactRole(QContactRelationship::First);
    membersFilter.setRelatedContactId(mContact->id());   
    mOriginalGroupMembers = getContactManager()->contactIds(membersFilter).toSet();
    
    /*
    if (!mFetchView) {
        mFetchView = new CntFetchContacts(*getContactManager());
        connect(mFetchView, SIGNAL(clicked()), this, SLOT(handleManageFavorites()));
    }
    mFetchView->setDetails(hbTrId("txt_phob_subtitle_favorites"), hbTrId("txt_common_button_save"));
    mFetchView->displayContacts(HbAbstractItemView::MultiSelection, mOriginalGroupMembers);
    */
    CntFetchContactPopup* popup = CntFetchContactPopup::createMultiSelectionPopup(
            hbTrId("txt_phob_subtitle_favorites"), 
            hbTrId("txt_common_button_save"),
            *getContactManager());
    connect( popup, SIGNAL(fetchReady(QSet<QContactLocalId>)), this, SLOT(handleManageFavorites(QSet<QContactLocalId>)) );
    popup->setSelectedContacts( mOriginalGroupMembers );
    popup->showPopup();
}

void CntFavoritesMemberView::handleManageFavorites(QSet<QContactLocalId> aIds)
{
/*
    QSet<QContactLocalId> selectedContacts = mFetchView->getSelectedContacts();
    bool saveChanges = !mFetchView->wasCanceled();

    delete mFetchView;
    mFetchView = 0;
    
    if (!saveChanges) {
        return;
    }
*/
    for (int i = 0; i < 2; ++i) {
        // first iteration processes added members, second removed members
        QSet<QContactLocalId> members = (i == 0 ? aIds - mOriginalGroupMembers
                                                : mOriginalGroupMembers - aIds);
        QList<QContactRelationship> memberships;

        foreach (QContactLocalId id, members) {
            QContact contact = getContactManager()->contact(id);
            QContactRelationship membership;
            membership.setRelationshipType(QContactRelationship::HasMember);
            membership.setFirst(mContact->id());
            membership.setSecond(contact.id());
            memberships.append(membership);
        }

        if (!memberships.isEmpty()) {
            QMap<int, QContactManager::Error> errors;
            if (i == 0) {
                getContactManager()->saveRelationships(&memberships, &errors);
            }
            else {
                getContactManager()->removeRelationships(memberships, &errors);
            }
        }
    }
}

void CntFavoritesMemberView::createModel()
{
    QContactRelationshipFilter rFilter;
    rFilter.setRelationshipType(QContactRelationship::HasMember);
    rFilter.setRelatedContactRole(QContactRelationship::First);
    rFilter.setRelatedContactId(mContact->id());

    mModel = new CntListModel(mViewManager->contactManager(SYMBIAN_BACKEND), rFilter, false);
    mFavoriteListView->setModel(mModel);
}

/*!
Called when a list item is longpressed
*/
void CntFavoritesMemberView::onLongPressed(HbAbstractViewItem *aItem, const QPointF &aCoords)
{
    QVariant data(aItem->modelIndex().row());
   
    QModelIndex index = aItem->modelIndex();
    QVariant variant = index.data(Qt::UserRole+1);
    const QMap<QString, QVariant> map = variant.toMap();

    HbMenu *menu = new HbMenu();
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->setPreferredPos( aCoords );
   
    HbAction *openContactAction = 0;
    HbAction *editContactAction = 0;
    HbAction *removeFromFavoritesAction = 0;
    HbAction *sendToHsAction = 0;

    QString action = map.value("action").toString();

    openContactAction = menu->addAction(hbTrId("txt_common_menu_open"));
    editContactAction = menu->addAction(hbTrId("txt_common_menu_edit"));
    removeFromFavoritesAction = menu->addAction(hbTrId("txt_phob_menu_remove_from_favorites"));
    sendToHsAction = menu->addAction(hbTrId("txt_phob_menu_send_to_homescreen"));
    
    openContactAction->setData( data );
    editContactAction->setData( data );
    removeFromFavoritesAction->setData( data );
    sendToHsAction->setData( data );

    menu->open(this, SLOT(handleMenu(HbAction*)));
}

void CntFavoritesMemberView::handleMenu(HbAction* action)
{
    int row = action->data().toInt();
    HbMenu *menuItem = static_cast<HbMenu*>(sender());
    QModelIndex index = mModel->index(row, 0);
       
    int id = index.data(Qt::UserRole).toInt();

    if ( action == menuItem->actions().first() )
        {
        openContact(index);
        }
    else if (action == menuItem->actions().at(1))
        {
        editContact(index);
        }
    else if (action == menuItem->actions().at(2))
        {
        removeFromFavorites(index);
        }
   else if (action == menuItem->actions().at(3))
       {
       sendToHs(index);
       }
}

void CntFavoritesMemberView::openContact(const QModelIndex &index)
{
    QContact selectedContact = mModel->contact(index);
    
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, contactCardView);
    QVariant var;
    var.setValue(selectedContact);
    viewParameters.insert(ESelectedContact, var);
    QVariant varGroup;
    varGroup.setValue(*mContact);
    viewParameters.insert(ESelectedGroupContact, varGroup);
    mViewManager->changeView(viewParameters);
}

void CntFavoritesMemberView::editContact(const QModelIndex &index)
{
    QContact selectedContact = mModel->contact(index);
    
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, editView);
    QVariant var;
    var.setValue(selectedContact);
    viewParameters.insert(ESelectedContact, var);
    QVariant varGroup;
    varGroup.setValue(*mContact);
    viewParameters.insert(ESelectedGroupContact, varGroup);
    mViewManager->changeView(viewParameters);
}

/*!
Called after user clicked on the listview.
*/
void CntFavoritesMemberView::removeFromFavorites(const QModelIndex &index)
{
    // get contact id using index
    QContact selectedContact = mModel->contact(index);
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(mContact->id());
    relationship.setSecond(selectedContact.id());
    getContactManager()->removeRelationship(relationship);
}

QContactManager* CntFavoritesMemberView::getContactManager()
{
    if (!mViewManager) {
        return NULL;
    }

    return mViewManager->contactManager(SYMBIAN_BACKEND);
}

/*!
Called after user clicked on the listview.
*/
void CntFavoritesMemberView::sendToHs(const QModelIndex &index)
{
    QVariantHash preferences;
    preferences["contactId"] = mModel->contact(index).id().localId();
    
    XQServiceRequest snd("com.nokia.symbian.IHomeScreenClient",
                         "addWidget(QString,QVariantHash)",
                         false);
    snd << QString("contactwidgethsplugin");
    snd << preferences;
    snd.send();
}

