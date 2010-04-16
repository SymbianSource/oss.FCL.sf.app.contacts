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
#include "ut_logsrecentcallsview.h"
#include "logscomponentrepository.h"
#include "logsmodel.h"
#include "logsfilter.h"
#include "logsdetailsmodel.h"
#include "logsdefs.h"
#include "logseffecthandler.h"
#include "hbstubs_helper.h"
#include "logscall.h"
#include "logsmessage.h"
#include "logscontact.h"
#include "logsmatchesmodel.h"

//SYSTEM
#include <QtTest/QtTest>
#include <QGesture>
#include <hbmainwindow.h>
#include <hblistview.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hblistviewitem.h>
#include <hbmenu.h>
#include <dialpad.h>
#include <hbgroupbox.h>
#include <hbmessagebox.h>


#define VERIFY_CHECKED_ACTION( v, actionName ) { \
QVERIFY( v->mShowFilterMenu );\
QVERIFY( v->mShowFilterMenu->actions().count() > 0 );\
QAction* action = 0;\
foreach ( action, v->mShowFilterMenu->actions() ){ \
    if ( action->objectName() == actionName ){ \
        QVERIFY( action->isChecked() ); \
    } else { \
        QVERIFY( !action->isChecked() ); \
    } \
}}


void UT_LogsRecentCallsView::initTestCase()
{
    mMainWindow = new HbMainWindow();
    mViewManager = new LogsViewManagerStub(*mMainWindow);
}

void UT_LogsRecentCallsView::cleanupTestCase()
{
    delete mMainWindow;
    delete mViewManager;
}

void UT_LogsRecentCallsView::init()
{
    mRepository = new LogsComponentRepository(*mViewManager);
    mRecentCallsView = new LogsRecentCallsView( *mRepository, *mViewManager );
}

void UT_LogsRecentCallsView::cleanup()
{
    delete mRecentCallsView;
    mRecentCallsView = 0;
    delete mRepository;
    mRepository = 0;
}

void UT_LogsRecentCallsView::testConstructor()
{
    QVERIFY( mRecentCallsView );
    QVERIFY( !mRecentCallsView->mViewName );
    QVERIFY( !mRecentCallsView->mListView );
    QVERIFY( !mRecentCallsView->mFilter );
    QVERIFY( mRecentCallsView->mModel );
    QVERIFY( !mRecentCallsView->mShowFilterMenu );
    QVERIFY( mRecentCallsView->mCurrentView == LogsServices::ViewAll );
    QVERIFY( mRecentCallsView->viewId() == LogsRecentViewId );
    QVERIFY( mRecentCallsView->mLayoutSectionName == "" );
}

void UT_LogsRecentCallsView::testInitView()
{
    LogsRecentCallsView* view = mRepository->recentCallsView();
    QVERIFY( view );
    QVERIFY( !view->mInitialized );
    QVERIFY( !view->mFilter );
    QVERIFY( !view->mShowFilterMenu );
    QVERIFY( view->mTitleMap.isEmpty() );
    QVERIFY( view->mActionMap.isEmpty() );
    view->initView();
    QVERIFY( view->mInitialized );
    QVERIFY( !view->mFilter );
    QVERIFY( view->mShowFilterMenu );
    QVERIFY( view->mTitleMap.count() == 4 );
    QVERIFY( view->mActionMap.count() == 4 );
    
    //TODO: Init twice not allowed
}

