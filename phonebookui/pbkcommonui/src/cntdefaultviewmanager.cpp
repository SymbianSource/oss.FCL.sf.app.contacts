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
#include "cntabstractview.h"
#include "cntmainwindow.h"
#include <hbview.h>
//#include "cntmodelprovider.h"
#include <qtcontacts.h>
#include "cntviewnavigator.h"
#include "cntbaseview.h"

CntDefaultViewManager::CntDefaultViewManager(CntMainWindow* aWindow,
    CntViewParameters::ViewId defaultView) :
    CntViewManager(aWindow, defaultView), 
    mCurrent(0),
    mArgs( 0 )
{
    mFactory = new CntDefaultViewFactory();
    
    QContactManager* manager = new QContactManager(SIM_BACKEND);
    mBackends.append( manager );
   
    mNavigator = new CntViewNavigator(this);
    mNavigator->addException( CntViewParameters::editView, CntViewParameters::namesView );
    mNavigator->addException( CntViewParameters::FavoritesMemberView, CntViewParameters::collectionView );

    if (defaultView != CntViewParameters::noView) {
        //activate the view
        CntViewParameters viewParameters(defaultView, defaultView);
        changeView(viewParameters);
    }
}

CntDefaultViewManager::~CntDefaultViewManager()
{
    delete mFactory;
    delete mArgs;
}

void CntDefaultViewManager::back(const CntViewParameters& aArgs)
{
    if ( mArgs ) 
    {
        delete mArgs;
        mArgs = NULL;
    }
    mArgs = new CntViewParameters( mNavigator->back() );
    mArgs->setSelectedAction(aArgs.selectedAction());
    mArgs->setSelectedContact(aArgs.selectedContact());
    mArgs->setSelectedDetail(aArgs.selectedDetail());
    mArgs->setParameters(aArgs.parameters());
    mArgs->setSelectedGroupContact(aArgs.selectedGroupContact());

    removeCurrentView();
    if (mArgs->nextViewId() != CntViewParameters::noView) {
        switchView( *mArgs );
    }
}

void CntDefaultViewManager::changeView(const CntViewParameters& aArgs)
{
    removeCurrentView();
    
    mNavigator->next( aArgs.nextViewId() );
    switchView(aArgs);
}

QContactManager* CntDefaultViewManager::contactManager( const QString& aType )
{
    foreach ( QContactManager* mgr, mBackends ) 
    {
        if ( aType.compare(mgr->managerName(), Qt::CaseInsensitive) == 0 )
        {
            return mgr;
        }
    }
    QContactManager* manager = new QContactManager( aType );
    if ( manager )
    {
        mBackends.append( manager );
    }
    
    return manager;
}

bool CntDefaultViewManager::isDepracatedView(CntViewParameters::ViewId aId)
{
    switch (aId) 
    {
    case CntViewParameters::commLauncherView:return true;
    case CntViewParameters::serviceContactCardView:return true;
    case CntViewParameters::serviceAssignContactCardView:return true;
    case CntViewParameters::myCardSelectionView:return true;
    case CntViewParameters::serviceContactSelectionView:return true;
    case CntViewParameters::FavoritesMemberView:return true;
    case CntViewParameters::editView:return true;
    case CntViewParameters::serviceEditView:return true;
    case CntViewParameters::serviceSubEditView:return true;
    case CntViewParameters::emailEditorView:return true;
    case CntViewParameters::namesEditorView:return true;
    case CntViewParameters::urlEditorView:return true;
    case CntViewParameters::companyEditorView:return true;
    case CntViewParameters::phoneNumberEditorView:return true;
    case CntViewParameters::onlineAccountEditorView:return true;
    case CntViewParameters::noteEditorView:return true;
    case CntViewParameters::familyDetailEditorView:return true;
    case CntViewParameters::addressEditorView:return true;
    case CntViewParameters::dateEditorView:return true;
    case CntViewParameters::serviceContactFetchView:return true;
    case CntViewParameters::groupEditorView:return true;
    case CntViewParameters::groupMemberView:return true;
    case CntViewParameters::groupActionsView:return true;
    case CntViewParameters::historyView:return true;
    default:
        return false;
    }
}

void CntDefaultViewManager::removeCurrentView()
{
    if (mCurrent == NULL) {
        removeDepracatedCurrentView();
        return;
    }

    if (mCurrent) {
        mCurrent->deactivate();
        mMainWindow->removeView(mCurrent->view());
        
        if (!mCurrent->isDefault()) {
            delete mCurrent;
            mCurrent = NULL;
        }
    }
}

void CntDefaultViewManager::switchView(const CntViewParameters& aArgs)
{
    CntViewParameters::ViewId id = aArgs.nextViewId();
    if (isDepracatedView(id))
    {
        // current view might not be deleted if it was a "default" view
        // Still, needs to be nulled out so it won't be deactivated unnecessarily
        mCurrent = NULL;
           
        CntBaseView *newView = getView( aArgs );

        if (newView) {        
            mCurrentViewId = newView->viewId();
            addViewToWindow(newView);
            newView->activateView(aArgs);
        }
        
    }
    else
    {
        CntAbstractView* nextView(NULL);
        if (mDefaults.contains(id))
        {
            nextView = mDefaults.value(id);
        }
        else
        {
            nextView = mFactory->createView((int) id);
            if (nextView->isDefault())
            {
                mDefaults.insert(id, nextView);
            }
        }

        mCurrent = nextView;
        
        mMainWindow->addView(mCurrent->view());
        mMainWindow->setCurrentView(mCurrent->view());
        mCurrent->activate(this, aArgs);
    }
}
// End of File
