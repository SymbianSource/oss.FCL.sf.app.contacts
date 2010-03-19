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

#ifndef CNTEDITVIEWDETAILITEM_H
#define CNTEDITVIEWDETAILITEM_H

#include <QObject>
#include <hbwidget.h>
#include <qcontactdetails.h>
#include "cntstringmapper.h"

class HbIconItem;
class HbTextItem;
class QGraphicsGridLayout;
class HbFrameItem;
class HbGestureSceneFilter;
class HbGesture;

class CntEditViewDetailItem : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY( QString text READ getText )
    Q_PROPERTY( QString valueText READ getValueText )
    Q_PROPERTY( HbIcon icon READ getIcon )

public:
    CntEditViewDetailItem(QGraphicsItem *parent = 0);
    ~CntEditViewDetailItem();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
    void initGesture();
    void createPrimitives();
    void recreatePrimitives();
    void updatePrimitives();
    void setDetail(const QContactDetail &detail, const QString &type = QString());
    QContactDetail detail();
    QString fieldType();

    QString getText() const { return text; }
    QString getValueText() const { return valueText; }
    HbIcon getIcon() const { return icon; }

public slots:
    void onLongPress(const QPointF &point);

signals:
    void clicked();
    void longPressed(const QPointF &point);

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    HbIconItem              *mIcon;
    HbTextItem              *mLabel;
    HbTextItem              *mContent;
    QContactDetail           mDetail;
    QString                  mType;
    HbFrameItem             *mFrameItem;
    HbFrameItem             *mFocusItem;
    bool                     mHasFocus;
    HbGestureSceneFilter    *mGestureFilter;
    HbGesture               *mGestureLongpressed;
    CntStringMapper          mStringMapper;

    QString                  text;
    QString                  valueText;
    HbIcon                   icon;

};

#endif // CNTEDITVIEWDETAILITEM_H