void UT_LogsRecentCallsView::testActivated()
{
    LogsRecentCallsView* view = mRepository->recentCallsView();
    QVERIFY( view );
    // Change for uninitialized causes initialization
    QVERIFY( !view->mInitialized );
    QVERIFY( !view->mFilter );
    QVERIFY( !view->mEmptyListLabel );
    QVERIFY( !view->mModel->mMissedCallsCounterCleared );
    view->activated(false, QVariant(LogsServices::ViewAll));
    QVERIFY( view->mInitialized );
    QVERIFY( view->mFilter );
    QVERIFY( view->mEmptyListLabel );
    QVERIFY( view->mModel->mMissedCallsCounterCleared );
    VERIFY_CHECKED_ACTION( view, logsShowFilterRecentMenuActionId )
    
    // Change views
    view->activated(false, QVariant(LogsServices::ViewReceived));
    QVERIFY( view->mFilter );
    QVERIFY( view->mFilter->filterType() == LogsFilter::Received );  
    VERIFY_CHECKED_ACTION( view, logsShowFilterReceivedMenuActionId )
    
    view->activated(false, QVariant(LogsServices::ViewMissed));
    QVERIFY( view->mFilter->filterType() == LogsFilter::Missed );  
    VERIFY_CHECKED_ACTION( view, logsShowFilterMissedMenuActionId )
    
    view->activated(false, QVariant(LogsServices::ViewCalled));
    QVERIFY( view->mFilter->filterType() == LogsFilter::Called );  
    VERIFY_CHECKED_ACTION( view, logsShowFilterDialledMenuActionId )
    
    view->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    view->mDialpad->editor().setText( QString("hello") );
    view->activated(false, QVariant(LogsServices::ViewAll));
    QVERIFY( view->mFilter->filterType() == LogsFilter::All );  
    VERIFY_CHECKED_ACTION( view, logsShowFilterRecentMenuActionId )
    QVERIFY( view->mDialpad->editor().text().isEmpty() );
    QVERIFY( view->mListView->layoutName() == logsListLandscapeLayout );
    QVERIFY( view->mLayoutSectionName == logsViewDefaultSection );
}

void UT_LogsRecentCallsView::testDeactivated()
{
    mRecentCallsView->deactivated();
}

void UT_LogsRecentCallsView::testInitListWidget()
{
    //TODO:Q_ASSERT_X
/*    //list widget can't be found from repository
    QVERIFY( !mRecentCallsView->mListView );
    mRecentCallsView->initListWidget();
    QVERIFY( !mRecentCallsView->mListView );
  */
    //list widget is in repository
    LogsRecentCallsView* view = mRepository->recentCallsView();
    QVERIFY( !view->mListView );
    view->initListWidget();
    QVERIFY( view->mListView );
    view = 0; 
}

void UT_LogsRecentCallsView::testUpdateFilter()
{
    //no list view
    QVERIFY( !mRecentCallsView->mListView );
    QVERIFY( !mRecentCallsView->mFilter );
    mRecentCallsView->updateFilter(LogsFilter::All);
    QVERIFY( !mRecentCallsView->mFilter );
    
    //first filter created with update
    HbStubHelper::reset();
    QVERIFY( !mRecentCallsView->mFilter );
    mRecentCallsView->mListView = new HbListView();
    mRecentCallsView->updateFilter(LogsFilter::All);
    QVERIFY( mRecentCallsView->mFilter );
    QVERIFY( mRecentCallsView->mFilter->filterType() == LogsFilter::All );    
    QVERIFY( !HbStubHelper::singleShotTimerActive() );
    delete mRecentCallsView->mListView;
    mRecentCallsView->mListView = 0;
            
    //filter is updated with a new one, missed calls marking as seen is started (by timer)
    QVERIFY( mRecentCallsView->mFilter );
    mRecentCallsView->mListView = new HbListView();
    mRecentCallsView->updateFilter(LogsFilter::Missed);
    QVERIFY( mRecentCallsView->mFilter );
    QVERIFY( mRecentCallsView->mFilter->filterType() == LogsFilter::Missed );
    QVERIFY( HbStubHelper::singleShotTimerActive() );
    delete mRecentCallsView->mListView;
    mRecentCallsView->mListView = 0;
}

void UT_LogsRecentCallsView::testUpdateViewName()
{
    //no view name label
    QVERIFY( !mRecentCallsView->mViewName );
    mRecentCallsView->updateViewName();
    QVERIFY( !mRecentCallsView->mViewName );
    
    //view name label exists
    mRecentCallsView->mCurrentView = LogsServices::ViewMissed;
    mRecentCallsView->addStringsToMap();
    mRecentCallsView->mViewName = new HbGroupBox();
    mRecentCallsView->updateViewName();
    QVERIFY( mRecentCallsView->mViewName->heading() 
            == mRecentCallsView->mTitleMap.value(
                   mRecentCallsView->mConversionMap.value(LogsServices::ViewMissed)));
    delete mRecentCallsView->mViewName;
    mRecentCallsView->mViewName = 0;
}

