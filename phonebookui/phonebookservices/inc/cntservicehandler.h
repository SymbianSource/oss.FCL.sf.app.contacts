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

#ifndef CNTSERVICEHANDLER_H
#define CNTSERVICEHANDLER_H

#include "cntservicescontact.h"
#include <xqserviceprovider.h>
#include <qtcontacts.h>

QTM_USE_NAMESPACE

class CntServiceHandler : public XQServiceProvider
{
    Q_OBJECT
public:
    CntServiceHandler(QObject *parent = 0);
    ~CntServiceHandler();

public:
    void completeFetch(const CntServicesContactList &contactList);
    void completeEdit(int result);

public slots:
    void fetch(const QString &title, const QString &action = KCntActionAll, const QString &filter = KCntFilterDisplayAll);
    void editCreateNew(const QString &definitionName, const QString &value);
    void editUpdateExisting(const QString &definitionName, const QString &value);
    void open(int contactId);
    void open(const QString &definitionName, const QString &value);

signals:
    void launchFetch(const QString &title, const QString &action, const QString &filter);
    void launchEditor(QContact contact);
    void launchContactSelection(QContactDetail detail);
    void launchContactCard(QContact contact);
    void launchAssignContactCard(QContact contact, QContactDetail detail);

private:
    int mCurrentRequestIndex;

};

#endif /* CNTSERVICEHANDLER_H */
