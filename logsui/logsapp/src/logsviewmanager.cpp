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
#include "logsviewmanager.h"
#include "logscomponentrepository.h"
#include "logsrecentcallsview.h"
#include "logsmatchesview.h"
#include "logsdetailsview.h"
#include "logsfilter.h"
#include "logsdefs.h"
#include "logslogger.h"
#include "logsservicehandler.h"
#include "logsmainwindow.h"

//SYSTEM
#include <hbmainwindow.h>
#include <hbview.h>
#include <logsservices.h>
#include <QApplication>

// -----------------------------------------------------------------------------
// LogsViewManager::LogsViewManager
// -----------------------------------------------------------------------------
//
LogsViewManager::LogsViewManager( 
        LogsMainWindow& mainWindow, LogsServiceHandler& service ) : 
    QObject( 0 ), mMainWindow( mainWindow ), mService( service )
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::LogsViewManager()" );

    //It is important that we always handle orientation change first, before
    //dialpad widget. If connection is moved to a view, then it's not guarantied.
    connect( &mainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(handleOrientationChanged()) );

    mComponentsRepository = new LogsComponentRepository(*this);
    initViews();
    
    connect( &mService, SIGNAL( activateView(LogsServices::LogsView, bool) ), 
             this, SLOT( changeRecentView(LogsServices::LogsView, bool) ));
    
    connect( &mMainWindow, SIGNAL(appFocusGained()), this, SLOT(appFocusGained()) );
    
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::LogsViewManager()" );
}


// -----------------------------------------------------------------------------
// LogsViewManager::~LogsViewManager
// -----------------------------------------------------------------------------
//
LogsViewManager::~LogsViewManager()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::~LogsViewManager()" );
     
    delete mComponentsRepository;
    
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::~LogsViewManager()" );
}

// -----------------------------------------------------------------------------
// LogsViewManager::initViews
// -----------------------------------------------------------------------------
//
void LogsViewManager::initViews()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::initViews()" );
    
    // Disable view switching as otherwise flick gestures change
    // views which is not desired.
    //Deprecated:
    //mMainWindow.setViewSwitchingEnabled(false);
    
    LogsRecentCallsView* view = mComponentsRepository->recentCallsView();    
    Q_ASSERT_X(view != 0, "logs [UI] ", "recentCallsView not found!");
    LOGS_QDEBUG( "logs [UI] adding view" );
    mViewStack.append(view);
    mMainWindow.addView(view);

    // If app was started by using service request, wait the request 
    // to appear before activating view.
    if ( !mService.isStartedUsingService() ){
        changeRecentView( mService.currentlyActivatedView(), false );
    }
        
    LogsDetailsView* detailsView = mComponentsRepository->detailsView();
    mViewStack.append(detailsView);
    mMainWindow.addView(detailsView);
    
    LogsMatchesView* matchesView = mComponentsRepository->matchesView();
    mViewStack.append(matchesView);
    mMainWindow.addView(matchesView);

    mComponentsRepository->setObjectTreeToView( LogsRecentViewId );

    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::initViews()" );
}

// -----------------------------------------------------------------------------
// LogsViewManager::changeRecentView
// -----------------------------------------------------------------------------
//
void LogsViewManager::changeRecentView(LogsServices::LogsView view, bool showDialpad)
{
    QVariant args(view);
    doActivateView(LogsRecentViewId, showDialpad, args);
}

// -----------------------------------------------------------------------------
// LogsViewManager::proceedExit
// -----------------------------------------------------------------------------
//
void LogsViewManager::proceedExit()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::proceedExit()" );
    exitApplication();
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::proceedExit()" );
}

// -----------------------------------------------------------------------------
// LogsViewManager::appFocusGained
// Dialer is sent to background in case some asycn operations need to
// be completed before app can be really exited. If app is brought
// to foreground during that period, asycn completion should not 
// cause anymore exit.
// -----------------------------------------------------------------------------
//
void LogsViewManager::appFocusGained()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::appFocusGained()" );
    foreach ( LogsBaseView* view, mViewStack ){
        disconnect( view, SIGNAL(exitAllowed()), this, SLOT(proceedExit()) );
    }
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::appFocusGained()" );
}

