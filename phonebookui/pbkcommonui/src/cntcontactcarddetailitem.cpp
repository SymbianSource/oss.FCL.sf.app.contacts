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

#include "cntcontactcarddetailitem.h"
#include "cntcontactcarddataitem.h"
#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbrichtextitem.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hbeffect.h>
#include <hbgesturefilter.h>
#include <hbgesture.h>
#include <hbstyleloader.h>
#include <QGraphicsSceneMouseEvent>
#include <hbinstantfeedback.h>

CntContactCardDetailItem::CntContactCardDetailItem(int index, QGraphicsItem *parent, bool isFocusable) :
    HbWidget(parent),
    mIcon(NULL),
    mFirstLineText(NULL),
    mSecondLineText(NULL),
    mFrameItem(NULL),
    mFocusItem(NULL),
    mHasFocus(false),
    mGestureFilter(NULL),
    mGestureLongpressed(NULL),
    mIndex(index),
    mIsFocusable(isFocusable),
    mValueTextElideMode(Qt::ElideRight),
    mIsUnderline(false)
{  
    if (mIsFocusable)
    {
        HbEffect::add(QString("frameitem"), QString(":/xml/edit_button_pressed.fxml"), QString("pressed"));
        HbEffect::add(QString("frameitem"), QString(":/xml/edit_button_released.fxml"), QString("released"));
        initGesture();
    }
}

CntContactCardDetailItem::~CntContactCardDetailItem()
{

}

void CntContactCardDetailItem::createPrimitives()
{
    if (!icon.isNull())
    {
        if (!mIcon)
        {
            mIcon = new HbIconItem(this);
            mIcon->setIcon(icon);
            style()->setItemName(mIcon, "icon");
        }
    }
    else
    {
        if (mIcon)
        {
            delete mIcon;
        }
        mIcon = 0;
    }

    if (!text.isNull())
    {
        mFirstLineText = new HbTextItem(this);
        mFirstLineText->setText(text);        
        mFirstLineText->setTextWrapping(Hb::TextWordWrap);
        style()->setItemName(mFirstLineText, "text");    
    }
    else
    {
        if (mFirstLineText)
        {
            delete mFirstLineText;
        }
        mFirstLineText = 0;
    }
    
    if (!valueText.isNull())
    {
        if (!mSecondLineText)
        {
            mSecondLineText = new HbTextItem(this);
            mSecondLineText->setText(valueText);
            mSecondLineText->setElideMode(mValueTextElideMode);
            style()->setItemName(mSecondLineText, "valueText");
        }
    }
    else
    {
        if (mSecondLineText)
        {
            delete mSecondLineText;
        }
        mSecondLineText = 0;
    }

    if (!mFrameItem)
    {
        mFrameItem = new HbFrameItem(this);
        mFrameItem->frameDrawer().setFrameGraphicsName("qtg_fr_list_normal");
        mFrameItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        mFrameItem->setZValue(-2);
        style()->setItemName(mFrameItem, "background");
    }

    if (!mFocusItem && mIsFocusable)
    {
        mFocusItem = new HbFrameItem(this);
        mFocusItem->frameDrawer().setFrameGraphicsName("qtg_fr_list_pressed");
        mFocusItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        mFocusItem->setZValue(-1);
        mFocusItem->setVisible(false);
        style()->setItemName(mFocusItem, "highlight");
    }
}

void CntContactCardDetailItem::recreatePrimitives()
{
    HbWidget::recreatePrimitives();

    delete mIcon;
    mIcon = 0;

    delete mFirstLineText;
    mFirstLineText = 0;

    delete mSecondLineText;
    mSecondLineText = 0;

    delete mFrameItem;
    mFrameItem = 0;

    delete mFocusItem;
    mFocusItem = 0;

    createPrimitives();
}

void CntContactCardDetailItem::initGesture()
{
    mGestureFilter = new HbGestureSceneFilter(Qt::LeftButton, this);
    mGestureLongpressed = new HbGesture(HbGesture::longpress, 5);
    mGestureFilter->addGesture(mGestureLongpressed);
    mGestureFilter->setLongpressAnimation(true);
    installSceneEventFilter(mGestureFilter);

    connect(mGestureLongpressed, SIGNAL(longPress(QPointF)), this, SLOT(onLongPress(QPointF)));
}

void CntContactCardDetailItem::onLongPress(const QPointF &point)
{
    mHasFocus = false;
    updatePrimitives();
    emit longPressed(point);
}

void CntContactCardDetailItem::updatePrimitives()
{
    HbWidget::updatePrimitives();
    if (mHasFocus && mIsFocusable)
    {
        mFocusItem->setVisible(true);
        HbEffect::start(mFocusItem, QString("frameitem"), QString("pressed"));
    }
    else
    {
        HbEffect::start(mFocusItem, QString("frameitem"), QString("released"));
    }
}

void CntContactCardDetailItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (mIsFocusable)
    {
        HbInstantFeedback::play(HbFeedback::Basic);
    }
    event->accept();
    mHasFocus = true;
    updatePrimitives();
}

void CntContactCardDetailItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!rect().contains(event->pos()) && mHasFocus)
    {
        mHasFocus = false;
        updatePrimitives();
    }
    event->accept();
}


void CntContactCardDetailItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    bool hadFocus = mHasFocus;
    mHasFocus = false;
    if (rect().contains(event->pos()) && hadFocus)
    {
        updatePrimitives();
        emit clicked();
    }
    event->accept();
}

void CntContactCardDetailItem::setDetails(CntContactCardDataItem* aDataItem)
{
    text.clear();
    valueText.clear();
    icon.clear();
    mValueTextElideMode = aDataItem->elideMode();

    if (!aDataItem->icon().isNull())
        icon = aDataItem->icon();
    if (!aDataItem->titleText().isEmpty())
        text = aDataItem->titleText();
    if (!aDataItem->valueText().isEmpty())
        valueText = aDataItem->valueText();

    recreatePrimitives();
}

int CntContactCardDetailItem::index()
{
    return mIndex;
}

