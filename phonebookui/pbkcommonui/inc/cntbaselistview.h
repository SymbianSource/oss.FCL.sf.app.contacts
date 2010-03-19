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

#ifndef CNTBASELISTVIEW_H
#define CNTBASELISTVIEW_H

#include "cntbaseview.h"
#include "cntstringmapper.h"

#include <hblistview.h>
#include <hblabel.h>
#include <hbgroupbox.h>
#include <hbabstractviewitem.h>

// forward declarations
class QGraphicsLinearLayout;

class CntBaseListView : public CntBaseView
{
    Q_OBJECT

public slots:
    virtual void onLongPressed(HbAbstractViewItem *item, const QPointF &coords);
    virtual void onListViewActivated(const QModelIndex &index);

public:
    CntBaseListView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntBaseListView();

public: // from CntBaseView
    virtual void setupView();
    virtual void activateView(const CntViewParameters &viewParameters);

#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    //layout
    QGraphicsLinearLayout *listLayout();
    virtual void addItemsToLayout();

    //banner
    HbGroupBox *banner();
    bool isBannerVisible();
    void setBannerVisibility(bool aVisible);
    void setBannerName(QString aName);
    QString bannerName();

    //list view
    virtual HbListView *listView();

    //model
    virtual void setDataModel();
    virtual void clearDataModelFilter();

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif

    HbGroupBox              *mBanner;
    bool                     mHasBanner;
    QString                  mBannerName;

    HbListView              *mListView;
    QGraphicsLinearLayout   *mListLayout;
    CntStringMapper          mStringMapper;
};

#endif // CNTBASELISTVIEW_H

// EOF
