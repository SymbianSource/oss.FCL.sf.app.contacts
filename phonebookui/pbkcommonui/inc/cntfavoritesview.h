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

#ifndef CNTFAVORITESVIEW_H
#define CNTFAVORITESVIEW_H

#include "cntbaselistview.h"

class CntFavoritesView : public CntBaseListView
{
    Q_OBJECT

public slots:
    void aboutToCloseView();
    void openFetch();
    void openNamesList();

public:
    CntFavoritesView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntFavoritesView();

public://from baseview
    CntViewParameters::ViewId viewId() const { return CntViewParameters::collectionFavoritesView; }

#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    void addActionsToToolBar();

};

#endif // CNTFAVORITESVIEW_H

// EOF
