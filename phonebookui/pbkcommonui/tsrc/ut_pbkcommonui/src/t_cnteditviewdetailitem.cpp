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
#include <QSignalSpy>

#include <hbicon.h>
#include <hbframeitem.h>

#include "cnteditviewdetailitem.h"
#include "t_cnteditviewdetailitem.h"


void TestCntEditViewDetailItem::initTestCase()
{
    mDetailItem = 0;
}

void TestCntEditViewDetailItem::create()
{
    mDetailItem = new CntEditViewDetailItem();
    QVERIFY(mDetailItem);
    QVERIFY(mDetailItem->mGestureFilter);
    QVERIFY(mDetailItem->mGestureLongpressed);
}

void TestCntEditViewDetailItem::createPrimitives()
{
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    // no given data
    mDetailItem->createPrimitives();
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(!mDetailItem->mLabel);
    QVERIFY(!mDetailItem->mContent);
    QVERIFY(mDetailItem->mFrameItem);
    QVERIFY(mDetailItem->mFocusItem);
    
    // all data given
    mDetailItem->text = "text";
    mDetailItem->valueText = "valueText";
    mDetailItem->icon = HbIcon(":/icons/qtg_large_avatar.svg");
    
    mDetailItem->createPrimitives();
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    QVERIFY(mDetailItem->mFrameItem);
    QVERIFY(mDetailItem->mFocusItem);
    
    // calling this a second time shouldn't have any effect
    mDetailItem->createPrimitives();
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    QVERIFY(mDetailItem->mFrameItem);
    QVERIFY(mDetailItem->mFocusItem);
    
    // remove all data
    mDetailItem->text.clear();
    mDetailItem->valueText.clear();
    mDetailItem->icon.clear();
    
    mDetailItem->createPrimitives();
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(!mDetailItem->mLabel);
    QVERIFY(!mDetailItem->mContent);
    QVERIFY(mDetailItem->mFrameItem);
    QVERIFY(mDetailItem->mFocusItem);
}

void TestCntEditViewDetailItem::recreatePrimitives()
{
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    mDetailItem->text = "text";
    mDetailItem->valueText = "valueText";
    mDetailItem->icon = HbIcon(":/icons/qtg_large_avatar.svg");
    
    mDetailItem->createPrimitives();
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    QVERIFY(mDetailItem->mFrameItem);
    QVERIFY(mDetailItem->mFocusItem);
    
    mDetailItem->recreatePrimitives();
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    QVERIFY(mDetailItem->mFrameItem);
    QVERIFY(mDetailItem->mFocusItem);
}

void TestCntEditViewDetailItem::updatePrimitives()
{
    mDetailItem->updatePrimitives();
    QVERIFY(!mDetailItem->mFocusItem->isVisible());
    mDetailItem->mHasFocus = true;
    mDetailItem->updatePrimitives();
    QVERIFY(mDetailItem->mFocusItem->isVisible());
}

void TestCntEditViewDetailItem::setDetail()
{
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    // mobile home & special update case
    QContactPhoneNumber number;
    
    mDetailItem->setDetail(number);
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(!mDetailItem->mContent);
    
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setContexts(QContactDetail::ContextHome);
    number.setNumber("123");
    mDetailItem->setDetail(number);
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    mDetailItem->setDetail(number);
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    // email address
    QContactEmailAddress email;
    
    mDetailItem->setDetail(email);
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(!mDetailItem->mContent);
    
    email.setEmailAddress("email@email.fi");
    mDetailItem->setDetail(email);
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    // postal address
    QContactAddress address;
    
    mDetailItem->setDetail(address);
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(!mDetailItem->mContent);
    
    address.setStreet("street");
    mDetailItem->setDetail(address);
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    address.setStreet(QString());
    address.setPostcode("postcode");
    address.setPostOfficeBox("123");
    address.setLocality("locality");
    address.setRegion("region");
    address.setCountry("country");
    mDetailItem->setDetail(address);
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    // online account
    QContactOnlineAccount account;
    mDetailItem->setDetail(account);
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(!mDetailItem->mContent);
    
    account.setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
    account.setAccountUri("accounturi");
    mDetailItem->setDetail(account);
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    // url
    QContactUrl url;
    mDetailItem->setDetail(url);
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(!mDetailItem->mContent);
    
    url.setUrl("http://www.nokia.com");
    mDetailItem->setDetail(url);
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    // company details
    QContactOrganization org;
    QStringList departments;
    departments << "department";
    org.setDepartment(departments);
    mDetailItem->setDetail(org, QContactOrganization::DefinitionName);
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    org.setDepartment(QStringList());
    org.setTitle("title");
    org.setName("name");
    mDetailItem->setDetail(org, QContactOrganization::DefinitionName);
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    org.setAssistantName("assistant");
    mDetailItem->setDetail(org, QContactOrganization::FieldAssistantName);
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    // birthday
    QContactBirthday bDay;
    bDay.setDate(QDate::currentDate());
    mDetailItem->setDetail(bDay);
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    // anniversary
    QContactAnniversary anniversary;
    anniversary.setOriginalDate(QDate::currentDate());
    mDetailItem->setDetail(anniversary);
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    // ringing tone
    QContactAvatar avatar;
    avatar.setAvatar("path");
    avatar.setSubType(QContactAvatar::SubTypeAudioRingtone);
    mDetailItem->setDetail(avatar);
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    // note
    QContactNote note;
    note.setNote("note");
    mDetailItem->setDetail(note);
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    // family details
    QContactFamily family;
    family.setSpouse("spouse");
    QStringList children;
    children << "child";
    family.setChildren(children);
    
    mDetailItem->setDetail(family, QContactFamily::FieldSpouse);
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
    
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    mDetailItem->setDetail(family, QContactFamily::FieldChildren);
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(mDetailItem->mLabel);
    QVERIFY(mDetailItem->mContent);
}

void TestCntEditViewDetailItem::detail()
{
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    QContactPhoneNumber number;
    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    number.setNumber("123");
    mDetailItem->setDetail(number);
    
    QVERIFY(mDetailItem->detail().definitionName() == QContactPhoneNumber::DefinitionName);
    QVERIFY(static_cast<QContactPhoneNumber>(mDetailItem->detail()).number() == "123");
}

void TestCntEditViewDetailItem::fieldType()
{
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    QContactDetail detail;
    mDetailItem->setDetail(detail, "dummytype");
    
    QVERIFY(mDetailItem->fieldType() == "dummytype");  
}

void TestCntEditViewDetailItem::onLongPress()
{
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    QSignalSpy spy(mDetailItem, SIGNAL(longPressed(const QPointF&)));
    
    mDetailItem->mHasFocus = true;
    mDetailItem->onLongPress(QPointF());
    QCOMPARE(spy.count(), 1);
    QVERIFY(!mDetailItem->mHasFocus);
}

void TestCntEditViewDetailItem::cleanupTestCase()
{
    delete mDetailItem;
}

// EOF
