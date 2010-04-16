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
#include "logscontact.h"
#include "logsmessage.h"

//SYSTEM
#include <QtTest/QtTest>
#include <hblistview.h>
#include <hblabel.h>
#include <dialpad.h>
#include <hblineedit.h>
#include <hbpushbutton.h>

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
    QVERIFY( !mMatchesView->mAddToContactsButton );
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
    QVERIFY( mMatchesView->mAddToContactsButton );
    
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

void UT_LogsMatchesView::testVideoCallToCurrentNum()
{
    LogsCall::resetTestData();
    QString dial("12345");
    mMatchesView->mDialpad->editor().setText( dial );
    mMatchesView->videoCallToCurrentNum();
    QVERIFY( LogsCall::isCallToNumberCalled() );
}

void UT_LogsMatchesView::testSendMessageToCurrentNum()
{
    LogsMessage::resetTestData();
    mMatchesView->mDialpad->editor().setText( "" );
    mMatchesView->sendMessageToCurrentNum();
    QVERIFY( !LogsMessage::isMessageSent() );
    
    QString dial("12345");
    mMatchesView->mDialpad->editor().setText( dial );
    mMatchesView->sendMessageToCurrentNum();
    QVERIFY( LogsMessage::isMessageSent() );
}

void UT_LogsMatchesView::testSaveNumberInDialpadToContacts()
{
    // Nothing happens if there is no text in dialpad
    QVERIFY(!mMatchesView->mContact);
    QCOMPARE(mMatchesView->mDialpad->editor().text().length(), 0);
    mMatchesView->saveNumberInDialpadToContacts();
    QVERIFY(!mMatchesView->mContact);
    
    // If there is a number, new contact will be created
    mMatchesView->mDialpad->editor().setText("123");
    mMatchesView->saveNumberInDialpadToContacts();
    QVERIFY(mMatchesView->mContact);
    QCOMPARE(mMatchesView->mContact->mNumber, QString("123"));
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
    mMatchesView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    mMatchesView->mListView = new HbListView();
    mMatchesView->mLayoutSectionName = QString("landscape_dialpad");
    QString hello("hello");
    mMatchesView->mDialpad->editor().setText( hello );
    mMatchesView->mAddToContactsButton = new HbPushButton();
    mMatchesView->mAddToContactsButton->setVisible(true);
    
    mMatchesView->dialpadClosed();
    
    QVERIFY( mMatchesView->mDialpad->editor().text() == hello );
    QVERIFY( mMatchesView->mLayoutSectionName == logsViewDefaultSection );
    QVERIFY( !mMatchesView->mAddToContactsButton->isVisible() );
    delete mMatchesView->mListView;
    mMatchesView->mListView = 0;
    delete mMatchesView->mAddToContactsButton;
    mMatchesView->mAddToContactsButton = 0;
}

void UT_LogsMatchesView::testDialpadOpened()
{
    //widgets size and layout updated
    mMatchesView->mViewManager.mainWindow().setOrientation( Qt::Vertical );
    mMatchesView->mListView = new HbListView();
    mMatchesView->mLayoutSectionName = QString("dummy");
    mMatchesView->mListView->setLayoutName("dummy");
    mMatchesView->mDialpad->mIsOpen = true;
    mMatchesView->mAddToContactsButton = new HbPushButton();
    mMatchesView->mAddToContactsButton->setVisible(false);
    mMatchesView->mDialpad->editor().setText( "hello" );
    mMatchesView->mDialpad->mIsOpen = true;
    
    mMatchesView->dialpadOpened();
    
    QVERIFY( mMatchesView->mListView->layoutName() == logsListDefaultLayout );
    QVERIFY( mMatchesView->mLayoutSectionName == logsViewPortraitDialpadSection );
    QVERIFY( mMatchesView->mAddToContactsButton->isVisible() );
    delete mMatchesView->mListView;
    mMatchesView->mListView = 0;
    delete mMatchesView->mAddToContactsButton;
    mMatchesView->mAddToContactsButton = 0;
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
    
    //listView exists, layout and size updated, dialpad not visible
    mRepository->matchesView();
    mMatchesView->mViewManager.mainWindow().setOrientation( Qt::Vertical );
    mMatchesView->mDialpad->closeDialpad();
    mMatchesView->mListView = new HbListView();
    mMatchesView->mListView->setLayoutName("dummy");
    mMatchesView->updateWidgetsSizeAndLayout();
    QVERIFY( mMatchesView->mListView->layoutName() == logsListDefaultLayout );
    QVERIFY( mMatchesView->mLayoutSectionName == logsViewDefaultSection );
    QObject* obj = mRepository->findObject( logsCommonVideoCallMenuActionId );
    HbAction* videoCallAction = qobject_cast<HbAction*>( obj );
    QObject* obj2 = mRepository->findObject( logsCommonMessageMenuActionId );
    HbAction* messageAction = qobject_cast<HbAction*>( obj2 );
    QVERIFY( !videoCallAction->isVisible() );
    QVERIFY( !messageAction->isVisible() );
    
    //listView exists, layout and size updated, dialpad visible
    mMatchesView->mDialpad->openDialpad();
    QString hello("hello");
    mMatchesView->mDialpad->editor().setText( hello );
    mMatchesView->mListView->setLayoutName("dummy");
    mMatchesView->updateWidgetsSizeAndLayout();
    QVERIFY( mMatchesView->mListView->layoutName() == logsListDefaultLayout );
    QVERIFY( mMatchesView->mLayoutSectionName == logsViewPortraitDialpadSection );
    QVERIFY( videoCallAction->isVisible() );
    QVERIFY( messageAction->isVisible() );
    
    delete mMatchesView->mListView;
    mMatchesView->mListView = 0; 
}