void UT_LogsRecentCallsView::testChangeFilter()
{
    mRepository->recentCallsView();
    mRecentCallsView->activated( false, QVariant(LogsServices::ViewAll) );
    QVERIFY( mRecentCallsView->mFilter );
    QVERIFY( mRecentCallsView->mFilter->filterType() == LogsFilter::All );
    
    HbAction*  action = new HbAction();
    action->setObjectName(logsShowFilterMissedMenuActionId);
    mRecentCallsView->changeFilter(action);
    
    // Because of effects, filter is not changed immediately, simulate effect completion
    QVERIFY( mRecentCallsView->mAppearingView == LogsServices::ViewMissed );
    mRecentCallsView->dissappearByMovingComplete();
    QVERIFY( mRecentCallsView->mFilter->filterType() == LogsFilter::Missed );        
    
    delete action;
    delete mRecentCallsView->mListView;
    mRecentCallsView->mListView = 0;
    delete mRecentCallsView->mViewName;
    mRecentCallsView->mViewName = 0;
}

void UT_LogsRecentCallsView::testInitiateCallback()
{
    LogsCall::resetTestData();
    // Call cannot be constructed in model
    QModelIndex filterIndex;
    mRecentCallsView->initiateCallback(filterIndex);
    QVERIFY( LogsCall::lastCalledFuntion().isEmpty() );

    // Call can be constructed in model
    filterIndex = makeValidFilterIndex(*mRecentCallsView);
    mRecentCallsView->initiateCallback(filterIndex);
    QVERIFY( LogsCall::lastCalledFuntion() == QString("initiateCallback") );
}

void UT_LogsRecentCallsView::testCallKeyPressed()
{    LogsRecentCallsView* view = mRepository->recentCallsView();
    
    // Not ready for calling
    view->callKeyPressed();
    
    // No any item where to call
    view->activated( false, QVariant(LogsServices::ViewAll) );
    view->callKeyPressed();
    
    // Calling possible
    mRepository->model()->mTextData.append("testdata");
    mRepository->model()->mTextData.append("testdata2");
    view->mListView->setCurrentIndex( 
        mRepository->model()->index( 1, 0 ), QItemSelectionModel::Select );
    view->callKeyPressed();
    QVERIFY( view->mListView->currentIndex().row() == 0 );
}

void UT_LogsRecentCallsView::testShowCallDetails()
{
    // No details model, nothing to show
    QVERIFY( mRecentCallsView->mDetailsModel == 0 );
    mRecentCallsView->showCallDetails();
    QVERIFY( mViewManager->mViewId != LogsDetailsViewId );
    
    // Details model shown
    mRecentCallsView->mDetailsModel = new LogsDetailsModel;
    mRecentCallsView->showCallDetails();
    QVERIFY( mViewManager->mViewId == LogsDetailsViewId );
}

void UT_LogsRecentCallsView::testOpenDialpad()
{
    QVERIFY( !mRecentCallsView->mFilter );
    QVERIFY( !mRecentCallsView->mDialpad->isOpen() );
    mRecentCallsView->openDialpad();
    QVERIFY( mRecentCallsView->mMatchesModel );
    QVERIFY( mRecentCallsView->mDialpad->isOpen() );
    
    
    mRecentCallsView->mFilter = new LogsFilter( LogsFilter::Missed );
    LogsMatchesModel* oldmatchesModel = mRecentCallsView->mMatchesModel;
    mRecentCallsView->openDialpad();
    QVERIFY( mRecentCallsView->mMatchesModel == oldmatchesModel );
    QVERIFY( mRecentCallsView->mDialpad->isOpen() );
}

void UT_LogsRecentCallsView::testDialpadEditorTextChanged()
{
    // Matches view is opened and matches model is passed as arg
    // text editor is one character long
    LogsDbConnector* dbConnector = 0;
    delete mRecentCallsView->mMatchesModel;
    mRecentCallsView->mMatchesModel = 0;
    mRecentCallsView->mMatchesModel = new LogsMatchesModel(*dbConnector);
    mRecentCallsView->mDialpad->editor().setText( QString("h") );
    mRecentCallsView->dialpadEditorTextChanged();
    QVERIFY( mViewManager->mViewId == LogsMatchesViewId );
    QVERIFY( mRecentCallsView->mMatchesModel == 0 );
    
    // No text in editor, no view change, no lines in model, call button
    // gets disabled
    mViewManager->reset();
    mRecentCallsView->mDialpad->mIsCallButtonEnabled = true;
    delete mRecentCallsView->mFilter;
    mRecentCallsView->mFilter = new LogsFilter();
    mRecentCallsView->mDialpad->editor().setText( QString("") );
    mRecentCallsView->dialpadEditorTextChanged();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
    QVERIFY( !mRecentCallsView->mDialpad->mIsCallButtonEnabled );
    
    // No text in editor but items in list, call button is enabled
    mRecentCallsView->mDialpad->mIsCallButtonEnabled = false;
    mRecentCallsView->mFilter->setSourceModel(mRecentCallsView->mModel);
    mRecentCallsView->mDialpad->editor().setText( QString("") );
    mRecentCallsView->dialpadEditorTextChanged();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
    QVERIFY( mRecentCallsView->mDialpad->mIsCallButtonEnabled );
}

