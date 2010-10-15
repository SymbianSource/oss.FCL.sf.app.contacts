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

#ifndef CNTNAMESVIEWITEM_H
#define CNTNAMESVIEWITEM_H

#include <hblistviewitem.h>

const int CNT_NAME_LIST_LAYOUT_CHANGED_TIMEOUT = 200;
const qreal CNT_NAME_LIST_LAYOUT_CHANGE_PINCH_IN_TRESHOLD = 0.6;
const qreal CNT_NAME_LIST_LAYOUT_CHANGE_PINCH_OUT_TRESHOLD = 1.4;
const QString CNT_NAME_LIST_LAYOUT_PROPERTY_ANIMATE = "animate";
const QString CNT_NAME_LIST_LAYOUT_PROPERTY_NORMAL = "normal";

class CntNamesViewItem : public HbListViewItem
{
    friend class TestCntNamesView;
    Q_OBJECT
    
public:
    CntNamesViewItem(QGraphicsItem* parent = NULL);

    HbAbstractViewItem* createItem();
    void updateChildItems();
    
private slots:
    void doUpdateChildItems();
};

#endif /* CNTNAMESVIEWITEM_H */
