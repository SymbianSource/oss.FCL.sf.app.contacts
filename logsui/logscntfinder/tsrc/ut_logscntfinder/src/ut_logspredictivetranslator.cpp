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
#include "ut_logspredictivetranslator.h"
#include "logspredictivetranslator.h"

#include <QtTest/QtTest>
#include <QSignalSpy>


void UT_LogsPredictiveTranslator::initTestCase()
{
}

void UT_LogsPredictiveTranslator::cleanupTestCase()
{
}


void UT_LogsPredictiveTranslator::init()
{
    mTranslator = LogsPredictiveTranslator::instance();
}

void UT_LogsPredictiveTranslator::cleanup()
{
    LogsPredictiveTranslator::deleteInstance();
}

void UT_LogsPredictiveTranslator::testConstructor()
{
    QVERIFY( mTranslator );
    QVERIFY( mTranslator->mKeyMap );
    
}

void UT_LogsPredictiveTranslator::testTranslate()
{
    QVERIFY( mTranslator->translate( QString( "Nokia" ) ) == QString( "66542" ) );
    QVERIFY( mTranslator->translate( QString( "Aarne" ) ) == QString( "22763" ) );
    QVERIFY( mTranslator->translate( QString( "Bertta" ) ) == QString( "237882" ) );
    QVERIFY( mTranslator->translate( QString( "Celsius" ) ) == QString( "2357487" ) );
    QVERIFY( mTranslator->translate( QString( "Daavid" ) ) == QString( "322843" ) );
    QVERIFY( mTranslator->translate( QString( "Eemeli" ) ) == QString( "336354" ) );
    QVERIFY( mTranslator->translate( QString( "Faarao" ) ) == QString( "322726" ) );
    QVERIFY( mTranslator->translate( QString( "Gideon" ) ) == QString( "443366" ) );
    QVERIFY( mTranslator->translate( QString( "Heikki" ) ) == QString( "434554" ) );
    QVERIFY( mTranslator->translate( QString( "Iivari" ) ) == QString( "448274" ) );
    QVERIFY( mTranslator->translate( QString( "Jussi" ) ) == QString( "58774" ) );
    QVERIFY( mTranslator->translate( QString( "Kalle" ) ) == QString( "52553" ) );
    QVERIFY( mTranslator->translate( QString( "Lauri" ) ) == QString( "52874" ) );
    QVERIFY( mTranslator->translate( QString( "Matti" ) ) == QString( "62884" ) );
    QVERIFY( mTranslator->translate( QString( "Niilo" ) ) == QString( "64456" ) );
    QVERIFY( mTranslator->translate( QString( "Otto" ) ) == QString( "6886" ) );
    QVERIFY( mTranslator->translate( QString( "Paavo" ) ) == QString( "72286" ) );
    QVERIFY( mTranslator->translate( QString( "Kuu" ) ) == QString( "588" ) );
    QVERIFY( mTranslator->translate( QString( "Risto" ) ) == QString( "74786" ) );
    QVERIFY( mTranslator->translate( QString( "Sakari" ) ) == QString( "725274" ) );
    QVERIFY( mTranslator->translate( QString( "Tyyne" ) ) == QString( "89963" ) );
    QVERIFY( mTranslator->translate( QString( "Urho" ) ) == QString( "8746" ) );
    QVERIFY( mTranslator->translate( QString( "Vihtori" ) ) == QString( "8448674" ) );
    QVERIFY( mTranslator->translate( QString( "Wiski" ) ) == QString( "94754" ) );
    QVERIFY( mTranslator->translate( QString( "Yrjö" ) ) == QString( "9756" ) );
    QVERIFY( mTranslator->translate( QString( "Tseta" ) ) == QString( "87382" ) );
    QVERIFY( mTranslator->translate( QString( "Åke" ) ) == QString( "253" ) );
    QVERIFY( mTranslator->translate( QString( "Äiti" ) ) == QString( "2484" ) );
    QVERIFY( mTranslator->translate( QString( "Öljy" ) ) == QString( "6559" ) );
    QVERIFY( mTranslator->translate( QString( "Adam" ) ) == QString( "2326" ) );
    QVERIFY( mTranslator->translate( QString( "Bertil" ) ) == QString( "237845" ) );
    QVERIFY( mTranslator->translate( QString( "Cesar" ) ) == QString( "23727" ) );
    QVERIFY( mTranslator->translate( QString( "David" ) ) == QString( "32843" ) );
    QVERIFY( mTranslator->translate( QString( "Erik" ) ) == QString( "3745" ) );
    QVERIFY( mTranslator->translate( QString( "Filip" ) ) == QString( "34547" ) );
    QVERIFY( mTranslator->translate( QString( "Gustav" ) ) == QString( "487828" ) );
    QVERIFY( mTranslator->translate( QString( "Helge" ) ) == QString( "43543" ) );
    QVERIFY( mTranslator->translate( QString( "Ivar" ) ) == QString( "4827" ) );
    QVERIFY( mTranslator->translate( QString( "Johan" ) ) == QString( "56426" ) );
    QVERIFY( mTranslator->translate( QString( "Kalle" ) ) == QString( "52553" ) );
    QVERIFY( mTranslator->translate( QString( "Ludvig" ) ) == QString( "583844" ) );
    QVERIFY( mTranslator->translate( QString( "Martin" ) ) == QString( "627846" ) );
    QVERIFY( mTranslator->translate( QString( "Niklas" ) ) == QString( "645527" ) );
    QVERIFY( mTranslator->translate( QString( "Olof" ) ) == QString( "6563" ) );
    QVERIFY( mTranslator->translate( QString( "Petter" ) ) == QString( "738837" ) );
    QVERIFY( mTranslator->translate( QString( "Quintus" ) ) == QString( "7846887" ) );
    QVERIFY( mTranslator->translate( QString( "Rudolf" ) ) == QString( "783653" ) );
    QVERIFY( mTranslator->translate( QString( "Sigurd" ) ) == QString( "744873" ) );
    QVERIFY( mTranslator->translate( QString( "Tore" ) ) == QString( "8673" ) );
    QVERIFY( mTranslator->translate( QString( "Urban" ) ) == QString( "87226" ) );
    QVERIFY( mTranslator->translate( QString( "Viktor" ) ) == QString( "845867" ) );
    QVERIFY( mTranslator->translate( QString( "Wilhelm" ) ) == QString( "9454356" ) );
    QVERIFY( mTranslator->translate( QString( "Xerxes" ) ) == QString( "937937" ) );
    QVERIFY( mTranslator->translate( QString( "Yngve" ) ) == QString( "96483" ) );
    QVERIFY( mTranslator->translate( QString( "Zäta" ) ) == QString( "9282" ) );
    QVERIFY( mTranslator->translate( QString( "Ärlig" ) ) == QString( "27544" ) );
    QVERIFY( mTranslator->translate( QString( "Östen" ) ) == QString( "67836" ) );
    QVERIFY( mTranslator->translate( QString( "Alpha" ) ) == QString( "25742" ) );
    QVERIFY( mTranslator->translate( QString( "Bravo" ) ) == QString( "27286" ) );
    QVERIFY( mTranslator->translate( QString( "Charlie" ) ) == QString( "2427543" ) );
    QVERIFY( mTranslator->translate( QString( "Delta" ) ) == QString( "33582" ) );
    QVERIFY( mTranslator->translate( QString( "Echo" ) ) == QString( "3246" ) );
    QVERIFY( mTranslator->translate( QString( "Foxtrot" ) ) == QString( "3698768" ) );
    QVERIFY( mTranslator->translate( QString( "Golf" ) ) == QString( "4653" ) );
    QVERIFY( mTranslator->translate( QString( "Hotel" ) ) == QString( "46835" ) );
    QVERIFY( mTranslator->translate( QString( "India" ) ) == QString( "46342" ) );
    QVERIFY( mTranslator->translate( QString( "Juliet" ) ) == QString( "585438" ) );
    QVERIFY( mTranslator->translate( QString( "Kilo" ) ) == QString( "5456" ) );
    QVERIFY( mTranslator->translate( QString( "Lima" ) ) == QString( "5462" ) );
    QVERIFY( mTranslator->translate( QString( "Mike" ) ) == QString( "6453" ) );
    QVERIFY( mTranslator->translate( QString( "November" ) ) == QString( "66836237" ) );
    QVERIFY( mTranslator->translate( QString( "Oscar" ) ) == QString( "67227" ) );
    QVERIFY( mTranslator->translate( QString( "Papa" ) ) == QString( "7272" ) );
    QVERIFY( mTranslator->translate( QString( "Quebec" ) ) == QString( "783232" ) );
    QVERIFY( mTranslator->translate( QString( "Romeo" ) ) == QString( "76636" ) );
    QVERIFY( mTranslator->translate( QString( "Sierra" ) ) == QString( "743772" ) );
    QVERIFY( mTranslator->translate( QString( "Tanga" ) ) == QString( "82642" ) );
    QVERIFY( mTranslator->translate( QString( "Uniform" ) ) == QString( "8643676" ) );
    QVERIFY( mTranslator->translate( QString( "Vârlan" ) ) == QString( "827526" ) );
    QVERIFY( mTranslator->translate( QString( "Victor" ) ) == QString( "842867" ) );
    QVERIFY( mTranslator->translate( QString( "Whiskey" ) ) == QString( "9447539" ) );
    QVERIFY( mTranslator->translate( QString( "Xray" ) ) == QString( "9729" ) );
    QVERIFY( mTranslator->translate( QString( "Yankee" ) ) == QString( "926533" ) );
    QVERIFY( mTranslator->translate( QString( "Zulu" ) ) == QString( "9858" ) );
    QVERIFY( mTranslator->translate( QString( "1234567890" ) ) == QString( "1234567890" ) );
    
    QString uni;
    uni.append(QChar(0x0219));
    uni.append(QChar(0x4E0F));
    QString result("7");
    result.append(QChar(0x4E0F));
    QCOMPARE(mTranslator->translate(uni), result);
}

