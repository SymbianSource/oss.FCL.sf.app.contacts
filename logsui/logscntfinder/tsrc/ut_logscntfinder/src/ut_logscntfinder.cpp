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
    QVERIFY( finder.mCachedCounter == 0 );
    
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
    QVERIFY( mFinder->resultsCount() == 0 );
    QVERIFY( mFinder->mCachedCounter == 0 );
    
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
 
    //
    // -- reuse results: do not create new entry, if there is already 
    //    entry with same cid as in cid list (from db).
    //
    qDeleteAll( mFinder->mResults );
    mFinder->mResults.clear();
    qDeleteAll( mFinder->mHistoryEvents );
    mFinder->mHistoryEvents.clear();
    
    pattern = QString("3");
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 5 );
    QVERIFY( mFinder->resultsCount() );
    QVERIFY( !mFinder->mResults[0]->isCached() );
    const LogsCntEntry* firstE = &mFinder->resultAt( 0 );
    QVERIFY( firstE->isCached());
    QVERIFY( firstE->firstName()[0].highlights() == 1 );
    
    pattern = QString("34");
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 6 );
    QVERIFY( mFinder->resultsCount() );
    QVERIFY( &mFinder->resultAt( 0 ) == firstE );
    QVERIFY( firstE->firstName()[0].highlights() == 2 );
    
    entry1 = new LogsCntEntry( *handle1,0 );
    entry1->setFirstName( QString("First Beam") );
    mFinder->insertEntry( 0, entry1 );

    pattern = QString("3");
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 7 );
    QVERIFY( mFinder->resultsCount() );
    const LogsCntEntry* firstHe = &mFinder->resultAt( 0 );
    QVERIFY( firstHe != entry1 );
    QVERIFY( firstHe->firstName()[0].text() == entry1->firstName()[0].text() );
    QVERIFY( &mFinder->resultAt( 1 ) == firstE );
    QVERIFY( firstE->firstName()[0].highlights() == 1 );
    
    //
    // -- reuse results: do search on results, if all are cached
    //
    qDeleteAll( mFinder->mResults );
    mFinder->mResults.clear();
    qDeleteAll( mFinder->mHistoryEvents );
    mFinder->mHistoryEvents.clear();
    mFinder->mCachedCounter = 0;

    pattern = QString("3");
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 8 );
    QVERIFY( mFinder->resultsCount() );
    QVERIFY( mFinder->mCachedCounter == 0 );
    
    for( int i=0;i<mFinder->resultsCount();i++) {
        mFinder->resultAt( i ); 
    }
    QVERIFY( mFinder->mCachedCounter == mFinder->resultsCount() );
    
    //stub gives always 10+ results regardless pattern,
    //thus, if stub is not used, there should be only one
    //match. The match was found from cache.
    pattern = QString("347781");//first1 cache
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 9 );
    QVERIFY( mFinder->resultsCount() == 2 ); //first1 and first10
    QVERIFY( mFinder->mCachedCounter == 2 );
    QVERIFY( mFinder->mResults[0]->isCached() );
    QVERIFY( mFinder->resultAt( 0 ).type() == LogsCntEntry::EntryTypeContact );
    QVERIFY( mFinder->resultAt( 0 ).firstName()[0].text() == QString("first1") );
    QVERIFY( mFinder->resultAt( 0 ).firstName()[0].highlights() == 6 );
    QVERIFY( mFinder->resultAt( 1 ).firstName()[0].text() == QString("first10") );
    QVERIFY( mFinder->resultAt( 1 ).firstName()[0].highlights() == 6 );

    
    pattern = QString("3477810");//first10 cache
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 10 );
    QVERIFY( mFinder->resultsCount() == 1 ); //first10
    QVERIFY( mFinder->mCachedCounter == 1 );
    QVERIFY( mFinder->mResults[0]->isCached() );
    QVERIFY( mFinder->resultAt( 0 ).type() == LogsCntEntry::EntryTypeContact );
    QVERIFY( mFinder->resultAt( 0 ).firstName()[0].text() == QString("first10") );
    QVERIFY( mFinder->resultAt( 0 ).firstName()[0].highlights() == 7 );

    pattern = QString("34778104");//missmatch cache
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 11 );
    QVERIFY( mFinder->resultsCount() == 0 );
    QVERIFY( mFinder->mCachedCounter == 0 );
    
    pattern = QString("3477810");//first10 must go to db
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 12 );
    QVERIFY( mFinder->resultsCount() );
    QVERIFY( mFinder->mCachedCounter == 0 );//reuse cached cids
    

    entry1 = new LogsCntEntry( *handle1,0 );
    entry1->setFirstName( QString("First Beam") );
    mFinder->insertEntry( 0, entry1 );
    
    pattern = QString("34778");//first must go to db
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 13 );
    QVERIFY( mFinder->resultsCount() > 1 );
    QVERIFY( mFinder->mCachedCounter == 1 );//history event, which is result
    QVERIFY( mFinder->mResults[0]->isCached() );//history event is always cached
    QVERIFY( !mFinder->mResults[1]->isCached() );//was not cached

    QVERIFY( mFinder->resultAt( 0 ).firstName()[0].highlights() == 5 );
    QVERIFY( mFinder->resultAt( 1 ).firstName()[0].highlights() == 5 );
    
    pattern = QString("123456789112345"); //15 digits query
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 14 );
    QCOMPARE( mFinder->mCurrentPredictivePattern.length(), 15 );
    
    pattern = QString("1234567891123456"); //16 digits query
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 15 );
    QCOMPARE( mFinder->mCurrentPredictivePattern.length(), 15 );
    
    pattern = QString("12345678911234567891"); //20 digits query
    mFinder->predictiveSearchQuery( pattern );
    QCOMPARE( spy.count(), 16 );
    QCOMPARE( mFinder->mCurrentPredictivePattern.length(), 15 );
}

void UT_LogsCntFinder::testResultAt()
{
    QVERIFY( mFinder->resultsCount() == 0 );
    QString pattern( "5" );
    mFinder->predictiveSearchQuery( pattern );
    QVERIFY( mFinder->resultsCount() > 0 );
    
    const LogsCntEntry& e = mFinder->resultAt( 0 );
    QVERIFY( e.firstName()[0].text() == QString("first1") );//stub
    QVERIFY( e.lastName()[0].text() == QString("last1") );//stub
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



