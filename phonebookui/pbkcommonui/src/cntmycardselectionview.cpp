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

#include "cntmycardselectionview.h"

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntMyCardSelectionView::CntMyCardSelectionView(CntViewManager *viewManager, QGraphicsItem *parent, HbAbstractItemView::SelectionMode newMode)
    : CntBaseSelectionView(viewManager, parent, newMode)
{
    contactModel()->showMyCard(false);
}

/*!
Destructor
*/
CntMyCardSelectionView::~CntMyCardSelectionView()
{
    
}

/*!
Set selection to mycard
*/
void CntMyCardSelectionView::aboutToCloseView()
{
    viewManager()->onActivatePreviousView();
}


/*!
Set index to mycard
*/
void CntMyCardSelectionView::onListViewActivated(const QModelIndex& index)
{
    if (index.isValid())
    {  
        QContact contact = contactModel()->contact(index);
        contactManager()->setSelfContactId(contact.localId());
        viewManager()->onActivateView(CntViewParameters::namesView);
    }
}
    


