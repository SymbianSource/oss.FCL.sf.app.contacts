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

class HbSearchPanel;
class HbTextItem;

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
    void callNamesList();

    void manageMembers();
    void editGroup();
    void placeGroupToHs();
    void deleteGroup();
    void removeFromGroup(const QModelIndex &index);
    void viewDetailsOfGroupContact(const QModelIndex &index);
    void find();
    void closeFind();
    void setFilter(const QString &filterString);
    

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
    HbSearchPanel               *mSearchPanel;
    HbTextItem                  *mEmptyListLabel;
    QList<QContactLocalId>       mLocalIdList;
    QList<QContactLocalId>       mFilteredLocalIdList;
};

#endif // CNTGROUPMEMBERVIEW_H

// EOF
