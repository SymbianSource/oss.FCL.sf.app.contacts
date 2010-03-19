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
#include "logsbaseview.h"
#include "ut_logsbaseview.h"
#include "logscomponentrepository.h"
#include "qthighway_stub_helper.h"
#include "logscall.h"
#include "logsmessage.h"
#include "logscontact.h"
#include "hbstubs_helper.h"

//SYSTEM
#include <QtTest/QtTest>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hbinstance.h>
#include <hbmenu.h>
#include <dialpad.h>
#include <hblineedit.h>
#include <hblistviewitem.h>
#include <hblistview.h>
#include <QStringListModel>

void UT_LogsBaseView::initTestCase()
{
    mMainWindow = new HbMainWindow();
    mViewManager = new LogsViewManagerStub(*mMainWindow);
}

void UT_LogsBaseView::cleanupTestCase()
{
    delete mMainWindow;
    delete mViewManager;
}

void UT_LogsBaseView::init()
{
    mRepository = new LogsComponentRepository( *mViewManager );
    mBaseView = new LogsBaseView( LogsRecentViewId, *mRepository, *mViewManager );
}

void UT_LogsBaseView::cleanup()
{
    delete mBaseView;
    mBaseView = 0;
    delete mRepository;
    mRepository = 0;
}

void UT_LogsBaseView::testConstructor()
{
    QVERIFY( mBaseView );
    QVERIFY( mBaseView->mSoftKeyBackAction );
    QVERIFY( !mBaseView->mShowFilterMenu );
    QVERIFY( !mBaseView->mInitialized );
    QVERIFY( mBaseView->mActionMap.isEmpty() );
    QVERIFY( mBaseView->viewId() == LogsRecentViewId );
    QVERIFY( mBaseView->mDialpad );
    QVERIFY( !mBaseView->mCall );
    QVERIFY( !mBaseView->mMessage );
    QVERIFY( !mBaseView->mContact );
    QVERIFY( !mBaseView->mCallTypeMapper );
    QVERIFY( !mBaseView->mForceDialpadOpened );
}

void UT_LogsBaseView::testActivated()
{
    mBaseView->activated(false, QVariant());
    QVERIFY( !mBaseView->mDialpad->isVisible() );
    QVERIFY( mBaseView->mInitialized );
    QVERIFY( !mBaseView->mShowFilterMenu );
    QVERIFY( mBaseView->mActionMap.count() == 4 );

    mBaseView->clearSoftKey();
    mBaseView->activated(true, QVariant());
    QVERIFY( mBaseView->mDialpad->isVisible() );
}

void UT_LogsBaseView::testDeactivated()
{
    mBaseView->deactivated(); // NOP
}

void UT_LogsBaseView::testActivateSoftKey()
{
    QVERIFY( !hbInstance->allMainWindows().isEmpty() );
    mBaseView->activateSoftKey();
    QVERIFY( mBaseView->mSoftKeyBackAction == 
            hbInstance->allMainWindows().at(0)->softKeyAction(Hb::SecondarySoftKey) );
}

void UT_LogsBaseView::testClearSoftKey()
{
    QVERIFY( !hbInstance->allMainWindows().isEmpty() );
    mBaseView->clearSoftKey();
    QVERIFY( mBaseView->mSoftKeyBackAction != 
            hbInstance->allMainWindows().at(0)->softKeyAction(Hb::SecondarySoftKey) );
}


void UT_LogsBaseView::testShowFilterMenu()
{
    HbStubHelper::reset();
    //no context menu
    QVERIFY( !mBaseView->mShowFilterMenu );
    mBaseView->showFilterMenu();
    QVERIFY( !HbStubHelper::menuShown() );
    
    //contect menu exists
    mBaseView->mShowFilterMenu = new HbMenu();
    QVERIFY( mBaseView->mShowFilterMenu );
    mBaseView->showFilterMenu();
    QVERIFY( HbStubHelper::menuShown() );
    delete mBaseView->mShowFilterMenu;
    mBaseView->mShowFilterMenu = 0;
}

void  UT_LogsBaseView::testOpenDialpad()
{
    QVERIFY( !mBaseView->mDialpad->isVisible() );
    mBaseView->mDialpad->openDialpad();
    QVERIFY( mBaseView->mDialpad->isVisible() );    
}

void  UT_LogsBaseView::testSetDialpadPostion()
{
    QPointF pos = mBaseView->mDialpad->pos();
    HbMainWindow* window = hbInstance->allMainWindows().at(0);
    QVERIFY(window);
    
    window->setOrientation( Qt::Horizontal );
    mBaseView->setDialpadPosition();
    QVERIFY( pos != mBaseView->mDialpad->pos() );
    pos = mBaseView->mDialpad->pos();
    
    window->setOrientation( Qt::Vertical );
    mBaseView->setDialpadPosition();
    QVERIFY( pos != mBaseView->mDialpad->pos() );
}