void UT_LogsRecentCallsView::testGestureEvent()
{
    LogsRecentCallsView* view = mRepository->recentCallsView();
    view->activated( false, QVariant(LogsServices::ViewCalled) );
    view->mCurrentView = LogsServices::ViewCalled;
    view->mAppearingView = LogsServices::ViewCalled;
    mRecentCallsView->mViewManager.mainWindow().setOrientation( Qt::Vertical );

    QSwipeGesture* swipe = new QSwipeGesture();
    QList<QGesture*> list;
    QGestureEvent event(list);
    event.ignore(Qt::SwipeGesture);

    //no swipe gesture in event
    QVERIFY(!event.isAccepted(Qt::SwipeGesture));    
    view->gestureEvent(&event);
    QVERIFY(!event.isAccepted(Qt::SwipeGesture));
    QVERIFY(view->mAppearingView == LogsServices::ViewCalled);
    
    //swipe gesture in event, but gesture isn't finished
    list.append(swipe);
    QGestureEvent event2(list);
    event2.ignore(Qt::SwipeGesture);
    QVERIFY(!event2.isAccepted(Qt::SwipeGesture));
    QVERIFY(swipe->state() != Qt::GestureFinished);
    view->gestureEvent(&event2);
    QVERIFY(!event2.isAccepted(Qt::SwipeGesture));
    QVERIFY(view->mAppearingView == LogsServices::ViewCalled);
    
    //vertical orientation swipe right up
    HbStubHelper::setGestureState(Qt::GestureFinished);
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSwipeAngle(10);
    view->gestureEvent(&event2);
    QVERIFY( view->mAppearingView == LogsServices::ViewAll );
    QVERIFY( event2.isAccepted(Qt::SwipeGesture) );
    
    //vertical orientation swipe left up
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSwipeAngle(170);
    view->gestureEvent(&event2);
    QVERIFY(view->mAppearingView == LogsServices::ViewReceived);
    QVERIFY(event2.isAccepted(Qt::SwipeGesture));

    //vertical orientation swipe down, nothing happens
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSwipeAngle(70);
    view->mAppearingView = view->mCurrentView;
    view->gestureEvent(&event2);
    QVERIFY(view->mAppearingView == LogsServices::ViewCalled);
    QVERIFY(!event2.isAccepted(Qt::SwipeGesture));
    
    //horizontal orientation swipe right up
    mRecentCallsView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSwipeAngle(80);
    view->gestureEvent(&event2);
    QVERIFY(view->mAppearingView == LogsServices::ViewReceived);
    QVERIFY(event2.isAccepted(Qt::SwipeGesture));
    
    //horizontal orientation swipe right down
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSwipeAngle(280);
    view->gestureEvent(&event2);
    QVERIFY(view->mAppearingView == LogsServices::ViewAll);
    QVERIFY(event2.isAccepted(Qt::SwipeGesture));

    //horizontal orientation swipe left, nothing happens
    event2.setAccepted(Qt::SwipeGesture, false);
    swipe->setSwipeAngle(200);
    view->mAppearingView = view->mCurrentView;
    view->gestureEvent(&event2);
    QVERIFY(view->mAppearingView == LogsServices::ViewCalled);
    QVERIFY(!event2.isAccepted(Qt::SwipeGesture));
}

