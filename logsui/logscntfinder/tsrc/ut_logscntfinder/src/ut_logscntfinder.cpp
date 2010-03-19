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
#include "ut_logscntfinder.h"
#include "logscntfinder.h"

#include <QtTest/QtTest>
#include <QSignalSpy>


void UT_LogsCntFinder::initTestCase()
{
}

void UT_LogsCntFinder::cleanupTestCase()
{
}


void UT_LogsCntFinder::init()
{
    mFinder = new LogsCntFinder();
}

void UT_LogsCntFinder::cleanup()
{
    delete mFinder;
    mFinder = 0;
}

void UT_LogsCntFinder::testConstructor()
{
    QVERIFY( mFinder );
    QVERIFY( mFinder->mCurrentPredictivePattern == "" );
    QVERIFY( mFinder->mResults.count() == 0 );
    QVERIFY( mFinder->mHistoryEvents.count() == 0 );
    QVERIFY( mFinder->mContactManager );
    
    LogsCntFinder finder( *mFinder->mContactManager ); 
    QVERIFY( finder.mCurrentPredictivePattern == "" );
    QVERIFY( finder.mResults.count() == 0 );
    QVERIFY( finder.mHistoryEvents.count() == 0 );
    QVERIFY( finder.mContactManager );
    
}

void UT_LogsCntFinder::testPredictiveSearchQuery()
{
    QSignalSpy spy(mFinder, SIGNAL(queryReady() ) );
    int resultsCount = 0;
    
    QString pattern( "23" );
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 1 );
    QVERIFY( mFinder->mCurrentPredictivePattern == pattern );
    resultsCount = mFinder->resultsCount();
    QVERIFY( resultsCount > 0 );
    
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 2 );
    QVERIFY( mFinder->mCurrentPredictivePattern == pattern );
    QVERIFY( mFinder->resultsCount() == resultsCount );

    LogsCntEntry* e = mFinder->mResults[ 0 ];
    pattern = QString("");
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 3 );
    QVERIFY( mFinder->mResults[ 0 ] == e );
    
//---
    
    LogsCntEntryHandle* handle1 = (LogsCntEntryHandle*)1;
    LogsCntEntryHandle* handle2 = (LogsCntEntryHandle*)2;
    LogsCntEntryHandle* handle3 = (LogsCntEntryHandle*)3;
    
    LogsCntEntry* entry1 = new LogsCntEntry( *handle1,0 );
    QVERIFY( entry1->type() == LogsCntEntry::EntryTypeHistory );
    QVERIFY( entry1->firstName()[0].text() == QString("") );
    QVERIFY( entry1->lastName()[0].text() == QString("") );
    
    entry1->setFirstName( QString("Jim Beam") );
    
    QVERIFY( entry1->firstName()[0].text() == QString("Jim") );
    QVERIFY( entry1->firstName()[0].mTranslatedText != QString("") );
    QVERIFY( entry1->firstName()[1].text() == QString("Beam") );
    QVERIFY( entry1->firstName()[1].mTranslatedText != QString("") );
    QVERIFY( entry1->lastName()[0].text() == QString("") );
    QVERIFY( entry1->lastName()[0].mTranslatedText == QString("") );
    mFinder->insertEntry( 0, entry1 );
    QVERIFY( mFinder->mHistoryEvents.count() == 1 );
    
    LogsCntEntry* entry2 = new LogsCntEntry( *handle2,0 );
    entry2->setFirstName( QString("first1") );
    entry2->setLastName( QString("last1") );
    entry2->setAvatarPath( QString("c:\\data\\images\\logstest1.jpg") );
    mFinder->insertEntry( 0, entry2 );
    QVERIFY( mFinder->mHistoryEvents.count() == 2 );    
    
    LogsCntEntry* entry3 = new LogsCntEntry( *handle3,0 );
    entry3->setFirstName( QString("Nokia") );
    entry3->setLastName( QString("") );
    mFinder->insertEntry( 0, entry3 );
    QVERIFY( mFinder->mHistoryEvents.count() == 3 );    
    
    pattern = QString("3");
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 4 );
    QVERIFY( mFinder->resultsCount() > resultsCount );
    QVERIFY( mFinder->resultAt( 0 ).handle() == handle2 );
    QVERIFY( mFinder->resultAt( 0 ).firstName()[0].text() == QString("first1") );
    QVERIFY( mFinder->resultAt( 0 ).lastName()[0].text() == QString("last1") );
    QVERIFY( mFinder->resultAt( 0 ).firstName()[0].highlights() == 1 );
    QVERIFY( mFinder->resultAt( 0 ).lastName()[0].highlights() == 0 );
    QVERIFY( mFinder->resultAt( 0 ).avatarPath() == QString("c:\\data\\images\\logstest1.jpg") );
    
}

