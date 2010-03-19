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

#ifndef CNTFAVORITESSELECTIONVIEW_H
#define CNTFAVORITESSELECTIONVIEW_H

#include "cntbaseselectionview.h"

class CntFavoritesSelectionView : public CntBaseSelectionView
{

public:
    CntFavoritesSelectionView(CntViewManager *viewManager, QGraphicsItem *parent=0);
    ~CntFavoritesSelectionView();

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    void aboutToCloseView();
    CntViewParameters::ViewId viewId() const { return CntViewParameters::collectionFavoritesSelectionView; }

};

#endif /* CNTFAVORITESSELECTIONVIEW_H */
