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

#include "t_namelist.h"
#include "cntnamesview.h"
#include "cntnamesview_p.h"
#include "hbstubs_helper.h"

#include <hbabstractviewitem.h>
#include <hbtextitem.h>
#include <hbgroupbox.h>
#include <hblistview.h>
#include <hbsearchpanel.h>
#include <QtGui>
#include <QtTest/QtTest>


T_NameListTest::T_NameListTest()
{
}

T_NameListTest::~T_NameListTest()
{
}
    
void T_NameListTest::init()
{
    mViewManager = new TestViewManager();
    mNamesPrivate = new CntNamesViewPrivate();
    
    CntViewParameters args;
    mNamesPrivate->activate( mViewManager, args );
}

void T_NameListTest::cleanup()
{
    mNamesPrivate->deactivate();
    delete mViewManager;
    delete mNamesPrivate;
}

void T_NameListTest::testPublicNamesList()
{
    CntNamesView* namesView = new CntNamesView();
        
    CntViewParameters args( CntViewParameters::namesView );
    namesView->activate( mViewManager, args );
        
    QVERIFY( namesView->isDefault() );
    QVERIFY( namesView->view() );
    QCOMPARE( namesView->viewId(), CntViewParameters::namesView );
    
    namesView->deactivate();
    delete namesView;
}

void T_NameListTest::testBanner()
{
    QString str( "GroupBoxText" );
    mNamesPrivate->showBanner( str );
    QVERIFY( mNamesPrivate->groupBox()->isVisible() );
    QCOMPARE(mNamesPrivate->groupBox()->titleText(), str);
    
    mNamesPrivate->hideBanner();
    QVERIFY( !mNamesPrivate->groupBox()->isVisible() );   
}

void T_NameListTest::testFinder()
{
    mNamesPrivate->showFinder();
    QVERIFY( mNamesPrivate->search()->isVisible() );
    QVERIFY( mNamesPrivate->isFinderVisible() );
    
    mNamesPrivate->hideFinder();
    QVERIFY( !mNamesPrivate->search()->isVisible() );
    QVERIFY( !mNamesPrivate->isFinderVisible() );
    
    mNamesPrivate->setFilter( "Abc" );
    QVERIFY( !mNamesPrivate->isFinderVisible() );
}

void T_NameListTest::testKeyboard()
{
    HbListView* list = mNamesPrivate->list();
    HbTextItem* text = mNamesPrivate->emptyLabel();
    
    qreal maxListHeight = list->maximumHeight();
    qreal maxTextHeight = text->maximumHeight();
    
    mNamesPrivate->showFinder();
    mNamesPrivate->hideFinder();

    // Impossible to make programmatically a mouse click/tap on searchpanel.
}
// End of File
