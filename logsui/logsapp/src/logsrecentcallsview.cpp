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
#include "logsrecentcallsview.h"
#include "logscomponentrepository.h"
#include "logsabstractviewmanager.h"
#include "logsmodel.h"
#include "logsdefs.h"
#include "logslogger.h"
#include "logscall.h"
#include "logsmessage.h"
#include "logscontact.h"
#include "logseffecthandler.h"
#include "logsmatchesmodel.h"

//SYSTEM
#include <hbview.h>
#include <hblistview.h>
#include <hbabstractviewitem.h>
#include <hblabel.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <dialpad.h>
#include <hbgesturefilter.h>
#include <hbgesture.h>
#include <hblineedit.h>
#include <hbgroupbox.h>
#include <QTimer>
#include <hbmessagebox.h>

Q_DECLARE_METATYPE(LogsMatchesModel*)

const int logsMissedCallsMarkingDelayMs = 1000;

// -----------------------------------------------------------------------------
// LogsRecentCallsView::LogsRecentCallsView
// -----------------------------------------------------------------------------
//
LogsRecentCallsView::LogsRecentCallsView( 
    LogsComponentRepository& repository, LogsAbstractViewManager& viewManager )
    : LogsBaseView(LogsRecentViewId, repository, viewManager),
      mViewName(0),
      mListView(0),
      mFilter(0),
      mCurrentView(LogsServices::ViewAll),
      mAppearingView(LogsServices::ViewAll),
      mMoveLeftInList(false),
      mEffectHandler(0),
      mListViewX(0),
      mMatchesModel(0),
      mMarkingMissedAsSeen(false)
{
    LOGS_QDEBUG( "logs [UI] <-> LogsRecentCallsView::LogsRecentCallsView()" );
    mModel = mRepository.model();
    	
    mConversionMap.insert(LogsServices::ViewAll, LogsBaseView::ViewAll);
 	mConversionMap.insert(LogsServices::ViewCalled, LogsBaseView::ViewCalled);
 	mConversionMap.insert(LogsServices::ViewReceived, LogsBaseView::ViewReceived);
    mConversionMap.insert(LogsServices::ViewMissed, LogsBaseView::ViewMissed);	
    	
    //TODO: taking away due to toolbar bug. If toolbar visibility changes on view
    //activation, there will be a crash due to previous view effect is playing
    //addViewSwitchingEffects();
}
    
// -----------------------------------------------------------------------------
// LogsRecentCallsView::~LogsRecentCallsView
// -----------------------------------------------------------------------------
//
LogsRecentCallsView::~LogsRecentCallsView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::~LogsRecentCallsView()" );
    
    delete mMatchesModel;
    mModel = 0;
    
    delete mEffectHandler;
    delete mFilter;
    
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::~LogsRecentCallsView()" );
}


