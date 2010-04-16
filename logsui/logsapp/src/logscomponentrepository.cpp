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
#include "logscomponentrepository.h"
#include "logsrecentcallsview.h"
#include "logsmatchesview.h"
#include "logsdetailsview.h"
#include "logslogger.h"
#include "logsmodel.h"
#include "logsabstractviewmanager.h"

//SYSTEM
#include <QObject>
#include <dialpad.h>
#include <dialpadkeyhandler.h>
#include <hbtoolbar.h>

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsComponentRepository::LogsComponentRepository(LogsAbstractViewManager& viewManager)
    : HbDocumentLoader(),
      mViewManager(viewManager),
      mRecentCallsView(0),
      mDetailsView(0),
      mMatchesView(0),
      mDialpad(0),
      mDialpadKeyHandler(0)
{
    mModel = new LogsModel();
}
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsComponentRepository::~LogsComponentRepository()
{
    LOGS_QDEBUG( "logs [UI] -> LogsComponentRepository::~LogsComponentRepository()" );

    mRecentViewComponents.clear();
    mDetailsViewComponents.clear();
    mMatchesViewComponents.clear();
    delete mDialpadKeyHandler;
    delete mDialpad;
    delete mModel;
    
    LOGS_QDEBUG( "logs [UI] <- LogsComponentRepository::~LogsComponentRepository()" );
}


// -----------------------------------------------------------------------------
// HbDocumentLoader will not add HbToolBar into the object list (at least on w02),
// since toolbar has a parent (view), so let's do it ourselves
// -----------------------------------------------------------------------------
//
void LogsComponentRepository::addToolbarToObjectList( QObjectList& list )
{
    LOGS_QDEBUG( "logs [UI] -> LogsComponentRepository::addToolbarToObjectList()" );
    HbToolBar* toolbar = qobject_cast<HbToolBar*>( findWidget( logsToolbarId ) );            
    if ( toolbar && !list.contains(toolbar) ) {
        LOGS_QDEBUG( "logs [UI] adding toolbar to object list" );
        list.append(toolbar);
    }
    LOGS_QDEBUG( "logs [UI] <- LogsComponentRepository::addToolbarToObjectList()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsRecentCallsView* LogsComponentRepository::recentCallsView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsComponentRepository::recentCallsView()" );
    setObjectTreeToView( LogsRecentViewId );
    
    if ( !mRecentCallsView ) {
        bool ok = false;
        mRecentViewComponents = load( logsRecentCallsViewFile, &ok );
        if ( ok ) {
            mRecentCallsView = qobject_cast<LogsRecentCallsView*>
                ( findWidget(logsRecentCallsViewId) );
            
            addToolbarToObjectList(mRecentViewComponents);
        } else {
            LOGS_QCRITICAL( "logs [UI] XML loading failed..." );
        }         
    }
     
    LOGS_QDEBUG( "logs [UI] <- LogsComponentRepository::recentCallsView()" );
    return mRecentCallsView;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsDetailsView* LogsComponentRepository::detailsView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsComponentRepository::detailsView()" );   
    setObjectTreeToView( LogsDetailsViewId );
    
    if ( !mDetailsView ) {
        bool ok = false;
        mDetailsViewComponents = load( logsDetailsViewFile, &ok );
        if ( ok ) {
            mDetailsView = qobject_cast<LogsDetailsView*>
                ( findWidget(logsDetailsViewId) );
            
            addToolbarToObjectList(mDetailsViewComponents);
        } else {
            LOGS_QCRITICAL( "logs [UI] XML loading failed..." );
        }         
    }
    LOGS_QDEBUG( "logs [UI] -> LogsComponentRepository::detailsView()" );
    return mDetailsView;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsMatchesView* LogsComponentRepository::matchesView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsComponentRepository::matchedCallsView()" );
    setObjectTreeToView( LogsMatchesViewId );

    if ( !mMatchesView ) {
        bool ok = false;
        mMatchesViewComponents = load( logsMatchedCallsViewFile, &ok );
        if ( ok ) {
            mMatchesView = qobject_cast<LogsMatchesView*>
                ( findWidget(logsMatchesViewId) );

            addToolbarToObjectList(mMatchesViewComponents);
        } else {
            LOGS_QCRITICAL( "logs [UI] XML loading failed..." );
        }         
    }
     
    LOGS_QDEBUG( "logs [UI] <- LogsComponentRepository::matchedCallsView()" );
    return mMatchesView;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
Dialpad* LogsComponentRepository::dialpad()
{
    if ( !mDialpad ) {
        mDialpad = new Dialpad(mViewManager.mainWindow());
        mDialpadKeyHandler = 
            new DialpadKeyHandler(mDialpad, mViewManager.mainWindow());
    }
    return mDialpad;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsModel*  LogsComponentRepository::model() const
{
    return mModel;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void  LogsComponentRepository::setObjectTreeToView( LogsAppViewId viewId )
{
    reset();
    if ( viewId == LogsRecentViewId ) {
        setObjectTree( mRecentViewComponents );
    } else if ( viewId == LogsDetailsViewId ) {
        setObjectTree( mDetailsViewComponents );
    } else if ( viewId == LogsMatchesViewId ) {
        setObjectTree( mMatchesViewComponents );        
    }
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
QObject* LogsComponentRepository::createObject(const QString& type, 
                                               const QString &name)
{
    LOGS_QDEBUG(  "logs [UI] -> LogsComponentRepository::createObject()" );

    QObject* object = 0;

    if ( name == logsRecentCallsViewId ) {
        LOGS_QDEBUG(  "logs [UI]    creating recent calls view" );
        object = new LogsRecentCallsView(*this, mViewManager);           
    } else if ( name == logsDetailsViewId ){
        LOGS_QDEBUG(  "logs [UI]    creating details view" );
        object = new LogsDetailsView(*this, mViewManager); 
    } else if ( name == logsMatchesViewId ){
        LOGS_QDEBUG(  "logs [UI]    creating matches view" );
        object = new LogsMatchesView(*this, mViewManager); 
    } else {
        LOGS_QDEBUG( "logs [UI]    creating object using document loader" );
        object = HbDocumentLoader::createObject( type, name );
    }
    
    if (object)
        object->setObjectName(name);

    LOGS_QDEBUG(  "logs [UI] <- LogsComponentRepository::createObject()" );
    return object;  
}


// -----------------------------------------------------------------------------
// LogsComponentRepository::loadSection
// -----------------------------------------------------------------------------
//
bool LogsComponentRepository::loadSection(  LogsAppViewId viewId, 
        const QString& sectionName )
    {
    LOGS_QDEBUG( "logs [UI] -> LogsComponentRepository::loadSection()" )    
    bool ok( false );
  
    QString fileName;
    if ( viewId == LogsRecentViewId ) {
        fileName = logsRecentCallsViewFile;
    } else if ( viewId == LogsMatchesViewId ) {
        fileName = logsMatchedCallsViewFile;
    } else if ( viewId == LogsDetailsViewId ) {
        fileName = logsDetailsViewFile;
    }
    
    if ( fileName.length() > 0 ) {
        load( fileName, sectionName, &ok );
    }
    
    LOGS_QDEBUG_2( "logs [UI] -> LogsComponentRepository::loadSection(): ", ok )    
    return ok;
    }

