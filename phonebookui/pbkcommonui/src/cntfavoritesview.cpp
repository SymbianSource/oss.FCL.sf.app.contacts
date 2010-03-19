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

/*!
\class CntFavoritesView
\brief

This is the namesview class that shows list of contacts for the user. View contains banner for OVI and a listview that shows actual contacts.
There is also toolbar and menu for navigating between different views. Instance of this class is created by our viewmanager but view itself is
owned by the mainwindow which will also delete it in the end.

*/

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)

*/
CntFavoritesView::CntFavoritesView(CntViewManager *viewManager, QGraphicsItem *parent)
    : CntBaseListView(viewManager, parent)

{
setBannerName(hbTrId("txt_phob_subtitle_favorites"));
}

/*!
Destructor
*/
CntFavoritesView::~CntFavoritesView()
{

}

void CntFavoritesView::openFetch()
{
    CntViewParameters viewParameters(CntViewParameters::collectionFavoritesSelectionView);
    viewManager()->onActivateView(viewParameters);
}

void CntFavoritesView::openNamesList()
{
    CntViewParameters viewParameters(CntViewParameters::namesView);
    viewManager()->onActivateView(viewParameters);
}

void CntFavoritesView::aboutToCloseView()
{
    CntViewParameters viewParameters(CntViewParameters::collectionView);
    viewManager()->onActivateView(viewParameters);
}

/*!
Add actions also to toolbar
*/
void CntFavoritesView::addActionsToToolBar()
{
    //Add Action to the toolbar
    actions()->clearActionList();
    actions()->actionList() << actions()->baseAction("cnt:nameslist") << actions()->baseAction("cnt:collections")
        << actions()->baseAction("cnt:addfavorites");
    actions()->addActionsToToolBar(toolBar());
    
    connect(actions()->baseAction("cnt:nameslist"), SIGNAL(triggered()),
            this, SLOT(openNamesList()));

    connect(actions()->baseAction("cnt:collections"), SIGNAL(triggered()),
            this, SLOT(aboutToCloseView()));

    connect(actions()->baseAction("cnt:addfavorites"), SIGNAL(triggered()),
            this, SLOT(openFetch()));
}

// end of file
