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

#ifndef CNTHISTORYVIEW_H
#define CNTHISTORYVIEW_H

#include <QObject>
#include "cntbaseview.h"

class HbListView;
class MobHistoryModel;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntHistoryView : public CntBaseView
{
    Q_OBJECT

public:
    CntHistoryView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntHistoryView();

    CntViewParameters::ViewId viewId() const { return CntViewParameters::historyView; }
    void activateView(const CntViewParameters &viewParameters);
    void addMenuItems();

public slots:
    void aboutToCloseView();
    void updateScrollingPosition();
    void clearHistory();
    void longPressed(HbAbstractViewItem *item, const QPointF &coords);
    void pressed(const QModelIndex &index);
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif    
    HbListView*             mHistoryListView;
    MobHistoryModel*        mHistoryModel;
    QContact*               mContact;
    bool                    mIsMyCard;
    
};

#endif // CNTHISTORYVIEW_H

// EOF
