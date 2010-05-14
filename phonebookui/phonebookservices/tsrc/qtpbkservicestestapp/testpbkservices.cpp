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

#include "testpbkservices.h"
#include <cntservicescontact.h>
#include <hbmessagebox.h>
#include <hbdialog.h>
#include <hblistwidget.h>
#include <hblabel.h>
#include <mobcntmodel.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <qtcontacts.h>
#include <hbview.h>
#include <QGraphicsLinearLayout>
#include <QDebug>


testPbkServices::testPbkServices(HbMainWindow *aParent)
{
    mRequest=NULL;
    mMainWindow=aParent;
    setParent(aParent);

}

testPbkServices::~testPbkServices()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest=0;
    }
}

void testPbkServices::launchEditorNumber()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest=0;
    }

    QVariantList args; 
    QString serviceName("com.nokia.services.phonebookservices"); 
    QString operation("editCreateNew(QString,QString)");
    XQApplicationManager appMng;
    mRequest = appMng.create(serviceName, "Fetch", operation, true); // embedded 
    
    // Result handlers 
    connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onEditCompleted(const QVariant&)));
    
    args << QContactPhoneNumber::DefinitionName.operator QVariant();
    args << "1234567";
    
    mRequest->setArguments(args); 
    mRequest->send();
}

void testPbkServices::launchEditorEmail()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest=0;
    }

    QVariantList args; 
    QString serviceName("com.nokia.services.phonebookservices"); 
    QString operation("editCreateNew(QString,QString)");
    XQApplicationManager appMng;
    mRequest = appMng.create(serviceName, "Fetch", operation, true); // embedded 
    
    // Result handlers 
    connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onEditCompleted(const QVariant&)));
    
    args << QContactEmailAddress::DefinitionName.operator QVariant();
    args << "email@mailprovider.com";
    
    mRequest->setArguments(args); 
    mRequest->send();
}

void testPbkServices::launchEditorOnlineAccount()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest=0;
    }

    QVariantList args; 
    QString serviceName("com.nokia.services.phonebookservices"); 
    QString operation("editCreateNew(QString,QString)");
    XQApplicationManager appMng;
    mRequest = appMng.create(serviceName, "Fetch", operation, true); // embedded 
    
    // Result handlers 
    connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onEditCompleted(const QVariant&)));
    
    args << QContactOnlineAccount::DefinitionName.operator QVariant();
    args << "account@provider.com";
    
    mRequest->setArguments(args); 
    mRequest->send();
}

void testPbkServices::launchEditorVCard()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest=0;
    }

    QVariantList args; 
    QString serviceName("com.nokia.services.phonebookservices"); 
    QString operation("editCreateNew(QString)");
    XQApplicationManager appMng;
    mRequest = appMng.create(serviceName, "Fetch", operation, true); // embedded 
    
    // Result handlers 
    connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onEditCompleted(const QVariant&)));
    
    args << "C:\\data\\Others\\testvcard.vcf";
    
    mRequest->setArguments(args); 
    mRequest->send();
}

void testPbkServices::launchUpdateEditorNumber()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest=0;
    }

    QVariantList args; 
    QString serviceName("com.nokia.services.phonebookservices"); 
    QString operation("editUpdateExisting(QString,QString)");
    XQApplicationManager appMng;
    mRequest = appMng.create(serviceName, "Fetch", operation, true); // embedded 
    
    // Result handlers 
    connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onEditCompleted(const QVariant&)));
    
    args << QContactPhoneNumber::DefinitionName.operator QVariant();
    args << "1234567";
    
    mRequest->setArguments(args); 
    mRequest->send();
}

void testPbkServices::launchUpdateEditorEmail()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest=0;
    }

    QVariantList args; 
    QString serviceName("com.nokia.services.phonebookservices"); 
    QString operation("editUpdateExisting(QString,QString)");
    XQApplicationManager appMng;
    mRequest = appMng.create(serviceName, "Fetch", operation, true); // embedded 
    
    // Result handlers 
    connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onEditCompleted(const QVariant&)));
    
    args << QContactEmailAddress::DefinitionName.operator QVariant();
    args << "email@mailprovider.com";
    
    mRequest->setArguments(args); 
    mRequest->send();
}

void testPbkServices::launchUpdateEditorOnlineAccount()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest=0;
    }

    QVariantList args; 
    QString serviceName("com.nokia.services.phonebookservices"); 
    QString operation("editUpdateExisting(QString,QString)");
    XQApplicationManager appMng;
    mRequest = appMng.create(serviceName, "Fetch", operation, true); // embedded 
    
    // Result handlers 
    connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onEditCompleted(const QVariant&)));
    
    args << QContactOnlineAccount::DefinitionName.operator QVariant();
    args << "account@provider.com";
    
    mRequest->setArguments(args); 
    mRequest->send();
}

