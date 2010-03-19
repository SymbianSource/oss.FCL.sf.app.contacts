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

#include <mobhistorymodel.h>

//---------------------------------------------------------------
// HbListViewItem::HbListViewItem
// Constructor
//---------------------------------------------------------------
CntHistoryViewItem::CntHistoryViewItem(QGraphicsItem* parent)
: HbAbstractViewItem(parent),
mLayout(0),
itemWidget(0)
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
// HbListViewItem::polish
//
//---------------------------------------------------------------
void CntHistoryViewItem::polish(HbStyleParameters& /*params*/)
{
}

//---------------------------------------------------------------
// HbListViewItem::updateChildItems
//
//---------------------------------------------------------------
void CntHistoryViewItem::updateChildItems()
{
    //Create whole item layout
    if (!mLayout) {
        mLayout = new QGraphicsLinearLayout(this);
        mLayout->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        mLayout->setOrientation(Qt::Horizontal);
        mLayout->setContentsMargins(0,0,0,0);
    }

    //Create item widget 
    if(!itemWidget)
        {
        itemWidget = new CntHistoryViewItemWidget(this);
        mLayout->addItem(itemWidget);
        }
    
    //Update item widget content
    QStringList data = modelIndex().data(Qt::DisplayRole).toStringList();
    QString iconName = modelIndex().data(Qt::DecorationRole).toString();
    bool incoming = false;
    bool status = false;
    if (modelIndex().data(MobHistoryModel::DirectionRole).toInt(&status) == 0) {
        incoming = true;
    }
 
    itemWidget->setDetails(data.at(0), data.at(1), data.at(2), iconName, incoming);

    setLayout(mLayout);
}
// EOF
