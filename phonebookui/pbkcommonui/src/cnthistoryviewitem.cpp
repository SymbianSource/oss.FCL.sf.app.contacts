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

#include "cnthistoryviewitem.h"

#include <cnthistorymodel.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <QGraphicsWidget>

#define NEW_EVENT_FRAME "qtg_fr_list_new_item"
#define INCOMING_FOCUS_FRAME "qtg_fr_convlist_received_pressed"
#define OUTGOING_FOCUS_FRAME "qtg_fr_convlist_sent_pressed"

//---------------------------------------------------------------
// HbListViewItem::HbListViewItem
// Constructor
//---------------------------------------------------------------
CntHistoryViewItem::CntHistoryViewItem(QGraphicsItem* parent)
: HbListViewItem(parent),
  mIncoming(false),
  mNewMessage(false),
  mNewItem(NULL),
  mFocusItem(NULL)
{
}

//---------------------------------------------------------------
// HbListViewItem::createItem
// Create a new decorator item.
//---------------------------------------------------------------
HbAbstractViewItem* CntHistoryViewItem::createItem()
{
    return new CntHistoryViewItem(*this);
}

//---------------------------------------------------------------
// HbListViewItem::updateChildItems
//
//---------------------------------------------------------------
void CntHistoryViewItem::updateChildItems()
{
    int flags = modelIndex().data(CntFlagsRole).toInt();
    mIncoming = flags & CntIncoming ? true : false;
    mNewMessage = flags & CntUnseen ? true : false;

    if (mNewMessage)
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
    
    HbListViewItem::updateChildItems();
}

//---------------------------------------------------------------
// HbAbstractViewItem::pressStateChanged
// This function is called whenever item press state changes.
//---------------------------------------------------------------
void CntHistoryViewItem::pressStateChanged(bool pressed, bool animate)
{
    Q_UNUSED(animate);
    if (pressed)
    {
        if (!mFocusItem)
        {
            // focus frame position can't be read from widgetml, we set it manually
            QRectF frameRect = HbWidget::primitive("frame")->boundingRect();
            QPointF framePoint = HbWidget::primitive("frame")->pos();
            
            frameRect.moveTo(framePoint);
            
            if (mIncoming)
            {
                mFocusItem = new HbFrameItem(INCOMING_FOCUS_FRAME, HbFrameDrawer::NinePieces, this);
            }
            else
            {
                mFocusItem = new HbFrameItem(OUTGOING_FOCUS_FRAME, HbFrameDrawer::NinePieces, this);
            }
            
            mFocusItem->setGeometry(frameRect);
            mFocusItem->setZValue(-1.0);
            style()->setItemName(mFocusItem, "focusframe");
        }
    }
    else
    {
        if (mFocusItem)
        {
            delete mFocusItem;
            mFocusItem = NULL;
        }
    }
}

bool CntHistoryViewItem::event(QEvent* e)
{
    bool result;
    if (e->type() == QEvent::GraphicsSceneResize)
    {
        // HbAbstractItemView has a performance improvement when drawing backrounds but seems
        // to screw the layout of history view items. This workaround fixes the issue. There should
        // be minimal performance drawbacks since GraphicsSceneResize events are quite few.
        // TODO: Remove this once Orbit changes their implementation. Not known when
        QGraphicsWidget *frame = static_cast<QGraphicsWidget*>(primitive("frame"));
        QRectF frameGeometry = frame->geometry();
        result = HbListViewItem::event(e);
        frame->setGeometry(frameGeometry);
    } 
    else
    {
        result = HbListViewItem::event(e);
    }
    return result;
}

// EOF
