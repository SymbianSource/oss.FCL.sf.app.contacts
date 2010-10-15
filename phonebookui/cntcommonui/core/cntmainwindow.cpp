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
#include "cntabstractviewfactory.h"
#include "cntkeygrabber.h"
#include "cntapplication.h"
#include "cntactivities.h"
#include <cntdebug.h>
#include <cntabstractengine.h>


CntMainWindow::CntMainWindow(QWidget *parent, int defaultView)
    : HbMainWindow(parent),
    mViewManager(NULL),
    mDefaultView(defaultView),
    mActivities(NULL)
{
    CNT_ENTRY
    
    CntKeyGrabber *keyGrabber = new CntKeyGrabber(this, this);
    
    if (defaultView != noView)
    {
        mViewManager = new CntDefaultViewManager( this );
        mActivities = new CntActivities();
        
        connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(saveActivity()));
         
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
        
        mViewManager->setViewNavigator( navigator );
        mViewManager->setViewFactory( new CntDefaultViewFactory( mViewManager->engine().extensionManager()) );
         
        //load the activity
        QByteArray serializedModel;
        if (mActivities->loadActivity(serializedModel))
        {
            // restore state from activity data 
            QDataStream stream(&serializedModel, QIODevice::ReadOnly);
            if (mViewManager->internalize(stream))
            {
                // activity loaded.
                return;
            }
        }
        
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
    
    delete mActivities;
    mActivities = NULL;
    
    CNT_EXIT
}

void CntMainWindow::saveActivity()
{
    CNT_ENTRY
        
    QByteArray serializedActivity;
    QDataStream stream(&serializedActivity, QIODevice::WriteOnly | QIODevice::Append);
    QString name = mViewManager->externalize(stream);
    mActivities->saveActivity(name, serializedActivity, QPixmap::grabWidget(this, rect()));
    
    CNT_EXIT
}

// end of file
