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
 * Description:Message chat View decorator item prototype
 *
 */

#ifndef CNTHISTORYVIEWITEM_H
#define CNTHISTORYVIEWITEM_H

// INCLUDES
#include "cnthistoryviewitemwidget.h"
#include <hbtextitem.h>
#include <hbiconitem.h>
#include <hbabstractviewitem.h>
#include <QGraphicsLinearLayout>

/**
 * This class represents the item decorator of 
 * the comm history view.  
 */
class CntHistoryViewItem : public HbAbstractViewItem
    {    
public:
    /*
     * Constructor
     * @param parent, reference of QGraphicsItem
     * default set to 0
     */
    CntHistoryViewItem(QGraphicsItem* parent=0);
    
    /*
     * Factory method to the items
     */
    HbAbstractViewItem* createItem();
    
    /*
     * Overriden method to postprocess custom item
     */
    void polish(HbStyleParameters& params);
    
    /*
     * Overriden method to draw the custom item in the list view
     */
    void updateChildItems();
    
private:
    
    /*
     * Custom widget containing text fields and icon
     * Not owned
     */
    CntHistoryViewItemWidget* itemWidget;
    };

#endif // CNTHISTORYVIEWITEM_H
