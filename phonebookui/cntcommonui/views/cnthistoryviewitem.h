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

#ifndef CNTHISTORYVIEWITEM_H
#define CNTHISTORYVIEWITEM_H

#include <hblistviewitem.h>

class CntHistoryViewItemWidget;

/**
 * This class represents the item decorator of 
 * the comm history view.  
 */
class CntHistoryViewItem : public HbListViewItem
{
    friend class TestCntHistoryView;
    Q_OBJECT
    
public:
    CntHistoryViewItem(QGraphicsItem* parent = NULL);

    HbAbstractViewItem* createItem();
    void updateChildItems();
    
protected:
    void pressStateChanged(bool pressed, bool animate);
    
private:
    CntHistoryViewItemWidget *mWidget;
};

#endif /* CNTHISTORYVIEWITEM_H */
