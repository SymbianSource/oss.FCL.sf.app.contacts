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

#include "cnteditviewheadingitem.h"

#include <qtcontacts.h>

#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hbtoucharea.h>
#include <hbinstantfeedback.h>

#include <QGraphicsSceneMouseEvent>

CntEditViewHeadingItem::CntEditViewHeadingItem(QGraphicsItem *parent) :
    HbWidget(parent),
    mIcon(0),
    mLabel(0),
    mSecondLabel(0),
    mFrameItem(0),
    mIconTouchArea(0),
    mIconFocused(false),
    mTextFocused(false)
{

}

CntEditViewHeadingItem::~CntEditViewHeadingItem()
{

}

void CntEditViewHeadingItem::createPrimitives()
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
        if (!mLabel)
        {
            mLabel = new HbTextItem(this);
            mLabel->setText(text);
            mLabel->setMaximumLines(2);
            mLabel->setTextWrapping(Hb::TextWordWrap);
            style()->setItemName(mLabel, "text");
        }
    }
    else
    {
        if (mLabel)
        {
            delete mLabel;
        }
        mLabel = 0;
    }

    if (!second_text.isNull())
    {
        if (!mSecondLabel)
        {
            mSecondLabel = new HbTextItem(this);
            mSecondLabel->setText(second_text);
            style()->setItemName(mSecondLabel, "second_text");
        }
    }
    else
    {
        if (mSecondLabel)
        {
            delete mSecondLabel;
        }
        mSecondLabel = 0;
    }

    if (!mFrameItem)
    {
        mFrameItem = new HbFrameItem(this);
        mFrameItem->frameDrawer().setFrameGraphicsName("qtg_fr_groupbox");
        mFrameItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        mFrameItem->setZValue(-2);
        style()->setItemName(mFrameItem, "background");
    }

    if (!mIconTouchArea)
    {
        mIconTouchArea = new HbTouchArea(this);
        style()->setItemName(mIconTouchArea, "iconArea");
    }
}

void CntEditViewHeadingItem::recreatePrimitives()
{
    HbWidget::recreatePrimitives();

    delete mIcon;
    mIcon = 0;

    delete mLabel;
    mLabel = 0;

    delete mSecondLabel;
    mSecondLabel = 0;

    delete mFrameItem;
    mFrameItem = 0;

    delete mIconTouchArea;
    mIconTouchArea = 0;

    createPrimitives();
}


void CntEditViewHeadingItem::updatePrimitives()
{
    HbWidget::updatePrimitives();
}

void CntEditViewHeadingItem::setDetails(const QContact* contact)
{
    text.clear();
    second_text.clear();
    icon.clear();

    // icon label shows default icon first
    icon = HbIcon("qtg_large_avatar");

    QContactName name = contact->detail<QContactName>();

    QStringList nameList;
    if (!name.prefix().isEmpty())
    {
        nameList << name.prefix();
    }
    if (!name.firstName().isEmpty())
    {
        nameList << name.firstName();
    }
    if (!name.middleName().isEmpty())
    {
        nameList << name.middleName();
    }
    if (!name.lastName().isEmpty())
    {
        nameList << name.lastName();
    }
    if (!name.suffix().isEmpty())
    {
        nameList << name.suffix();
    }
    text = nameList.join(" ");

    if (!contact->detail<QContactNickname>().nickname().isEmpty())
    {
        second_text = hbTrId("\"%1\"").arg(contact->detail<QContactNickname>().nickname());
    }

    if (text.isEmpty() && second_text.isEmpty())
    {
        text = hbTrId("txt_phob_list_enter_name");
    }

    recreatePrimitives();
}

void CntEditViewHeadingItem::setIcon(const HbIcon newIcon)
{
    if (newIcon != icon)
    {
        icon = newIcon;
        createPrimitives();
        mIcon->setIcon(icon);
        repolish();
    }
}

void CntEditViewHeadingItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HbInstantFeedback::play(HbFeedback::Basic);
    if (mIconTouchArea->rect().contains(event->pos()))
    {
        mIconFocused = true;
    }
    else if (rect().contains(event->pos()))
    {
        mTextFocused = true;
    }

    event->accept();
}

void CntEditViewHeadingItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!mIconTouchArea->rect().contains(event->pos()) && mIconFocused)
    {
        mIconFocused = false;
    }
    else if ((!rect().contains(event->pos()) || mIconTouchArea->rect().contains(event->pos())) && mTextFocused)
    {
        mTextFocused = false;
    }
    event->accept();
}


void CntEditViewHeadingItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (mIconTouchArea->rect().contains(event->pos()) && mIconFocused)
    {
        mIconFocused = false;
        emit iconClicked();
    }
    else if (rect().contains(event->pos()) && !mIconTouchArea->rect().contains(event->pos()) && mTextFocused)
    {
        mTextFocused = false;
        emit textClicked();
    }
    event->accept();
}