// -----------------------------------------------------------------------------
// LogsViewManager::activateView
// -----------------------------------------------------------------------------
//
bool LogsViewManager::activateView(LogsAppViewId viewId)
{
    return doActivateView(viewId, false, QVariant());
}

// -----------------------------------------------------------------------------
// LogsViewManager::activateView
// -----------------------------------------------------------------------------
//
bool LogsViewManager::activateView(
        LogsAppViewId viewId, bool showDialpad, QVariant args)
{
    return doActivateView(viewId, showDialpad, args);
}

// -----------------------------------------------------------------------------
// LogsViewManager::mainWindow
// -----------------------------------------------------------------------------
//
HbMainWindow& LogsViewManager::mainWindow()
{
    return mMainWindow;
}

// -----------------------------------------------------------------------------
// LogsViewManager::exitApplication
// -----------------------------------------------------------------------------
//
void LogsViewManager::exitApplication()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::exitApplication()" );
    
    bool exitAllowed( true );
    foreach ( LogsBaseView* view, mViewStack ){
        if ( !view->isExitAllowed() ){
            exitAllowed = false;
            connect( view, SIGNAL(exitAllowed()), this, SLOT(proceedExit()) );
        }
    }
    if ( exitAllowed ){
        LOGS_QDEBUG( "logs [UI] Exit immediataly" );
        qApp->quit();
    } else {
        LOGS_QDEBUG( "logs [UI] Exit delayed" );
        // Fake exit by sending app to background and do real exit
        // once it is allowed (i.e. all async cleanup operations have completed)
        mMainWindow.sendAppToBackground();
        // Activate default view as user could bring app back
        // to foreground before async exit operations have completed
        activateView( LogsRecentViewId );
    }
    
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::exitApplication()" );
}

// -----------------------------------------------------------------------------
// LogsViewManager::activatePreviousView
// Previously activated view is at slot 1 of view stack
// -----------------------------------------------------------------------------
//
bool LogsViewManager::activatePreviousView()
{
    if ( mViewStack.count() < 2 ){
        return false;
    }
    return doActivateView(mViewStack.at(1)->viewId(), false, QVariant());
}

// -----------------------------------------------------------------------------
// LogsViewManager::doActivateView
// Currently activated view is always at beginning of view stack. Deactivates
// previously activated view and activates new view and place it at top of
// view stack.
// -----------------------------------------------------------------------------
//
bool LogsViewManager::doActivateView(
        LogsAppViewId viewId, bool showDialpad, QVariant args)
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::doActivateView()" );
    
    bool activated(false);
    LogsBaseView* newView = 0; 
    LogsBaseView* oldView = mViewStack.count() > 0 ? mViewStack.at(0) : 0;
    
    for ( int i = 0; i < mViewStack.count(); ++i ){
        if ( mViewStack.at(i)->viewId() == viewId ){
            newView = mViewStack.takeAt(i);
        }
    }
    
    if ( oldView && newView && oldView != newView ){   
        oldView->deactivated();
        disconnect( &mMainWindow, SIGNAL( callKeyPressed() ), oldView, 0 );
    }
    
    if ( newView ){
        mViewStack.insert(0, newView);
        mMainWindow.setCurrentView(newView, true);
        newView->activated(showDialpad, args);
        connect( &mMainWindow, SIGNAL( callKeyPressed() ), newView, SLOT( callKeyPressed() ) );
        activated = true;
    }

    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::doActivateView()" );
    
    return activated;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsViewManager::handleOrientationChanged()
{
    LOGS_QDEBUG( "logs [UI] -> LogsViewManager::handleOrientationChanged()" );
    QMetaObject::invokeMethod(mMainWindow.currentView(), "handleOrientationChanged");
    LOGS_QDEBUG( "logs [UI] <- LogsViewManager::handleOrientationChanged()" );
}
