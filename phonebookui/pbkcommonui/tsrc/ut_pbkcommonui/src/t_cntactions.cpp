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
#include <hbtoolbar.h>
#include <hbmenu.h>

#include "cntactions.h"
#include "t_cntactions.h"

#include "hbstubs_helper.h"

void TestCntActions::initTestCase()
{
    mActions = new CntActions(this);
}

void TestCntActions::baseAction()
{
    HbAction *action = 0;
    action = mActions->baseAction("cnt:nameslist");
    QVERIFY(action);

    HbAction *sameAction = 0;
    sameAction = mActions->baseAction("cnt:nameslist");
    QVERIFY(sameAction == action);

    action = mActions->baseAction("cnt:dummy");
    QVERIFY(!action);
}

void TestCntActions::addActionToToolBar()
{
    HbAction *action = mActions->baseAction("cnt:nameslist");
    HbToolBar *toolBar = new HbToolBar();
    HbStubHelper::reset();
    mActions->addActionToToolBar(action, toolBar);
    QVERIFY(HbStubHelper::widgetActionsCount() == 1);

    delete toolBar;
}

void TestCntActions::addActionsToToolBar()
{
    mActions->actionList() << mActions->baseAction("cnt:nameslist") << mActions->baseAction("cnt:collections");
    HbToolBar *toolBar = new HbToolBar();
    HbStubHelper::reset();
    mActions->addActionsToToolBar(toolBar);
    QVERIFY(HbStubHelper::widgetActionsCount() == 2);

    mActions->clearActionList();

    delete toolBar;
}

void TestCntActions::addActionToMenu()
{
    HbAction *action = mActions->baseAction("cnt:nameslist");
    HbMenu *menu = new HbMenu();
    HbStubHelper::reset();
    mActions->addActionToMenu(action, menu);
    QVERIFY(HbStubHelper::widgetActionsCount() == 1);

    delete menu;
}

void TestCntActions::addActionsToMenu()
{
    mActions->actionList() << mActions->baseAction("cnt:nameslist") << mActions->baseAction("cnt:collections");
    HbMenu *menu = new HbMenu();
    HbStubHelper::reset();
    mActions->addActionsToMenu(menu);
    QVERIFY(HbStubHelper::widgetActionsCount() == 2);

    mActions->clearActionList();

    delete menu;
}

void TestCntActions::actionList()
{
    mActions->actionList() << mActions->baseAction("cnt:nameslist") << mActions->baseAction("cnt:collections");
    QVERIFY(mActions->actionList().count() == 2);

    mActions->clearActionList();
    QVERIFY(mActions->actionList().count() == 0);
}

void TestCntActions::cleanupTestCase()
{
    delete mActions;
}

// EOF