void testPbkServices::onEditCompleted(const QVariant& value)
{
    mMainWindow->activateWindow();
    
    if (value.toBool())
    {
        HbMessageBox note;
        note.setTimeout(10000);
        note.setText("Contact saved");
        note.exec();
    }
    else
    {
        HbMessageBox note;
        note.setTimeout(10000);
        note.setText("Contact saving failed");
        note.exec();
    }
}

void testPbkServices::launchFetch()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest=0;
    }

    QVariantList args; 
    QString serviceName("com.nokia.services.phonebookservices"); 
    QString operation("fetch(QString,QString,QString)");
    XQApplicationManager appMng;
    mRequest = appMng.create(serviceName, "Fetch", operation, true); // embedded 
    
    // Result handlers 
    connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onRequestCompleted(const QVariant&)));
    
    args << "Non-filtered multi-fetch";
    args << KCntActionAll; 
    args << KCntFilterDisplayAll; 
    
    mRequest->setArguments(args); 
    mRequest->send();
}

void testPbkServices::launchSingleFetch()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest=0;
    }

    QVariantList args; 
    QString serviceName("com.nokia.services.phonebookservices"); 
    QString operation("Dofetch(QString,QString,QString,QString)");
    XQApplicationManager appMng;
    mRequest = appMng.create(serviceName, "Fetch", operation, true); // embedded 
    
    // Result handlers 
    connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onRequestCompleted(const QVariant&)));
    
    args << "Single-fetching";
    args << KCntActionAll; 
    args << KCntFilterDisplayAll;
    args << KCntSingleSelectionMode;
    
    mRequest->setArguments(args); 
    mRequest->send();
}

void testPbkServices::launchSmsFilteredFetch()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest=0;
    }

    QVariantList args; 
    QString serviceName("com.nokia.services.phonebookservices"); 
    QString operation("fetch(QString,QString,QString)");
    XQApplicationManager appMng;
    mRequest = appMng.create(serviceName, "Fetch", operation, true); // embedded 
    
    // Result handlers 
    connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onRequestCompleted(const QVariant&)));
    
    args << "Filtered multi-fetch";
    args << KCntActionSms; 
    args << KCntFilterDisplayAll;
    
    mRequest->setArguments(args); 
    mRequest->send();
}

void testPbkServices::launchEmailFilteredFetch()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest=0;
    }

    QVariantList args; 
    QString serviceName("com.nokia.services.phonebookservices"); 
    QString operation("fetch(QString,QString,QString)");
    XQApplicationManager appMng;
    mRequest = appMng.create(serviceName, "Fetch", operation, true); // embedded 
    
    // Result handlers 
    connect (mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(onRequestCompleted(const QVariant&)));
    
    args << "Filtered multi-fetch";
    args << KCntActionEmail; 
    args << KCntFilterDisplayAll;
    
    mRequest->setArguments(args); 
    mRequest->send();
}



void testPbkServices::onRequestCompleted(const QVariant& value)
{

    mMainWindow->activateWindow();

    CntServicesContactList retValue;
    retValue=qVariantValue<CntServicesContactList>(value);


    if (retValue.count() == 0)
    {
        HbMessageBox note;
        note.setTimeout(10000);
        note.setText("Nothing returned");
        note.exec();
    }

    else
    {
        HbListWidget *listWidget = new HbListWidget();
        for(int i = 0; i < retValue.count(); i++ )
            {
            QString name      = retValue[i].mDisplayName;
            listWidget->addItem("name:" );
            listWidget->addItem(name);

            QString number    = retValue[i].mPhoneNumber;
            if (number!="")
                {
                listWidget->addItem("number:");
                listWidget->addItem(number);
                }

            QString emailAddress    = retValue[i].mEmailAddress;
            if (emailAddress!="")
                {
                listWidget->addItem("emailAddress:");
                listWidget->addItem(emailAddress);
                }


            QString contactId = QString("id: %1").arg(retValue[i].mContactId);
            listWidget->addItem( contactId );

            }
            HbDialog popup;

            // Set dismiss policy that determines what tap events will cause the popup
            // to be dismissed
            popup.setDismissPolicy(HbDialog::NoDismiss);

            // Set the label as heading widget
            popup.setHeadingWidget(new HbLabel(tr("Contact")));

            // Set a list widget as content widget in the popup
            popup.setContentWidget(listWidget);

            // Sets the primary action and secondary action
            //popup.setPrimaryAction(new HbAction(tr("Ok"),&popup));
            //popup.setSecondaryAction(new HbAction(tr("Cancel"),&popup));

            // Launch popup syncronously
            popup.setTimeout(15000);
            popup.exec();
    }

}


Q_IMPLEMENT_USER_METATYPE(CntServicesContact)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(CntServicesContactList)
