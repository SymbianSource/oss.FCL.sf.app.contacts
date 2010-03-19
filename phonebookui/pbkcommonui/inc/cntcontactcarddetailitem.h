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

#ifndef CNTCOMMLAUNCHERDETAILITEM_H
#define CNTCOMMLAUNCHERDETAILITEM_H

#include <QObject>
#include <hbwidget.h>

class HbIconItem;
class HbTextItem;
class HbRichTextItem;
class QGraphicsGridLayout;
class HbFrameItem;
class HbGestureSceneFilter;
class HbGesture;

class CntContactCardDetailItem : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY( QString text READ getText )
    Q_PROPERTY( QString primaryText READ getPrimaryText )
    Q_PROPERTY( QString valueText READ getValueText )
    Q_PROPERTY( HbIcon icon READ getIcon )

public:
    CntContactCardDetailItem(int index, QGraphicsItem *parent = 0, bool isFocusable = true);
    ~CntContactCardDetailItem();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
    void initGesture();
    void createPrimitives();
    void recreatePrimitives();
    void updatePrimitives();
    void setDetails(const HbIcon aIcon, const QString& aText, const QString& aValueText, Qt::TextElideMode valueTextElideMode = Qt::ElideRight, bool underLine = false);
    void setDetails(const HbIcon detailIcon, const QString& detailText);
    int index();
    void setUnderLine(bool underLine);

public slots:
    void onLongPress(const QPointF &point);

signals:
    void clicked();
    void longPressed(const QPointF &point);
    
private:
    QString getText() const { return text; }
    QString getPrimaryText() const { return primaryText; }
    QString getValueText() const { return valueText; }
    HbIcon getIcon() const { return icon; }

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    HbIconItem              *mIcon;
    HbRichTextItem          *mFirstLineText;
    HbTextItem              *mPrimaryText;
    HbTextItem              *mSecondLineText;
    HbFrameItem             *mFrameItem;
    HbFrameItem             *mFocusItem;
    bool                    mHasFocus;
    HbGestureSceneFilter    *mGestureFilter;
    HbGesture               *mGestureLongpressed;
    int                     mIndex;
    bool                    mIsFocusable;
    Qt::TextElideMode       mValueTextElideMode;
    bool                    mIsUnderline;

    QString                 text;
    QString                 primaryText;
    QString                 valueText;
    HbIcon                  icon;
};

#endif // CNTCOMMLAUNCHERDETAILITEM_H

