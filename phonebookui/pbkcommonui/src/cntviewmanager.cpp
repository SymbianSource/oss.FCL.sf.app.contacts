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

#include "cntviewmanager.h"
#include "cntbaseview.h"
#include "cntmainwindow.h"

#include "cntnamesview.h"
#include "cntcollectionview.h"
#include "cntfavoritesview.h"
#include "cntfavoritesselectionview.h"
#include "cntcontactcardview.h"
#include "cntemaileditorview.h"
#include "cntnameseditorview.h"
#include "cnturleditorview.h"
#include "cntcompanyeditorview.h" 
#include "cntphonenumbereditorview.h"
#include "cntonlineaccounteditorview.h"
#include "cntnoteeditorview.h"
#include "cntfamilydetaileditorview.h"
#include "cntaddresseditorview.h"
#include "cntdateeditorview.h"
#include "cntimageeditorview.h"
#include "cnteditview.h"
#include "cntmycardview.h"
#include "cntmycardselectionview.h"
#include "cntgroupeditorview.h"
#include "cntgroupmemberview.h"
#include "cntgroupactionsview.h"
#include "cnthistoryview.h"

#include <hbstyleloader.h>
/*!
\class CntViewManager
\brief
View manager is a class between mainwindow and different views. It is used to provide navigation between views and also creating the views for the mainwindow.
Pointer to viewmanager is passed to each created view and therefore they can access also QMainWindow that is normally not possible. This is needed e.g. when we want to
change or activate view from some other view.

*/


/*!
Constructor
*/
CntViewManager::CntViewManager(CntMainWindow *mainWindow, CntViewParameters::ViewId defaultView):
    mMainWindow(mainWindow),
    mDefaultView(0),
    mDefaultViewId(CntViewParameters::noView)
{
    // load styles
    HbStyleLoader::registerFilePath(":/style");

    if (defaultView!=CntViewParameters::noView)
    {
        //set the default view
        setDefaultView(defaultView);
    
        //activate the view
        CntViewParameters viewParameters(defaultView);
        setPreviousViewParameters(mDefaultView, viewParameters);
        mDefaultView->activateView(viewParameters);
    }
}

/*!
Destructor
*/
CntViewManager::~CntViewManager()
{

}

/*!
\return Pointer to mainwindow
*/
CntMainWindow *CntViewManager::mainWindow()
{
    return mMainWindow;
}

/*!
Set the default view
*/
void CntViewManager::setDefaultView(CntViewParameters::ViewId defaultView)
{
    CntBaseView *view = getView(defaultView);

    if (view && view != mDefaultView)
    {
        addViewToWindow(view);

        // delete the old default view if it exists
        if (mDefaultView)
        {
            mainWindow()->removeView(mDefaultView);
            mDefaultView->setParent(0);
            mDefaultView->deleteLater();
        }

        //set the new view as default
        mDefaultViewId = defaultView;
        mDefaultView = view;
    }
}

//save previous view's parameters
void CntViewManager::setPreviousViewParameters(const CntBaseView *currentView, const CntViewParameters &previousViewParameters)
{
    if (currentView)
    {
        mPreviousViewParameters = previousViewParameters;

        //set current view to be the activated when call previous
        mPreviousViewParameters.setNextViewId(currentView->viewId());

        //previous view is the view that is being actiavated
        mPreviousViewParameters.setPreviousViewId(previousViewParameters.nextViewId());
    }
}

//return previous view's parameters
CntViewParameters &CntViewManager::previousViewParameters()
{
    return mPreviousViewParameters;
}


/*!
Activate view based on \a id
View's will be kept in QStack locally, but ownership is in main window.
NamesView is kept in stack & memory all the time
*/
void CntViewManager::onActivateView(int aViewId)
{
    CntViewParameters viewParameters;
    viewParameters.setNextViewId(static_cast<CntViewParameters::ViewId>(aViewId));

    onActivateView(viewParameters);
}

