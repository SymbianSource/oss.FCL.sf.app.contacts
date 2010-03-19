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

#include <hbicon.h>

#include "cntcontactcardheadingitem.h"
#include "t_cntcontactcardheadingitem.h"


void TestCntContactCardHeadingItem::initTestCase()
{
    mHeadingItem = 0;
}

void TestCntContactCardHeadingItem::create()
{
    mHeadingItem = new CntContactCardHeadingItem();
    QVERIFY(mHeadingItem);
}

void TestCntContactCardHeadingItem::createPrimitives()
{
    delete mHeadingItem;
    mHeadingItem = 0;
    
    create();
    
    // no given data
    mHeadingItem->createPrimitives();
    QVERIFY(!mHeadingItem->mIcon);
    QVERIFY(!mHeadingItem->mFirstLineText);
    QVERIFY(!mHeadingItem->mPrimaryText);
    QVERIFY(!mHeadingItem->mSecondLineText);
    QVERIFY(!mHeadingItem->mSecondaryText);
    QVERIFY(!mHeadingItem->mMarqueeItem);
    QVERIFY(mHeadingItem->mFrameItem);
    
    // all data given
    mHeadingItem->firstLineText = "firstLineText";
    mHeadingItem->primaryText = "primaryText";
    mHeadingItem->secondLineText = "secondLineText";
    mHeadingItem->secondaryText = "secondaryText";
    mHeadingItem->tinyMarqueeText = "tinyMarqueeText";
    mHeadingItem->icon = HbIcon(":/icons/qtg_large_avatar.svg");
    
    mHeadingItem->createPrimitives();
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(mHeadingItem->mFirstLineText);
    QVERIFY(mHeadingItem->mPrimaryText);
    QVERIFY(mHeadingItem->mSecondLineText);
    QVERIFY(mHeadingItem->mSecondaryText);
    QVERIFY(mHeadingItem->mMarqueeItem);
    
    // calling this a second time shouldn't have any effect
    mHeadingItem->createPrimitives();
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(mHeadingItem->mFirstLineText);
    QVERIFY(mHeadingItem->mPrimaryText);
    QVERIFY(mHeadingItem->mSecondLineText);
    QVERIFY(mHeadingItem->mSecondaryText);
    QVERIFY(mHeadingItem->mMarqueeItem);
    
    // remove all data
    mHeadingItem->firstLineText.clear();
    mHeadingItem->primaryText.clear();
    mHeadingItem->secondLineText.clear();
    mHeadingItem->secondaryText.clear();
    mHeadingItem->tinyMarqueeText.clear();
    mHeadingItem->icon.clear();
    
    mHeadingItem->createPrimitives();
    QVERIFY(!mHeadingItem->mIcon);
    QVERIFY(!mHeadingItem->mFirstLineText);
    QVERIFY(!mHeadingItem->mPrimaryText);
    QVERIFY(!mHeadingItem->mSecondLineText);
    QVERIFY(!mHeadingItem->mSecondaryText);
    QVERIFY(!mHeadingItem->mMarqueeItem);
    QVERIFY(mHeadingItem->mFrameItem);
}

void TestCntContactCardHeadingItem::recreatePrimitives()
{
    delete mHeadingItem;
    mHeadingItem = 0;
    
    create();
    
    mHeadingItem->firstLineText = "firstLineText";
    mHeadingItem->primaryText = "primaryText";
    mHeadingItem->secondLineText = "secondLineText";
    mHeadingItem->secondaryText = "secondaryText";
    mHeadingItem->tinyMarqueeText = "tinyMarqueeText";
    mHeadingItem->icon = HbIcon(":/icons/qtg_large_avatar.svg");
    
    mHeadingItem->createPrimitives();
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(mHeadingItem->mFirstLineText);
    QVERIFY(mHeadingItem->mPrimaryText);
    QVERIFY(mHeadingItem->mSecondLineText);
    QVERIFY(mHeadingItem->mSecondaryText);
    QVERIFY(mHeadingItem->mMarqueeItem);
    
    mHeadingItem->recreatePrimitives();
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(mHeadingItem->mFirstLineText);
    QVERIFY(mHeadingItem->mPrimaryText);
    QVERIFY(mHeadingItem->mSecondLineText);
    QVERIFY(mHeadingItem->mSecondaryText);
    QVERIFY(mHeadingItem->mMarqueeItem);
    
}

