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

#include "logsmainwindow.h"
#include "logslogger.h"
#include <QKeyEvent>
#include <xqserviceutil.h>


// -----------------------------------------------------------------------------
// LogsMainWindow::LogsMainWindow
// -----------------------------------------------------------------------------
//
LogsMainWindow::LogsMainWindow() : HbMainWindow()
{
    if ( viewport() ){
        viewport()->grabGesture(Qt::SwipeGesture);
    }
}

// -----------------------------------------------------------------------------
// LogsMainWindow::~LogsMainWindow
// -----------------------------------------------------------------------------
//
LogsMainWindow::~LogsMainWindow()
{
    if ( viewport() ){
        viewport()->ungrabGesture(Qt::SwipeGesture);
    }
}

// -----------------------------------------------------------------------------
// LogsMainWindow::sendAppToBackground
// -----------------------------------------------------------------------------
//
void LogsMainWindow::sendAppToBackground()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMainWindow::sendAppToBackground" );
    XQServiceUtil::toBackground(true);
    LOGS_QDEBUG( "logs [UI] <- LogsMainWindow::sendAppToBackground" );
}

// -----------------------------------------------------------------------------
// LogsMainWindow::keyPressEvent
// -----------------------------------------------------------------------------
//
void LogsMainWindow::keyPressEvent( QKeyEvent *event )
{
    LOGS_QDEBUG_2( "LogsMainWindow::keyPressEvent, key", event->key() );
    if ( event->key() == Qt::Key_Call || event->key() == Qt::Key_Yes ) {
        // Handling at window level seems to be only way to avoid other
        // applications to handle call key as well.
        emit callKeyPressed();
        event->accept();
        return;
    }
    HbMainWindow::keyPressEvent(event);
}

// -----------------------------------------------------------------------------
// LogsMainWindow::event
// -----------------------------------------------------------------------------
//
bool LogsMainWindow::event(QEvent *event)
{
    if ( event->type() == QEvent::FocusIn ){
        emit appFocusGained();
    }
    return HbMainWindow::event(event);
}

