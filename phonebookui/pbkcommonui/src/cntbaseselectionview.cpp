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

#include "cntbaseselectionview.h"

#include <hblistview.h>
#include <hblistviewitem.h>
#include <QGraphicsLinearLayout>
#include <hbframebackground.h>

const char *CNT_SELECTION_LISTVIEW_UI_XML = ":/xml/contacts_list.docml";

CntBaseSelectionView::CntBaseSelectionView(CntViewManager *viewManager, QGraphicsItem *parent, HbAbstractItemView::SelectionMode newMode)
    : CntBaseView(viewManager, parent),
    mListView(0),
    mListLayout(0),
    mSelectionMode(newMode)
{
    bool ok = false;
    ok = loadDocument(CNT_SELECTION_LISTVIEW_UI_XML);
    if (!ok)
    {
       qFatal("Unable to read :/xml/contacts_list.docml");
    }
}

CntBaseSelectionView::~CntBaseSelectionView()
{
}

void CntBaseSelectionView::setupView()
{
    addItemsToLayout();
    CntBaseView::setupView();
}


void CntBaseSelectionView::activateView(const CntViewParameters &viewParameters)
{
    Q_UNUSED(viewParameters)
    listView()->setModel(contactModel());
}

/*!
Adds created controls to the layout
*/
void CntBaseSelectionView::addItemsToLayout()
{
    listView();
    setWidget(findWidget(QString("container")));
}

/*!
\return pointer to listLayout component
*/
QGraphicsLinearLayout *CntBaseSelectionView::listLayout()
{
    QGraphicsWidget *w = findWidget(QString("container"));
    return static_cast<QGraphicsLinearLayout*>(w->layout());
}

/*!
\return pointer to HbListView component
*/
HbListView *CntBaseSelectionView::listView()
{
    if (mListView==0)
    {
        mListView = static_cast<HbListView*>(findWidget(QString("listView")));

        HbListViewItem *prototype = mListView->listItemPrototype();
        prototype->setGraphicsSize(HbListViewItem::Thumbnail);

        mListView->setSelectionMode(mSelectionMode);
        
        mListView->setFrictionEnabled(true);
        mListView->setScrollingStyle(HbScrollArea::PanOrFlick);
        
        HbFrameBackground frame;
        frame.setFrameGraphicsName("qtg_fr_list_normal");
        frame.setFrameType(HbFrameDrawer::NinePieces);
        mListView->itemPrototypes().first()->setDefaultFrame(frame);
        
        if (mListView->selectionMode() == HbAbstractItemView::NoSelection)
        {
            connect(mListView, SIGNAL(activated(const QModelIndex&)), 
                    this, SLOT(onListViewActivated(const QModelIndex&)));
        }
    }
    return mListView;
}

QItemSelectionModel* CntBaseSelectionView::selectionModel()
{
    return listView()->selectionModel();
}

// EOF
