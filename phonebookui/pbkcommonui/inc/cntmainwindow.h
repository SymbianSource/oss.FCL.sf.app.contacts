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

#ifndef CNTMAINWINDOW_H
#define CNTMAINWINDOW_H

#include <hbmainwindow.h>
#include <QKeyEvent>

#include "qtpbkglobal.h"
#include <cntviewparams.h>

class CntDefaultViewManager;

class QTPBK_EXPORT CntMainWindow : public HbMainWindow
{
    Q_OBJECT

public:
    CntMainWindow(QWidget *parent=0, int defaultView = namesView);
    virtual ~CntMainWindow();

    void keyPressEvent(QKeyEvent *event);

signals:

    void keyPressed(QKeyEvent *event);

public slots:

private slots:

#ifdef PBK_UNIT_TEST
public:
#else
protected:
#endif
    CntDefaultViewManager        *mViewManager;

};

#endif // CNTMAINWINDOW_H