void TestCntContactCardHeadingItem::isNickName()
{
    // empty contact
    QContact contact;
    QVERIFY(mHeadingItem->isNickName(&contact) == false);
    // nickname saved
    QContactNickname nick;
    nick.setNickname("nick");
    contact.saveDetail(&nick);
    QVERIFY(mHeadingItem->isNickName(&contact) == true);
    
}

void TestCntContactCardHeadingItem::isCompanyName()
{
    // empty contact
    QContact contact;
    QVERIFY(mHeadingItem->isCompanyName(&contact) == false);
    // title
    QContactOrganization organization;
    organization.setTitle("title");
    contact.saveDetail(&organization);
    QVERIFY(mHeadingItem->isCompanyName(&contact) == true);
    organization.setName("name");
    contact.saveDetail(&organization);
    QVERIFY(mHeadingItem->isCompanyName(&contact) == true);  
}

void TestCntContactCardHeadingItem::updatePrimitives()
{
    mHeadingItem->updatePrimitives(); // NOP
}

void TestCntContactCardHeadingItem::setDetails()
{
    delete mHeadingItem;
    mHeadingItem = 0;
    
    create();
    
    // empty contact
    QContact contact;
    
    mHeadingItem->setDetails(&contact);
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(!mHeadingItem->mFirstLineText);
    QVERIFY(mHeadingItem->mPrimaryText);
    QVERIFY(!mHeadingItem->mSecondLineText);
    QVERIFY(!mHeadingItem->mSecondaryText);
    QVERIFY(!mHeadingItem->mMarqueeItem);
    QVERIFY(mHeadingItem->mFrameItem);
    
    // nickname saved
    QContactNickname nick;
    nick.setNickname("nick");
    contact.saveDetail(&nick);
    
    mHeadingItem->setDetails(&contact);
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(mHeadingItem->mFirstLineText);
    QVERIFY(!mHeadingItem->mPrimaryText);
    QVERIFY(!mHeadingItem->mSecondLineText);
    QVERIFY(mHeadingItem->mSecondaryText);
    QVERIFY(!mHeadingItem->mMarqueeItem);
    QVERIFY(mHeadingItem->mFrameItem);
    
    // nickname and organization
    QContactOrganization organization;
    organization.setTitle("title");
    organization.setName("name");
    contact.saveDetail(&organization);
    
    mHeadingItem->setDetails(&contact);
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(mHeadingItem->mFirstLineText);
    QVERIFY(!mHeadingItem->mPrimaryText);
    QVERIFY(!mHeadingItem->mSecondLineText);
    QVERIFY(mHeadingItem->mSecondaryText);
    QVERIFY(mHeadingItem->mMarqueeItem);
    QVERIFY(mHeadingItem->mFrameItem);
}

void TestCntContactCardHeadingItem::setIcon()
{
    delete mHeadingItem;
    mHeadingItem = 0;
    
    create();
    
    mHeadingItem->icon = HbIcon(":/icons/qtg_large_avatar.svg");
    mHeadingItem->setIcon(HbIcon(":/icons/qtg_large_avatar.svg"));
    QVERIFY(!mHeadingItem->mIcon);
    
    mHeadingItem->setIcon(HbIcon(":/icons/qtg_large_custom.svg"));
    QVERIFY(mHeadingItem->mIcon);
}

void TestCntContactCardHeadingItem::cleanupTestCase()
{
    delete mHeadingItem;
}

// EOF