// -----------------------------------------------------------------------------
// LogsRecentCallsView::activated
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::activated(bool showDialer, QVariant args)
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::activated()" );
    //base class handling first
    LogsBaseView::activated(showDialer, args);
    
    LogsServices::LogsView view = static_cast<LogsServices::LogsView>( args.toInt() );

    //View update is needed when we activate view for the first time (!mFilter)
    //or if view has to be changed
    if (  !mFilter || ( !args.isNull() && (mCurrentView != view) ) ) {
        updateView( view );
    }
    activateEmptyListIndicator(mFilter);
    
    mDialpad->editor().setText(QString());
    
    mModel->clearMissedCallsCounter();
    
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::activated()" );  
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::deactivated
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::deactivated()
{
    //base class handling first
    LogsBaseView::deactivated();
    
    deactivateEmptyListIndicator(mFilter);
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::isExitAllowed
// -----------------------------------------------------------------------------
//
bool LogsRecentCallsView::isExitAllowed()
{
    bool marking = markMissedCallsSeen();
    return !marking;
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::initView
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::initView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::initView()" );
    //base class handling first
    LogsBaseView::initView();
    
    mViewName = 
        qobject_cast<HbGroupBox*>( mRepository.findWidget( logsGroupboxViewNameId ) );
    
    addStringsToMap();
    initListWidget();
    
    mEffectHandler = new LogsEffectHandler;
    connect(mEffectHandler, SIGNAL(dissappearByMovingComplete()), 
            this, SLOT(dissappearByMovingComplete()));
    connect(mEffectHandler, SIGNAL(dissappearByFadingComplete()), 
            this, SLOT(dissappearByFadingComplete()));
    
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::initView()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::model
// -----------------------------------------------------------------------------
//
QAbstractItemModel* LogsRecentCallsView::model() const
{
    return mFilter;
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::callKeyPressed
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::callKeyPressed()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::callKeyPressed()" ); 
    // Call to topmost item in current list
    if ( mListView && mFilter && mFilter->hasIndex(0,0) ) {
        QModelIndex topIndex = mFilter->index(0,0);
        mListView->scrollTo( topIndex );
        mListView->setCurrentIndex( topIndex, QItemSelectionModel::Select );
        initiateCallback(topIndex);
    }  
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::callKeyPressed()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::markingCompleted
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::markingCompleted(int err)
{
    LOGS_QDEBUG_2( "logs [UI] -> LogsRecentCallsView::markingCompleted(), err", err );
    Q_UNUSED(err);
    mMarkingMissedAsSeen = false;
    emit exitAllowed();
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::markingCompleted()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::openDialpad
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::openDialpad()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::openDialpad()" );    
    LogsBaseView::openDialpad();
    
    // Create matches model already before any input to optimize 
    // first search
    if ( !mMatchesModel ){
        mMatchesModel = mModel->logsMatchesModel();
    }
    
    updateCallButton();

    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::openDialpad()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::dialpadEditorTextChanged
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::dialpadEditorTextChanged()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::dialpadEditorTextChanged()" );
    if ( mDialpad->editor().text().length() > 0 ) {
        QVariant arg = qVariantFromValue( mMatchesModel );
        if ( mViewManager.activateView( LogsMatchesViewId, true, arg ) ){
            mMatchesModel = 0; // Ownership was given to matches view
        }
    } else {
        updateCallButton();
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::dialpadEditorTextChanged()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::clearList
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::clearList()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::clearList()->" );
    if ( mFilter ) {
             	  
       HbMessageBox *note = new HbMessageBox("", HbMessageBox::MessageTypeQuestion);
       note->setHeadingWidget(new HbLabel(hbTrId("txt_dialer_ui_title_clear_list")));
       note->setText(hbTrId("txt_dialer_ui_info_all_call_events_will_be_remo"));
       note->setPrimaryAction(new HbAction(hbTrId("txt_common_button_ok"), note));
       note->setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"), note));
       HbAction *selected = note->exec();

       if (selected == note->primaryAction()){ 
        mModel->clearList( mFilter->clearType() );
       }
       delete note;
    }
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::clearList()<-" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateView
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateView(LogsServices::LogsView view)
{
    LOGS_QDEBUG_2( "logs [UI] -> LogsRecentCallsView::updateView(), view:", view );
    mCurrentView = view;
    LogsFilter::FilterType filter = getFilter( view );
    updateFilter(filter);
    updateViewName();
    updateContextMenuItems(mCurrentView);    
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::updateView()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::changeFilter
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::changeFilter(HbAction* action)
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::changeFilter()" );
    LogsServices::LogsView view = mActionMap.key( action->objectName(),
            LogsServices::ViewAll );
    updateContextMenuItems(view);
    changeView(view);

    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::changeFilter()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::handleBackSoftkey
// -----------------------------------------------------------------------------
//
void  LogsRecentCallsView::handleBackSoftkey()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::handleBackSoftkey()" );
    handleExit();
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::handleBackSoftkey()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::addStringsToMap
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::addStringsToMap()
{
 	mTitleMap.insert(LogsBaseView::ViewAll, tr("Recent calls"));
 	mTitleMap.insert(LogsBaseView::ViewCalled, tr("Dialled calls"));
 	mTitleMap.insert(LogsBaseView::ViewReceived, tr("Received calls"));
    mTitleMap.insert(LogsBaseView::ViewMissed, tr("Missed calls"));
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::initListWidget
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::initListWidget()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::initListWidget()" );
    mListView = qobject_cast<HbListView*> 
                    ( mRepository.findWidget( logsListWidgetId ) );
    Q_ASSERT_X(mListView != 0, "logs [UI] ", "couldn't find list widget !!");
    
    
     // Optimize memory usage, list reserves only memory for visible items
    mListView->setItemRecycling(true);
    
    // Optimizes speed as we know that list items have always the same size
    mListView->setUniformItemSizes(true); 
    
    connect(mListView, SIGNAL(activated(const QModelIndex)),
            this, SLOT(initiateCallback(const QModelIndex)));
    connect(mListView,
            SIGNAL(longPressed(HbAbstractViewItem*, const QPointF&)),
            this,
            SLOT(showListItemMenu(HbAbstractViewItem*, const QPointF&)));
    
    connect(mListView,SIGNAL(gestureSceneFilterChanged(HbGestureSceneFilter*)),
            this,SLOT(initializeGestures(HbGestureSceneFilter*)));
    
    // Need to set scrolling style back and forth to force
    // gestureSceneFilterChanged signal inside which we can
    // add additional gestures.
    mListView->setScrollingStyle(HbScrollArea::Pan);
    mListView->setScrollingStyle(HbScrollArea::PanOrFlick); 
    
    mListViewX = mListView->pos().x();

    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::initListWidget() " );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateFilter
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateFilter(LogsFilter::FilterType type)
{
    if ( mListView ) {
        LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::updateFilter()" );
        
        deactivateEmptyListIndicator(mFilter);
        
        handleMissedCallsMarking();
        
        delete mFilter;
        mFilter = 0;
        mFilter = new LogsFilter( type );
        mFilter->setSourceModel( mModel );
        
        mListView->setModel( mFilter );//ownership not transferred
        
        if ( mFilter->hasIndex(0,0) ) {
            mListView->scrollTo( mFilter->index(0,0) );
        }
        
        activateEmptyListIndicator(mFilter);
         
        LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::updateFilter() " );
    }  else {
        LOGS_QWARNING( "logs [UI] LogsRecentCallsView::updateFilter(), !no list widget!" );
    }
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateViewName
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateViewName()
{
    if ( mViewName ) {
       mViewName->setTitleText( mTitleMap.value(mConversionMap.value(mCurrentView))); 
    }
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateContextMenuItems
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateContextMenuItems(LogsServices::LogsView view)
{
    LOGS_QDEBUG_2( 
        "logs [UI] -> LogsRecentCallsView::updateContextMenuItems(), view:", view );
    if ( mShowFilterMenu ) {
        QString activeActionName = mActionMap.value(view);
        foreach (QAction* action, mShowFilterMenu->actions() ) {
            action->setChecked( action->objectName() == activeActionName );
        }
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::updateContextMenuItems()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::getFilter
// -----------------------------------------------------------------------------
//
LogsFilter::FilterType LogsRecentCallsView::getFilter(LogsServices::LogsView view)
{
    LogsFilter::FilterType filter = LogsFilter::All;
    switch (view){
        case LogsServices::ViewAll:
            filter = LogsFilter::All;
            break;
        case LogsServices::ViewReceived:
            filter = LogsFilter::Received;
            break;
        case LogsServices::ViewCalled:
            filter = LogsFilter::Called;
            break;
        case LogsServices::ViewMissed:
            filter = LogsFilter::Missed;
            break;
        default:
            break;
    }
    return filter;
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::initializeGestures
// Gestures are added a bit ackwardly as scene (of list) needs to be present
// at gesture addition phase and we need to add gestures each time list 
// changes its gesture filter.
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::initializeGestures(HbGestureSceneFilter* filter)
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::initializeGestures()" );
    if (filter && mListView){
        LOGS_QDEBUG( "logs [UI] Adding flick gestures" );
        HbGesture* gesture = new HbGesture(HbGesture::left, 50);
        filter->addGesture(gesture);
        QObject::connect(gesture, SIGNAL(triggered(int)), this, SLOT(leftFlick(int)));
        
        gesture = new HbGesture(HbGesture::right, 50);
        filter->addGesture(gesture);
        QObject::connect(gesture, SIGNAL(triggered(int)), this, SLOT(rightFlick(int)));
        
        mListView->installSceneEventFilter(filter);
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::initializeGestures()" );
}


// -----------------------------------------------------------------------------
// LogsRecentCallsView::leftFlick
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::leftFlick(int value)
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::leftFlick()" );
    Q_UNUSED(value);
	if ( mConversionMap.value(mCurrentView) + 1 < mTitleMap.count() ){
    	LogsBaseView::LogsViewMap viewmap = 
            static_cast<LogsBaseView::LogsViewMap>(mConversionMap.value(mCurrentView) +1);
    	changeView( mConversionMap.key(viewmap) );
    } else {
        // At end of the lists, rollover to first list
        changeView( mConversionMap.key(static_cast<LogsBaseView::LogsViewMap>(0)), true );
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::leftFlick()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::rightFlick
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::rightFlick(int value)
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::rightFlick()" );
    Q_UNUSED(value);
    if ( mConversionMap.value(mCurrentView) > 0 ){
    	LogsBaseView::LogsViewMap viewmap = 
            static_cast<LogsBaseView::LogsViewMap>(mConversionMap.value(mCurrentView) - 1);
    	changeView( mConversionMap.key(viewmap) );	
    } else {
        // At beginning of the lists, rollover to last list
        changeView(mConversionMap.key( static_cast<LogsBaseView::LogsViewMap>(mTitleMap.count() - 1)), true );
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::rightFlick()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::changeView
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::changeView(LogsServices::LogsView view, bool rollOver)
{
    LOGS_QDEBUG_2( "logs [UI] -> LogsRecentCallsView::changeView(), view:", view );
    
    if ( view == mCurrentView ){
        LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::changeView(), view already correct" );
        // Already correct view, running animation can continue but make sure that
        // appearing view is then correct.
        mAppearingView = view;
        return;
    }

    if ( rollOver ){
        mMoveLeftInList = mConversionMap.value(view) > mConversionMap.value(mCurrentView);
    } else {
        mMoveLeftInList = mConversionMap.value(view) < mConversionMap.value(mCurrentView);
    }

    mAppearingView = view;
    mEffectHandler->startDissappearAppearByFadingEffect(*mViewName);
    mEffectHandler->startDissappearAppearByMovingEffect(
            *mListView, *mEmptyListLabel, !mMoveLeftInList, mListViewX);
 
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::changeView()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::dissappearByFadingComplete()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::dissappearByFadingComplete()" )

    // Previous view name has dissappeared by fading, set new view name 
    // as it is brought visible by effect
    mViewName->setTitleText( mTitleMap.value(mConversionMap.value(mAppearingView)) );
    
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::dissappearByFadingComplete()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::dissappearByMovingComplete()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::dissappearByMovingComplete()" )
    
    updateView( mAppearingView );

    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::dissappearByMovingComplete()" )
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateMenu
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateMenu()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::updateMenu()" );
    HbAction* action = qobject_cast<HbAction*>( 
            mRepository.findObject( logsRecentViewClearListMenuActionId ) );
    if ( action && model() ) {
        bool visible( model()->rowCount() > 0 );
        action->setVisible( visible );
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::updateMenu()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateEmptyListWidgetsVisibility
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateEmptyListWidgetsVisibility()
{
    updateMenu();
    updateEmptyListLabelVisibility();
    updateCallButton();
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateWidgetsSizeAndLayout
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateWidgetsSizeAndLayout()
{
    LOGS_QDEBUG( "logs [UI] -> LogsRecentCallsView::updateWidgetsSizeAndLayout()" );
    if ( mListView ) {
        updateListLayoutName(*mListView);
        updateListSize(mLayoutSectionName);
    }
    LOGS_QDEBUG( "logs [UI] <- LogsRecentCallsView::updateWidgetsSizeAndLayout()" );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::updateCallButton
// If dialpad contains text or there is items in list, call button is enabled
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::updateCallButton()
{  
    bool isVisible = !mDialpad->editor().text().isEmpty();
    if ( !isVisible && mFilter ) {
        isVisible = ( mFilter->rowCount() > 0 );
    }
    mDialpad->setCallButtonEnabled( isVisible );
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::handleMissedCallsMarking
// -----------------------------------------------------------------------------
//
void LogsRecentCallsView::handleMissedCallsMarking()
{
    if ( mFilter && !mMarkingMissedAsSeen && 
          ( mFilter->filterType() == LogsFilter::Missed || 
            mFilter->filterType() == LogsFilter::All ) ){
        // Don't care if timer would be already running, slot's implementation
        // takes care that marking is done only once
        LOGS_QDEBUG( "logs [UI] <-> LogsRecentCallsView::handleMissedCallsMarking()" );
        QTimer::singleShot( 
                logsMissedCallsMarkingDelayMs, this, SLOT(markMissedCallsSeen()) );
    }
}

// -----------------------------------------------------------------------------
// LogsRecentCallsView::markMissedCallsSeen
// -----------------------------------------------------------------------------
//
bool LogsRecentCallsView::markMissedCallsSeen()
{
    if ( !mMarkingMissedAsSeen ){
        connect( mModel, SIGNAL(markingCompleted(int)), 
                 this, SLOT(markingCompleted(int)) );
        mMarkingMissedAsSeen = 
            mModel->markEventsSeen(LogsModel::TypeLogsClearMissed);
    }
    return mMarkingMissedAsSeen;
}
