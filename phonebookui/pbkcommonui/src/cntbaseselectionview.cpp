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

#include <hbview.h>
#include <hblistview.h>
#include <hbindexfeedback.h>
#include <hbscrollbar.h>
#include <hbdocumentloader.h>
#include <hbaction.h>
#include <mobcntmodel.h>

const char *CNT_SELECTION_LISTVIEW_UI_XML = ":/xml/contacts_list.docml";

CntBaseSelectionView::CntBaseSelectionView() : QObject()
{
    mDocument = new HbDocumentLoader();
    
    bool ok;
    mDocument->load( CNT_SELECTION_LISTVIEW_UI_XML, &ok );
    if ( !ok )
    {
        qFatal( "Unable to load %S", CNT_SELECTION_LISTVIEW_UI_XML );
    }
    mView = static_cast<HbView*>( mDocument->findWidget("view") );
    mListView = static_cast<HbListView*>( mDocument->findWidget("listView") );

    mListView->setFrictionEnabled(true);
    mListView->setScrollingStyle(HbScrollArea::PanWithFollowOn);
    mListView->verticalScrollBar()->setInteractive(true);
    mListView->setUniformItemSizes(true);

    HbIndexFeedback *indexFeedback = new HbIndexFeedback(mView);
    indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
    indexFeedback->setItemView(mListView);
    
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect( mSoftkey, SIGNAL(triggered()), this, SLOT(closeView()) );
}

CntBaseSelectionView::~CntBaseSelectionView()
{
    delete mDocument;
}

void CntBaseSelectionView::activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs )
{
    mMgr = aMgr;
    
    if ( mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);
    
    HbMainWindow* window = mView->mainWindow();
    if ( window )
    {
        //connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
        //setOrientation(window->orientation());
    }
    
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,QContactName::FieldFirst);
    sortOrderFirstName.setCaseSensitivity(Qt::CaseInsensitive);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,QContactName::FieldLast);
    sortOrderLastName.setCaseSensitivity(Qt::CaseInsensitive);

    QList<QContactSortOrder> sortOrders;
    sortOrders.append(sortOrderFirstName);
    sortOrders.append(sortOrderLastName);
    
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    QString typeContact = QContactType::TypeContact;
    filter.setValue(typeContact);

    mListModel = new MobCntModel(mMgr->contactManager(SYMBIAN_BACKEND), filter, sortOrders);
    mListModel->showMyCard( false );
    
    mListView->setModel( mListModel );
    
    if ( aArgs.contains(ESelectionMode) ) {
        mListView->setSelectionMode( static_cast<HbAbstractItemView::SelectionMode>(aArgs.value(ESelectionMode).toInt()) );
    }
    
    emit viewOpened( aArgs );
}

void CntBaseSelectionView::deactivate()
{
}

bool CntBaseSelectionView::isDefault() const
{
    return false;
}

HbView* CntBaseSelectionView::view() const
{
    return mView;
}

void CntBaseSelectionView::closeView()
{
    emit viewClosed();
    
    CntViewParameters args;
    mMgr->back( args );
}

    
/*
CntBaseSelectionView::CntBaseSelectionView()
    : CntBaseView(viewManager, parent),
    mListView(NULL),
    mListLayout(NULL),
    mSelectionMode(HbAbstractItemView::NoSelection)
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


void CntBaseSelectionView::activateView(const CntViewParameters viewParameters)
{
    listView()->setModel(contactModel());
    
    if (viewParameters.contains(ESelectionMode)) {
        mSelectionMode = static_cast<HbAbstractItemView::SelectionMode>(viewParameters.value(ESelectionMode).toInt());
    }    
    listView()->setSelectionMode(mSelectionMode);
}

void CntBaseSelectionView::addItemsToLayout()
{
    listView();
    setWidget(findWidget(QString("container")));
}

QGraphicsLinearLayout *CntBaseSelectionView::listLayout()
{
    QGraphicsWidget *w = findWidget(QString("container"));
    return static_cast<QGraphicsLinearLayout*>(w->layout());
}

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
*/
// EOF