void  UT_LogsBaseView::testDialpadClosed()
{
    mBaseView->mDialpad->editor().setText( QString("hello") );
    mBaseView->dialpadClosed();
    QVERIFY( mBaseView->mDialpad->editor().text().isEmpty() );
}


void  UT_LogsBaseView::testDialpadEditorTextChanged()
{   
    //text editor is one character long
    mBaseView->mDialpad->editor().setText( QString("h") );
    mBaseView->dialpadEditorTextChanged();
    QVERIFY( mViewManager->mViewId == LogsMatchesViewId );
    
    mViewManager->reset();
    mBaseView->mDialpad->mIsCallButtonEnabled = true;
    mBaseView->mDialpad->editor().setText( QString("") );
    mBaseView->dialpadEditorTextChanged();
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
    QVERIFY( !mBaseView->mDialpad->mIsCallButtonEnabled );
}

void  UT_LogsBaseView::testChangeFilter()
{
    QVERIFY( mViewManager->mViewId == LogsUnknownViewId );
    HbAction*  action = new HbAction();
    action->setObjectName(logsShowFilterMissedMenuActionId);
    mBaseView->changeFilter(action);
    QVERIFY( mViewManager->mViewId == LogsRecentViewId );
}

void UT_LogsBaseView::testModel()
{
    QVERIFY( !mBaseView->model() );
}

void UT_LogsBaseView::testOpenContactsApp()
{
    QtHighwayStubHelper::reset();
    mBaseView->openContactsApp();
    QVERIFY( QtHighwayStubHelper::service() == "com.nokia.services.phonebookappservices.Launch" );
    QVERIFY( QtHighwayStubHelper::message() == "launch()" );
}


void UT_LogsBaseView::testInitiateCallback()
{
    LogsCall::resetTestData();
    //call can't be created
    QVERIFY( !mBaseView->mCall );
    mBaseView->initiateCallback(QModelIndex());
    QVERIFY( LogsCall::lastCalledFuntion().isEmpty() );
    
    //call can be created should be tested in derived class    
}

void UT_LogsBaseView::testInitiateCall()
{
    LogsCall::resetTestData();
    //no call
    QVERIFY( !mBaseView->mCall );
    mBaseView->initiateCall(LogsCall::TypeLogsVoiceCall);
    QVERIFY( LogsCall::lastCalledFuntion().isEmpty() );
    
    //call exists
    mBaseView->mCall = new LogsCall();
    mBaseView->initiateCall(LogsCall::TypeLogsVideoCall);
    QVERIFY( LogsCall::lastCalledFuntion() == QString("call") );
    QVERIFY( mBaseView->mCall->mTestLastCallType == LogsCall::TypeLogsVideoCall );
}

void UT_LogsBaseView::testShowListItemMenu()
{
    HbStubHelper::reset();
    //menu doen't have any actions
    HbListViewItem* item = new HbListViewItem();
    mBaseView->showListItemMenu( item, QPointF() );
    QVERIFY( HbStubHelper::widgetActionsCount() == 0 );
    QVERIFY( !HbStubHelper::menuShown() );
    
    //menu has actions and can be executed should be tested in derived class
}

void UT_LogsBaseView::testPopulateListItemMenu()
{
    HbStubHelper::reset();
    //no call/message/contact, menu has not actions
    HbMenu menu;
    mBaseView->populateListItemMenu(menu);
    QVERIFY( HbStubHelper::widgetActionsCount() == 0 );
    
    //call, message and contact exist, menu has actions
    mBaseView->mCall = new LogsCall();
    mBaseView->mMessage = new LogsMessage();
    mBaseView->mContact = new LogsContact();
    mBaseView->populateListItemMenu(menu);
    QVERIFY( HbStubHelper::widgetActionsCount() == 5 );
}

void UT_LogsBaseView::testUpdateCall()
{
    mBaseView->mCall = new LogsCall();
    mBaseView->updateCall( QModelIndex() );
    QVERIFY( !mBaseView->mCall );
}

void UT_LogsBaseView::testUpdateMessage()
{    
    mBaseView->mMessage = new LogsMessage();
    mBaseView->updateMessage( QModelIndex() );
    QVERIFY( !mBaseView->mMessage );
}

void UT_LogsBaseView::testUpdateContact()
{
    mBaseView->mContact = new LogsContact();
    mBaseView->updateContact( QModelIndex() );
    QVERIFY( !mBaseView->mContact );
}

