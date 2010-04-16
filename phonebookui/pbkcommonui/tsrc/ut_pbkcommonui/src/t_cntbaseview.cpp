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

#include "t_cntbaseview.h"

#include <QtTest/QtTest>
#include <QObject>

#include <hbtoolbar.h>
#include "cntdefaultviewmanager.h"
#include "cntmainwindow.h"

#include "hbstubs_helper.h"
#include "qthighway_stub_helper.h"

void TestCntBaseView::initTestCase()
{
    mWindow = 0;
    mViewManager = 0;
    mBaseView = 0;

}

void TestCntBaseView::createClasses()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::noView);
    mBaseView = new CntBaseTestView(mViewManager);
    mWindow->addView(mBaseView);
    mWindow->setCurrentView(mBaseView);

    // check that we have a view
    QVERIFY(mWindow != 0);
    QVERIFY(mViewManager != 0);
    QVERIFY(mBaseView != 0);
}

void TestCntBaseView::setupView()
{
    mBaseView->setupView();
    QVERIFY(mBaseView->toolBar() != 0);
    QVERIFY(mBaseView->mSoftKeyBackAction != 0);
}

void TestCntBaseView::viewManager()
{
    CntViewManager *viewManager =  mBaseView->viewManager();
    QVERIFY(viewManager == mViewManager);
}

void TestCntBaseView::contactModel()
{
    MobCntModel *model = mBaseView->contactModel();
    QVERIFY(model == mBaseView->contactModel());
}

void TestCntBaseView::contactManager()
{
    QContactManager *manager = mBaseView->contactManager();
    QVERIFY(manager == mBaseView->contactManager());
}

void TestCntBaseView::commands()
{
    CntCommands *commands = mBaseView->commands();
    QVERIFY(commands == mBaseView->commands());
}

void TestCntBaseView::addSoftkeyAction()
{
    mBaseView->addSoftkeyAction();
    QVERIFY(mBaseView->mSoftKeyBackAction == mBaseView->navigationAction());
}

void TestCntBaseView::clearToolBar()
{
    HbStubHelper::reset();
    mBaseView->toolBar()->addAction("testi");
    mBaseView->clearToolBar();
    QVERIFY(HbStubHelper::widgetActionsCount() == 0);
}

void TestCntBaseView::addToolBar()
{
    mBaseView->addToolBar();
    QVERIFY(mBaseView->toolBar() != 0);
}

void TestCntBaseView::setToolBarOrientation()
{
    mBaseView->setToolBarOrientation();
    QVERIFY(mBaseView->toolBar()->orientation() == Qt::Horizontal);
}

void TestCntBaseView::actions()
{
    CntActions *actions = mBaseView->actions();
    QVERIFY(actions == mBaseView->actions());
}

void TestCntBaseView::loadDocument()
{
    QVERIFY(mBaseView->loadDocument(":/xml/contacts_ev.docml"));
}

void TestCntBaseView::findWidget()
{
    QVERIFY(mBaseView->findWidget(QString("cnt_listview_labels")) != 0);
}

void TestCntBaseView::findObject()
{
    QVERIFY(mBaseView->loadDocument(":/xml/contacts_actions.docml"));
    QVERIFY(mBaseView->findObject(QString("cnt:nameslist")) != 0);
}

void TestCntBaseView::keyPressEvent()
{
    QKeyEvent event(QEvent::KeyPress, Qt::Key_5, Qt::NoModifier);
    mBaseView->keyPressEvent(&event);
    QKeyEvent event2(QEvent::KeyRelease, Qt::Key_5, Qt::NoModifier);

    QtHighwayStubHelper::reset();
    QKeyEvent event3(QEvent::KeyPress, Qt::Key_Yes, Qt::NoModifier);
    mBaseView->keyPressEvent(&event3);
    QVERIFY(QtHighwayStubHelper::service() == "com.nokia.services.logsservices.starter");
    QVERIFY(QtHighwayStubHelper::message() == "start(int,bool)");
    QKeyEvent event4(QEvent::KeyRelease, Qt::Key_Yes, Qt::NoModifier);
}

void TestCntBaseView::cleanupTestCase()
{
    mWindow->deleteLater();
    delete mViewManager;
    mViewManager = 0;
}

// EOF
