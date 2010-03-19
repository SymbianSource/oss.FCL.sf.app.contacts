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
#include "ut_logscntentry.h"
#include "logscntfinder.h"
#include "logspredictivetranslator.h"

#include <QtTest/QtTest>


void UT_LogsCntEntry::initTestCase()
{
}

void UT_LogsCntEntry::cleanupTestCase()
{
}


void UT_LogsCntEntry::init()
{
    mEntry = new LogsCntEntry( 0 );
}

void UT_LogsCntEntry::cleanup()
{
    delete mEntry;
    mEntry = 0;
    LogsPredictiveTranslator::deleteInstance();
}

void UT_LogsCntEntry::testConstructor()
{
    QVERIFY( !mEntry->isCached() );
    QVERIFY( mEntry->contactId() == 0 );
    QVERIFY( mEntry->type() == LogsCntEntry::EntryTypeContact );
    QVERIFY( mEntry->firstName()[0].text() == QString("") );
    QVERIFY( mEntry->lastName()[0].text() == QString("") );
    QVERIFY( mEntry->phoneNumber().text() == QString("") );
    QVERIFY( mEntry->avatarPath() == QString("") );
    QVERIFY( mEntry->handle() == 0 );
    QVERIFY( mEntry->speedDial() == QString("") );
    
    
    mEntry->setFirstName( QString( "foo" ));
    mEntry->setLastName( QString( "bar" ));
    
    LogsCntEntry* e = new LogsCntEntry( *mEntry );
    QVERIFY( e->isCached() );
    QVERIFY( e->contactId() == mEntry->contactId() );
    QVERIFY( e->type() == mEntry->type() );
    QVERIFY( e->firstName()[0].text() == mEntry->firstName()[0].text() );
    QVERIFY( e->lastName()[0].text() == mEntry->lastName()[0].text() );
    QVERIFY( e->phoneNumber().text() == mEntry->phoneNumber().text() );
    QVERIFY( e->mHandle == mEntry->mHandle );
    QVERIFY( e->speedDial() == mEntry->speedDial() );
    
    mEntry->setFirstName( QString( "Nokia" ));
    QVERIFY( e->firstName()[0].text() != mEntry->firstName()[0].text() );
    
    delete e;
    e = 0;
    
    LogsCntEntryHandle* foo = (LogsCntEntryHandle*)1;
    e = new LogsCntEntry( *foo, 0 );
    QVERIFY( e->isCached() );
    QVERIFY( e->contactId() == 0 );
    QVERIFY( e->type() == LogsCntEntry::EntryTypeHistory );
    QVERIFY( e->mHandle == foo );
    
    delete e;
    e = 0;
    
}


