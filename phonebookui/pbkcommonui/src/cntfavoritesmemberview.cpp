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
#include "cntgroupselectionpopup.h"
#include "qtpbkglobal.h"

#include <hblistview.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbview.h>
#include <hbaction.h>
#include <hblabel.h>
#include <mobcntmodel.h>
#include <hbframebackground.h>

const char *CNT_FAVORITESMEMBERVIEW_XML = ":/xml/contacts_favmember.docml";

CntFavoritesMemberView::CntFavoritesMemberView() :
mContact(NULL),
mModel(NULL),
mFavoriteListView(NULL), 
mViewManager(NULL)
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
    
    mContact = new QContact(aArgs.value(ESelectedContact).value<QContact>());
    mViewManager = aMgr;

    mFavoriteListView = static_cast<HbListView*> (mDocumentLoader.findWidget("listView"));
    mFavoriteListView->setUniformItemSizes(true);
    connect(mFavoriteListView, SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)),
                      this,  SLOT(onLongPressed(HbAbstractViewItem *, const QPointF &)));
    
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mFavoriteListView->itemPrototypes().first()->setDefaultFrame(frame);

    mFavoriteListView->listItemPrototype()->setGraphicsSize(HbListViewItem::Thumbnail);

    if (!mModel)
    {
        QContactRelationshipFilter rFilter;
        rFilter.setRelationshipType(QContactRelationship::HasMember);
        rFilter.setRelatedContactRole(QContactRelationship::First);
        rFilter.setRelatedContactId(mContact->id());

        QContactSortOrder sortOrderFirstName;
        sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
            QContactName::FieldFirst);
        sortOrderFirstName.setCaseSensitivity(Qt::CaseInsensitive);

        QContactSortOrder sortOrderLastName;
        sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
            QContactName::FieldLast);
        sortOrderLastName.setCaseSensitivity(Qt::CaseInsensitive);

        QList<QContactSortOrder> sortOrders;
        sortOrders.append(sortOrderFirstName);
        sortOrders.append(sortOrderLastName);

        mModel = new MobCntModel(mViewManager->contactManager(SYMBIAN_BACKEND), rFilter, sortOrders, false);
    }

    mFavoriteListView->setModel(mModel);

    connect(mFavoriteListView, SIGNAL(activated (const QModelIndex&)), this,
            SLOT(openContact(const QModelIndex&)));
}

void CntFavoritesMemberView::deactivate()
{
    
}

void CntFavoritesMemberView::manageFavorites()
{
    // call a dialog to display the contacts
    CntGroupSelectionPopup *groupSelectionPopup = new CntGroupSelectionPopup(mViewManager->contactManager(SYMBIAN_BACKEND), mContact);
    mFavoriteListView->setModel(0);
    groupSelectionPopup->populateListOfContact();

    HbAction* action = groupSelectionPopup->exec();
    if (action == groupSelectionPopup->primaryAction())
    {
        groupSelectionPopup->saveOldGroup();
    }
    delete groupSelectionPopup;
	
    mFavoriteListView->setModel(mModel);
}


/*!
Called when a list item is longpressed
*/
void CntFavoritesMemberView::onLongPressed (HbAbstractViewItem *aItem, const QPointF &aCoords)
{
    QModelIndex index = aItem->modelIndex();
    QVariant variant = index.data(Qt::UserRole+1);
    const QMap<QString, QVariant> map = variant.toMap();

    HbMenu *menu = new HbMenu();
   
    HbAction *openContactAction = 0;
    HbAction *editContactAction = 0;
    HbAction *removeFromFavoritesAction = 0;
    HbAction *sendToHsAction = 0;

    QString action = map.value("action").toString();

    openContactAction = menu->addAction(hbTrId("txt_common_menu_open"));
    editContactAction = menu->addAction(hbTrId("txt_common_menu_edit"));
    removeFromFavoritesAction = menu->addAction(hbTrId("txt_phob_menu_remove_from_favorites"));
    sendToHsAction = menu->addAction(hbTrId("Send to HS"));

    HbAction *selectedAction = menu->exec(aCoords);

    if (selectedAction)
    {
        if (selectedAction == openContactAction)
        {
            openContact(index);
        }
        else if (selectedAction == editContactAction)
        {
            editContact(index);
        }
        else if (selectedAction == removeFromFavoritesAction)
        {
            removeFromFavorites(index);
        }
        else if (selectedAction == sendToHsAction)
        {
//            sendToHs(index);
        }
    }
    menu->deleteLater();
}

void CntFavoritesMemberView::openContact(const QModelIndex &index)
{
    QContact selectedContact = mModel->contact(index);
    
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, commLauncherView);
    QVariant var;
    var.setValue(selectedContact);
    viewParameters.insert(ESelectedContact, var);
    QVariant varGroup;
    varGroup.setValue(*mContact);
    viewParameters.insert(ESelectedGroupContact, varGroup);
    viewParameters.insert(ESelectedAction, "FromGroupMemberView");
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
        mViewManager->contactManager(SYMBIAN_BACKEND)->removeRelationship(relationship);
}

