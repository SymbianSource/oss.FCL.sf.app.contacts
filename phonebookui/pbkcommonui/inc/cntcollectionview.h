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

#ifndef CNTCOLLECTIONVIEW_H
#define CNTCOLLECTIONVIEW_H

#include "cntbaselistview.h"
#include "cntcollectionlistmodel.h"

class QStringListModel;

class CntCollectionView : public CntBaseListView
{
    Q_OBJECT

public slots:

    void aboutToCloseView();
    void onListViewActivated(const QModelIndex &index);
    void onLongPressed(HbAbstractViewItem *item, const QPointF &coords);
    void handleExecutedCommand(QString command, QContact contact);
    void newGroup();
    void addMenuItems();
    void reorderGroup();
    void deleteGroups();
    void disconnectAll();

public:
    CntCollectionView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntCollectionView();

    CntViewParameters::ViewId viewId() const { return CntViewParameters::collectionView; }

#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    void addActionsToToolBar();
    void setDataModel();
    void refreshDataModel();

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    QList<QContactLocalId>   mContactsLocalIdList;
    CntCollectionListModel * mModel;
    QList<QContactId>        mContactIdList;
    //Option menu item
    HbAction         *mReorderAction; 
    HbAction         *mDeleteGroupAction; 
    HbAction         *mDisconnectAllAction;
    HbMenu           *mOptionsMenu;
};

#endif // CNTCOLLECTIONVIEW_H

// EOF
