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
#include "logsdetailsview.h"
#include "ut_logsdetailsview.h"
#include "logscomponentrepository.h"
#include "logsdetailsmodel.h"
#include "logscall.h"
#include "logsmessage.h"
#include "logsdefs.h"

//SYSTEM
#include <QtTest/QtTest>
#include <hbmainwindow.h>
#include <hblistview.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hblistviewitem.h>
#include <hbmenu.h>
#include <hblineedit.h>
#include <hbgroupbox.h>
#include <dialpad.h>

Q_DECLARE_METATYPE(LogsDetailsModel *)


void UT_LogsDetailsView::initTestCase()
{
    mMainWindow = new HbMainWindow();
    mViewManager = new LogsViewManagerStub(*mMainWindow);
}

void UT_LogsDetailsView::cleanupTestCase()
{
    delete mMainWindow;
    delete mViewManager;
}

void UT_LogsDetailsView::init()
{
    mRepository = new LogsComponentRepository(*mViewManager);
    mDetailsView = new LogsDetailsView( *mRepository, *mViewManager );
}

void UT_LogsDetailsView::cleanup()
{
    delete mDetailsView;
    delete mRepository;
}

void UT_LogsDetailsView::testConstructor()
{
    QVERIFY(mDetailsView);
    QVERIFY(mDetailsView->mSoftKeyBackAction);
    QVERIFY(mDetailsView->viewId() == LogsDetailsViewId );
    QVERIFY(!mDetailsView->mDetailsModel);
}

void UT_LogsDetailsView::testActivated()
{
    //list widget can't be found from repository
/*    //TODO: Q_ASSERT_X
    LogsDetailsModel* model = new LogsDetailsModel;
    QVariant arg = qVariantFromValue( model );
    mDetailsView->activated(false, arg);
    QVERIFY(!mDetailsView->mListView);
*/
    //list widget is in repository
    LogsDetailsModel* model2 = new LogsDetailsModel;
    QVariant arg2 = qVariantFromValue( model2 );
    LogsDetailsView* view = mRepository->detailsView();
    QVERIFY( !view->mInitialized );
    view->activated(false, arg2);
    QVERIFY( view->mInitialized );
    QVERIFY(view->mListView);
    QVERIFY(view->mDetailsModel == model2);
    QVERIFY(view->mListView->model() == model2);
    QVERIFY(view->mActionMap.count() == 4);
    
    LogsDetailsModel* model3 = new LogsDetailsModel();
    QVariant arg3 = qVariantFromValue( model3 );
    mDetailsView->activated(false, arg3);
    QVERIFY( mDetailsView->mDetailsModel == model3);
    QVERIFY(view->mListView->model() == model3);
    //QT_NO_DEBUG_OUTPUT
}

void UT_LogsDetailsView::testHandleBackSoftkey()
{
    mViewManager->reset();
    mDetailsView->handleBackSoftkey();
    QVERIFY(  mViewManager->mPreviousActivated );
}

void UT_LogsDetailsView::testCallKeyPressed()
{
    // No call, nothing happens
    QVERIFY( !mDetailsView->mCall );
    mDetailsView->callKeyPressed();
    LogsDetailsModel* model = new LogsDetailsModel();
    
    // Calling possible
    delete model->mEvent;
    model->mEvent = 0;
    model->mEvent = new LogsEvent();
    QVariant arg = qVariantFromValue( model );
    mRepository->detailsView();
    mDetailsView->activated(false, arg);
    QVERIFY( mDetailsView->mCall );
    mDetailsView->callKeyPressed();
    QVERIFY( mDetailsView->mCall->mTestLastCallType != -1 );
}

void UT_LogsDetailsView::testInitiateVoiceCall()
{
    // No call, nothing happens
    QVERIFY( !mDetailsView->mCall );
    mDetailsView->initiateVoiceCall();
    
    // Call possible
    mDetailsView->mCall = new LogsCall;
    mDetailsView->initiateVoiceCall();
    QVERIFY( mDetailsView->mCall->mTestLastCallType == LogsCall::TypeLogsVoiceCall );
}

void UT_LogsDetailsView::testInitiateVideoCall()
{
    // No call, nothing happens
    QVERIFY( !mDetailsView->mCall );
    mDetailsView->initiateVideoCall();
    
    // Call possible
    mDetailsView->mCall = new LogsCall;
    mDetailsView->initiateVideoCall();
    QVERIFY( mDetailsView->mCall->mTestLastCallType == LogsCall::TypeLogsVideoCall );
}

