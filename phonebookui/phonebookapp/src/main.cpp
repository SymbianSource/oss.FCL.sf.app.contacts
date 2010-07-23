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

#include <QTranslator>
#include <QLocale>
#include "cntmainwindow.h"
#include "cntappservicehandler.h"
#include "cntglobal.h"
#include "cntdebug.h"
#include "cntapplication.h"

int main(int argc, char **argv)
{
    qInstallMsgHandler(MSG_HANDLER);

    CntApplication a( argc, argv );

    QTranslator translator;
    QString lang = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
    translator.load(path + "contacts_" + lang);
    a.installTranslator(&translator);
    
    CntAppServiceHandler serviceHandler;
    CntMainWindow mainWindow;

    mainWindow.show();

    return a.exec();
}

