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
#include "simutility.h"

CntDefaultViewManager::CntDefaultViewManager( HbMainWindow* aWindow ) : QObject(),
    mFactory(NULL),
    mCurrent(NULL),
    mOldView(NULL),
    mNavigator(NULL),
    mMainWindow( aWindow ),
    mSimUtility(NULL)

{    
    setViewFactory(new CntDefaultViewFactory());
    setViewNavigator(new CntViewNavigator(this));

    int error = -1;
    mSimUtility = new SimUtility(SimUtility::AdnStore, error);
    if (error != 0) 
    {
        delete mSimUtility; 
        mSimUtility = 0;
    }
}

CntDefaultViewManager::~CntDefaultViewManager()
{
    qDeleteAll(mDefaults.values());
    delete mFactory;
    delete mSimUtility;
}

void CntDefaultViewManager::setViewFactory( CntAbstractViewFactory* aFactory ) 
{
    if ( aFactory )
    {
        mFactory = aFactory;
    }
}

void CntDefaultViewManager::setViewNavigator( CntViewNavigator* aNavigator )
{
    if ( aNavigator )
    {
        mNavigator = aNavigator;
    }
}

void CntDefaultViewManager::back(const CntViewParameters aArgs)
{
    mArgs.clear();
    
    QFlags<Hb::ViewSwitchFlag> flags;
    int back = mNavigator->back( flags );

    foreach( int k, aArgs.keys() )
    {
        mArgs.insert( k, aArgs.value(k) );
    }
    mArgs.insert(EViewId, back );
//    mArgs.insert(ESelectedAction, aArgs.value(ESelectedAction));
//    mArgs.insert(ESelectedContact, aArgs.value(ESelectedContact));
//    mArgs.insert(ESelectedGroupContact, aArgs.value(ESelectedGroupContact));
//    mArgs.insert(ESelectedDetail, aArgs.value(ESelectedDetail));

    if (mArgs.value(EViewId).toInt() != noView)
    {
        switchView( mArgs, flags );
    }
}

void CntDefaultViewManager::changeView(const CntViewParameters aArgs)
{
    QFlags<Hb::ViewSwitchFlag> flags;
    mNavigator->next(aArgs.value(EViewId).toInt(), flags);
    switchView(aArgs, flags);
}

QContactManager* CntDefaultViewManager::contactManager( const QString& aType )
{
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
    
    return manager;
}

void CntDefaultViewManager::removeCurrentView()
{
    if (mOldView) 
    {
        connect(mMainWindow, SIGNAL(viewReady()), this, SLOT(deleteOldView()));
    }
    else
    {
        mMainWindow->setInteractive( true );
    }
}

void CntDefaultViewManager::deleteOldView()
{
    if (mOldView && !mOldView->view()->isVisible())
    {
        disconnect(mMainWindow, SIGNAL(viewReady()), this, SLOT(deleteOldView()));
        mOldView->deactivate();
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

        mMainWindow->setInteractive(true);
    }
}

void CntDefaultViewManager::switchView(const CntViewParameters aArgs, QFlags<Hb::ViewSwitchFlag> flags)
{
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
}
// End of File
