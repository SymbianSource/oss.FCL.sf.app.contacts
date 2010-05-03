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

#ifndef TESTPBKSERVICES_H
#define TESTPBKSERVICES_H

#include <QObject>
#include <QVariant>

#include <qmobilityglobal.h> //defines Q_SFW_EXPORT
#include <xqservicerequest.h>

class HbMainWindow;

class testPbkServices : public QObject
{
    Q_OBJECT

public slots:
    void launchFetch();
    void launchSingleFetch();
    void launchSmsFilteredFetch();
    void launchEmailFilteredFetch();
    void launchEditorNumber();
    void launchEditorEmail();
    void launchEditorOnlineAccount();
    void launchEditorVCard();
    void launchUpdateEditorNumber();
    void launchUpdateEditorEmail();
    void launchUpdateEditorOnlineAccount();
    void onEditCompleted(const QVariant& value);
    void onRequestCompleted(const QVariant& value);

public:
    testPbkServices(HbMainWindow *aParent);
    ~testPbkServices();

private:

    XQServiceRequest    *mSndFetch;
    XQServiceRequest    *mSndEdit;
    HbMainWindow        *mMainWindow;

};

#endif
