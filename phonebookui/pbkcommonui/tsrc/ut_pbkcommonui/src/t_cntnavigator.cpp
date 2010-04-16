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
#include "t_cntnavigator.h"
#include "cntviewnavigator.h"
T_NavigatorTest::T_NavigatorTest() : QObject() 
{
    
}

T_NavigatorTest::~T_NavigatorTest()
{
    
}

void T_NavigatorTest::init()
{
    mNavigator = new CntViewNavigator( NULL );
}

void T_NavigatorTest::cleanup()
{
    delete mNavigator;
}

 void T_NavigatorTest::testWithoutExceptions()
 {
    mNavigator->next( CntViewParameters::namesView );
    mNavigator->next( CntViewParameters::commLauncherView );
    QCOMPARE( mNavigator->back(), CntViewParameters::namesView );
    QCOMPARE( mNavigator->back(), CntViewParameters::noView );
    
    mNavigator->next( CntViewParameters::namesView );
    mNavigator->next( CntViewParameters::commLauncherView );
    mNavigator->next( CntViewParameters::editView );
     
    QCOMPARE( mNavigator->back(), CntViewParameters::commLauncherView );
    QCOMPARE( mNavigator->back(), CntViewParameters::namesView );
    QCOMPARE( mNavigator->back(), CntViewParameters::noView );
  
    mNavigator->next( CntViewParameters::namesView );
    mNavigator->next( CntViewParameters::commLauncherView );
    mNavigator->next( CntViewParameters::editView );
     
    QCOMPARE( mNavigator->back(), CntViewParameters::commLauncherView );
     
    mNavigator->next( CntViewParameters::emailEditorView );
    mNavigator->next( CntViewParameters::addressEditorView );
     
    QCOMPARE( mNavigator->back(), CntViewParameters::emailEditorView );
    QCOMPARE( mNavigator->back(), CntViewParameters::commLauncherView );
    QCOMPARE( mNavigator->back(), CntViewParameters::namesView );
    QCOMPARE( mNavigator->back(), CntViewParameters::noView );
 }
 
 void T_NavigatorTest::testWithExecptions()
 {
     mNavigator->addException( CntViewParameters::editView, CntViewParameters::namesView );
     
     mNavigator->next( CntViewParameters::namesView );
     mNavigator->next( CntViewParameters::commLauncherView );
     mNavigator->next( CntViewParameters::editView );
         
     QCOMPARE( mNavigator->back(), CntViewParameters::namesView );
     mNavigator->next( CntViewParameters::commLauncherView );
     QCOMPARE( mNavigator->back(), CntViewParameters::namesView );
     QCOMPARE( mNavigator->back(), CntViewParameters::noView );
     
     mNavigator->removeException( CntViewParameters::editView );
     
     mNavigator->next( CntViewParameters::namesView );
     mNavigator->next( CntViewParameters::commLauncherView );
     mNavigator->next( CntViewParameters::editView );
         
     QCOMPARE( mNavigator->back(), CntViewParameters::commLauncherView );
     QCOMPARE( mNavigator->back(), CntViewParameters::namesView );
     QCOMPARE( mNavigator->back(), CntViewParameters::noView );
 }
// End of File
