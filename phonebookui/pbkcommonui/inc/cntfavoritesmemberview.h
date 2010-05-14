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

#include <hbdocumentloader.h>

#include <cntabstractview.h>

class MobCntModel;
class CntAbstractViewManager;
class HbView;
class HbAction;
class HbListView;
class HbAbstractViewItem;
class QModelIndex;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

class CntFavoritesMemberView : public QObject, public CntAbstractView
{
    Q_OBJECT

public: // From CntAbstractView
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const { return false; }
    HbView* view() const { return mView; }
    int viewId() const { return FavoritesMemberView; }
    
public:
    CntFavoritesMemberView();
    ~CntFavoritesMemberView();

#ifdef PBK_UNIT_TEST
public slots:
#else
private slots:
#endif

    void manageFavorites();
    void handleManageFavorites(HbAction *action);
    
    void onLongPressed (HbAbstractViewItem *item, const QPointF &coords);
    void handleMenu(HbAction* action);
    
    void openContact(const QModelIndex &index);
    void editContact(const QModelIndex &index);
    void removeFromFavorites(const QModelIndex &index);
    void showPreviousView();
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif

    QContact*                   mContact;
    HbAction*                   mManageFavoritesAction; 
    MobCntModel*                mModel; // own
    HbListView*                 mFavoriteListView; 
    CntAbstractViewManager*     mViewManager;
    HbDocumentLoader            mDocumentLoader;
    HbView*                     mView; // own

    HbAction*                   mSoftkey; // owned by view
};

#endif /* CNTFAVORITESMEMBERVIEW_H */
