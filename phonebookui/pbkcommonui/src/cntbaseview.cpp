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
#include "cntbaseview.h"
#include "cntmainwindow.h"
#include "cntdocumentloader.h"
          
#include <hbtoolbar.h>

CntBaseView::CntBaseView(CntViewManager *viewManager, QGraphicsItem *parent) :
    HbView(parent),
    mViewManager(viewManager),
    mActions(0),
    mCommands(0),
    mSoftKeyBackAction(new HbAction(Hb::BackNaviAction, this))
{
    mModelProvider=CntModelProvider::instance();
    setTitle(hbTrId("txt_phob_title_contacts"));
    connect(mViewManager->mainWindow(), SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(keyPressEvent(QKeyEvent*)));
    connect(mViewManager->mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
    
    connect(mSoftKeyBackAction, SIGNAL(triggered()), this, SLOT(aboutToCloseView()));
}

CntBaseView::~CntBaseView()
{
    mModelProvider->release();
}

void CntBaseView::setupView()
{
    addToolBar();
    addMenuItems();
    addSoftkeyAction();
}

CntViewManager* CntBaseView::viewManager()
{
    return mViewManager;
}

/*!
\return pointer to MobCntModel
*/
MobCntModel *CntBaseView::contactModel()
{
    return mModelProvider->contactModel();
}

QContactManager *CntBaseView::contactManager()
{
    return mModelProvider->contactManager();
}

CntCommands *CntBaseView::commands()
{
    if (mCommands == 0)
    {
        mCommands = new CntCommands(*viewManager(), mModelProvider->contactManager(),
                mModelProvider->contactSimManager(), this);
    }
    return mCommands;
}

void CntBaseView::addSoftkeyAction()
{
    if (navigationAction() != mSoftKeyBackAction)
        setNavigationAction(mSoftKeyBackAction);
}

void CntBaseView::keyPressEvent(QKeyEvent *event)
{
    if (this == viewManager()->mainWindow()->currentView() && event->key() == Qt::Key_Yes)
    {
        event->accept();
        commands()->launchDialer();
    }
    else if (this == viewManager()->mainWindow()->currentView() && event->key() == Qt::Key_No)
    {
        event->accept();
        qApp->quit();
    }
    else
    {
        HbView::keyPressEvent(event);
    }
}

/*!
Clears toolbar items.
*/
void CntBaseView::clearToolBar()
{
    toolBar()->clearActions();
}

/*!
Adds toolbar control.
*/
void CntBaseView::addToolBar()
{
    clearToolBar();

    //Add Action to the toolbar and set its orientation (default horizontal always)
    addActionsToToolBar();
    setToolBarOrientation();
}

void CntBaseView::setToolBarOrientation()
{
    toolBar()->setOrientation(Qt::Horizontal);
}

CntActions *CntBaseView::actions()
{
    if (mActions == 0)
    {
        mActions = new CntActions(this);
    }
    return mActions;
}

bool CntBaseView::loadDocument(const char *aDocument)
{
    bool ok = false;
    documentLoader().load(aDocument, &ok);
    return ok;
}

bool CntBaseView::loadDocument(const char *aDocument, const char *aSection)
{
    bool ok = false;
    documentLoader().load(aDocument, aSection, &ok);
    return ok;
}

CntDocumentLoader &CntBaseView::documentLoader()
{
    return mDocumentLoader;
}

QGraphicsWidget* CntBaseView::findWidget(const QString& aWidget)
{
    return documentLoader().findWidget(aWidget);
}

QObject* CntBaseView::findObject(const QString& aObject)
{
    return documentLoader().findObject(aObject);
}
// end of file
