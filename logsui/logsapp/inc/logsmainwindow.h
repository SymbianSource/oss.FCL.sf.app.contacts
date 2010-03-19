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
#ifndef LOGSMAINWINDOW_H
#define LOGSMAINWINDOW_H


#include <QObject>
#include <hbmainwindow.h>


class LogsMainWindow : public HbMainWindow
    {
    Q_OBJECT
    
public:
    
    LogsMainWindow();
    virtual ~LogsMainWindow();
    
    void sendAppToBackground();
    
protected: // From HbMainWindow
    
    void keyPressEvent( QKeyEvent *event );
    bool event(QEvent *event);
    
signals:
        
    void callKeyPressed();
    void appFocusGained();
    
private:
    
    friend class UT_LogsMainWindow;
    
    };


#endif //LOGSMAINWINDOW_H