void UT_LogsPredictiveTranslator::testTranslateChar()
{
    // Romanian unicode character "sh"
    QChar rom(0x0219);
    QCOMPARE( mTranslator->translate( rom ), QChar('7') );
    
    // Chinese unicode character
    QChar chn(0x4E0F);
    QCOMPARE( mTranslator->translate( chn ), chn );
}


void UT_LogsPredictiveTranslator::testStartsWith()
{
    QVERIFY( mTranslator->startsWith( QString(""), QString("") ) == 0 );
    QVERIFY( mTranslator->startsWith( QString(""), QString("12") ) == 0 );
    QVERIFY( mTranslator->startsWith( QString("Nokia"), QString("") ) == 0 );
    QVERIFY( mTranslator->startsWith( QString("Nokia"), QString("6") ) == 1 );
    QVERIFY( mTranslator->startsWith( QString("Nokia"), QString("66") ) == 2 );
    QVERIFY( mTranslator->startsWith( QString("Nokia"), QString("665") ) == 3 );
    QVERIFY( mTranslator->startsWith( QString("Nokia"), QString("6654") ) == 4 );
    QVERIFY( mTranslator->startsWith( QString("Nokia"), QString("66542") ) == 5 );
    QVERIFY( mTranslator->startsWith( QString("Nokia"), QString("665423") ) == 0 );
    
    
}


