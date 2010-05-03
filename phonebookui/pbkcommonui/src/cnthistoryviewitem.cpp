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

//---------------------------------------------------------------
// HbListViewItem::HbListViewItem
// Constructor
//---------------------------------------------------------------
CntHistoryViewItem::CntHistoryViewItem(QGraphicsItem* parent)
: HbAbstractViewItem(parent),
itemWidget(NULL)
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
    QStringList data = modelIndex().data(Qt::DisplayRole).toStringList();
    QString iconName = modelIndex().data(Qt::DecorationRole).toString();
    bool incoming = false;
    bool newMessage = false;
    bool status = false;
    if (modelIndex().data(CntHistoryModel::DirectionRole).toInt(&status) == CntHistoryModel::Incoming) {
        incoming = true;
    }
    // This indication applies to smses only
    if (modelIndex().data(CntHistoryModel::ItemTypeRole).toInt(&status) == CntHistoryModel::Message && 
        modelIndex().data(CntHistoryModel::SeenStatusRole).toInt(&status) == CntHistoryModel::Unseen) {
        newMessage = true;
    }
    
    QGraphicsLinearLayout* currentLayout = static_cast<QGraphicsLinearLayout*>(layout());
    
    if (currentLayout == NULL) {
    
        currentLayout = new QGraphicsLinearLayout(this);
        currentLayout->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        currentLayout->setOrientation(Qt::Horizontal);
        currentLayout->setContentsMargins(0,0,0,0);
            
        //Create item widget 
        itemWidget = new CntHistoryViewItemWidget(this);
        currentLayout->addItem(itemWidget);
    
        //Update item widget contents
        itemWidget->setDetails(data.at(0), data.at(1), data.at(2), iconName, incoming, newMessage);
    
        setLayout(currentLayout);
    
    } else {
        // Find the itemWidget item from the layout and remove it
        int i = 0;
        while( itemWidget != static_cast<CntHistoryViewItemWidget*>(currentLayout->itemAt(i)) ) {
            i++;
        }
        currentLayout->removeAt(i);
        delete itemWidget;
        
        //Create new item widget 
        itemWidget = new CntHistoryViewItemWidget(this);
        currentLayout->addItem(itemWidget);
    
        //Update item widget contents
        itemWidget->setDetails(data.at(0), data.at(1), data.at(2), iconName, incoming, newMessage);
    
        updateGeometry();
    }
}
// EOF
