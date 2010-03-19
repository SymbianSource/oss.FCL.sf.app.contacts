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

#include <QPainter>
#include <QImage>
#include <qgraphicsview.h>

#include "cntsnapshotwidget.h"


/*!
 Constructs the snapshot widget.
 */
CntSnapshotWidget::CntSnapshotWidget( QGraphicsItem *parent )
    : HbWidget(parent),
      mSnapshot(0)
{

}

/*!
 Destructs the snapshot widget.
 */
CntSnapshotWidget::~CntSnapshotWidget()
{
    if (mSnapshot)
     delete mSnapshot;
}

/*!
 Takes a snapshot of an \a item within \a graphicsView, also sets its position
  and geometry so it can be used as a decoy for that itme.
 */
void CntSnapshotWidget::capture(QGraphicsView *graphicsView, QGraphicsItem *item)
{
    if (mSnapshot){
        delete mSnapshot;
        mSnapshot = 0;
    }
    setGeometry(QRect(QPoint(0,0),item->sceneBoundingRect().toRect().size()));
    setPos(item->sceneBoundingRect().toRect().topLeft());
    mSnapshot = new QImage(
            item->sceneBoundingRect().toRect().size() , 
            QImage::Format_ARGB32_Premultiplied);
    QPainter p( mSnapshot);
    graphicsView->render( &p, rect(), item->sceneBoundingRect().toRect());
}

/*!
    \reimp
 */
void CntSnapshotWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)
    if (mSnapshot)
        painter->drawImage( rect(), *mSnapshot );
}