void UT_LogsCntEntry::testSetName()
{
    QString empty("");
    
    QVERIFY( !mEntry->isCached() );
    QVERIFY( mEntry->type() == LogsCntEntry::EntryTypeContact );
    
    mEntry->setFirstName( QString( "first" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString( "first" ) );
    QVERIFY( !mEntry->firstName()[0].highlights());
    QVERIFY( mEntry->firstName()[0].mTranslatedText == empty );
    
    mEntry->setLastName( QString( "last" ) );
    QVERIFY( mEntry->lastName()[0].text() == QString( "last" ) );
    QVERIFY( !mEntry->lastName()[0].highlights());
    QVERIFY( mEntry->lastName()[0].mTranslatedText == empty );
    
    mEntry->mType = LogsCntEntry::EntryTypeHistory;
    mEntry->setFirstName( QString( "first1" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString( "first1" ) );
    QVERIFY( !mEntry->firstName()[0].highlights());
    QVERIFY( mEntry->firstName()[0].mTranslatedText != empty );
    
    mEntry->setLastName( QString( "last1" ) );
    QVERIFY( mEntry->lastName()[0].text() == QString( "last1" ) );
    QVERIFY( !mEntry->lastName()[0].highlights());
    QVERIFY( mEntry->lastName()[0].mTranslatedText != empty );
    
    mEntry->setFirstName( QString( "first last" ) );
    QVERIFY( mEntry->firstName().count() == 2 );
    QVERIFY( mEntry->firstName()[0].text() == QString( "first" ) );
    QVERIFY( mEntry->firstName()[1].text() == QString( "last" ) );
    QVERIFY( !mEntry->firstName()[0].highlights() );
    QVERIFY( !mEntry->firstName()[1].highlights() );
    QVERIFY( mEntry->firstName()[0].mTranslatedText != empty );
    QVERIFY( mEntry->firstName()[1].mTranslatedText != empty );

    mEntry->setFirstName( QString( "" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString("") );

}

void UT_LogsCntEntry::testSetHighlights()
{

    mEntry->setHighlights( QString( "665" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString("") );
    QVERIFY( mEntry->lastName()[0].text() == QString("") );
    
    mEntry->setFirstName( QString( "Nokia" ) );
    mEntry->setLastName( QString( "Test" ) );
    
    mEntry->setHighlights( QString( "665" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString( "Nokia" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 3 );
    QVERIFY( mEntry->lastName()[0].text() == QString( "Test" ) );
    QVERIFY( !mEntry->lastName()[0].highlights());

    mEntry->mType = LogsCntEntry::EntryTypeContact;
    mEntry->setFirstName( QString( "Jim" ) );
    mEntry->setLastName( QString( "Johnson" ) );
    
    mEntry->setHighlights( QString( "5" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString( "Jim" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1 );
    QVERIFY( mEntry->lastName()[0].text() == QString( "Johnson" ) );
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
    mEntry->setFirstName( QString( "Big Jim" ) );
    mEntry->setLastName( QString( "Johnson" ) );
    
    mEntry->setHighlights( QString( "5" ) );
    QVERIFY( mEntry->firstName()[0].text() == QString( "Big" ) );
    QVERIFY( mEntry->firstName()[1].text() == QString( "Jim" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0 );
    QVERIFY( mEntry->firstName()[1].highlights() == 1 );
    QVERIFY( mEntry->lastName()[0].text() == QString( "Johnson" ) );
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
    mEntry->setFirstName( QString( "John" ) );
    mEntry->setLastName( QString( "Johnson" ) );
    mEntry->setHighlights( QString( "5646" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 4 );
    QVERIFY( mEntry->lastName()[0].highlights() == 4);
    
    mEntry->setHighlights( QString( "56467" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0 );
    QVERIFY( mEntry->lastName()[0].highlights() == 5);
    
    mEntry->setHighlights( QString( "505" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
    mEntry->setHighlights( QString( "506" ) );//entry is not a match
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 1);//506 means 5 or 6 in higlighting
    
    mEntry->setFirstName( QString( "Big Jim Johnson" ) );
    mEntry->setLastName( QString( "" ) );
    
    mEntry->setHighlights( QString( "205" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 1);
    QVERIFY( mEntry->firstName()[2].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( QString( "20505" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 1);
    QVERIFY( mEntry->firstName()[2].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( QString( "50205" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 1);
    QVERIFY( mEntry->firstName()[2].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setHighlights( QString( "50502" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 1);
    QVERIFY( mEntry->firstName()[2].highlights() == 1);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);
    
    mEntry->setFirstName( QString( "Big John" ) );
    mEntry->setLastName( QString( "Johnson" ) );
    
    mEntry->setHighlights( QString( "2056" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 2);
    
    mEntry->setHighlights( QString( "5602" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 2);
    
    mEntry->setHighlights( QString( "564602" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 4);
    QVERIFY( mEntry->lastName()[0].highlights() == 4);
    
    mEntry->setHighlights( QString( "5646702" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 5);
    
    mEntry->setHighlights( QString( "5646" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 4);
    QVERIFY( mEntry->lastName()[0].highlights() == 4);
    
    mEntry->setFirstName( QString( "John 0John" ) );
    mEntry->setLastName( QString( "Malkovich" ) );
    mEntry->setHighlights( QString( "05" ) );
    
    QVERIFY( mEntry->firstName()[0].highlights() == 0);
    QVERIFY( mEntry->firstName()[1].highlights() == 2);
    QVERIFY( mEntry->lastName()[0].highlights() == 0);

    mEntry->setHighlights( QString( "0506" ) );
    QVERIFY( mEntry->firstName()[0].highlights() == 1);
    QVERIFY( mEntry->firstName()[1].highlights() == 0);
    QVERIFY( mEntry->lastName()[0].highlights() == 1);
    
}

void UT_LogsCntEntry::testSetPhoneNumber()
{
    mEntry->setPhoneNumber( QString( "1234567" ) );
    QVERIFY( mEntry->phoneNumber().text() == QString( "1234567" ) );
    QVERIFY( mEntry->phoneNumber().mTranslatedText == QString( "" ) );
    QVERIFY( mEntry->phoneNumber().mHighlights == 0 );
    
}

void UT_LogsCntEntry::testSetSpeedDial()
{
    mEntry->setSpeedDial( QString( "1" ) );
    QVERIFY( mEntry->speedDial() == QString( "1" ) );
    
}

void UT_LogsCntEntry::testSetAvatarPath()
{
    mEntry->setAvatarPath( QString( "c:\\data\\images\\logstest1.jpg" ) );
    QVERIFY( mEntry->avatarPath() == QString( "c:\\data\\images\\logstest1.jpg" ) );
    
}

void UT_LogsCntEntry::testRichText()
{
    
    mEntry->setFirstName( QString( "John" ) );
    mEntry->setHighlights( QString( "5" ) );
    QVERIFY( mEntry->firstName()[0].richText() == QString( "<b><u>J</u></b>ohn" ) );
    QVERIFY( mEntry->firstName()[0].richText( QString("<u>"),QString("</u>")  ) 
                                               == QString( "<u>J</u>ohn" ) );
    
    mEntry->setFirstName( QString( "aaa" ) );
    QVERIFY( mEntry->firstName()[0].richText() == QString( "aaa" ) );
    
    mEntry->setFirstName( QString( "" ) );
    QVERIFY( mEntry->firstName()[0].richText() == QString( "" ) );
    
    mEntry->setPhoneNumber( QString( "1234567" ) );
    mEntry->setHighlights( QString( "5" ) );
    
    QVERIFY( mEntry->phoneNumber().mHighlights == 0 );
    QVERIFY( mEntry->phoneNumber().text() == QString( "1234567" ) );
    QVERIFY( mEntry->phoneNumber().richText() == QString( "1234567" ) );
    
    mEntry->setHighlights( QString( "1" ) );
    
    QVERIFY( mEntry->phoneNumber().mHighlights == 1 );
    QVERIFY( mEntry->phoneNumber().text() == QString( "1234567" ) );
    QVERIFY( mEntry->phoneNumber().richText() == QString( "<b><u>1</u></b>234567" ) );
    
}

void UT_LogsCntEntry::testMatch()
{
    mEntry->mType = LogsCntEntry::EntryTypeHistory;
    
    mEntry->setFirstName( QString( "John" ) );
    QVERIFY( mEntry->match( QString( "5" ) ) );
    QVERIFY( !mEntry->match( QString( "6" ) ) );
    
    mEntry->setPhoneNumber( QString( "11111111111" ) );
    mEntry->setFirstName( QString( "John" ) );
    mEntry->setLastName( QString( "Malkovich" ) );
    QVERIFY( mEntry->match( QString( "6" ) ) );
    QVERIFY( mEntry->match( QString( "5" ) ) );
    QVERIFY( !mEntry->match( QString( "2" ) ) );
    QVERIFY( mEntry->match( QString( "56" ) ) );
    QVERIFY( !mEntry->match( QString( "566" ) ) );

    mEntry->setPhoneNumber( QString( "5669876566" ) );
    QVERIFY( mEntry->match( QString( "566" ) ) );

// - zero cases -
    
    mEntry->setPhoneNumber( QString( "20298457698576" ) );
    mEntry->setFirstName( QString( "John" ) );
    mEntry->setLastName( QString( "Malkovich" ) );
    
    QVERIFY( mEntry->match( QString( "202" ) ) );//match not John and match not Malkovich match phone
    QVERIFY( !mEntry->match( QString( "507" ) ) );//match John and match not Malkovich
    QVERIFY( !mEntry->match( QString( "206" ) ) );//match not John and macth Malkovich
    QVERIFY( mEntry->match( QString( "506" ) ) );//match John and macth Malkovich
    QVERIFY( mEntry->match( QString( "605" ) ) );//match John and macth Malkovich
    QVERIFY( !mEntry->match( QString( "505" ) ) );//match John "two times"
    QVERIFY( mEntry->match( QString( "60005" ) ) );//match John and match Malkovich
    QVERIFY( mEntry->match( QString( "6000500" ) ) );//match John and match Malkovich
    
    mEntry->setFirstName( QString( "John Malkovich" ) );
    mEntry->setLastName( QString( "" ) );
    
    QVERIFY( mEntry->match( QString( "202" ) ) );//match not John and match not Malkovich match phone
    QVERIFY( !mEntry->match( QString( "507" ) ) );//match John and match not Malkovich
    QVERIFY( !mEntry->match( QString( "206" ) ) );//match not John and macth Malkovich
    QVERIFY( mEntry->match( QString( "506" ) ) );//match John and macth Malkovich
    QVERIFY( mEntry->match( QString( "605" ) ) );//match John and macth Malkovich
    QVERIFY( !mEntry->match( QString( "505" ) ) );//match John "two times"
    QVERIFY( mEntry->match( QString( "60005" ) ) );//match John and match Malkovich
    QVERIFY( mEntry->match( QString( "6000500" ) ) );//match John and match Malkovich

    mEntry->setFirstName( QString( "" ) );
    mEntry->setLastName( QString( "John Malkovich" ) );
    
    QVERIFY( mEntry->match( QString( "202" ) ) );//match not John and match not Malkovich match phone
    QVERIFY( !mEntry->match( QString( "507" ) ) );//match John and match not Malkovich
    QVERIFY( !mEntry->match( QString( "206" ) ) );//match not John and macth Malkovich
    QVERIFY( mEntry->match( QString( "506" ) ) );//match John and macth Malkovich
    QVERIFY( mEntry->match( QString( "605" ) ) );//match John and macth Malkovich
    QVERIFY( !mEntry->match( QString( "505" ) ) );//match John "two times"
    QVERIFY( mEntry->match( QString( "60005" ) ) );//match John and match Malkovich
    QVERIFY( mEntry->match( QString( "6000500" ) ) );//match John and match Malkovich

    mEntry->setFirstName( QString( "John Peter" ) );
    mEntry->setLastName( QString( "Malkovich" ) );
    
    QVERIFY( mEntry->match( QString( "50607" ) ) );//match John and match Malkovich match Peter
    QVERIFY( mEntry->match( QString( "506" ) ) );//match John and macth Malkovich match ignore Peter
    QVERIFY( !mEntry->match( QString( "50608" ) ) );//match John and match Malkovich match not Peter
    QVERIFY( !mEntry->match( QString( "5060702" ) ) );//match John and match Malkovich match Peter + no match extra
    QVERIFY( !mEntry->match( QString( "5060706" ) ) );//match John and match Malkovich match Peter + macth extra 
    
    mEntry->setFirstName( QString( "John Kalkovich" ) );
    mEntry->setLastName( QString( "" ) );
    
    QVERIFY( mEntry->match( QString( "505" ) ) );//match John and match Kalkovich
    
    mEntry->setFirstName( QString( "John John Malkovich" ) );
    mEntry->setLastName( QString( "" ) );
    
    QVERIFY( mEntry->match( QString( "50605" ) ) );//match John and match Malkovich and match John
    QVERIFY( mEntry->match( QString( "506" ) ) );//match Johns and macth Malkovich
    QVERIFY( !mEntry->match( QString( "50608" ) ) );//match John and match Malkovich match not Peter
    
    mEntry->setFirstName( QString( "John 0John Malkovich" ) );
    mEntry->setLastName( QString( "" ) );
    
    QVERIFY( !mEntry->match( QString( "50605" ) ) );
    QVERIFY( !mEntry->match( QString( "505" ) ) );
    QVERIFY( mEntry->match( QString( "5" ) ) );
    QVERIFY( mEntry->match( QString( "0" ) ) );
    QVERIFY( mEntry->match( QString( "05" ) ) );
    
    //QVERIFY( mEntry->match( QString( "0505" ) ) ); //not supported
    QVERIFY( mEntry->match( QString( "0506" ) ) ); //John and Malkovich not 0Jonh
    //QVERIFY( mEntry->match( QString( "050506" ) ) ); //not supported
    
}


