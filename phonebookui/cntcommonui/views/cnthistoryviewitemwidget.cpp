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

#include "cnthistoryviewitemwidget.h"
#include "cntdebug.h"

#include <cnthistorymodel.h>
#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>

#define NEW_EVENT_FRAME "qtg_fr_list_new_item"
#define INCOMING_FOCUS_FRAME "qtg_fr_convlist_received_pressed"
#define OUTGOING_FOCUS_FRAME "qtg_fr_convlist_sent_pressed"

CntHistoryViewItemWidget::CntHistoryViewItemWidget(QGraphicsItem *parent) :
    HbWidget(parent),
    mIcon(NULL),
    mTitle(NULL),
    mBodyText(NULL),
    mTimeStamp(NULL),
    mFrameItem(NULL),
    mFocusItem(NULL),
    mNewItem(NULL),
    incoming(false),
    newmessage(false)
{
    CNT_ENTRY
    
    setProperty("state", "normal");
    
    CNT_EXIT
}

CntHistoryViewItemWidget::~CntHistoryViewItemWidget()
{
    CNT_ENTRY

    CNT_EXIT
}

void CntHistoryViewItemWidget::createPrimitives()
{
    CNT_ENTRY
    
    if (!icon.isNull())
    {
        if (!mIcon)
        {
            mIcon = new HbIconItem(this);
            mIcon->setAlignment(Qt::AlignCenter);
            mIcon->setIcon(icon);
            style()->setItemName(mIcon, "icon-1");
        }
    }
    else
    {
        if (mIcon)
        {
            delete mIcon;
        }
        mIcon = NULL;
    }

    if (!title.isNull())
    {
        if (!mTitle)
        {
            mTitle = new HbTextItem(this);
            mTitle->setText(title);
            style()->setItemName(mTitle, "text-1");
        }
    }
    else
    {
        if (mTitle)
        {
            delete mTitle;
        }
        mTitle = NULL;
    }

    if (!bodyText.isNull())
    {
        if (!mBodyText)
        {
            mBodyText = new HbTextItem(this);
            mBodyText->setText(bodyText);
            style()->setItemName(mBodyText, "text-2");
        }
    }
    else
    {
        if (mBodyText)
        {
            delete mBodyText;
        }
        mBodyText = NULL;
    }
    
    if (!timeStamp.isNull())
    {
        if (!mTimeStamp)
        {
            mTimeStamp = new HbTextItem(this);
            mTimeStamp->setText(timeStamp);
            style()->setItemName(mTimeStamp, "text-3");
        }
    }
    else
    {
        if (mTimeStamp)
        {
            delete mTimeStamp;
        }
        mTimeStamp = NULL;
    }

    if (!mFrameItem)
    {
        mFrameItem = new HbFrameItem(this);
        mFrameItem->frameDrawer().setFrameGraphicsName(backGround.frameGraphicsName());
        mFrameItem->frameDrawer().setFrameType(backGround.frameType());
        mFrameItem->setZValue(-2);
        style()->setItemName(mFrameItem, "frame");
    }
    
    CNT_EXIT
}

void CntHistoryViewItemWidget::recreatePrimitives()
{
    CNT_ENTRY
    
    HbWidget::recreatePrimitives();

    delete mIcon;
    mIcon = NULL;

    delete mTitle;
    mTitle = NULL;
    
    delete mBodyText;
    mBodyText = NULL;
    
    delete mTimeStamp;
    mTimeStamp= NULL;

    delete mFrameItem;
    mFrameItem = NULL;

    createPrimitives();
    
    CNT_EXIT
}

void CntHistoryViewItemWidget::setModelIndex(const QModelIndex& index)
{
    CNT_ENTRY
    
    // CntHistoryModel ALWAYS returns 3x QString for Qt::DisplayRole
    QStringList texts = index.data(Qt::DisplayRole).toStringList();
    title = texts.at(0);
    bodyText = texts.at(1);
    timeStamp = texts.at(2);
    
    icon = index.data(Qt::DecorationRole).value<HbIcon>();
    
    backGround = index.data(Qt::BackgroundRole).value<HbFrameBackground>();
    
    int flags = index.data(CntFlagsRole).toInt();
    incoming = flags & CntIncoming ? true : false;
    newmessage = flags & CntUnseen ? true : false;

    if (newmessage)
    {
        if (!mNewItem)
        {
            mNewItem = new HbFrameItem(NEW_EVENT_FRAME, HbFrameDrawer::ThreePiecesVertical, this);
            style()->setItemName(mNewItem, "newitem");
        }
    }
    else
    {
        if (mNewItem)
        {
            delete mNewItem;
            mNewItem = NULL;
        }
    }
    
    recreatePrimitives();
    repolish();
    
    setProperty("state", "normal");
    
    CNT_EXIT
}

void CntHistoryViewItemWidget::pressStateChanged(bool pressed)
{
    CNT_ENTRY
    
    if (pressed)
    {
        if (!mFocusItem)
        {
            if (incoming)
            {
                mFocusItem = new HbFrameItem(INCOMING_FOCUS_FRAME, HbFrameDrawer::NinePieces, this);
            }
            else
            {
                mFocusItem = new HbFrameItem(OUTGOING_FOCUS_FRAME, HbFrameDrawer::NinePieces, this);
            }
            mFocusItem->setZValue(-1.0);
            style()->setItemName(mFocusItem, "focusframe");
        }
        
        setProperty("state", "pressed");
    }
    else
    {
        if (mFocusItem)
        {
            delete mFocusItem;
            mFocusItem = NULL;
        }
        
        setProperty("state", "normal");
    }
    
    repolish();
    
    CNT_EXIT
}
