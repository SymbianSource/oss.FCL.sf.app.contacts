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

#include <QGraphicsLayout>
#include <QDebug>

#include "cntcontactcarddetailitem.h"
#include "cntcontactcarddataitem.h"
#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbrichtextitem.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hbeffect.h>
#include <hbstyleloader.h>
#include <QGraphicsSceneMouseEvent>
#include <hbinstantfeedback.h>
#include <hbcolorscheme.h>
#include <QGestureEvent>
#include <QTapGesture>
#include <hbtoucharea.h>


CntContactCardDetailItem::CntContactCardDetailItem(int index, QGraphicsItem *parent, bool isFocusable) :
    HbWidget(parent),
    mIcon(NULL),
    mSecondaryIcon(NULL),
    mFirstLineText(NULL),
    mSecondLineText(NULL),
    mFrameItem(NULL),
    mFocusItem(NULL),
    mHasFocus(false),
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
            style()->setItemName(mIcon, "icon");
        }
        mIcon->setIcon(icon);
    }
    else
    {
        if (mIcon)
        {
            delete mIcon;
        }
        mIcon = 0;
    }

    if (!mSecondaryIcon)
    {
        mSecondaryIcon = new HbIconItem(this);
        mSecondaryIcon->setFlags(HbIcon::Colorized);
        style()->setItemName(mSecondaryIcon, "secondaryIcon");
    }
    mSecondaryIcon->setIcon(secondaryIcon);
    mSecondaryIcon->setColor(HbColorScheme::color("foreground"));

    if (!text.isNull())
    {
        if (!mFirstLineText)
        {
            mFirstLineText = new HbTextItem(this);
            mFirstLineText->setTextWrapping(Hb::TextWordWrap);
            style()->setItemName(mFirstLineText, "text");    
        }
        mFirstLineText->setText(text);        
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
            mSecondLineText->setElideMode(mValueTextElideMode);
            style()->setItemName(mSecondLineText, "valueText");
        }
        mSecondLineText->setText(valueText);
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
   
    updatePrimitives();
    updateGeometry();
    repolish();
}

void CntContactCardDetailItem::recreatePrimitives()
{
    HbWidget::recreatePrimitives();
   
    createPrimitives();
}

void CntContactCardDetailItem::initGesture()
{
    grabGesture(Qt::TapAndHoldGesture, Qt::ReceivePartialGestures);
    grabGesture(Qt::TapGesture, Qt::ReceivePartialGestures);
}

void CntContactCardDetailItem::gestureEvent(QGestureEvent* event)
{
    qDebug() << "CntContactCardDetailItem::gestureEvent - IN";
    event->accept( Qt::TapAndHoldGesture );
    
    if (QGesture *tap = event->gesture(Qt::TapGesture))
    {
        tapTriggered(static_cast<QTapGesture *>(tap));
        event->accept( tap );
    }
    
    if (QGesture *tapAndHold = event->gesture(Qt::TapAndHoldGesture))
    {
        tapAndHoldTriggered(static_cast<QTapAndHoldGesture *>(tapAndHold));
        event->accept( tapAndHold );
    }
    qDebug() << "CntContactCardDetailItem::gestureEvent - OUT";
}

void CntContactCardDetailItem::tapTriggered(QTapGesture *gesture)
{
    qDebug() << "CntContactdDetailItem::tagTriggered - IN";
    
    Qt::GestureState state = gesture->state();
    qDebug() << "CntContactdDetailItem::tagTriggered, state: " << state;
    switch ( state ) 
    {
        case Qt::GestureStarted:
        {
            if (mIsFocusable)
            {
                HbInstantFeedback::play(HbFeedback::Basic);
            }
            mHasFocus = true;
            updatePrimitives();
            break;
        }
        case Qt::GestureFinished:
        case Qt::GestureCanceled:
        {
            mHasFocus = false;
            updatePrimitives();
            emit clicked(); // also in GestureCancelled?
            break;
        }
        default:
        {
            break;
        }
    }
    qDebug() << "CntContactdDetailItem::tagTriggered - OUT";
}

void CntContactCardDetailItem::tapAndHoldTriggered(QTapAndHoldGesture *gesture)
{
    if (gesture->state() == Qt::GestureFinished)
    {
        onLongPress(gesture->position());
    }
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

/*
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
*/
/*
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
*/

void CntContactCardDetailItem::setDetails(CntContactCardDataItem* aDataItem)
{
    mValueTextElideMode = aDataItem->elideMode();

    if (aDataItem->icon() != icon)
        {
        icon.clear();
        icon = aDataItem->icon();
        }
    if (aDataItem->secondaryIcon() != secondaryIcon)
        {
        secondaryIcon.clear();
        secondaryIcon = aDataItem->secondaryIcon();
        }
    if (aDataItem->titleText() != text)
        {
        text.clear();
        text = aDataItem->titleText();
        }
    if (aDataItem->valueText() != valueText)
        {
        valueText.clear();
        valueText = aDataItem->valueText();
        }

    recreatePrimitives();
}

int CntContactCardDetailItem::index()
{
    return mIndex;
}

