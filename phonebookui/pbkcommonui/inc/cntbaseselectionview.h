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

#ifndef CNTBASESELECTIONVIEW_H
#define CNTBASESELECTIONVIEW_H

#include "cntbaseview.h"
#include "qtpbkglobal.h"
#include <hbabstractviewitem.h>
#include <hbabstractitemview.h>

class HbListView;
class QGraphicsLinearLayout;
class QItemSelectionModel;

class QTPBK_EXPORT CntBaseSelectionView : public CntBaseView
{
    Q_OBJECT

public:
    CntBaseSelectionView(CntViewManager *viewManager, QGraphicsItem *parent = 0, HbAbstractItemView::SelectionMode newMode = HbAbstractItemView::MultiSelection);
    ~CntBaseSelectionView();

public:

    virtual void setupView();
    virtual void activateView(const CntViewParameters &viewParameters);
    void addItemsToLayout();
    
public slots:
    virtual void onListViewActivated(const QModelIndex &index) { Q_UNUSED(index); }

public:

    HbListView             *listView();
    QGraphicsLinearLayout  *listLayout();
    QItemSelectionModel    *selectionModel();

private:
    HbListView              *mListView;
    QGraphicsLinearLayout   *mListLayout;

    // needed in subclasses
#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    HbAbstractItemView::SelectionMode   mSelectionMode;

};

#endif /* CNTBASESELECTIONVIEW_H */
