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

#include <QString>
#include <QStringList>
#include <QDebug>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <logsservices.h>

int main(int argc, char *argv[])
{
    qDebug() << "LogsServicesTester entry";
    LogsServices::LogsView viewIndex = LogsServices::ViewAll;
    bool showDialpad = false;
    for ( int i = 0; i< argc; i++ ){
        qDebug() << "Arg" << ( i + 1 ) << argv[i];
        QString argStr( argv[i] );
        if ( argStr.length() == 1 ){
            viewIndex = (LogsServices::LogsView)argStr.toInt();
        }
        if ( argStr == QString("-dialpad") ){
            showDialpad = true;
        }
    }
    
    HbApplication app(argc, argv);
    HbMainWindow mainWindow;
    mainWindow.show();
    
    qDebug() << "LogsServicesTester start logs";    
    LogsServices::start( viewIndex, showDialpad );
    
    int ret = app.exec();
    qDebug() << "LogsServicesTester exit";
    return ret;
}