void CntViewManager::onActivateView(const CntViewParameters &viewParameters)
{
    //get the old view
    CntBaseView *oldView = static_cast<CntBaseView *>(mainWindow()->currentView());

    // fetch the new view and activate it
    CntBaseView *newView = getView(viewParameters.nextViewId());

    if (newView)
    {        
        newView->setPreferredSize(oldView->size());
        addViewToWindow(newView);
        
        if (oldView == mDefaultView)
        {
            oldView->deActivateView();
        }
        
        if (oldView)
        {
            setPreviousViewParameters(oldView, viewParameters);
            removeViewFromWindow(oldView);
        }
        newView->activateView(viewParameters);
    }
}

void CntViewManager::onActivatePreviousView()
{
    onActivateView(previousViewParameters());
}

/*!
Add \a view to main window and set it as current view
*/
void CntViewManager::addViewToWindow(CntBaseView *view)
{
    // add view to mainwindow and set it as current one
    if (view)
    {
        //if not default view add the view to main window
        if (view != mDefaultView)
        {
            view->setupView();
            mainWindow()->addView(view);
        }

        //set the view as current
        mainWindow()->setCurrentView( view );
    }
}

/*!
Remove \a view from main window and delete it
*/
void CntViewManager::removeViewFromWindow(CntBaseView *view)
{
    if (view && view != mDefaultView)
    {
        mainWindow()->removeView(view);
        view->setParent(0);
        view->deleteLater();
    }
}

/*!
Create a view based on ID. \Return pointer to new object if success, 0 if not.
*/
CntBaseView *CntViewManager::getView(CntViewParameters::ViewId id)
{
    CntBaseView* view(0);

    //return default view if it exists
    if (id == mDefaultViewId && mDefaultView)
    {
        view = mDefaultView;
    }

    //return any other view
    else
    {
        switch (id)
        {
            case CntViewParameters::namesView:
                {
                view = new CntNamesView(this);
                break;
                }
            case CntViewParameters::collectionView:
                {
                view = new CntCollectionView(this);
                break;
                }
            case CntViewParameters::collectionFavoritesView:
                {
                view = new CntFavoritesView(this);
                break;
                }
            case CntViewParameters::collectionFavoritesSelectionView:
                {
                view = new CntFavoritesSelectionView(this);
                break;
                }
            case CntViewParameters::commLauncherView:
                {
                view = new CntContactCardView(this);
                break;
                }
            case CntViewParameters::emailEditorView:
                {
                view = new CntEmailEditorView(this);
                break;
                }
            case CntViewParameters::namesEditorView:
                {
                view = new CntNamesEditorView(this);
                break;
                }
            case CntViewParameters::urlEditorView:
                {
                view = new CntUrlEditorView(this);
                break;
                }
            case CntViewParameters::companyEditorView:
                {
                view = new CntCompanyEditorView(this);
                break;
                }
            case CntViewParameters::phoneNumberEditorView:
                {
                view = new CntPhoneNumberEditorView(this);
                break;
                }
            case CntViewParameters::onlineAccountEditorView:
                {
                view = new CntOnlineAccountEditorView(this);
                break;
                }
            case CntViewParameters::noteEditorView:
                {
                view = new CntNoteEditorView(this);
                break;
                }
            case CntViewParameters::familyDetailEditorView:
                {
                view = new CntFamilyDetailEditorView(this);
                break;
                }
            case CntViewParameters::addressEditorView:
                {
                view = new CntAddressEditorView(this);
                break;
                }
            case CntViewParameters::dateEditorView:
                {
                view = new CntDateEditorView(this);
                break;
                }
            case CntViewParameters::imageEditorView:
                {
                view = new CntImageEditorView(this);
                break;
                }
            case CntViewParameters::editView:
                {
                view = new CntEditView(this);
                break;
                }
            case CntViewParameters::myCardSelectionView:
                {
                view = new CntMyCardSelectionView(this);
                break;
                }
            case CntViewParameters::myCardView:
                {
                view = new CntMyCardView(this);
                break;
                }
            case CntViewParameters::groupEditorView:
                {
                view = new CntGroupEditorView(this);
                break;
                }
            case CntViewParameters::groupMemberView:
                {
                view = new CntGroupMemberView(this);
                break;
                }
            case CntViewParameters::groupActionsView:
                {
                view = new CntGroupActionsView(this);
                break;
                }
            case CntViewParameters::historyView:
                {
                view = new CntHistoryView(this);
                break;
                }
            default:
                {
                break;
                }
         }
    }

    return view;
}

// end of file
