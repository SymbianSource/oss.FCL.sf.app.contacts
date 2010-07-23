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

#include <hbapplication.h>
#include <hbmainwindow.h>
#include <QTranslator>
#include <QLocale>
#include "cntserviceproviderold.h"
#include "cntserviceproviderold2.h"
#include "cntserviceproviderfetch.h"
#include "cntserviceproviderold2.h"
#include "cntserviceproviderviewing.h"
#include "cntserviceprovideredit.h"
#include "cntservices.h"
#include "cntserviceviewmanager.h"
#include "cntviewnavigator.h"
#include "cntdebug.h"
#include <cntviewparams.h>

int main(int argc, char **argv)
{
    qInstallMsgHandler(MSG_HANDLER);
    
    HbApplication a( argc, argv );

    QTranslator translator;
    QString lang = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
    translator.load(path + "contacts_" + lang);
    a.installTranslator(&translator);

    HbMainWindow mainWindow;

    CntViewNavigator* navigator = new CntViewNavigator( &mainWindow );
    navigator->addException( serviceEditView, noView );
    navigator->addEffect( serviceContactCardView, historyView );
    navigator->addEffect( historyView, serviceContactCardView );

    // This object actually executes the services
    CntServices* services = new CntServices();
    services->setParent( &mainWindow ); // for ownership

    CntServiceViewManager* viewManager = new CntServiceViewManager(
        &mainWindow,
        *services ); // as CntAbstractServiceProvider
    viewManager->setViewNavigator( navigator );

    services->setViewManager( *viewManager );

    // These objects talk with QT Highway (send/receive)
    CntServiceProviderOld* serviceProviderOld = new CntServiceProviderOld( *services, &mainWindow ); // com.nokia.services.phonebookservices.Fetch
    CntServiceProviderOld2* serviceProviderOld2 = new CntServiceProviderOld2( *services, &mainWindow ); // phonebookservices.Fetch
    CntServiceProviderFetch* serviceProviderFetch = new CntServiceProviderFetch( *services, &mainWindow ); // phonebookservices.com.nokia.symbian.IContactFetch
    CntServiceProviderViewing* serviceProviderViewing = new CntServiceProviderViewing( *services, &mainWindow ); // phonebookservices.com.nokia.symbian.IContactView
    CntServiceProviderEdit* serviceProviderEdit = new CntServiceProviderEdit( *services, &mainWindow ); // phonebookservices.com.nokia.symbian.IContactEdit

    mainWindow.show();

    return a.exec();
}

