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

#include "cnteditviewheadingitem.h"
#include "t_cnteditviewheadingitem.h"


void TestCntEditViewHeadingItem::initTestCase()
{
    mHeadingItem = 0;
}

void TestCntEditViewHeadingItem::create()
{
    mHeadingItem = new CntEditViewHeadingItem();
    QVERIFY(mHeadingItem);
}

void TestCntEditViewHeadingItem::createPrimitives()
{
    delete mHeadingItem;
    mHeadingItem = 0;
    
    create();
    
    // no given data
    mHeadingItem->createPrimitives();
    QVERIFY(!mHeadingItem->mIcon);
    QVERIFY(!mHeadingItem->mLabel);
    QVERIFY(!mHeadingItem->mSecondLabel);
    QVERIFY(mHeadingItem->mFrameItem);
    QVERIFY(mHeadingItem->mIconTouchArea);
    
    // all data given
    mHeadingItem->text = "text";
    mHeadingItem->second_text = "secondText";
    mHeadingItem->icon = HbIcon(":/icons/qtg_large_avatar.svg");
    
    mHeadingItem->createPrimitives();
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(mHeadingItem->mLabel);
    QVERIFY(mHeadingItem->mSecondLabel);
    QVERIFY(mHeadingItem->mFrameItem);
    QVERIFY(mHeadingItem->mIconTouchArea);
    
    // calling this a second time shouldn't have any effect
    mHeadingItem->createPrimitives();
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(mHeadingItem->mLabel);
    QVERIFY(mHeadingItem->mSecondLabel);
    QVERIFY(mHeadingItem->mFrameItem);
    QVERIFY(mHeadingItem->mIconTouchArea);
    
    // remove all data
    mHeadingItem->text.clear();
    mHeadingItem->second_text.clear();
    mHeadingItem->icon.clear();
    
    mHeadingItem->createPrimitives();
    QVERIFY(!mHeadingItem->mIcon);
    QVERIFY(!mHeadingItem->mLabel);
    QVERIFY(!mHeadingItem->mSecondLabel);
    QVERIFY(mHeadingItem->mFrameItem);
    QVERIFY(mHeadingItem->mIconTouchArea);
}

void TestCntEditViewHeadingItem::recreatePrimitives()
{
    delete mHeadingItem;
    mHeadingItem = 0;
    
    create();
    
    mHeadingItem->text = "text";
    mHeadingItem->second_text = "secondText";
    mHeadingItem->icon = HbIcon(":/icons/qtg_large_avatar.svg");
    
    mHeadingItem->createPrimitives();
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(mHeadingItem->mLabel);
    QVERIFY(mHeadingItem->mSecondLabel);
    QVERIFY(mHeadingItem->mFrameItem);
    QVERIFY(mHeadingItem->mIconTouchArea);
    
    mHeadingItem->recreatePrimitives();
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(mHeadingItem->mLabel);
    QVERIFY(mHeadingItem->mSecondLabel);
    QVERIFY(mHeadingItem->mFrameItem);
    QVERIFY(mHeadingItem->mIconTouchArea);
    
}

void TestCntEditViewHeadingItem::updatePrimitives()
{
    mHeadingItem->updatePrimitives(); // NOP
}

void TestCntEditViewHeadingItem::setDetails()
{
    delete mHeadingItem;
    mHeadingItem = 0;
    
    create();
    
    // empty contact
    QContact contact;
    
    mHeadingItem->setDetails(&contact);
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(mHeadingItem->mLabel);
    QVERIFY(!mHeadingItem->mSecondLabel);
    QVERIFY(mHeadingItem->mFrameItem);
    QVERIFY(mHeadingItem->mIconTouchArea);
    
    // nickname saved
    QContactNickname nick;
    nick.setNickname("nick");
    contact.saveDetail(&nick);
    
    mHeadingItem->setDetails(&contact);
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(!mHeadingItem->mLabel);
    QVERIFY(mHeadingItem->mSecondLabel);
    QVERIFY(mHeadingItem->mFrameItem);
    QVERIFY(mHeadingItem->mIconTouchArea);
    
    // nickname and full name saved
    QContactName name;
    name.setPrefix("prefix");
    name.setFirst("first");
    name.setMiddle("middle");
    name.setLast("last");
    name.setSuffix("suffix");
    contact.saveDetail(&name);
    
    mHeadingItem->setDetails(&contact);
    QVERIFY(mHeadingItem->mIcon);
    QVERIFY(mHeadingItem->mLabel);
    QVERIFY(mHeadingItem->mSecondLabel);
    QVERIFY(mHeadingItem->mFrameItem);
    QVERIFY(mHeadingItem->mIconTouchArea);
}

void TestCntEditViewHeadingItem::setIcon()
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

void TestCntEditViewHeadingItem::cleanupTestCase()
{
    delete mHeadingItem;
}

// EOF
