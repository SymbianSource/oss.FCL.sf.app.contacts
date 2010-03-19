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
//USER
#include "logsmatchesview.h"
#include "logscomponentrepository.h"
#include "logsabstractviewmanager.h"
#include "logsdefs.h"
#include "logslogger.h"
#include "logsmodel.h"
#include "logsmatchesmodel.h"
#include "logscall.h"

//SYSTEM
#include <hblistview.h>
#include <hbaction.h>
#include <dialpad.h>
#include <hblineedit.h>
#include <hbabstractviewitem.h>
#include <hblistviewitem.h>

Q_DECLARE_METATYPE(LogsMatchesModel*)

// -----------------------------------------------------------------------------
// LogsMatchesView::LogsMatchesView
// -----------------------------------------------------------------------------
//
LogsMatchesView::LogsMatchesView( 
    LogsComponentRepository& repository, LogsAbstractViewManager& viewManager )
    : LogsBaseView(LogsMatchesViewId, repository, viewManager),
      mListView(0),
      mModel(0)
{
    LOGS_QDEBUG( "logs [UI] <-> LogsMatchesView::LogsMatchesView()" );
}
    
// -----------------------------------------------------------------------------
// LogsMatchesView::~LogsMatchesView
// -----------------------------------------------------------------------------
//
LogsMatchesView::~LogsMatchesView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::~LogsMatchesView()" );
    delete mModel;
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::~LogsMatchesView()" );
}

// -----------------------------------------------------------------------------
// LogsMatchesView::activated
// -----------------------------------------------------------------------------
//
void LogsMatchesView::activated(bool showDialer, QVariant args)
{
    //base class handling first
    LogsBaseView::activated(showDialer, args);
    
    LogsMatchesModel* model = qVariantValue<LogsMatchesModel*>(args);
    updateModel(model);
    dialpadEditorTextChanged();
    
    activateEmptyListIndicator(mModel);
}

// -----------------------------------------------------------------------------
// LogsMatchesView::deactivated
// -----------------------------------------------------------------------------
//
void LogsMatchesView::deactivated()
{
    //base class handling first
    LogsBaseView::deactivated();
    
    deactivateEmptyListIndicator(mModel);
    
    delete mModel;
    mModel = 0;
}

// -----------------------------------------------------------------------------
// LogsMatchesView::initView
// -----------------------------------------------------------------------------
//
void LogsMatchesView::initView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::initView()" );    
    //base class handling first
    LogsBaseView::initView();
    
    initListWidget();
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::initView()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QAbstractItemModel* LogsMatchesView::model() const
{
    return mModel;
}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsMatchesView::callKeyPressed()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::callKeyPressed()" );
    
    if ( mDialpad->editor().text().length() > 0 ){
        // Call to inputted number
        LogsCall::callToNumber( LogsCall::TypeLogsVoiceCall, mDialpad->editor().text() );
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::callKeyPressed()" );
}

// -----------------------------------------------------------------------------
// LogsMatchesView::initListWidget
// -----------------------------------------------------------------------------
//
void LogsMatchesView::initListWidget()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::initListWidget()" );
    mListView = qobject_cast<HbListView*>
                    ( mRepository.findWidget( logsListWidgetId ) );
    Q_ASSERT_X(mListView != 0, "logs [UI] ", "couldn't find list widget !!");
    
    mListView->setItemRecycling(true);
    mListView->setFrictionEnabled(true);
    mListView->listItemPrototype()->setTextFormat(Qt::RichText);
    
    connect( mListView, SIGNAL(activated(const QModelIndex)),
             this, SLOT(initiateCallback(const QModelIndex)) );

    connect( mListView,
             SIGNAL(longPressed(HbAbstractViewItem*, const QPointF&)),
             this,
             SLOT(showListItemMenu(HbAbstractViewItem*, const QPointF&)) );
   
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::initListWidget() " );
}

// -----------------------------------------------------------------------------
// LogsMatchesView::updateModel
// -----------------------------------------------------------------------------
//
void LogsMatchesView::updateModel(LogsMatchesModel* model)
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::updateModel()" );
    
    if ( !model || model != mModel ){
        LOGS_QDEBUG( "logs [UI]     Delete old model" );
        delete mModel;
        mModel = 0;
    }
    
    if ( !model ){
        LOGS_QDEBUG( "logs [UI]     Create new model" );
        mModel = mRepository.model()->logsMatchesModel();
    } else {
        LOGS_QDEBUG( "logs [UI]     Use given model" );
        mModel = model;
    }
    if ( mListView ) {
        mListView->setModel( mModel );//ownership not transferred
    }
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::updateModel()" );
}

// -----------------------------------------------------------------------------
// LogsMatchesView::dialpadEditorTextChanged
// -----------------------------------------------------------------------------
//
void LogsMatchesView::dialpadEditorTextChanged()
{
    updateCallButton();
    
    QString pattern = mDialpad->editor().text();
    if ( pattern.isEmpty() ){
        // Treat empty input field same was as back press
        handleBackSoftkey();
    } else if ( mModel ) {
        mModel->logsMatches( pattern );
    }
}


// -----------------------------------------------------------------------------
// LogsMatchesView::dialpadClosed
// -----------------------------------------------------------------------------
//
void LogsMatchesView::dialpadClosed()
{
    updateWidgetsSizeAndLayout();
}

// -----------------------------------------------------------------------------
// LogsMatchesView::updateWidgetsSizeAndLayout
// -----------------------------------------------------------------------------
//
void LogsMatchesView::updateWidgetsSizeAndLayout()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::updateWidgetsSizeAndLayout()" );
    if ( mListView ) {
        updateListLayoutName(*mListView);
        updateListSize(mLayoutSectionName);
    }
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::updateWidgetsSizeAndLayout()" );
}
