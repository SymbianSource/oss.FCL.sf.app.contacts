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




#include "cntmainwindow.h"
#include "cntviewmanager.h"
#include "cntviewparameters.h"
 #include <QCoreApplication>

CntMainWindow::CntMainWindow(QWidget *parent, CntViewParameters::ViewId defaultView)
    : HbMainWindow(parent),mViewManager(0)
{
    if (defaultView != CntViewParameters::noView)
    {
        mViewManager = new CntViewManager(this,defaultView);
    }
}

CntMainWindow::~CntMainWindow()
{
    // TODO: Remove
    // This is done because the HbMainWindow bug in wk04 platform. Next release, this must be removed.
    // Another fix is in main.cpp (tsrc) where TestCntBaseSelectionView test was removed due KERN-EXEC 3.
    // The KERN-EXEC 3 is caused by the following QCoreApplication::processEvents. Don't know why.
    QCoreApplication::processEvents(); 
    delete mViewManager;
    mViewManager=0;
}

/*
 * Lets let views handle send/end key press events, because if toolbar or menu has been focused
 * the view wont get the key press events
 */
void CntMainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Yes || event->key() == Qt::Key_No)
    {
        emit keyPressed(event);
    }
    else
    {
        HbMainWindow::keyPressEvent(event);
    }
}


//

// end of file