void UT_LogsRecentCallsView::testSwipeAngleToDirection()
{
    int delta = 30;
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(61, delta), QSwipeGesture::Up);    
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(119, delta), QSwipeGesture::Up);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(90, delta), QSwipeGesture::Up);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(60, delta), QSwipeGesture::NoDirection);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(120, delta), QSwipeGesture::NoDirection);

    QCOMPARE(mRecentCallsView->swipeAngleToDirection(241, delta), QSwipeGesture::Down);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(299, delta), QSwipeGesture::Down);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(270, delta), QSwipeGesture::Down);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(300, delta), QSwipeGesture::NoDirection);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(240, delta), QSwipeGesture::NoDirection);
    
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(29, delta), QSwipeGesture::Right);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(331, delta), QSwipeGesture::Right);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(0, delta), QSwipeGesture::Right);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(360, delta), QSwipeGesture::Right);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(30, delta), QSwipeGesture::NoDirection);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(330, delta), QSwipeGesture::NoDirection);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(361, delta), QSwipeGesture::NoDirection);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(-1, delta), QSwipeGesture::NoDirection);

    QCOMPARE(mRecentCallsView->swipeAngleToDirection(151, delta), QSwipeGesture::Left);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(209, delta), QSwipeGesture::Left);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(180, delta), QSwipeGesture::Left);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(150, delta), QSwipeGesture::NoDirection);
    QCOMPARE(mRecentCallsView->swipeAngleToDirection(210, delta), QSwipeGesture::NoDirection);
}

void UT_LogsRecentCallsView::testViewChangeByFlicking()
{
    // At leftmost list, moving to left causes going to last list
    LogsRecentCallsView* view = mRepository->recentCallsView();
    view->activated( false, QVariant(LogsServices::ViewAll) );
    view->mCurrentView = LogsServices::ViewAll;
    view->rightFlick();
    QVERIFY(view->mCurrentView == LogsServices::ViewAll);
    QVERIFY(view->mAppearingView == LogsServices::ViewMissed);
    
    // Moving to right possible
    view->leftFlick();
    QVERIFY(view->mCurrentView == LogsServices::ViewAll);
    QVERIFY(view->mAppearingView == LogsServices::ViewCalled);
    
    // At rightmost list, moving causes going to first list
    view->mCurrentView = LogsServices::ViewMissed;
    view->leftFlick();
    QVERIFY(view->mCurrentView == LogsServices::ViewMissed);
    QVERIFY(view->mAppearingView == LogsServices::ViewAll);
    
    // Now moving to left is possible
    view->rightFlick();
    QVERIFY(view->mCurrentView == LogsServices::ViewMissed);
    QVERIFY(view->mAppearingView == LogsServices::ViewReceived);
    
    // Simulate effect completion which activates new view
    view->dissappearByFadingComplete();
    QVERIFY( view->mViewName->heading() 
             == view->mTitleMap.value(view->mConversionMap.value(LogsServices::ViewReceived)) );
    
    view->dissappearByMovingComplete();
    QVERIFY(view->mCurrentView == LogsServices::ViewReceived);
    QVERIFY(view->mAppearingView == LogsServices::ViewReceived);
    
    // If view would be already what expected, nothing happens
    view->dissappearByMovingComplete();
    QVERIFY(view->mCurrentView == LogsServices::ViewReceived);
    QVERIFY(view->mAppearingView == LogsServices::ViewReceived);
}


void UT_LogsRecentCallsView::testModel()
{
    LogsRecentCallsView* view = mRepository->recentCallsView();
    view->activated(false, QVariant(LogsServices::ViewAll));
    QVERIFY( view->mFilter );
    QVERIFY( view->model() == view->mFilter );
}

void UT_LogsRecentCallsView::testShowListItemMenu()
{
    HbStubHelper::reset();
    QModelIndex filterIndex = makeValidFilterIndex(*mRecentCallsView);
    HbListViewItem* item = new HbListViewItem;
    item->setModelIndex(filterIndex);//mRecentCallsView->mFilter->index( 0, 0 ));

    mRecentCallsView->showListItemMenu(item, QPointF());
  
    //can't test, showed only if itemContextMenu.actions().count() > 0
    //if we stub actions(), then testActivated() will fail
//    QVERIFY( HbStubHelper::menuShown() );
    QVERIFY( HbStubHelper::widgetActionsCount() == 7 );
    delete item;
}