void UT_LogsDetailsView::testUpdateMenu()
{
    // No model, nothing happens
    mDetailsView->updateMenu();

    // No call, call actions disabled
    QVERIFY( !mDetailsView->mCall );
    mRepository->detailsView(); // Set correct object tree
    LogsDetailsModel* model = new LogsDetailsModel();
    QVariant arg = qVariantFromValue( model );
    mDetailsView->activated(false, arg);
    mDetailsView->updateMenu();
    QObject* obj = mRepository->findObject( logsDetailsViewVoiceCallMenuActionId );
    HbAction* voiceCallAction = qobject_cast<HbAction*>( obj );
    QObject* obj2 = mRepository->findObject( logsDetailsViewVideoCallMenuActionId );
    HbAction* videoCallAction = qobject_cast<HbAction*>( obj2 );
    QObject* obj3 = mRepository->findObject( logsDetailsMessageMenuActionId );
    HbAction* messageAction = qobject_cast<HbAction*>( obj3 );    
    QVERIFY( !voiceCallAction->isVisible() );
    QVERIFY( !videoCallAction->isVisible() );
    QVERIFY( !messageAction->isVisible() );
    
    // Call exists, call actions enabled
    model->mEvent = new LogsEvent;
    mDetailsView->updateMenu();
    QVERIFY( voiceCallAction->isVisible() );
    QVERIFY( videoCallAction->isVisible() );
    QVERIFY( messageAction->isVisible() );
}

void UT_LogsDetailsView::testDeleteEvent()
{
    // No model, nothing happens
    QSignalSpy spy(mDetailsView, SIGNAL(viewActivated(HbView*)));
    QVERIFY( LogsDetailsModel::mLastCallName.isEmpty() );
    QVERIFY( !mDetailsView->mDetailsModel );
    mDetailsView->deleteEvent();
    QVERIFY( LogsDetailsModel::mLastCallName.isEmpty() );
    QVERIFY( spy.count() == 0 );

    // Model exists, call to delete event made and view is closed
    mViewManager->reset();
    LogsDetailsModel* model = new LogsDetailsModel();
    mDetailsView->mDetailsModel = model;
    mDetailsView->deleteEvent();
    QVERIFY( LogsDetailsModel::mLastCallName == QString("clearEvent") );
    QVERIFY( mViewManager->mPreviousActivated );
}

void UT_LogsDetailsView::testCopyNumberToClipboard()
{
    LogsDetailsModel* model = new LogsDetailsModel();
    mDetailsView->mDetailsModel = model;
    mDetailsView->copyNumberToClipboard();
    QVERIFY( LogsDetailsModel::mLastCallName == QString("getNumberToClipboard") );
}

void UT_LogsDetailsView::testChangeFilter()
{
    mViewManager->reset();
    HbAction action;
    action.setObjectName(logsShowFilterMissedMenuActionId);
    mDetailsView->changeFilter(&action);
    QVERIFY( mViewManager->mViewId == LogsRecentViewId );
    QVERIFY( mViewManager->mArgs.toInt() == (int)LogsServices::ViewAll );
}

void UT_LogsDetailsView::testContactActionCompleted()
{
    // No viewname for some reason, no effect
    HbGroupBox viewName; 
    LogsDetailsModel* model = new LogsDetailsModel();
    mDetailsView->mDetailsModel = model;
    mDetailsView->contactActionCompleted(true);
    
    // Contact modified, viewname updated accordingly
    mDetailsView->mViewName = &viewName;
    model->mEvent = new LogsEvent;
    model->mEvent->setRemoteParty("someparty");
    mDetailsView->contactActionCompleted(true);
    QVERIFY( mDetailsView->mViewName->titleText().length() > 0 );
    
    // No effect if no contact modify occurred
    mDetailsView->mViewName->setTitleText("");
    mDetailsView->contactActionCompleted(false);
    QVERIFY( mDetailsView->mViewName->titleText().length() == 0 );
}

void UT_LogsDetailsView::testModel()
{
    QVERIFY( !mDetailsView->model() );
    mDetailsView->mDetailsModel = new LogsDetailsModel();
    QVERIFY( mDetailsView->model() );
}

void UT_LogsDetailsView::testUpdateWidgetsSizeAndLayout()
{
    //no listView, nothing happens
    QVERIFY( !mDetailsView->mListView );
    mDetailsView->updateWidgetsSizeAndLayout();
    
    HbListView list;
    //listView exists, layout updated
    mDetailsView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    mDetailsView->mDialpad->setVisible(true);
    mDetailsView->mListView = &list;
    mDetailsView->mListView->setLayoutName("dummy");
    mDetailsView->updateWidgetsSizeAndLayout();
    QVERIFY( mDetailsView->mListView->layoutName() == logsListLandscapeLayout );
}
