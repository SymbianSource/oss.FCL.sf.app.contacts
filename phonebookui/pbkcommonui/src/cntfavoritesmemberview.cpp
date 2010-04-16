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

#include <hblistview.h>
#include <hbtoolbar.h>
#include <hbmenu.h>
#include <hbaction.h>

const char *CNT_FAVORITESMEMBERVIEW_XML = ":/xml/contacts_favmember.docml";

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntFavoritesMemberView::CntFavoritesMemberView(CntViewManager *viewManager, QGraphicsItem *parent)
:CntBaseView(viewManager, parent),
mManageFavoritesAction(0), 
mFavoritesMenu(0),
mFavoriteListView(0)
{
    bool ok = false;
    ok = loadDocument(CNT_FAVORITESMEMBERVIEW_XML);
    
    if (ok)
    {
        QGraphicsWidget *content = findWidget(QString("content"));
        setWidget(content);
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_favmember.docml");
    }
}

/*!
Destructor
*/
CntFavoritesMemberView::~CntFavoritesMemberView()
{
delete mManageFavoritesAction; 
delete mFavoritesMenu;
delete mContact;
}

/*!
Save selections
*/
void CntFavoritesMemberView::aboutToCloseView()
{
    CntViewParameters viewParameters;
    viewManager()->back(viewParameters);
}

void CntFavoritesMemberView::activateView(const CntViewParameters &viewParameters)
{
    //group box
    HbGroupBox* groupBox = static_cast<HbGroupBox *>(findWidget(QString("groupBox")));
    mContact = new QContact(viewParameters.selectedContact());

    mFavoriteListView = static_cast<HbListView*>(findWidget(QString("cnt_listview_favorites")));
    connect(mFavoriteListView, SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)),
                      this,  SLOT(onLongPressed(HbAbstractViewItem *, const QPointF &)));
    
    QContactRelationshipFilter rFilter;
    rFilter.setRelationshipType(QContactRelationship::HasMember);
    rFilter.setRelatedContactRole(QContactRelationshipFilter::First);
    rFilter.setRelatedContactId(mContact->id());
           
    contactModel()->setFilterAndSortOrder(rFilter);
    contactModel()->showMyCard(false);
    mFavoriteListView->setModel(contactModel());
    }

void CntFavoritesMemberView::manageFavorites()
{
    // call a dialog to display the contacts
    CntGroupSelectionPopup *groupSelectionPopup = new CntGroupSelectionPopup(contactManager(), mContact);
    mFavoriteListView->setModel(0);
    groupSelectionPopup->populateListOfContact();

    HbAction* action = groupSelectionPopup->exec();
    if (action == groupSelectionPopup->primaryAction())
    {
        groupSelectionPopup->saveOldGroup();
    }
    delete groupSelectionPopup;
	
    mFavoriteListView->setModel(contactModel());
}

/*!
Add actions also to toolbar
*/
void CntFavoritesMemberView::addMenuItems()
{
mManageFavoritesAction = new HbAction(hbTrId("Manage favorites"));
mFavoritesMenu = new HbMenu();
mFavoritesMenu->addAction(mManageFavoritesAction);

connect(mManageFavoritesAction, SIGNAL(triggered()),
        this, SLOT (manageFavorites()));

setMenu(mFavoritesMenu);
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
    QContact selectedContact = contactModel()->contact(index);
    CntViewParameters viewParameters(CntViewParameters::commLauncherView);
    viewParameters.setSelectedContact(selectedContact);
    viewManager()->changeView(viewParameters);  
}

void CntFavoritesMemberView::editContact(const QModelIndex &index)
{
    QContact selectedContact = contactModel()->contact(index);
    CntViewParameters viewParameters(CntViewParameters::editView);
    viewParameters.setSelectedContact(selectedContact);
    viewManager()->changeView(viewParameters);
}

/*!
Called after user clicked on the listview.
*/
void CntFavoritesMemberView::removeFromFavorites(const QModelIndex &index)
{
    // get contact id using index
        QContact selectedContact = contactModel()->contact(index);
        QContactRelationship relationship;
        relationship.setRelationshipType(QContactRelationship::HasMember);
        relationship.setFirst(mContact->id());
        relationship.setSecond(selectedContact.id());
        contactManager()->removeRelationship(relationship);
}

