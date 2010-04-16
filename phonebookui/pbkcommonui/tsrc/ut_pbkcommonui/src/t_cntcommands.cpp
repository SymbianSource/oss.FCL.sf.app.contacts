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


#include <QtTest/QtTest>
#include <QObject>

#include "cntcommands.h"
#include "t_cntcommands.h"
#include "cntdefaultviewmanager.h"
#include "cntbaseview.h"
#include "cntmainwindow.h"

#include "qthighway_stub_helper.h"

void TestCntCommands::initTestCase()
{
    mWindow = new CntMainWindow(0, CntViewParameters::noView);
    mViewManager = new CntDefaultViewManager(mWindow, CntViewParameters::namesView);
    mManager = new QContactManager("symbian");
    mSimManager = new QContactManager("symbiansim");
    mCommands = new CntCommands(*mViewManager, mManager, mSimManager);
}

void TestCntCommands::newContact()
{
    mCommands->newContact();

    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::editView);
}

void TestCntCommands::editContact()
{
    cleanupTestCase();
    initTestCase();

    mCommands->editContact(mContact);

    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::editView);
}

void TestCntCommands::openContact()
{
    cleanupTestCase();
    initTestCase();

    mCommands->openContact(mContact);

    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::commLauncherView);
}

void TestCntCommands::viewHistory()
{
    cleanupTestCase();
    initTestCase();

    QContactName name;
    name.setFirst("first");
    mContact.saveDetail(&name);
    mManager->saveContact(&mContact);
    mCommands->viewHistory(mContact);

    QVERIFY(static_cast<CntBaseView*>(mWindow->currentView())->viewId() == CntViewParameters::historyView);
}

void TestCntCommands::launchDialer()
{
    cleanupTestCase();
    initTestCase();

    QtHighwayStubHelper::reset();
    mCommands->launchDialer();
    QVERIFY(QtHighwayStubHelper::service() == "com.nokia.services.logsservices.starter");
    QVERIFY(QtHighwayStubHelper::message() == "start(int,bool)");
}

void TestCntCommands::progress()
{
    QList<QContactActionDescriptor> callActionDescriptors = QContactAction::actionDescriptors("call", "symbian");
    mCommands->mContactAction = QContactAction::action(callActionDescriptors.at(0));

    mCommands->progress(QContactAction::Active, QVariantMap());
    QVERIFY(mCommands->mContactAction);
    mCommands->progress(QContactAction::Finished, QVariantMap());
    QVERIFY(!mCommands->mContactAction);
    
    mCommands->mContactAction = QContactAction::action(callActionDescriptors.at(0));
    mCommands->progress(QContactAction::FinishedWithError, QVariantMap());
    QVERIFY(!mCommands->mContactAction);
}

void TestCntCommands::cleanupTestCase()
{
    delete mViewManager;
    mViewManager = 0;
    // ORBIT BUG - so deleting main window removed
    mWindow->deleteLater();
    mCommands = 0;
    delete mManager;
    mManager = 0;
    delete mSimManager;
    mSimManager = 0;
}

// EOF
