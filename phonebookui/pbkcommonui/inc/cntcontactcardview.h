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

#ifndef CNTCOMMLAUNCHERVIEW_H
#define CNTCOMMLAUNCHERVIEW_H

#include <QObject>
#include "cntbaseview.h"
#include "qtpbkglobal.h"

class QGraphicsWidget;
class HbScrollArea;
class QGraphicsLinearLayout;
class CntContactCardDataContainer;
class CntContactCardHeadingItem;
class ThumbnailManager;
class CntContactCardDetailItem;
QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class QTPBK_EXPORT CntContactCardView : public CntBaseView
{
    Q_OBJECT

public slots:
    virtual void aboutToCloseView();
    void onItemActivated();
    virtual void editContact();
	void viewHistory();
    void onLongPressed(const QPointF &aCoords);
    void setPreferredAction(const QString &aAction, const QContactDetail &aDetail);
    void thumbnailReady(const QPixmap& pixmap, void *data, int id, int error);
	void keyPressEvent(QKeyEvent *event);
	
private slots:
    void sendBusinessCard();
    void addToGroup();
    void deleteContact();
    void handleExecutedCommand(QString command, QContact contact);

public:
    CntContactCardView(CntViewManager *viewManager, QGraphicsItem *parent = 0);
    ~CntContactCardView();

    CntViewParameters::ViewId viewId() const { return CntViewParameters::commLauncherView; }
    virtual void activateView(const CntViewParameters &viewParameters);

signals:
    void preferredUpdated();

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    virtual void addActionsToToolBar();
    virtual void addMenuItems();

#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    HbScrollArea                *mScrollArea;
    QGraphicsWidget             *mContainerWidget;
    QGraphicsLinearLayout       *mContainerLayout;
    QContact                    *mContact;
    QGraphicsWidget             *mDetailsWidget;
    CntContactCardDataContainer *mDataContainer;
    CntContactCardHeadingItem   *mHeadingItem;
    ThumbnailManager            *mThumbnailManager;
    QContact                    *mGroupContact;
    bool                         mIsGroupMember;
    QMap<QString, CntContactCardDetailItem*> mPreferredItems;
};

#endif // CNTCOMMLAUNCHERVIEW_H

// EOF
