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

#include "cntmainwindow.h"
#include "cntdefaultviewmanager.h"
#include "cntviewnavigator.h"
#include <QCoreApplication>
#include <cntdebug.h>

CntMainWindow::CntMainWindow(QWidget *parent, int defaultView)
    : HbMainWindow(parent),
    mViewManager(NULL)
{
    CNT_ENTRY

    if (defaultView != noView)
    {
        CntViewNavigator* navigator = new CntViewNavigator(this);
        navigator->addException( editView, namesView );
        navigator->addException( FavoritesMemberView, collectionView );
        navigator->addEffect( groupMemberView, groupActionsView );
        navigator->addEffect( groupActionsView, groupMemberView );
        navigator->addEffect( commLauncherView, historyView );
        navigator->addEffect( historyView, commLauncherView );
                
        mViewManager = new CntDefaultViewManager( this );
        mViewManager->setViewNavigator( navigator );
        
        //activate the view
        CntViewParameters viewParameters;
        viewParameters.insert(EViewId, defaultView);
        mViewManager->changeView( viewParameters );
    }

    CNT_EXIT
}

CntMainWindow::~CntMainWindow()
{
    CNT_ENTRY

    delete mViewManager;
    mViewManager = NULL;

    CNT_EXIT
}

/*
 * Lets let views handle send/end key press events, because if toolbar or menu has been focused
 * the view wont get the key press events
 */
void CntMainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Yes || event->key() == Qt::Key_No)
    {
       emit keyPressed(event);
    }
    else
    {
        HbMainWindow::keyPressEvent(event);
    }
}

// end of file