void UT_LogsCntFinder::testResultAt()
{
    QVERIFY( mFinder->resultsCount() == 0 );
    QString pattern( "5" );
    mFinder->predictiveSearchQuery( pattern );
    QVERIFY( mFinder->resultsCount() > 0 );
    
    const LogsCntEntry& e = mFinder->resultAt( 0 );
    QVERIFY( e.firstName()[0].text() == QString("first") );//stub
    QVERIFY( e.lastName()[0].text() == QString("last") );//stub
    QVERIFY( e.speedDial() == QString("") );
    QVERIFY( e.phoneNumber().text() == QString("555789987") );//stub
    QVERIFY( e.phoneNumber().highlights() == 1 );
    QVERIFY( e.mType == LogsCntEntry::EntryTypeContact );
    QVERIFY( e.isCached() );
    
    const LogsCntEntry& f = mFinder->resultAt( 0 );
    QVERIFY( &e == &f );
    
    pattern = QString("2");
    mFinder->predictiveSearchQuery( pattern );
    QVERIFY( mFinder->resultsCount() > 0 );
    const LogsCntEntry& g = mFinder->resultAt( 0 );
    
}

void UT_LogsCntFinder::testInsertEntry()
{
    
    LogsCntEntryHandle* handle1 = (LogsCntEntryHandle*)1;
    LogsCntEntryHandle* handle2 = (LogsCntEntryHandle*)2;
    LogsCntEntryHandle* handle3 = (LogsCntEntryHandle*)3;
    
    LogsCntEntry* entry1 = new LogsCntEntry( *handle1,0 );
    mFinder->insertEntry( 0, entry1 );
    QVERIFY( mFinder->mHistoryEvents.count() == 1 );    
    LogsCntEntry* entry2 = new LogsCntEntry( *handle2,0 );
    mFinder->insertEntry( 0, entry2 );
    QVERIFY( mFinder->mHistoryEvents.count() == 2 );    
    LogsCntEntry* entry3 = new LogsCntEntry( *handle3,0 );
    mFinder->insertEntry( 1, entry3 );
    QVERIFY( mFinder->mHistoryEvents.count() == 3 );    
    
    QVERIFY( mFinder->getEntry( *handle1 ) == entry1 );
    QVERIFY( mFinder->getEntry( *handle2 ) == entry2 );
    QVERIFY( mFinder->getEntry( *handle3 ) == entry3 );
    
    
}

void UT_LogsCntFinder::testDeleteEntry()
{

    LogsCntEntryHandle* handle1 = (LogsCntEntryHandle*)1;
    LogsCntEntryHandle* handle2 = (LogsCntEntryHandle*)2;
    LogsCntEntryHandle* handle3 = (LogsCntEntryHandle*)3;
    
    LogsCntEntry* entry1 = new LogsCntEntry( *handle1,0 );
    mFinder->insertEntry( 0, entry1 );
    LogsCntEntry* entry2 = new LogsCntEntry( *handle2,0 );
    mFinder->insertEntry( 0, entry2 );
    LogsCntEntry* entry3 = new LogsCntEntry( *handle3,0 );
    mFinder->insertEntry( 1, entry3 );
    QVERIFY( mFinder->mHistoryEvents.count() == 3 );    
    
    mFinder->deleteEntry( *handle1 );
    QVERIFY( mFinder->mHistoryEvents.count() == 2 );
    mFinder->deleteEntry( *handle2 );
    QVERIFY( mFinder->mHistoryEvents.count() == 1 );
    mFinder->deleteEntry( *handle3 );
    QVERIFY( mFinder->mHistoryEvents.count() == 0 );
 
    entry1 = new LogsCntEntry( *handle1,0 );
    mFinder->insertEntry( 0, entry1 );
    QVERIFY( mFinder->mHistoryEvents.count() == 1 );
    entry2 = new LogsCntEntry( *entry1 );
    mFinder->mResults.append( entry2 );
    mFinder->deleteEntry( *handle1 );
    QVERIFY( mFinder->mHistoryEvents.count() == 0 );
    QVERIFY( mFinder->mResults.count() == 1 );
    
}

void UT_LogsCntFinder::testGetEntry()
{
    LogsCntEntryHandle* handle1 = (LogsCntEntryHandle*)1;
    
    LogsCntEntry* entry1 = new LogsCntEntry( *handle1,0 );
    QVERIFY( entry1->firstName()[0].text() == QString("") );
    QVERIFY( entry1->lastName()[0].text() == QString("") );
    entry1->setFirstName( QString("first") );
    entry1->setLastName( QString("last") );
    QVERIFY( entry1->firstName()[0].text() == QString("first") );
    QVERIFY( entry1->lastName()[0].text() == QString("last") );
    mFinder->insertEntry( 0, entry1 );

    LogsCntEntry* entry = mFinder->getEntry( *handle1 );
    QVERIFY( entry == entry1 );
    entry->setFirstName( QString("foo") );
    entry->setLastName( QString("bar") );
    
    LogsCntEntry* entry3 = mFinder->getEntry( *handle1 );
    QVERIFY( entry3 == entry1 );
    QVERIFY( entry3->firstName()[0].text() == QString("foo") );
    QVERIFY( entry3->lastName()[0].text() == QString("bar") );
    
}



