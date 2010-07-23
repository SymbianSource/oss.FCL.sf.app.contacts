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
#include <xqaiwrequest.h>
#include <xqaiwdecl.h>
#include <xqappmgr.h>
#include <logsservices.h>
#include <cntdebug.h>

CntMainWindow::CntMainWindow(QWidget *parent, int defaultView)
    : HbMainWindow(parent),
    mViewManager(NULL),
    mDefaultView(defaultView)
{
    CNT_ENTRY
    
    if (defaultView != noView)
    {
        CntViewNavigator* navigator = new CntViewNavigator(this);
        navigator->addException( favoritesMemberView, collectionView );
        navigator->addEffect( groupMemberView, groupActionsView );
        navigator->addEffect( groupActionsView, groupMemberView );
        navigator->addEffect( contactCardView, historyView );
        navigator->addEffect( historyView, contactCardView );
        navigator->addRoot( namesView );
        navigator->addRoot( collectionView );
        navigator->addRoot( groupMemberView );
        navigator->addRoot( favoritesMemberView );
                
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
    if (event->key() == Qt::Key_Yes && mDefaultView != noView)
    {
        if (mViewManager->currentViewId() == contactCardView)
        {
            emit keyPressed(event);
        }
        else
        {
            XQApplicationManager appManager;   
            XQAiwRequest* request = appManager.create("com.nokia.symbian.ILogsView", "show(QVariantMap)", false);
            
            if (request)
            {
                QList<QVariant> args;
                QVariantMap map;
                map.insert("view_index", QVariant(int(LogsServices::ViewAll)));
                map.insert("show_dialpad", QVariant(true));
                map.insert("dialpad_text", QVariant(QString()));
                args.append(QVariant(map));
                request->setArguments(args);
                request->send();
            }
        }
    }
    else
    {
        HbMainWindow::keyPressEvent(event);
    }
}

// end of file
