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

#ifndef CNTIMAGELABEL_H
#define CNTIMAGELABEL_H

#include <QObject>
#include <hblabel.h>

class QGesture;

class CntImageLabel : public HbLabel
{
    Q_OBJECT

public:
    CntImageLabel(QGraphicsItem *parent = 0);
    ~CntImageLabel();

protected:
    void gestureEvent(QGestureEvent* event);
    
signals:
    void iconClicked();
    void iconLongPressed(const QPointF&);
};

#endif // CNTEDITVIEWHEADINGITEM_H
