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
* Description: Copied from Music Player snapshot widget.
*
*/

#ifndef CNTSNAPSHOTWIDGET_H
#define CNTSNAPSHOTWIDGET_H


#include <qtglobal>
#include <hbwidget.h>


class QGraphicsView;
class QImage;

class  CntSnapshotWidget : public HbWidget
{
    Q_OBJECT

public:
    explicit CntSnapshotWidget( QGraphicsItem *parent=0 );
    virtual ~CntSnapshotWidget();
    
    void capture(QGraphicsView *graphicsView, QGraphicsItem *item);
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, 
                QWidget *widget = 0 );

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    Q_DISABLE_COPY(CntSnapshotWidget)
    QImage *mSnapshot;

};

#endif    // CNTSNAPSHOTWIDGET_H
