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
#include "cntfavoritesmemberview.h"
#include "cntcontactcardview.h"
#include "cntonlineaccounteditorview.h"
#include "cnteditview.h"
#include "cntmycardview.h"
#include "cntmycardselectionview.h"
#include "cntgroupeditorview.h"
#include "cntgroupmemberview.h"
#include "cntgroupactionsview.h"
#include "cnthistoryview.h"

// new editors
#include "cnteditorfactory.h"
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
CntViewManager::CntViewManager(CntMainWindow *mainWindow, CntViewParameters::ViewId defaultView) :
    mMainWindow(mainWindow), 
    mDefaultView(0),
    mEditorFactory(0),
    mCurrentViewId(CntViewParameters::noView)    
{
    Q_UNUSED( defaultView );
    mEditorFactory = new CntEditorFactory(this);
    HbStyleLoader::registerFilePath(":/style");
}

/*!
 Destructor
 */
CntViewManager::~CntViewManager()
{
    delete mEditorFactory;
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
    CntViewParameters args(defaultView);
    CntBaseView *view = getView(args);

    if (view && view != mDefaultView) {
        addViewToWindow(view);

        // delete the old default view if it exists
        if (mDefaultView) {
            mainWindow()->removeView(mDefaultView);
            mDefaultView->setParent(0);
            mDefaultView->deleteLater();
        }

        //set the new view as default
        mCurrentViewId = defaultView;
        mDefaultView = view;
    }
}

void CntViewManager::removeDepracatedCurrentView()
{
    CntBaseView* oldView(NULL);
    // note that if current view is with "New API", its "changeView" must have
    // been called, thus, the remove view is already called
    if (isDepracatedView(mCurrentViewId)) {
        oldView = static_cast<CntBaseView *> (mainWindow()->currentView());
        if (oldView)
            removeViewFromWindow(oldView);
    }
}
/*!
 Add \a view to main window and set it as current view
 */
void CntViewManager::addViewToWindow(CntBaseView *view)
{
    // add view to mainwindow and set it as current one
    if (view) {
        //if not default view add the view to main window
        if (view != mDefaultView) {
            view->setupView();
            mainWindow()->addView(view);
        }

        //set the view as current
        mainWindow()->setCurrentView(view);
    }
}

/*!
 Remove \a view from main window and delete it
 */
void CntViewManager::removeViewFromWindow(CntBaseView *view)
{
    if (view && view != mDefaultView) {
        mainWindow()->removeView(view);
        view->setParent(0);
        view->deleteLater();
    }
}

/*!
 Create a view based on ID. \Return pointer to new object if success, 0 if not.
 */
CntBaseView *CntViewManager::getView(const CntViewParameters &aArgs)
{
    CntBaseView* view(0);
    CntViewParameters::ViewId id = aArgs.nextViewId();

    switch (id) {
    case CntViewParameters::FavoritesMemberView:
    {
        view = new CntFavoritesMemberView(this);
        break;
    }
    case CntViewParameters::commLauncherView:
    {
        view = new CntContactCardView(this);
        break;
    }
    case CntViewParameters::emailEditorView:
    {
        view = mEditorFactory->createEmailEditorView(aArgs.selectedContact());
        break;
    }
    case CntViewParameters::namesEditorView:
    {
        view = mEditorFactory->createNameEditorView(aArgs.selectedContact());
        break;
    }
    case CntViewParameters::urlEditorView:
    {
        view = mEditorFactory->createUrlEditorView(aArgs.selectedContact());
        break;
    }
    case CntViewParameters::companyEditorView:
    {
        view = mEditorFactory->createCompanyEditorView(aArgs.selectedContact());
        break;
    }
    case CntViewParameters::phoneNumberEditorView:
    {
        view = mEditorFactory->createNumberEditorView(aArgs.selectedContact());
        break;
    }
    case CntViewParameters::onlineAccountEditorView:
    {
        view = new CntOnlineAccountEditorView(this);
        break;
    }
    case CntViewParameters::noteEditorView:
    {
        view = mEditorFactory->createNoteEditorView(aArgs.selectedContact());
        break;
    }
    case CntViewParameters::familyDetailEditorView:
    {
        view = mEditorFactory->createFamilyEditorView(aArgs.selectedContact());
        break;
    }
    case CntViewParameters::addressEditorView:
    {
        view = mEditorFactory->createAddressEditorView(aArgs.selectedContact());
        break;
    }
    case CntViewParameters::dateEditorView:
    {
        view = mEditorFactory->createDateEditorView(aArgs.selectedContact());
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
    return view;
}

// end of file
