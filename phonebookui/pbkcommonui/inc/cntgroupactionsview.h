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

#ifndef CNTGROUPACTIONSVIEW_H
#define CNTGROUPACTIONSVIEW_H

#include <QObject>
#include "cntbaseview.h"
#include "qtpbkglobal.h"

class QModelIndex;
class QGraphicsWidget;
class HbListView;
class HbLabel;


QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class QGraphicsWidget;
class HbScrollArea;
class QGraphicsLinearLayout;
class CntContactCardDataContainer;
class CntContactCardHeadingItem;
class HbGroupBox;

class CntGroupActionsView : public CntBaseView
{
    Q_OBJECT

public slots:

    virtual void aboutToCloseView();
    virtual void editContact();
    void editGroup();
    void addMenuItems();
       

public:
    CntGroupActionsView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntGroupActionsView();

    CntViewParameters::ViewId viewId() const { return CntViewParameters::groupActionsView; }
    virtual void activateView(const CntViewParameters &viewParameters);

signals:
    void preferredUpdated();

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    void resizeEvent(QGraphicsSceneResizeEvent *event);
       
#ifdef PBK_UNIT_TEST
public:
#else
protected:    
#endif        
    virtual void addActionsToToolBar();
        
#ifdef PBK_UNIT_TEST
public:
#else
protected:    
#endif
    QContact                    *mGroupContact;
    HbScrollArea                *mScrollArea;
    QGraphicsWidget             *mContainerWidget;
    QGraphicsLinearLayout       *mContainerLayout;
    CntContactCardDataContainer *mDataContainer;
    CntContactCardHeadingItem   *mHeadingItem;
    HbGroupBox                  *mBanner;
};

#endif // CNTGROUPACTIONSVIEW_H

// EOF
