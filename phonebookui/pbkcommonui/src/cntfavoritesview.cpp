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

#include "cntfavoritesview.h"
#include "cntfetchcontactsview.h"
#include "cntglobal.h"
#include <hbpushbutton.h>
#include <hbaction.h>
#include <hbview.h>
#include <hbmainwindow.h>
#include <QSet>

const char *CNT_FAVORITE_UI_XML = ":/xml/contacts_favorite.docml";

CntFavoritesView::CntFavoritesView() :
    mContact(NULL),
    mView(NULL),
    mSoftkey(NULL),
    mViewManager(NULL),
    mFetchView(NULL)
{
    bool ok = false;
    mDocumentLoader.load(CNT_FAVORITE_UI_XML, &ok);

    if (ok)
    {
        mView = static_cast<HbView*>(mDocumentLoader.findWidget(QString("favoritesView")));
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_favorite.docml");
    }
    
    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
}
/*!
Destructor
*/
CntFavoritesView::~CntFavoritesView()
{
    mView->deleteLater();
    
    delete mContact;
    mContact = 0;

    delete mFetchView;
    mFetchView = 0;
}

void CntFavoritesView::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    if (mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);
    
    HbMainWindow* window = mView->mainWindow();
    connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
    setOrientation(window->orientation());
    
    mContact = new QContact(aArgs.value(ESelectedGroupContact).value<QContact>());
    mViewManager = aMgr;

    HbPushButton *addButton = static_cast<HbPushButton*>(mDocumentLoader.findWidget(QString("cnt_button_add")));
    connect(addButton, SIGNAL(clicked()), this, SLOT(openSelectionPopup()));
}

void CntFavoritesView::deactivate()
{
}

void CntFavoritesView::openSelectionPopup()
{
    QSet<QContactLocalId> emptySet;
    
    if (!mFetchView) {
        mFetchView = new CntFetchContacts(getContactManager());
        connect(mFetchView, SIGNAL(clicked()), this, SLOT(handleMemberSelection()));
    }
    mFetchView->setDetails(hbTrId("txt_phob_subtitle_favorites"), hbTrId("txt_common_button_save"));
    mFetchView->displayContacts(CntFetchContacts::popup, HbAbstractItemView::MultiSelection, emptySet);
}

void CntFavoritesView::handleMemberSelection()
{
    QSet<QContactLocalId> members = mFetchView->getSelectedContacts();
    QList<QContactRelationship> memberships;
    bool saveChanges = !mFetchView->wasCanceled();

    delete mFetchView;
    mFetchView = 0;

    if (!saveChanges || members.count() == 0) {
        showPreviousView();
        return;
    }

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
        getContactManager()->saveRelationships(&memberships, &errors);
    }

    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, FavoritesMemberView);
    QVariant var;
    var.setValue(*mContact);
    viewParameters.insert(ESelectedGroupContact, var);
    mViewManager->changeView(viewParameters);
}

void CntFavoritesView::setOrientation(Qt::Orientation orientation)
{
    if (orientation == Qt::Vertical) 
    {
        // reading "portrait" section
        mDocumentLoader.load(CNT_FAVORITE_UI_XML, "portrait");
    } 
    else 
    {
        // reading "landscape" section
        mDocumentLoader.load(CNT_FAVORITE_UI_XML, "landscape");
    }
}

void CntFavoritesView::showPreviousView()
{
    CntViewParameters args;
    mViewManager->back(args);
}

QContactManager* CntFavoritesView::getContactManager()
{
    if (!mViewManager) {
        return NULL;
    }

    return mViewManager->contactManager(SYMBIAN_BACKEND);
}

// end of file
