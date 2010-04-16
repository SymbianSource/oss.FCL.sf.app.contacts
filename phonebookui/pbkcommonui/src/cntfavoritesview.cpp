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
#include "cntgroupselectionpopup.h"
#include "qtpbkglobal.h"
#include <hbpushbutton.h>
#include <hbaction.h>
#include <hbview.h>
#include <hbmainwindow.h>

const char *CNT_FAVORITE_UI_XML = ":/xml/contacts_favorite.docml";

CntFavoritesView::CntFavoritesView() :
    mContact(0),
    mView(0),
    mSoftkey(0),
    mViewManager(0)
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
}

void CntFavoritesView::activate( CntAbstractViewManager* aMgr, const CntViewParameters& aArgs )
{
    if (mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);
    
    HbMainWindow* window = mView->mainWindow();
    connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
    setOrientation(window->orientation());
    
    mContact = new QContact(aArgs.selectedContact());
    mViewManager = aMgr;

    HbPushButton *addButton = static_cast<HbPushButton*>(mDocumentLoader.findWidget(QString("cnt_button_add")));
    connect(addButton, SIGNAL(clicked()), this, SLOT(openSelectionPopup()));
}

void CntFavoritesView::deactivate()
{

}

void CntFavoritesView::openSelectionPopup()
{
   // call a dialog to display the contacts
   CntGroupSelectionPopup *groupSelectionPopup = new CntGroupSelectionPopup(mViewManager->contactManager(SYMBIAN_BACKEND), mContact);
   groupSelectionPopup->populateListOfContact();
   HbAction* action = groupSelectionPopup->exec();
   
   if (action == groupSelectionPopup->primaryAction())
   {
       groupSelectionPopup->saveOldGroup();
       delete groupSelectionPopup;
       CntViewParameters viewParameters(CntViewParameters::FavoritesMemberView);
       viewParameters.setSelectedContact(*mContact);
       mViewManager->changeView(viewParameters);
   }
   else if (action == groupSelectionPopup->secondaryAction())
   {
       delete groupSelectionPopup;
       showPreviousView();
   }
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

// end of file
