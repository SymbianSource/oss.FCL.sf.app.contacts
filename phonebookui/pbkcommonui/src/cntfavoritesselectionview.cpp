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

#include "cntfavoritesselectionview.h"

#include <hblistview.h>

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)
*/
CntFavoritesSelectionView::CntFavoritesSelectionView(CntViewManager *viewManager, QGraphicsItem *parent)
: CntBaseSelectionView(viewManager, parent)
{
}

/*!
Destructor
*/
CntFavoritesSelectionView::~CntFavoritesSelectionView()
{
}

/*!
Save selections
*/
void CntFavoritesSelectionView::aboutToCloseView()
{
    CntViewParameters viewParameters(CntViewParameters::collectionView);
    viewManager()->onActivateView(viewParameters);
}

