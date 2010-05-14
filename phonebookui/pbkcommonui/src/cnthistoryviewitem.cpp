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

#define NEW_EVENT_FRAME "qtg_fr_list_new_item"

//---------------------------------------------------------------
// HbListViewItem::HbListViewItem
// Constructor
//---------------------------------------------------------------
CntHistoryViewItem::CntHistoryViewItem(QGraphicsItem* parent)
: HbListViewItem(parent),
  mIncoming(false),
  mNewMessage(false),
  mNewItem(NULL)
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
    int flags = modelIndex().data(CntHistoryModel::FlagsRole).toInt();
    mIncoming = flags & CntHistoryModel::Incoming ? true : false;
    mNewMessage = flags & CntHistoryModel::Unseen ? true : false;
    
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
// EOF
