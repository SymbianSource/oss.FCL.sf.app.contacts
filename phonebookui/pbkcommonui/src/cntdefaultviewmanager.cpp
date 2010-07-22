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

#include "cntdefaultviewmanager.h"
#include "cntabstractviewfactory.h"
#include <cntabstractview.h>
#include "cntmainwindow.h"
#include <hbview.h>
#include <hbmainwindow.h>
#include <qtcontacts.h>
#include "cntviewnavigator.h"
#include "cntsimutility.h"
#include "cntdebug.h"
#include <QApplication>

CntDefaultViewManager::CntDefaultViewManager( HbMainWindow* aWindow ) : QObject(),
    mFactory(NULL),
    mCurrent(NULL),
    mOldView(NULL),
    mNavigator(NULL),
    mMainWindow( aWindow ),
    mSimUtility(NULL)
{
    CNT_ENTRY
    
    setViewFactory(new CntDefaultViewFactory());
    setViewNavigator(new CntViewNavigator(this));

    int error = -1;
    mSimUtility = new CntSimUtility(CntSimUtility::AdnStore, error);
    if (error != 0) 
    {
        delete mSimUtility; 
        mSimUtility = 0;
    }
    
    CNT_EXIT
}

CntDefaultViewManager::~CntDefaultViewManager()
{
    CNT_ENTRY
    
    cleanup();

    CNT_EXIT
}

void CntDefaultViewManager::cleanup()
{
    CNT_ENTRY

    mOldView = mCurrent;
    mCurrent = NULL;
    deleteOldView();
    
    qDeleteAll(mBackends);
    mBackends.clear();

    qDeleteAll(mDefaults.values());
    mDefaults.clear();

    delete mFactory;
    mFactory = NULL;

    delete mSimUtility;
    mSimUtility = NULL;
    
    CNT_EXIT
}

void CntDefaultViewManager::setViewFactory( CntAbstractViewFactory* aFactory ) 
{
    CNT_ENTRY
    
    if ( aFactory )
    {
        if (mFactory) {
            delete mFactory;
        }
        mFactory = aFactory;
    }
    
    CNT_EXIT
}

void CntDefaultViewManager::setViewNavigator( CntViewNavigator* aNavigator )
{
    CNT_ENTRY
    
    if ( aNavigator )
    {
        if (mNavigator) {
            delete mNavigator;
        }
        mNavigator = aNavigator;
    }
    
    CNT_EXIT
}

void CntDefaultViewManager::back(const CntViewParameters aArgs)
{
    CNT_ENTRY
    
    mArgs.clear();
    
    QFlags<Hb::ViewSwitchFlag> flags;
    int back = mNavigator->back( flags );

    foreach( int k, aArgs.keys() )
    {
        mArgs.insert( k, aArgs.value(k) );
    }
    mArgs.insert(EViewId, back );

    if (mArgs.value(EViewId).toInt() != noView)
    {
        switchView( mArgs, flags );
    }
    else {
        // exiting application
        cleanup();
        closeApp();
    }
    
    CNT_EXIT
}

void CntDefaultViewManager::closeApp()
{
    CNT_ENTRY
    
    qApp->quit();
    
    CNT_EXIT
}

void CntDefaultViewManager::changeView(const CntViewParameters aArgs)
{
    CNT_ENTRY
    
    QFlags<Hb::ViewSwitchFlag> flags;
    mNavigator->next(aArgs.value(EViewId).toInt(), flags);
    switchView(aArgs, flags);
    
    CNT_EXIT
}

QContactManager* CntDefaultViewManager::contactManager( const QString& aType )
{
    CNT_ENTRY
    
    foreach ( QContactManager* mgr, mBackends ) 
    {
        QString uri = mgr->managerUri();
        if ( aType.compare(uri, Qt::CaseInsensitive) == 0 )
        {
            return mgr;
        }
    }
    QContactManager* manager = QContactManager::fromUri( aType );
    
    if ( manager )
    {
        mBackends.append( manager );
    }
    
    CNT_EXIT
    return manager;
}

void CntDefaultViewManager::removeCurrentView()
{
    CNT_ENTRY
    
    if (mOldView) 
    {
        connect(mMainWindow, SIGNAL(viewReady()), this, SLOT(deleteOldView()));
    }
    else
    {
        mMainWindow->setInteractive( true );
    }
    
    CNT_EXIT
}

void CntDefaultViewManager::deleteOldView()
{
    CNT_ENTRY
    
    disconnect(mMainWindow, SIGNAL(viewReady()), this, SLOT(deleteOldView()));
    
    if (mOldView)
    {
        mOldView->deactivate();
        
        // Due to something strange in wk16, this check will fail occationally and cause
        // a memory leak... most likely when opening edit view for the first time
        if (mCurrent == NULL || !mOldView->view()->isVisible())
        {
            mMainWindow->removeView(mOldView->view());
            
            if (!mOldView->isDefault())
            {
                delete mOldView;
                mOldView = NULL;
            }
            // If view id is not in defaults list, it means that view has changed
            // its opinnion about preserving state to true.
            else if ( !mDefaults.contains(mOldView->viewId()) ) 
            {
                mDefaults.insert( mOldView->viewId(), mOldView );
            }
        }
    }

    mMainWindow->setInteractive(true);
    
    CNT_EXIT
}

void CntDefaultViewManager::switchView(const CntViewParameters aArgs, QFlags<Hb::ViewSwitchFlag> flags)
{
    CNT_ENTRY
    
    mMainWindow->setInteractive(false);
    int id = aArgs.value(EViewId).toInt();
    if ( id != noView )
    {
        CntAbstractView* nextView(NULL);
        if (mDefaults.contains(id))
        {
            nextView = mDefaults.value(id);
        }
        else
        {
            nextView = mFactory->createView( id );
            if (nextView->isDefault())
            {
                mDefaults.insert(id, nextView);
            }
        }
        
        mOldView = mCurrent;
        mCurrent = nextView;
        mMainWindow->addView(mCurrent->view());
        mMainWindow->setCurrentView(mCurrent->view(), true, flags);
        mCurrent->activate(this, aArgs);
        
        removeCurrentView();
    }
    
    CNT_EXIT
}
// End of File
