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

#ifndef CNTHEADINGWIDGET_H
#define CNTHEADINGWIDGET_H

#include <QObject>
#include <qmobilityglobal.h>
#include <hbwidget.h>
#include <hbgesturefilter.h>
#include <hbgesture.h>


class HbTouchArea;
class HbIconItem;
class HbTextItem;
class HbFrameItem;
class HbMarqueeItem;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntContactCardHeadingItem : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY( QString first_line_text READ getFirstLineText )
    Q_PROPERTY( QString primary_text READ getPrimaryText )
    Q_PROPERTY( QString second_line_text READ getSecondLineText )
    Q_PROPERTY( QString secondary_text READ getSecondaryText )
    Q_PROPERTY( QString marquee_item READ getTinyMarqueeText )
    Q_PROPERTY( HbIcon icon READ getIcon )

public:
    CntContactCardHeadingItem(QGraphicsItem *parent = 0);
    ~CntContactCardHeadingItem();

public:
    void createPrimitives();
    void recreatePrimitives();
    void updatePrimitives();
    void setDetails(const QContact* contact);
    void setIcon(const HbIcon newIcon);
    void setGroupDetails(const QContact* contact);

signals:
    void clicked();
    void passLongPressed(const QPointF &point);
    
public slots:
    void processLongPress(const QPointF &point);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    void initGesture();
    
#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    bool isNickName(const QContact* contact);
    bool isCompanyName(const QContact* contact);

    QString getFirstLineText() const { return firstLineText; }
    QString getPrimaryText() const { return primaryText; }
    QString getSecondLineText() const { return secondLineText; }
    QString getSecondaryText() const { return secondaryText; }
    QString getTinyMarqueeText() const { return tinyMarqueeText; }

    HbIcon getIcon() const { return icon; }

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    HbIconItem              *mIcon;
    HbTextItem              *mFirstLineText;
    HbTextItem              *mPrimaryText;
    HbTextItem              *mSecondLineText;
    HbTextItem              *mSecondaryText;
    HbMarqueeItem           *mMarqueeItem;
    HbFrameItem             *mFrameItem;
    HbGestureSceneFilter    *mGestureFilter;
    HbGesture               *mGestureLongpressed;
    HbTouchArea             *mPictureArea;
    
    QString                 firstLineText;
    QString                 primaryText;
    QString                 secondLineText;
    QString                 secondaryText;
    QString                 tinyMarqueeText;
    HbIcon                  icon;
};

#endif // CNTHEADINGWIDGET_H
