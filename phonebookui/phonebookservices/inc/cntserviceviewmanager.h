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

#ifndef CNTSERVICEVIEWMANAGER_H
#define CNTSERVICEVIEWMANAGER_H

#include "cntdefaultviewmanager.h"
#include <cntservicescontact.h>

#include <QObject>
#include <hbmainwindow.h>

class CntBaseView;
class CntServiceHandler;

class CntServiceViewManager : public CntDefaultViewManager
{
    Q_OBJECT

public:
    CntServiceViewManager(HbMainWindow *mainWindow, CntServiceHandler *aHandler);
    ~CntServiceViewManager();
    
public slots:
    void handleFetch(const QString &title, const QString &action, const QString &filter);
    void launchFetch(const QString &title, const QString &action, const QString &filter, const QString &mode = KCntMultiSelectionMode);
    void launchEditor(QContact contact);
    void launchContactSelection(QContactDetail detail);
    void launchContactCard(QContact contact);
    void launchAssignContactCard(QContact contact, QContactDetail detail);

private:
    CntServiceHandler *mServiceHandler;
};

#endif // CNTSERVICEVIEWMANAGER_H
