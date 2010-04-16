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

#ifndef CNTFAVORITESMEMBERVIEW_H
#define CNTFAVORITESMEMBERVIEW_H

#include "cntbaselistview.h"

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

class CntFavoritesMemberView : public CntBaseView
{
    Q_OBJECT

public:
    CntFavoritesMemberView(CntViewManager *viewManager, QGraphicsItem *parent=0);
    ~CntFavoritesMemberView();

    CntViewParameters::ViewId viewId() const { return CntViewParameters::FavoritesMemberView; }
    void activateView(const CntViewParameters &viewParameters);
    void addMenuItems();
 
public slots:
    void aboutToCloseView();
    void manageFavorites();
    void onLongPressed (HbAbstractViewItem *item, const QPointF &coords);
    void openContact(const QModelIndex &index);
    void editContact(const QModelIndex &index);
    void removeFromFavorites(const QModelIndex &index);
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif

    QContact* mContact;
    HbAction         *mManageFavoritesAction; 
    HbMenu        *mFavoritesMenu;
    HbListView*        mFavoriteListView; 

};

#endif /* CNTFAVORITESMEMBERVIEW_H */