void UT_LogsMatchesView::testHandleOrientationChanged()
{
    //dialpad position recalculated and layout/size updated
    QPointF pos;
    mMatchesView->mListView = new HbListView();
    mMatchesView->mListView->setLayoutName("dummy");
    mMatchesView->mLayoutSectionName = "dummy";
    mMatchesView->mDialpad->setPos(pos);    
    mMatchesView->mViewManager.mainWindow().setOrientation( Qt::Horizontal );
    
    mMatchesView->handleOrientationChanged();
    
    QVERIFY( pos != mMatchesView->mDialpad->pos() );
    QVERIFY( mMatchesView->mListView->layoutName() == logsListLandscapeLayout );
    QVERIFY( mMatchesView->mLayoutSectionName == logsViewDefaultSection );
    delete mMatchesView->mListView;
    mMatchesView->mListView = 0; 
}

void UT_LogsMatchesView::testUpdateEmptyListWidgetsVisibility()
{
    LogsDbConnector* dbConnector = 0;
    mMatchesView->mModel = new LogsMatchesModel(*dbConnector);
    mMatchesView->mModel->mTextData.clear();
    mMatchesView->mDialpad->openDialpad();
    mMatchesView->mDialpad->editor().setText( "hello" );
    mMatchesView->mEmptyListLabel = new HbLabel();
    mMatchesView->mAddToContactsButton = new HbPushButton();
    mMatchesView->mEmptyListLabel->setVisible(false);
    mMatchesView->mAddToContactsButton->setVisible(false);

    mMatchesView->updateEmptyListWidgetsVisibility();
    
    QVERIFY( mMatchesView->mEmptyListLabel->isVisible() );
    QVERIFY( mMatchesView->mAddToContactsButton->isVisible() );
    delete mMatchesView->mEmptyListLabel;
    mMatchesView->mEmptyListLabel = 0;
    delete mMatchesView->mAddToContactsButton;
    mMatchesView->mAddToContactsButton = 0;
}


void UT_LogsMatchesView::testUpdateAddContactButton()
{
    //no button, nothing happens
    QVERIFY( !mMatchesView->mAddToContactsButton );
    mMatchesView->updateAddContactButton();
    
    //dialpad closed => button set invisible
    mMatchesView->mAddToContactsButton = new HbPushButton();
    mMatchesView->mAddToContactsButton->setVisible(true);
    QVERIFY( !mMatchesView->mDialpad->isOpen() );
    mMatchesView->updateAddContactButton();
    QVERIFY( !mMatchesView->mAddToContactsButton->isVisible() );
    
    //no matches, dialpad open and has some text => button set visible
    QVERIFY( !mMatchesView->mModel );
    mMatchesView->mDialpad->openDialpad();
    mMatchesView->mDialpad->editor().setText( "hello" );
    mMatchesView->updateAddContactButton();
    QVERIFY( mMatchesView->mAddToContactsButton->isVisible() );
    
    //no matches, dialpad open and no text => button set invisible
    mMatchesView->mDialpad->editor().setText( "" );
    QVERIFY( mMatchesView->mDialpad->isOpen() );
    mMatchesView->updateAddContactButton();
    QVERIFY( !mMatchesView->mAddToContactsButton->isVisible() );

    //matches found => button set invisible
    LogsDbConnector* dbConnector = 0;
    mMatchesView->mModel = new LogsMatchesModel(*dbConnector);
    mMatchesView->mModel->mTextData.append("item1");
    mMatchesView->mDialpad->editor().setText( "hello" );
    mMatchesView->mAddToContactsButton->setVisible(true);
    QVERIFY( mMatchesView->mDialpad->isOpen() );
    mMatchesView->updateAddContactButton();
    QVERIFY( !mMatchesView->mAddToContactsButton->isVisible() );
    
    delete mMatchesView->mAddToContactsButton;
    mMatchesView->mAddToContactsButton = 0;
}
