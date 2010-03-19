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

#ifndef CNTGROUPMEMBERSELECTIONVIEW_H
#define CNTGROUPMEMBERSELECTIONVIEW_H

#include "cntbaseselectionview.h"

class HbGroupBox;

class CntGroupMemberSelectionView : public CntBaseSelectionView
{
    Q_OBJECT

public:
    CntGroupMemberSelectionView(CntViewManager *viewManager, QGraphicsItem *parent=0);
    ~CntGroupMemberSelectionView();

public slots:
    void activateView(const CntViewParameters &viewParameters);
    void openGroupNameEditor();
    void aboutToCloseView();
    void saveGroup();
    void markUnmarkAll();
    void OnCancel();
    void updateTitle(const QItemSelection &selected, const QItemSelection &deselected);

public:
    void addActionsToToolBar();
    CntViewParameters::ViewId viewId() const { return CntViewParameters::groupMemberSelectionView; }
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    QContact                *mContact;
    HbGroupBox              *mBanner;
    QList<QContactLocalId>   mContactsLocalIdList;
    QContactName             mGroupContactName;
    int                      mCountContacts;
    bool                     mActionNewGroup;
    QList<QContactLocalId>   mSelectionList;
    
};

#endif /* CNTGROUPMEMBERSELECTIONVIEW_H */
