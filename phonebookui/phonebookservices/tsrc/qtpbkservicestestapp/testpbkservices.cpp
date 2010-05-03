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
    mSndFetch=0;
    mSndEdit=0;
    mMainWindow=aParent;
    setParent(aParent);

}

testPbkServices::~testPbkServices()
{

}

void testPbkServices::launchEditorNumber()
{
    if (mSndEdit)
        {
        delete mSndEdit;
        mSndEdit=0;
        }

    mSndEdit = new XQServiceRequest("com.nokia.services.phonebookservices.Fetch", "editCreateNew(QString,QString)", false);
    connect(mSndEdit, SIGNAL(requestCompleted(QVariant)), this, SLOT(onEditCompleted(QVariant)));

    QString type = QContactPhoneNumber::DefinitionName;
    *mSndEdit << type;
    *mSndEdit << "1234567";

    QVariant retValue;
    bool res=mSndEdit->send(retValue);
}

void testPbkServices::launchEditorEmail()
{
    if (mSndEdit)
        {
        delete mSndEdit;
        mSndEdit=0;
        }

    mSndEdit = new XQServiceRequest("com.nokia.services.phonebookservices.Fetch", "editCreateNew(QString,QString)", false);
    connect(mSndEdit, SIGNAL(requestCompleted(QVariant)), this, SLOT(onEditCompleted(QVariant)));

    QString type = QContactEmailAddress::DefinitionName;
    *mSndEdit << type;
    *mSndEdit << "email@mailprovider.com";

    QVariant retValue;
    bool res=mSndEdit->send(retValue);
}

void testPbkServices::launchEditorOnlineAccount()
{
    if (mSndEdit)
        {
        delete mSndEdit;
        mSndEdit=0;
        }

    mSndEdit = new XQServiceRequest("com.nokia.services.phonebookservices.Fetch", "editCreateNew(QString,QString)", false);
    connect(mSndEdit, SIGNAL(requestCompleted(QVariant)), this, SLOT(onEditCompleted(QVariant)));

    QString type = QContactOnlineAccount::DefinitionName;
    *mSndEdit << type;
    *mSndEdit << "account@provider.com";

    QVariant retValue;
    bool res=mSndEdit->send(retValue);
}

void testPbkServices::launchEditorVCard()
{
    if (mSndEdit)
        {
        delete mSndEdit;
        mSndEdit=0;
        }

    mSndEdit = new XQServiceRequest("com.nokia.services.phonebookservices.Fetch", "editCreateNew(QString)", false);
    connect(mSndEdit, SIGNAL(requestCompleted(QVariant)), this, SLOT(onEditCompleted(QVariant)));

    QString filename("C:\\data\\Others\\testvcard.vcf");
    *mSndEdit << filename;

    QVariant retValue;
    bool res=mSndEdit->send(retValue);
}

void testPbkServices::launchUpdateEditorNumber()
{
    if (mSndEdit)
        {
        delete mSndEdit;
        mSndEdit=0;
        }

    mSndEdit = new XQServiceRequest("com.nokia.services.phonebookservices.Fetch", "editUpdateExisting(QString,QString)", false);
    connect(mSndEdit, SIGNAL(requestCompleted(QVariant)), this, SLOT(onEditCompleted(QVariant)));

    QString type = QContactPhoneNumber::DefinitionName;
    *mSndEdit << type;
    *mSndEdit << "1234567";

    QVariant retValue;
    bool res=mSndEdit->send(retValue);
}

void testPbkServices::launchUpdateEditorEmail()
{
    if (mSndEdit)
        {
        delete mSndEdit;
        mSndEdit=0;
        }

    mSndEdit = new XQServiceRequest("com.nokia.services.phonebookservices.Fetch", "editUpdateExisting(QString,QString)", false);
    connect(mSndEdit, SIGNAL(requestCompleted(QVariant)), this, SLOT(onEditCompleted(QVariant)));

    QString type = QContactEmailAddress::DefinitionName;
    *mSndEdit << type;
    *mSndEdit << "email@mailprovider.com";

    QVariant retValue;
    bool res=mSndEdit->send(retValue);
}

void testPbkServices::launchUpdateEditorOnlineAccount()
{
    if (mSndEdit)
        {
        delete mSndEdit;
        mSndEdit=0;
        }

    mSndEdit = new XQServiceRequest("com.nokia.services.phonebookservices.Fetch", "editUpdateExisting(QString,QString)", false);
    connect(mSndEdit, SIGNAL(requestCompleted(QVariant)), this, SLOT(onEditCompleted(QVariant)));

    QString type = QContactOnlineAccount::DefinitionName;
    *mSndEdit << type;
    *mSndEdit << "account@provider.com";

    QVariant retValue;
    bool res=mSndEdit->send(retValue);
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

    if (mSndFetch)
        {
        delete mSndFetch;
        mSndFetch=0;
        }

    //XQServiceRequest snd("com.nokia.services.hbserviceprovider.Dialer","dial(QString)",true);
    mSndFetch = new XQServiceRequest("com.nokia.services.phonebookservices.Fetch", "fetch(QString,QString,QString)",false);
    connect(mSndFetch, SIGNAL(requestCompleted(QVariant)), this, SLOT(onRequestCompleted(QVariant)));

    *mSndFetch << "Non-filtered multi-fetch";
    *mSndFetch << KCntActionAll;
    *mSndFetch << KCntFilterDisplayAll;

    QVariant retValue;
    bool res=mSndFetch->send(retValue);

}

void testPbkServices::launchSingleFetch()
{
    if (mSndFetch)
        {
        delete mSndFetch;
        mSndFetch=0;
        }

    mSndFetch = new XQServiceRequest("com.nokia.services.phonebookservices.Fetch", "Dofetch(QString,QString,QString,QString)",false);
    connect(mSndFetch, SIGNAL(requestCompleted(QVariant)), this, SLOT(onRequestCompleted(QVariant)));

    *mSndFetch << "Single-fetching";
    *mSndFetch << KCntActionAll;
    *mSndFetch << KCntFilterDisplayAll;
    *mSndFetch << KCntSingleSelectionMode;

    QVariant retValue;
    bool res=mSndFetch->send(retValue);
}

void testPbkServices::launchSmsFilteredFetch()
{

    if (mSndFetch)
        {
        delete mSndFetch;
        mSndFetch=0;
        }

    //XQServiceRequest snd("com.nokia.services.hbserviceprovider.Dialer","dial(QString)",true);
    mSndFetch = new XQServiceRequest("com.nokia.services.phonebookservices.Fetch", "fetch(QString,QString,QString)",false);
    connect(mSndFetch, SIGNAL(requestCompleted(QVariant)), this, SLOT(onRequestCompleted(QVariant)));

    *mSndFetch << "Filtered multi-fetch";
    *mSndFetch << KCntActionSms;
    *mSndFetch << KCntFilterDisplayAll;

    QVariant retValue;
    bool res=mSndFetch->send(retValue);
}

void testPbkServices::launchEmailFilteredFetch()
{

    if (mSndFetch)
        {
        delete mSndFetch;
        mSndFetch=0;
        }
    mSndFetch = new XQServiceRequest("com.nokia.services.phonebookservices.Fetch", "fetch(QString,QString,QString)",false);
    connect(mSndFetch, SIGNAL(requestCompleted(QVariant)), this, SLOT(onRequestCompleted(QVariant)));

    *mSndFetch << "Filtered multi-fetch";
    *mSndFetch << KCntActionEmail;
    *mSndFetch << KCntFilterDisplayAll;

    QVariant retValue;
    bool res=mSndFetch->send(retValue);
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
