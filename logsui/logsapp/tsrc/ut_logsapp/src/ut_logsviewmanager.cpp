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
#include "ut_logsviewmanager.h"
#include "logsviewmanager.h"
#include "logsservicehandler.h"
#include "logsmainwindow.h"
#include "logscomponentrepository.h"
#include "logsrecentcallsview.h"
#include "logsdetailsview.h"
#include "hbstubs_helper.h"

//SYSTEM
#include <HbMainWindow.h>
#include <QtTest/QtTest>
#include <HbView.h>
#include <hbapplication.h>

void UT_LogsViewManager::initTestCase()
{
    //mMainWindow =  new LogsMainWindow();
}

void UT_LogsViewManager::cleanupTestCase()
{
}


void UT_LogsViewManager::init()
{
    mMainWindow =  new LogsMainWindow();
    LogsServiceHandler* service = new LogsServiceHandler();
    mLogsViewManager = new LogsViewManager(*mMainWindow, *service);
}

void UT_LogsViewManager::cleanup()
{
    delete mLogsViewManager;
    mLogsViewManager = 0;
    delete mMainWindow;
    mMainWindow = 0;
}

void UT_LogsViewManager::testConstructorDestructor()
{
    QVERIFY( mLogsViewManager );
    QVERIFY( mLogsViewManager->mComponentsRepository );
    QVERIFY( mLogsViewManager->mMainWindow.viewCount() == 3 );
    QVERIFY( mLogsViewManager->mMainWindow.currentView() == 
                mLogsViewManager->mComponentsRepository->recentCallsView() );
    QVERIFY( mLogsViewManager->mViewStack.count() == 3 );
    
    delete mLogsViewManager;
    mLogsViewManager = 0;
    //TODO: removeView deprecated => this will fail
    //QVERIFY( mMainWindow->viewCount() == 0 );
}

void UT_LogsViewManager::testActivateView()
{
    // Activate already active view
    QVERIFY( mLogsViewManager->mMainWindow.currentView() == 
             mLogsViewManager->mComponentsRepository->recentCallsView() );
    QVERIFY( mLogsViewManager->activateView(LogsRecentViewId) );
    QVERIFY( mLogsViewManager->mMainWindow.viewCount() == 3 );
    QVERIFY( mLogsViewManager->mMainWindow.currentView() == 
             mLogsViewManager->mComponentsRepository->recentCallsView() );
    
    // Activate other view
    QVERIFY( mLogsViewManager->activateView(LogsDetailsViewId) );
    QVERIFY( mLogsViewManager->mMainWindow.viewCount() == 3 );
    // Current view does not immediately return new view as it waits
    // view change effect completion
    //QVERIFY( mLogsViewManager->mMainWindow.currentView() == 
    //         mLogsViewManager->mComponentsRepository->detailsView() );
    QVERIFY( mLogsViewManager->mViewStack.at(0) == 
             mLogsViewManager->mComponentsRepository->detailsView() );
    
    // Try to activate unknown view
    QVERIFY( !mLogsViewManager->activateView(LogsUnknownViewId) );
    QVERIFY( mLogsViewManager->mMainWindow.viewCount() == 3 );
    //QVERIFY( mLogsViewManager->mMainWindow.currentView() == 
    //         mLogsViewManager->mComponentsRepository->detailsView() );
    QVERIFY( mLogsViewManager->mViewStack.at(0) == 
             mLogsViewManager->mComponentsRepository->detailsView() );
    
    // Go back to previous view
    QVERIFY( mLogsViewManager->activatePreviousView() );
    QVERIFY( mLogsViewManager->mMainWindow.viewCount() == 3 );
    //QVERIFY( mLogsViewManager->mMainWindow.currentView() == 
    //         mLogsViewManager->mComponentsRepository->recentCallsView() );
    QVERIFY( mLogsViewManager->mViewStack.at(0) == 
             mLogsViewManager->mComponentsRepository->recentCallsView() );
}

void UT_LogsViewManager::testExitApplication()
{
    // Exit immediately possible
    HbStubHelper::reset();
    mLogsViewManager->exitApplication();
    QVERIFY( HbStubHelper::quitCalled() );
    
    // Exit not yet possible
    mLogsViewManager->mComponentsRepository->recentCallsView()->mMarkingMissedAsSeen = true;
    HbStubHelper::reset();
    mLogsViewManager->exitApplication();
    QVERIFY( !HbStubHelper::quitCalled() );
    
    // Simulate view allowing exit after denying it first
    mLogsViewManager->mComponentsRepository->recentCallsView()->mMarkingMissedAsSeen = false;
    mLogsViewManager->proceedExit();
    QVERIFY( HbStubHelper::quitCalled() );
}

void UT_LogsViewManager::testAppFocusGained()
{
    mLogsViewManager->appFocusGained();
    // Nothing to verify
}

void UT_LogsViewManager::testStartingWithService()
{     
    // If app is started with highway service, view is not activated
    // before service method call comes.
    LogsMainWindow window;
    window.setCurrentView(0); // clear stub static data
    LogsServiceHandler service;
    service.mIsAppStartedUsingService = true;
    LogsViewManager vm(window, service);
    QVERIFY( vm.mComponentsRepository );
    QVERIFY( vm.mMainWindow.viewCount() == 3 );
    QVERIFY( vm.mMainWindow.currentView() == 0 );
    QVERIFY( vm.mViewStack.count() == 3 );
}
