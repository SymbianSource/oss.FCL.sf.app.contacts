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
#include "cntdefaultengine.h"

CntDefaultViewManager::CntDefaultViewManager( HbMainWindow* aWindow ) : QObject(),
    mFactory(NULL),
    mCurrent(NULL),
    mOldView(NULL),
    mNavigator(NULL),
    mMainWindow( aWindow ),
    mSimUtility(NULL),
    mEngine(NULL)
{
    CNT_ENTRY
    
    mEngine = new CntDefaultEngine( *this );

    int error = -1;
    mSimUtility = new CntSimUtility(CntSimUtility::AdnStore, error);
    if (error != 0) 
    {
        delete mSimUtility; 
        mSimUtility = 0;
    }
    
    CNT_EXIT
}

CntAbstractEngine& CntDefaultViewManager::engine()
{
    return *mEngine;
}

CntDefaultViewManager::~CntDefaultViewManager()
{
    CNT_ENTRY
    
    cleanup();

    delete mEngine;
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

void CntDefaultViewManager::back(const CntViewParameters aArgs, bool toRoot)
{
    CNT_ENTRY
    
    mArgs.clear();
    
    QFlags<Hb::ViewSwitchFlag> flags;
    int back = mNavigator->back( flags, toRoot );

    foreach( int k, aArgs.keys() )
    {
        mArgs.insert( k, aArgs.value(k) );
    }
    
    mArgs.insert(EViewId, back );

    if (mArgs.value(EViewId).toInt() != noView)
    {
        switchView( mArgs, flags );
    }
    else 
    {  
        // exiting application
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
    
    foreach(int k, aArgs.keys())
    {
        mArgs.insert(k, aArgs.value(k));
    }
    
    switchView(aArgs, flags);
    
    CNT_EXIT
}

void CntDefaultViewManager::removeCurrentView()
{
    CNT_ENTRY
    
    if (mOldView) 
    {
        connect(mMainWindow, SIGNAL(viewReady()), this, SLOT(deleteOldView()));
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

        HbView* oldView = mOldView->view();
        mMainWindow->removeView( oldView );

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
    
    CNT_EXIT
}

void CntDefaultViewManager::switchView(const CntViewParameters args, QFlags<Hb::ViewSwitchFlag> flags)
{
    CNT_ENTRY

    int id = args.value(EViewId).toInt();
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
            nextView->setEngine( *mEngine );
            
            if (nextView->isDefault())
            {
                mDefaults.insert(id, nextView);
            }
        }
        
        mOldView = mCurrent;
        activateView(nextView, args, flags);
        removeCurrentView();
    }
    
    CNT_EXIT
}

void CntDefaultViewManager::activateView(CntAbstractView* nextView, const CntViewParameters args, QFlags<Hb::ViewSwitchFlag> flags)
{
    CNT_ENTRY
    
    mCurrent = nextView;
    mMainWindow->addView(mCurrent->view());
    mMainWindow->setCurrentView(mCurrent->view(), true, flags);
    mCurrent->activate(args);
    
    CNT_EXIT
}

bool CntDefaultViewManager::internalize(QDataStream &stream)
{
    CNT_ENTRY
    
    int id = mNavigator->internalize(stream);   
    if (id == noView)
    {
        return false;
    }
    
    CntAbstractView* nextView(NULL);
    nextView = mFactory->createView(id);
    nextView->setEngine(*mEngine);
     
    CntViewParameters viewParams;
    
    bool success = nextView->internalize(stream, viewParams);
    
    if (success)
    {
       if (nextView->isDefault())
       {
           mDefaults.insert(id, nextView);
       }
       QFlags<Hb::ViewSwitchFlag> flags;
       mNavigator->next(id, flags);
       
       activateView(nextView, viewParams, flags);
    }
    else
    {
        mNavigator->clearViewStack();
        delete nextView;
    }
      
    CNT_EXIT  
    return success;
}

QString CntDefaultViewManager::externalize(QDataStream& stream) const
{
    CNT_ENTRY
    
    mNavigator->externalize(stream);
    
    QString activity;
    
    if (mCurrent)
    {
        activity = mCurrent->externalize(stream); 
    }
       
    CNT_EXIT
    return activity;  
}

// End of File