void UT_LogsBaseView::testActivateEmptyListIndicator()
{
    QStringListModel model;
    
    // Label not in repository
    QVERIFY( !mBaseView->mEmptyListLabel );
    mBaseView->activateEmptyListIndicator(&model);
    QVERIFY( !mBaseView->mEmptyListLabel );
    
    // Empty model, label is set visible
    mRepository->recentCallsView();
    QVERIFY( !mBaseView->mEmptyListLabel );
    mBaseView->activateEmptyListIndicator(&model);
    QVERIFY( mBaseView->mEmptyListLabel );
    
    // Label exists already, situation does not change
    mBaseView->activateEmptyListIndicator(&model);
    QVERIFY( mBaseView->mEmptyListLabel );
    
    // Null list does nothing
    mBaseView->activateEmptyListIndicator(0);
    QVERIFY( mBaseView->mEmptyListLabel );
}

void UT_LogsBaseView::testDeactivateEmptyListIndicator()
{
    QVERIFY( !mBaseView->mEmptyListLabel );
    mBaseView->deactivateEmptyListIndicator(0);
    HbLabel label;
    mBaseView->mEmptyListLabel = &label;
    mBaseView->deactivateEmptyListIndicator(0);
    QVERIFY( mBaseView->mEmptyListLabel ); // Label not removed at deactivation
}

void UT_LogsBaseView::testUpdateEmptyListLabelVisibility()
{
     // Base view does not have model, nothing happens
    mBaseView->updateEmptyListLabelVisibility();
}

void UT_LogsBaseView::testUpdateListLayoutName()
{
    mBaseView->mViewManager.mainWindow().setOrientation( Qt::Vertical );
    HbListView list;

    //default layout
    list.setLayoutName("dummy");
    mBaseView->updateListLayoutName(list);
    QVERIFY( list.layoutName() == logsListDefaultLayout );
    
    //same layout, not updated
    mBaseView->updateListLayoutName(list);
    QVERIFY( list.layoutName() == logsListDefaultLayout );
    
    //landscape without dialpad
    mBaseView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    mBaseView->updateListLayoutName(list);
    QVERIFY( list.layoutName() == logsListLandscapeLayout );
    
    //landscape with dialpad
    mBaseView->mDialpad->setVisible(true);
    mBaseView->updateListLayoutName(list);
    QVERIFY( list.layoutName() == logsListLandscapeDialpadLayout );
    
    //landscape with dialpad, ignoreDialpad is true
    mBaseView->updateListLayoutName(list, true);
    QVERIFY( list.layoutName() == logsListLandscapeLayout );    
}

void UT_LogsBaseView::testUpdateListSize()
{
    QString currentSection("dummy");
    
    //default section is loaded
    mBaseView->mViewManager.mainWindow().setOrientation( Qt::Vertical );
    mBaseView->mDialpad->setVisible(false);
    mBaseView->updateListSize(currentSection);
    QVERIFY( currentSection == logsViewDefaultSection );
    
    //same section again, not loaded
    mBaseView->updateListSize(currentSection);
    QVERIFY( currentSection == logsViewDefaultSection );

    //portrait with dialpad
    mBaseView->mDialpad->setVisible(true);
    mBaseView->updateListSize(currentSection);
    QVERIFY( currentSection == logsViewPortraitDialpadSection );
    
    //landscape with dialpad
    mBaseView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    mBaseView->updateListSize(currentSection);
    QVERIFY( currentSection == logsViewLandscapeDialpadSection );
}

void UT_LogsBaseView::testHandleAboutToChangeOrientation()
{
    mBaseView->mDialpad->openDialpad();
    QVERIFY( mBaseView->mDialpad->isVisible() );
    QVERIFY( !mBaseView->mForceDialpadOpened );
    mBaseView->handleAboutToChangeOrientation();
    QVERIFY( !mBaseView->mDialpad->isVisible() );
    QVERIFY( mBaseView->mForceDialpadOpened );    
}

void UT_LogsBaseView::testHandleOrientationChanged()
{
    //dialpad won't be opened after orientation change
    QVERIFY( !mBaseView->mForceDialpadOpened );
    QVERIFY( !mBaseView->mDialpad->isVisible() );
    mBaseView->handleOrientationChanged();
    QVERIFY( !mBaseView->mForceDialpadOpened );
    QVERIFY( !mBaseView->mDialpad->isVisible() );
    
    //dialpad will be opened after orientation change
    mBaseView->mForceDialpadOpened = true;
    mBaseView->handleOrientationChanged();
    QVERIFY( !mBaseView->mForceDialpadOpened );
    QVERIFY( mBaseView->mDialpad->isVisible() );    
}

void UT_LogsBaseView::testHandleExit()
{
    mViewManager->reset();
    mBaseView->handleExit();
    QVERIFY( mViewManager->mExitCalled );
}

void UT_LogsBaseView::testIsExitAllowed()
{
    QVERIFY( mBaseView->isExitAllowed() );
}
