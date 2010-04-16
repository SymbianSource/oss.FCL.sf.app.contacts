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

#ifndef CNTGROUPMEMBERVIEW_H
#define CNTGROUPMEMBERVIEW_H

#include "cntbaselistview.h"

class CntContactCardHeadingItem;
class ThumbnailManager;

class CntGroupMemberView : public CntBaseListView
{
    Q_OBJECT


public:
    CntGroupMemberView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntGroupMemberView();

public slots:
    void aboutToCloseView();
    void onLongPressed (HbAbstractViewItem *item, const QPointF &coords);
    void onListViewActivated(const QModelIndex &index);
    void handleExecutedCommand(QString command, QContact contact);
    void addMenuItems();
    void addActionsToToolBar();
    void groupActions();
    void manageMembers();
    void editGroup();
    void deleteGroup();
    void removeFromGroup(const QModelIndex &index);
    void editContact(const QModelIndex &index);
    void thumbnailReady(const QPixmap& pixmap, void *data, int id, int error);
    

public:
    CntViewParameters::ViewId viewId() const { return CntViewParameters::groupMemberView; }
    void activateView(const CntViewParameters &viewParameters);

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif

    bool                         mNoGroupContactsPresent;
    QContact                    *mGroupContact;
    QList<QContactLocalId>       mLocalIdList;
    QList<QContactLocalId>       mFilteredLocalIdList;
    CntContactCardHeadingItem   *mHeadingItem;
    ThumbnailManager            *mThumbnailManager;
};

#endif // CNTGROUPMEMBERVIEW_H

// EOF
