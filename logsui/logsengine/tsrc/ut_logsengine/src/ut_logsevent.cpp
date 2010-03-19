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
#include "ut_logsevent.h"
#include "logsevent.h"
#include "logseventdata.h"
#include "qtcontacts_stubs_helper.h"
#include <logcli.h>
#include <qcontactmanager.h>
#include <qcontactname.h>

#include <QtTest/QtTest>

void UT_LogsEvent::initTestCase()
{

}

void UT_LogsEvent::cleanupTestCase()
{
}


void UT_LogsEvent::init()
{
    mEvent = new LogsEvent();
}

void UT_LogsEvent::cleanup()
{
    delete mEvent;
    mEvent = 0;
}

void UT_LogsEvent::testConstructor()
{
    QVERIFY( mEvent );
}

void UT_LogsEvent::testInitializeEventL()
{
	CLogEvent* logEvent = 0;
	QT_TRAP_THROWING( logEvent = CLogEvent::NewL(); )
	
	// Test number
	_LIT( eventTestNumber, "123444555" );
	QString strEventTestNumber = DESC_TO_QSTRING( eventTestNumber() );
	logEvent->SetNumber( eventTestNumber() );
	QT_TRAP_THROWING( mEvent->initializeEventL(*logEvent, mStrings); )
    QVERIFY( mEvent->mNumber == strEventTestNumber );
}

void UT_LogsEvent::testSetters()
{
}

void UT_LogsEvent::testGetters()
{
}

void UT_LogsEvent::testValidate()
{
    // No num, remote party nor remote url, not valid
    QVERIFY( !mEvent->validate() );
    
    // Num, valid
    mEvent->mNumber = "123454555";
    QVERIFY( mEvent->validate() );
    
    // Remote party, valid
    mEvent->mNumber.clear();
    mEvent->mRemoteParty = "remotePart";
    QVERIFY( mEvent->validate() );
    
    // Remote url, valid
    mEvent->mRemoteParty.clear();
    LogsEventData* eventData = new LogsEventData;
    eventData->mRemoteUrl = "2343425";
    mEvent->setLogsEventData(eventData);
    QVERIFY( mEvent->validate() );
}

void UT_LogsEvent::testGetNumberForCalling()
{
    mEvent->mNumber = "123454555";
    QVERIFY( mEvent->getNumberForCalling() == "123454555" );
    mEvent->mNumber.clear();
    QVERIFY( mEvent->getNumberForCalling() == "" );
    mEvent->mEventType = LogsEvent::TypeVoIPCall;
    LogsEventData* eventData = new LogsEventData;
    eventData->mRemoteUrl = "2343425";
    mEvent->setLogsEventData(eventData);
    QVERIFY( mEvent->getNumberForCalling() == "2343425" );
    LogsEventData* eventData2 = new LogsEventData;
    eventData2->mRemoteUrl = "test@pr.fi";
    mEvent->setLogsEventData(eventData2);
    QVERIFY( mEvent->getNumberForCalling() == "test@pr.fi" );
}

void UT_LogsEvent::testSetContactLocalId()
{
    //event data hasn't been created yet
    QVERIFY( !mEvent->logsEventData() );
    mEvent->setContactLocalId( 2 );
    QVERIFY( mEvent->logsEventData() );
    QVERIFY( mEvent->logsEventData()->contactLocalId() == 2 );    
    QVERIFY( mEvent->contactLocalId() == 2 );
    
    //event data exists
    mEvent->setContactLocalId( 3 );
    QVERIFY( mEvent->logsEventData()->contactLocalId() == 3 );    
    QVERIFY( mEvent->contactLocalId() == 3 );
    
    //getting contact id with no event data
    mEvent->setLogsEventData(NULL);
    QVERIFY( mEvent->contactLocalId() == 0 );
    QVERIFY( !mEvent->logsEventData() );
}

void UT_LogsEvent::testUpdateRemotePartyFromContacts()
{
    // No search term
    QContactManager manager;
    LogsEvent event;
    QVERIFY( event.updateRemotePartyFromContacts(manager).length() == 0 );
    QVERIFY( event.remoteParty().length() == 0 );
    
    // Number as search term, no match
    event.setNumber("12345");
    QVERIFY( event.updateRemotePartyFromContacts(manager).length() == 0 );
    QVERIFY( event.remoteParty().length() == 0 );
    
    // Number as search term, match
    QtContactsStubsHelper::setContactNames("first", "last");
    event.setNumber("11112222");
    QString newRemoteParty = event.updateRemotePartyFromContacts(manager);
    QVERIFY( newRemoteParty.length() > 0 );
    QVERIFY( newRemoteParty == event.remoteParty() );
    
    // Voip address as search term, no match
    LogsEvent event2;
    LogsEventData* eventData = new LogsEventData;
    eventData->mRemoteUrl = "1.2.3.4";
    event2.setLogsEventData(eventData);
    event2.setEventType(LogsEvent::TypeVoIPCall);
    QVERIFY( event2.updateRemotePartyFromContacts(manager).length() == 0 );
    QVERIFY( event2.remoteParty().length() == 0 );
    
    // Voip address as search term, match
    eventData->mRemoteUrl = "11112222";
    newRemoteParty = event2.updateRemotePartyFromContacts(manager);
    QVERIFY( newRemoteParty.length() > 0 );
    QVERIFY( newRemoteParty == event.remoteParty() );
}

void UT_LogsEvent::testParseContactName()
{
    LogsEvent event;
    QtContactsStubsHelper::reset();
    QContactName name;
    QtContactsStubsHelper::setContactNames("firstname", "");
    QVERIFY(event.parseContactName(name) == "firstname");    
    
    QtContactsStubsHelper::setContactNames("firstname", "lastname");
    QVERIFY(event.parseContactName(name) == "firstname lastname");
    
    QtContactsStubsHelper::setContactNames("", "lastname");
    QVERIFY(event.parseContactName(name) == "lastname");
    
    QtContactsStubsHelper::setContactNames("", "");
    QVERIFY(event.parseContactName(name) == "");
}

