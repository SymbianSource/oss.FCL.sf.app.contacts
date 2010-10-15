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

#include "cntnamesviewitem.h"
#include "cntdebug.h"

#include <hbeffect.h>
#include <QTimer>

/*!
Constructor, initialize member variables.
*/
CntNamesViewItem::CntNamesViewItem(QGraphicsItem* parent) :
    HbListViewItem(parent)
{
    CNT_ENTRY
    
    setProperty("animationState", CNT_NAME_LIST_LAYOUT_PROPERTY_NORMAL);
    HbEffect::add("nameslistitem", ":/effects/item_change_layout.fxml", "changed");
    
    CNT_EXIT
}


/*!
Factory method to the items
*/
HbAbstractViewItem* CntNamesViewItem::createItem()
{
    return new CntNamesViewItem(*this);
}

/*!
Update the custom and standard parameters of this item identified by modelindex
*/
void CntNamesViewItem::updateChildItems()
{
    CNT_ENTRY
    
    if (prototype()->property("animationState") == CNT_NAME_LIST_LAYOUT_PROPERTY_ANIMATE)
    {
        HbEffect::start(this, "nameslistitem", "changed");
        QTimer::singleShot(CNT_NAME_LIST_LAYOUT_CHANGED_TIMEOUT, this, SLOT(doUpdateChildItems()));
    }
    else
    {
        HbListViewItem::updateChildItems();
    }

    CNT_EXIT
}

void CntNamesViewItem::doUpdateChildItems()
{
    HbListViewItem::updateChildItems();
}

// EOF
