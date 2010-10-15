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
#include "cnthistoryviewitemwidget.h"
#include "cntdebug.h"

#include <QGraphicsLinearLayout>

/*!
Constructor, initialize member variables.
*/
CntHistoryViewItem::CntHistoryViewItem(QGraphicsItem* parent) :
    HbListViewItem(parent),
    mWidget(NULL)
{
    CNT_ENTRY
    
    CNT_EXIT
}

/*!
Factory method to the items
*/
HbAbstractViewItem* CntHistoryViewItem::createItem()
{
    return new CntHistoryViewItem(*this);
}

/*!
Update the custom and standard parameters of this item identified by modelindex
*/
void CntHistoryViewItem::updateChildItems()
{
    CNT_ENTRY
    
    if (!mWidget)
    {
        mWidget = new CntHistoryViewItemWidget(this);
        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addItem(mWidget);
        setLayout(layout);
    }
    
    mWidget->setModelIndex(modelIndex());
    
    CNT_EXIT
}

/*!
This function is called whenever item press state changes.
*/
void CntHistoryViewItem::pressStateChanged(bool pressed, bool animate)
{
    CNT_ENTRY
    
    Q_UNUSED(animate);

    mWidget->pressStateChanged(pressed);
    
    CNT_EXIT
}

// EOF
