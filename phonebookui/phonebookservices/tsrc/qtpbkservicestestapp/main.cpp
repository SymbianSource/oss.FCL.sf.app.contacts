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
#include <hbpushbutton.h>
#include <hbmainwindow.h>
#include <qapplication.h>
#include <hbview.h>
#include <QGraphicsLinearLayout>

#include "testpbkservices.h"

int main(int argc, char *argv[])
{
    // Initialization
    HbApplication app(argc, argv);

    // Main window widget.
    // Includes decorators such as signal strength and battery life indicator.
    HbMainWindow mainWindow;

    testPbkServices *service=new testPbkServices(&mainWindow);

    HbView* view = new HbView();
    view->setTitle("QtPhonebook Test Application");

    QGraphicsLinearLayout* mainLayout = new QGraphicsLinearLayout(Qt::Vertical);

    HbPushButton *button1 = new HbPushButton("Launch Fetch");
    QObject::connect(button1, SIGNAL(pressed()), service, SLOT(launchFetch()));

    HbPushButton *button2 = new HbPushButton("Launch SMS fetch");
    QObject::connect(button2, SIGNAL(pressed()), service, SLOT(launchSmsFilteredFetch()));

    HbPushButton *button3 = new HbPushButton("Launch EMAIL fetch");
    QObject::connect(button3, SIGNAL(pressed()), service, SLOT(launchEmailFilteredFetch()));

    HbPushButton *button4 = new HbPushButton("Launch editor with number");
    QObject::connect(button4, SIGNAL(pressed()), service, SLOT(launchEditorNumber()));
    
    HbPushButton *button5 = new HbPushButton("Launch editor with email");
    QObject::connect(button5, SIGNAL(pressed()), service, SLOT(launchEditorEmail()));
    
    HbPushButton *button6 = new HbPushButton("Launch editor with online account");
    QObject::connect(button6, SIGNAL(pressed()), service, SLOT(launchEditorOnlineAccount()));
    
    HbPushButton *button7 = new HbPushButton("Update contact with number");
    QObject::connect(button7, SIGNAL(pressed()), service, SLOT(launchUpdateEditorNumber()));
    
    HbPushButton *button8 = new HbPushButton("Update contact with email");
    QObject::connect(button8, SIGNAL(pressed()), service, SLOT(launchUpdateEditorEmail()));
    
    HbPushButton *button9 = new HbPushButton("Update contact with online account");
    QObject::connect(button9, SIGNAL(pressed()), service, SLOT(launchUpdateEditorOnlineAccount()));

    HbPushButton *button10 = new HbPushButton("Launch single fetch");
    QObject::connect(button10, SIGNAL(pressed()), service, SLOT(launchSingleFetch()));
    	
    HbPushButton *button11 = new HbPushButton("Launch editor with vCard");
    QObject::connect(button11, SIGNAL(pressed()), service, SLOT(launchEditorVCard()));

    mainLayout->addItem(button1);
    mainLayout->addItem(button2);
    mainLayout->addItem(button3);
    mainLayout->addItem(button4);
    mainLayout->addItem(button5);
    mainLayout->addItem(button6);
    mainLayout->addItem(button7);
    mainLayout->addItem(button8);
    mainLayout->addItem(button9);
    mainLayout->addItem(button10);
    mainLayout->addItem(button11);
    
    
    view->setLayout(mainLayout);

    mainWindow.addView(view);

    // Show widget
    mainWindow.show();

    // Enter event loop
    return app.exec();
}
