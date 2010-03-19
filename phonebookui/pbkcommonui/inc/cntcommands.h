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

#ifndef CNTCOMMANDS_H
#define CNTCOMMANDS_H

#include <QObject>
#include <qtcontacts.h>

#include "cntviewmanager.h"

QTM_USE_NAMESPACE

class CntCommands : public QObject
{
    Q_OBJECT

public:
    CntCommands(CntViewManager &viewManger,
                QContactManager* contactManager,
                QContactManager* contactSimManager,
                QObject *aParent = 0);
    ~CntCommands();

public slots:
    void newContact();
    void importFromSim();
    void editContact(QContact contact);
    void deleteContact(QContact contact);
    void openContact(QContact contact);
    void viewHistory(QContact contact);
    
    void launchDialer();

    void progress(QContactAction::Status status, const QVariantMap& result);

signals:
    void commandExecuted(QString command, QContact contact);

public:
    void launchAction(QContact contact, QContactDetail detail, QString action);

#ifdef PBK_UNIT_TEST
public:
#else
private:
#endif
    CntViewManager  &mViewManager;
    QContactAction  *mContactAction;
    QContactManager *mContactManager;
    QContactManager *mContactSimManager;
};

#endif // CNTCOMMANDS_H
