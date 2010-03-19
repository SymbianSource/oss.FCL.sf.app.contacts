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

//USER
#include "logsmatchesview.h"
#include "ut_logsmatchesview.h"
#include "logscomponentrepository.h"
#include "logsdefs.h"
#include "logsmatchesmodel.h"
#include "logscall.h"
#include "logsmodel.h"

//SYSTEM
#include <QtTest/QtTest>
#include <hblistview.h>
#include <hblabel.h>
#include <dialpad.h>
#include <hblineedit.h>

Q_DECLARE_METATYPE(LogsMatchesModel*)


void UT_LogsMatchesView::initTestCase()
{
    mMainWindow = new HbMainWindow();
    mViewManager = new LogsViewManagerStub(*mMainWindow);
}

void UT_LogsMatchesView::cleanupTestCase()
{
    delete mMainWindow;
    delete mViewManager;
}

void UT_LogsMatchesView::init()
{
    mRepository = new LogsComponentRepository(*mViewManager);
    mMatchesView = new LogsMatchesView( *mRepository, *mViewManager );
}

void UT_LogsMatchesView::cleanup()
{
    delete mMatchesView;
    delete mRepository;
}

void UT_LogsMatchesView::testConstructor()
{
    QVERIFY( mMatchesView );
    QVERIFY( mMatchesView->viewId() == LogsMatchesViewId );
    QVERIFY( !mMatchesView->mListView );
    QVERIFY( !mMatchesView->mModel );
    QVERIFY( mMatchesView->mActionMap.count() == 0 );
    QVERIFY( mMatchesView->mLayoutSectionName == "" );
}

void UT_LogsMatchesView::testActivated()
{
    //activate for the first time
    mMatchesView->mViewManager.mainWindow().setOrientation( Qt::Vertical );
    mRepository->matchesView();
    QVERIFY( !mMatchesView->mInitialized );
    QVERIFY( !mMatchesView->mListView );
    QVERIFY( !mMatchesView->mModel );
    QVERIFY( mMatchesView->mActionMap.count() == 0 );
    mMatchesView->activated( false,QVariant() );
    QVERIFY( mMatchesView->mInitialized );
    QVERIFY( mMatchesView->mListView );
    QVERIFY( mMatchesView->mModel );
    QVERIFY( mMatchesView->mActionMap.count() == 4 );
    QVERIFY( mMatchesView->mListView->layoutName() == logsListDefaultLayout );
    QVERIFY( mMatchesView->mLayoutSectionName == logsViewDefaultSection );
    
    //activate once again, model recreated
    mMatchesView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    LogsModel::setMatchesModelCreated(false);
    mMatchesView->activated( true,QVariant() );
    QVERIFY( mMatchesView->mListView );
    QVERIFY( mMatchesView->mModel );    
    QVERIFY( LogsModel::isMatchesModelCreated() ); //model recreated
    QVERIFY( mMatchesView->mActionMap.count() == 4 );
    QVERIFY( mMatchesView->mListView->layoutName() == logsListLandscapeDialpadLayout );
    QVERIFY( mMatchesView->mLayoutSectionName == logsViewLandscapeDialpadSection );
    
    //Pass model as input arg
    LogsDbConnector* dbConnector = 0;
    LogsMatchesModel* model1 = new LogsMatchesModel(*dbConnector);
    QVariant arg = qVariantFromValue( model1 );
    mMatchesView->activated( true, arg );
    QVERIFY( mMatchesView->mListView );
    QVERIFY( mMatchesView->mModel == model1 );  
    
    LogsMatchesModel* model2 = new LogsMatchesModel(*dbConnector);
    QVariant arg2 = qVariantFromValue( model2 );
    mMatchesView->activated( true, arg2 );
    QVERIFY( mMatchesView->mListView );
    QVERIFY( mMatchesView->mModel == model2 );
    
    // After passing model as input arg, do not pass model
    mMatchesView->activated( true,QVariant() );
    QVERIFY( mMatchesView->mListView );
    QVERIFY( mMatchesView->mModel );
    QVERIFY( mMatchesView->mModel != model1 );
    QVERIFY( mMatchesView->mModel != model2 );
}


