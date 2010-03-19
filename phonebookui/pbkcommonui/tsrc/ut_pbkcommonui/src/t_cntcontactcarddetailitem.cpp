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

#include "cntcontactcarddetailitem.h"
#include "t_cntcontactcarddetailitem.h"


void TestCntContactCardDetailItem::initTestCase()
{
    mDetailItem = 0;
}

void TestCntContactCardDetailItem::create()
{
    mDetailItem = new CntContactCardDetailItem(0);
    QVERIFY(mDetailItem);
    QVERIFY(mDetailItem->mGestureFilter);
    QVERIFY(mDetailItem->mGestureLongpressed);
}

void TestCntContactCardDetailItem::createPrimitives()
{
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    // no given data
    mDetailItem->createPrimitives();
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(!mDetailItem->mFirstLineText);
    QVERIFY(!mDetailItem->mPrimaryText);
    QVERIFY(!mDetailItem->mSecondLineText);
    QVERIFY(mDetailItem->mFrameItem);
    QVERIFY(mDetailItem->mFocusItem);
    
    // all data given
    mDetailItem->text = "text";
    mDetailItem->primaryText = "primaryText";
    mDetailItem->valueText = "valueText";
    mDetailItem->icon = HbIcon(":/icons/qtg_large_avatar.svg");
    
    mDetailItem->createPrimitives();
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mFirstLineText);
    QVERIFY(mDetailItem->mPrimaryText);
    QVERIFY(mDetailItem->mSecondLineText);
    QVERIFY(mDetailItem->mFrameItem);
    QVERIFY(mDetailItem->mFocusItem);
    
    // calling this a second time shouldn't have any effect
    mDetailItem->createPrimitives();
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mFirstLineText);
    QVERIFY(mDetailItem->mPrimaryText);
    QVERIFY(mDetailItem->mSecondLineText);
    QVERIFY(mDetailItem->mFrameItem);
    QVERIFY(mDetailItem->mFocusItem);
    
    // remove all data
    mDetailItem->text.clear();
    mDetailItem->primaryText.clear();
    mDetailItem->valueText.clear();
    mDetailItem->icon.clear();
    
    mDetailItem->createPrimitives();
    QVERIFY(!mDetailItem->mIcon);
    QVERIFY(!mDetailItem->mFirstLineText);
    QVERIFY(!mDetailItem->mPrimaryText);
    QVERIFY(!mDetailItem->mSecondLineText);
    QVERIFY(mDetailItem->mFrameItem);
    QVERIFY(mDetailItem->mFocusItem);
}

void TestCntContactCardDetailItem::recreatePrimitives()
{
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    
    mDetailItem->text = "text";
    mDetailItem->primaryText = "primaryText";
    mDetailItem->valueText = "valueText";
    mDetailItem->icon = HbIcon(":/icons/qtg_large_avatar.svg");
    
    mDetailItem->createPrimitives();
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mFirstLineText);
    QVERIFY(mDetailItem->mPrimaryText);
    QVERIFY(mDetailItem->mSecondLineText);
    QVERIFY(mDetailItem->mFrameItem);
    QVERIFY(mDetailItem->mFocusItem);
    
    mDetailItem->recreatePrimitives();
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mFirstLineText);
    QVERIFY(mDetailItem->mPrimaryText);
    QVERIFY(mDetailItem->mSecondLineText);
    QVERIFY(mDetailItem->mFrameItem);
    QVERIFY(mDetailItem->mFocusItem);
}

void TestCntContactCardDetailItem::updatePrimitives()
{
    mDetailItem->updatePrimitives();
    QVERIFY(!mDetailItem->mFocusItem->isVisible());
    mDetailItem->mHasFocus = true;
    mDetailItem->updatePrimitives();
    QVERIFY(mDetailItem->mFocusItem->isVisible());
}


void TestCntContactCardDetailItem::onLongPress()
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

void TestCntContactCardDetailItem::setDetails()
{
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    mDetailItem->setDetails(HbIcon(":/icons/qtg_large_avatar.svg"), "text", "valueText");
    
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(mDetailItem->mFirstLineText);
    QVERIFY(!mDetailItem->mPrimaryText);
    QVERIFY(mDetailItem->mSecondLineText);
    QVERIFY(mDetailItem->mValueTextElideMode == Qt::ElideRight);
    
    // remove all data
    mDetailItem->text.clear();
    mDetailItem->primaryText.clear();
    mDetailItem->valueText.clear();
    mDetailItem->icon.clear();
    
    mDetailItem->setDetails(HbIcon(":/icons/qtg_large_avatar.svg"), "primaryText");
    QVERIFY(mDetailItem->mIcon);
    QVERIFY(!mDetailItem->mFirstLineText);
    QVERIFY(mDetailItem->mPrimaryText);
    QVERIFY(!mDetailItem->mSecondLineText);
}

void TestCntContactCardDetailItem::initGesture()
{
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    mDetailItem->initGesture();
    QVERIFY(mDetailItem->mGestureFilter);
    QVERIFY(mDetailItem->mGestureLongpressed); 
}

void TestCntContactCardDetailItem::index()
{
    delete mDetailItem;
    mDetailItem = 0;
    
    create();
    QVERIFY(mDetailItem->index() == 0);
}

void TestCntContactCardDetailItem::cleanupTestCase()
{
    delete mDetailItem;
}

// EOF
