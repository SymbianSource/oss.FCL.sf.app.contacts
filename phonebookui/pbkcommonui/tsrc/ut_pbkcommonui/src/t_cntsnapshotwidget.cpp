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

#include <QtTest/QtTest>
#include <QObject>

#include <QGraphicsView>
#include <hbview.h>

#include "cntsnapshotwidget.h"
#include "t_cntsnapshotwidget.h"


void TestCntSnapshotWidget::initTestCase()
{
    mSnap = 0;
}

void TestCntSnapshotWidget::create()
{
    mSnap = new CntSnapshotWidget();
}

void TestCntSnapshotWidget::capture()
{
    delete mSnap;
    mSnap = 0;
    
    create();
    
    QGraphicsView *graphicsView = new QGraphicsView();
    HbView *view = new HbView();
    
    mSnap->capture(graphicsView, view);
    QVERIFY(mSnap->mSnapshot);
    
    mSnap->capture(graphicsView, view);
    QVERIFY(mSnap->mSnapshot);
    
    delete graphicsView;
    delete view;
}

void TestCntSnapshotWidget::cleanupTestCase()
{
    delete mSnap;
}

// EOF