void UT_LogsMatchesView::testDeactivated()
{
    LogsDbConnector* dbConnector = 0;
    mMatchesView->mDialpad->editor().setText( "hello" );
    mMatchesView->mModel = new LogsMatchesModel(*dbConnector);
    mMatchesView->deactivated();
    QVERIFY( !mMatchesView->mModel );
}

void UT_LogsMatchesView::testCallKeyPressed()
{
    HbListView list;
    mMatchesView->mListView = &list;
    mMatchesView->updateModel(0);
    
     // No matches, no dialed string, cannot call
    LogsCall::resetTestData();
    mMatchesView->mModel->mTextData.clear();
    mMatchesView->callKeyPressed();
    QVERIFY( !LogsCall::isCallToNumberCalled() );

    // No any item where to call, but dial string exists
    QString dial("12345");
    mMatchesView->mDialpad->editor().setText( dial );
    mMatchesView->callKeyPressed();
    QVERIFY( LogsCall::isCallToNumberCalled() );
    
    // Call is still made with entered number even if matches exists
    LogsCall::resetTestData();
    mMatchesView->mModel->mTextData.append("testdata");
    mMatchesView->mModel->mTextData.append("testdata2");
    mMatchesView->callKeyPressed();
    QVERIFY( LogsCall::isCallToNumberCalled() );
}

void UT_LogsMatchesView::testDialpadEditorTextChanged()
{
    //no model, call button gets enabled
    mMatchesView->mDialpad->mIsCallButtonEnabled = false;
    mMatchesView->mDialpad->editor().setText( QString("h") );
    QVERIFY(!mMatchesView->mModel);
    mMatchesView->dialpadEditorTextChanged();
    QVERIFY( mMatchesView->mDialpad->mIsCallButtonEnabled );
    
    //model exists
    LogsDbConnector* dbConnector = 0;
    mMatchesView->mModel = new LogsMatchesModel(*dbConnector);
    mMatchesView->dialpadEditorTextChanged();
    QVERIFY( mMatchesView->mModel->lastCall() == QString("logsMatches") );
    QVERIFY( mMatchesView->mDialpad->mIsCallButtonEnabled );
    
    //text erased from input, view changed to recent calls
    mMatchesView->mDialpad->editor().setText( QString("") );
    QVERIFY( mViewManager->mViewId == LogsRecentViewId );
    
}

void UT_LogsMatchesView::testDialpadClosed()
{
    HbListView list;
    mMatchesView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    mMatchesView->mDialpad->setVisible(false);
    mMatchesView->mListView = &list;
    mMatchesView->mLayoutSectionName = QString("landscape_dialpad");
    QString hello("hello");
    mMatchesView->mDialpad->editor().setText( hello );
    mMatchesView->dialpadClosed();
    QVERIFY( mMatchesView->mDialpad->editor().text() == hello );
    QVERIFY( mMatchesView->mLayoutSectionName == logsViewDefaultSection );
}

void UT_LogsMatchesView::testModel()
{
    QVERIFY( !mMatchesView->model() );
    LogsDbConnector* dbConnector = 0;
    mMatchesView->mModel = new LogsMatchesModel(*dbConnector);
    QVERIFY( mMatchesView->model() );
}


void UT_LogsMatchesView::testUpdateWidgetsSizeAndLayout()
{
    //no listView, nothing happens
    QVERIFY( !mMatchesView->mListView );
    mMatchesView->updateWidgetsSizeAndLayout();
    
    //listView exists, layout and size updated
    mMatchesView->mViewManager.mainWindow().setOrientation( Qt::Vertical );
    mMatchesView->mDialpad->setVisible(false);
    mMatchesView->mListView = new HbListView();
    mMatchesView->mListView->setLayoutName("dummy");
    mMatchesView->updateWidgetsSizeAndLayout();
    QVERIFY( mMatchesView->mListView->layoutName() == logsListDefaultLayout );
    QVERIFY( mMatchesView->mLayoutSectionName == logsViewDefaultSection );
    
    delete mMatchesView->mListView;
    mMatchesView->mListView = 0; 
}