void UT_LogsRecentCallsView::testUpdateListItemData()
{
    QModelIndex filterIndex = makeValidFilterIndex(*mRecentCallsView);
    QVERIFY( !mRecentCallsView->mCall );
    QVERIFY( !mRecentCallsView->mContact );
    QVERIFY( !mRecentCallsView->mMessage );
    QVERIFY( !mRecentCallsView->mDetailsModel );
    
    mRecentCallsView->updateListItemData( filterIndex );

    QVERIFY( mRecentCallsView->mCall );
    QVERIFY( mRecentCallsView->mContact );
    QVERIFY( mRecentCallsView->mMessage );
    QVERIFY( mRecentCallsView->mDetailsModel );
}

void UT_LogsRecentCallsView::testPopulateListItemMenu()
{
    //call, message and contact exist, menu has actions
    HbStubHelper::reset();
    mRecentCallsView->mCall = new LogsCall();
    mRecentCallsView->mMessage = new LogsMessage();
    mRecentCallsView->mContact = new LogsContact();
    mRecentCallsView->mDetailsModel = new LogsDetailsModel();
    HbMenu menu;
    mRecentCallsView->populateListItemMenu(menu);
    QVERIFY( HbStubHelper::widgetActionsCount() == 7 );
}

void UT_LogsRecentCallsView::testUpdateCall()
{
    //call can be created
    QModelIndex filterIndex = makeValidFilterIndex(*mRecentCallsView);
    QVERIFY( !mRecentCallsView->mCall );
    mRecentCallsView->updateCall( filterIndex );
    QVERIFY( mRecentCallsView->mCall );

    //call can't be created
    mRecentCallsView->updateCall( QModelIndex() );
    QVERIFY( !mRecentCallsView->mCall );
}

void UT_LogsRecentCallsView::testUpdateMessage()
{        
    //message can be created
    QModelIndex filterIndex = makeValidFilterIndex(*mRecentCallsView);
    QVERIFY( !mRecentCallsView->mMessage );
    mRecentCallsView->updateMessage( filterIndex );
    QVERIFY( mRecentCallsView->mMessage );

    //message can't be created
    mRecentCallsView->updateMessage( QModelIndex() );
    QVERIFY( !mRecentCallsView->mMessage );
}

void UT_LogsRecentCallsView::testUpdateContact()
{
    //contact can be created
    QModelIndex filterIndex = makeValidFilterIndex(*mRecentCallsView);
    QVERIFY( !mRecentCallsView->mContact );
    mRecentCallsView->updateContact( filterIndex );
    QVERIFY( mRecentCallsView->mContact );
    
    //contact can't be created
    mRecentCallsView->updateContact( QModelIndex() );
    QVERIFY( !mRecentCallsView->mContact );
}

void UT_LogsRecentCallsView::testActivateEmptyListIndicator()
{
    // Empty model, label is set visible
    LogsRecentCallsView* view = mRepository->recentCallsView();
    QObject* obj = mRepository->findObject( logsRecentViewClearListMenuActionId );
    HbAction* action = qobject_cast<HbAction*>( obj );
    delete view->mFilter;
    view->mFilter = 0;
    view->mFilter = new LogsFilter();
    QVERIFY( !view->mEmptyListLabel );
    view->activateEmptyListIndicator(view->mFilter);
    QVERIFY( view->mEmptyListLabel );
    QVERIFY( view->mEmptyListLabel->isVisible() );
    QVERIFY(action && !action->isVisible());
    
    // Model has items, label is set invisible
    LogsModel model;
    view->mFilter->setSourceModel(&model);
    view->activateEmptyListIndicator(view->mFilter);
    QVERIFY( view->mEmptyListLabel );
    QVERIFY( !view->mEmptyListLabel->isVisible() );
    QVERIFY(action && action->isVisible());
}

void UT_LogsRecentCallsView::testUpdateMenu()
{
    //menu is not in repository
    mRecentCallsView->updateMenu();
    
    //menu is in repository, empty model => "Clear list" is not visible
    LogsRecentCallsView* view = mRepository->recentCallsView();
    QObject* obj = mRepository->findObject( logsRecentViewClearListMenuActionId );
    HbAction* action = qobject_cast<HbAction*>( obj );
    QVERIFY(action && action->isVisible());
    delete view->mFilter;
    view->mFilter = 0;
    view->mFilter = new LogsFilter();
    view->updateMenu();
    QVERIFY(action && !action->isVisible());
    
    //menu is in repository, non-empty model => "Clear list" is visible
    makeValidFilterIndex(*view);
    view->updateMenu();
    QVERIFY(action && action->isVisible());
}


