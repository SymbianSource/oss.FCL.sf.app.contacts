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
//USER
#include "logsmainwindow.h"
#include "logsviewmanager.h"
#include "logsservicehandler.h"
#include "logslogger.h"
#include "logsdefs.h"

//SYSTEM
#include <QObject>
#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hbstyleloader.h>
#include <QTranslator>

int main(int argc, char *argv[])
{
    LOGS_QDEBUG( "logs [UI] -> main()" )

    
    HbApplication app(argc, argv);
    LogsMainWindow window;
    
    QString lang = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
    //Load common translator
    QTranslator commontranslator;
    bool returncode = false;
    LOGS_QDEBUG("logs [UI] loading common strings translator");
    returncode = commontranslator.load( path + "common_" + lang + ".qm");
    if (returncode==false) {
    	LOGS_QDEBUG("logs [UI] unable to open file: " + path + "common_" + lang + ".qm");
    } else {
    app.installTranslator(&commontranslator);
    }
    
    //Load application-specific translator
    QTranslator translator;
    LOGS_QDEBUG("logs [UI] loading application strings translator");
    LOGS_QDEBUG("logs [UI] translation filename dialer_" + lang + ".qm");
    returncode = translator.load( path + "dialer_" + lang + ".qm");
    if (returncode==false) {
    	LOGS_QDEBUG("logs [UI] .qm file not found from "+path);
    	path = ":/";
    	returncode = translator.load( path + "logs_default.qm");
    	LOGS_QDEBUG("logs [UI]: fallback to default language file logs_default.qm");
    }
       
    if (returncode==false) {
    	LOGS_QDEBUG( "logs [UI] .qm file not found from  "+path);
    } else {
    	LOGS_QDEBUG( "logs [UI] .qm loaded successfully from "+path);
    }

    app.installTranslator(&translator);    
    HbStyleLoader::registerFilePath(":/logslayouts");
    
    // Create service handler asap so that services are published fast.
    // Servicehandler cannot be created before HbApplication creation.
    // This can cause problem of service request not coming through if
    // HbApplication creation takes long time.
    LogsServiceHandler service;
    LogsViewManager viewManager(window, service);
    window.show();
    int err = app.exec();
    
    HbStyleLoader::unregisterFilePath(":/logslayouts");
    
    LOGS_QDEBUG( "logs [UI] <- main()" )
    return err;
}
