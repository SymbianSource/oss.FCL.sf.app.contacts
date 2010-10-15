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

#include "testrunner.h"

#include "ut_cntservices.h"

#include <QtTest/QtTest>

int main(int argc, char *argv[]) 
{
    bool promptOnExit(true);
    for (int i=0; i<argc; i++) {
        if (QString(argv[i]) == "-noprompt")
            promptOnExit = false;
    }
    printf("Running tests...\n");
    
    QTranslator translator;
    QString lang = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
    translator.load(path + "contacts_" + lang);
    
    Starter* starter = new Starter( argc, argv );
    starter->installTranslator(&translator); 
    starter->exec();
    
    if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }
    return 0;   
}

Starter::Starter(int &argc, char **argv)
     : HbApplication(argc, argv)
{
    mTestStarted = false;
}

Starter::~Starter()
{
}

bool Starter::event( QEvent* aEvent )
{
    if (!mTestStarted) {
        mTestStarted = true;
        postEvent(this, new QEvent(QEvent::User));
    }
    else if (aEvent->type() == QEvent::User) {
        TestRunner testRunner("ut_phonebookservices");
                              
            UT_CntServices cntServicesTest;
            testRunner.runTests( cntServicesTest );

        testRunner.printResults();
        postEvent(this, new QEvent(static_cast<QEvent::Type>(QEvent::User + 1)) );
        return true;
    }
   else if (aEvent->type() == QEvent::User + 1) {
       quit();
       return true;
    }

    return QObject::event( aEvent );
}