QModelIndex UT_LogsRecentCallsView::makeValidFilterIndex(LogsRecentCallsView& view)
{
    if ( !view.mModel ) {
        return QModelIndex();
    }
    view.mModel->mTextData.append("testdata");
    delete view.mFilter;
    view.mFilter = 0;
    view.mFilter = new LogsFilter( LogsFilter::All );
    view.mFilter->setSourceModel( view.mModel );
    return  view.mFilter->index(0,0);
}

void UT_LogsRecentCallsView::testUpdateWidgetsSizeAndLayout()
{
    //no listView, nothing happens
    QVERIFY( !mRecentCallsView->mListView );
    mRecentCallsView->updateWidgetsSizeAndLayout();
    
    HbListView list;
    //listView exists, layout and size updated
    mRecentCallsView->mViewManager.mainWindow().setOrientation( Qt::Vertical );
    mRecentCallsView->mDialpad->closeDialpad();
    mRecentCallsView->mListView = &list;
    mRecentCallsView->mListView->setLayoutName("dummy");
    mRecentCallsView->mLayoutSectionName = "dummy";
    mRecentCallsView->updateWidgetsSizeAndLayout();
    QVERIFY( mRecentCallsView->mListView->layoutName() == logsListDefaultLayout );
    QVERIFY( mRecentCallsView->mLayoutSectionName == logsViewDefaultSection );
}

void UT_LogsRecentCallsView::testDialpadClosed()
{
    HbListView list;
    mRecentCallsView->mListView = &list;

    mRecentCallsView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    mRecentCallsView->mDialpad->closeDialpad();
    mRecentCallsView->mLayoutSectionName = QString(logsViewLandscapeDialpadSection);
    QString hello("hello");
    mRecentCallsView->mDialpad->editor().setText( hello );
    mRecentCallsView->dialpadClosed();
    QVERIFY( mRecentCallsView->mDialpad->editor().text().isEmpty()  );
    QVERIFY( mRecentCallsView->mLayoutSectionName == logsViewDefaultSection );
}

void UT_LogsRecentCallsView::testClearList()
{

    //check that without filter list is not cleared
    mRecentCallsView->mModel->mIsCleared = false;
    HbMessageBox().setText(tr("Ok"));
    mRecentCallsView->clearList();
    QVERIFY( !mRecentCallsView->mModel->mIsCleared );

    mRecentCallsView->mFilter = new LogsFilter( LogsFilter::Missed );
       
    //simulate "Ok" button press of messagebox
    HbMessageBox().setText(tr("Ok"));
    mRecentCallsView->clearList();
    QVERIFY( mRecentCallsView->mModel->mIsCleared );

    //simulate "Cancel" button press of messagebox
    mRecentCallsView->mModel->mIsCleared = false;
    HbMessageBox().setText(tr("Cancel"));
    mRecentCallsView->clearList();
    QVERIFY( !mRecentCallsView->mModel->mIsCleared );
}

void UT_LogsRecentCallsView::testIsExitAllowed()
{
    // Exit allowed, nothing to mark as seen
    QVERIFY( mRecentCallsView->isExitAllowed() );
    QVERIFY( !mRecentCallsView->mMarkingMissedAsSeen );
    
    // Exit not allowed, marking started
    mRecentCallsView->mModel->mTestIsMarkingNeeded = true;
    QVERIFY( !mRecentCallsView->isExitAllowed() );
    QVERIFY( mRecentCallsView->mMarkingMissedAsSeen );
    
    // Exit not allowed marking still ongoing
    QVERIFY( !mRecentCallsView->isExitAllowed() );
    QVERIFY( mRecentCallsView->mMarkingMissedAsSeen );
    
    // Complete marking, exit allowed
    QSignalSpy spy( mRecentCallsView, SIGNAL(exitAllowed()) );
    mRecentCallsView->mModel->mTestIsMarkingNeeded = false;
    mRecentCallsView->markingCompleted(0);
    QVERIFY( !mRecentCallsView->mMarkingMissedAsSeen );
    QVERIFY( spy.count() == 1 );
    QVERIFY( mRecentCallsView->isExitAllowed() );
}
