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

#include "ut_cntservices.h"
#include "cntservices.h"
#include "testviewmanager.h"
#include <QtTest/QtTest>
#include "testengine.h"
#include <hbmainwindow.h>
#include "cntserviceviewparams.h"


void UT_CntServices::initTestCase()
{
    mTestViewManager = new TestViewManager();
    connect( mTestViewManager, SIGNAL(changeView( const CntViewParameters aArgs )), this, SLOT( onViewChanged( const CntViewParameters aArgs ) ) );
    mEngine = new TestEngine( *mTestViewManager );
}

void UT_CntServices::cleanupTestCase()
{
    delete mCntServices;
    mCntServices = NULL;
    delete mEngine;
    mEngine = NULL;
    delete mTestViewManager;
    mTestViewManager = NULL;
}

void UT_CntServices::testCreating()
{
    mCntServices = new CntServices();
    QVERIFY( mCntServices );
}

void UT_CntServices::test_setEngine()
{
    mCntServices = new CntServices();
    QVERIFY( mCntServices );
    mCntServices->setEngine( *mEngine );
}


void UT_CntServices::test_singleFetch()
{
    mCntServices = new CntServices();
    // with empty action string
    mCntServices->singleFetch( "someTitle", "someAction", *this );
    // a signal is sent to slot onViewChanged(). The view params are stored by the slot.
    QString title = mViewParams.value(KCntServiceViewParamTitle).toString();
    QVERIFY( title == "someTitle" );
}

void UT_CntServices::test_multiFetch()
{
    mCntServices = new CntServices();
    // with empty action string
    mCntServices->multiFetch( "someTitle", "someAction", *this );
    // a signal is sent to slot onViewChanged(). The view params are stored by the slot.
    QString title = mViewParams.value(KCntServiceViewParamTitle).toString();
    QVERIFY( title == "someTitle" );
}

void UT_CntServices::test_editCreateNew()
{
    mCntServices = new CntServices();
    // with empty action string
    mCntServices->editCreateNew(  QContactPhoneNumber::DefinitionName, "123456", *this );
    // a signal is sent to slot onViewChanged(). The view params are stored by the slot.
    QVariant contact = mViewParams.value( ESelectedContact );
    //QVERIFY( contact. );
}

void UT_CntServices::test_editCreateNew2()
{

}

void UT_CntServices::test_editCreateNewFromVCard()
{

}

void UT_CntServices::test_editUpdateExisting()
{

}

void UT_CntServices::test_editUpdateExisting2()
{

}

void UT_CntServices::test_editExisting()
{

}

void UT_CntServices::test_launchContactCard()
{

}

void UT_CntServices::test_launchGroupMemberView()
{

}

void UT_CntServices::test_launchTemporaryContactCard()
{

}


void UT_CntServices::test_terminateService()
{
}


void UT_CntServices::test_removeNotSupportedFields()
{
}


void UT_CntServices::removeNotSupportedDetails()
{
}


void UT_CntServices::test_fillOnlineAccount()
{
}


void UT_CntServices::test_updateLocalId()
{
}


void UT_CntServices::test_CompleteServiceAndCloseApp()
{
}


void UT_CntServices::test_overrideReturnValue()
{
}


void UT_CntServices::test_allowSubViewsExit()
{
}

// receives signal from testviewmanager
void UT_CntServices::onViewChanged( const CntViewParameters aArgs )
{
    mViewParams = aArgs;
}


// from CntAbstractServiceProvider
void UT_CntServices::CompleteServiceAndCloseApp(const QVariant& /*retValue*/)
{
}

// EOF
