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

#include <cntservicescontact.h>
#include <xqserviceprovider.h>
#include <cntviewparams.h>
#include <qtcontacts.h>

QTM_USE_NAMESPACE

class CntServiceHandler : public XQServiceProvider
{
    Q_OBJECT
public:
    CntServiceHandler(QObject *parent = 0);
    ~CntServiceHandler();
    
    enum CntServiceViewParams
    {
        ETitle = ECustomParam,
        EFilter
    };

public:
    void completeFetch(const CntServicesContactList &contactList);
    void completeEdit(int result);

public slots:
    // Services offered to clients
    void fetch(const QString &title,
               const QString &action,
               const QString &filter);
    
    void Dofetch(const QString &title,
                 const QString &action,
                 const QString &filter,
                 const QString &mode = KCntNoSelectionMode);

    void editCreateNew(const QString &definitionName, const QString &value);
    void editCreateNew(const QString &vCardFile);
    void editUpdateExisting(const QString &definitionName, const QString &value);
    void open(int contactId);
    void open(const QString &definitionName, const QString &value);

signals:
    void launchFetch(const QString &title,
                     const QString &action,
                     const QString &filter);
    
    void launchFetchVerified(const QString &title,
                             const QString &action,
                             const QString &filter,
                             const QString &mode);
    
    void launchEditor(QContact contact);
    void launchContactSelection(QContactDetail detail);
    void launchContactCard(QContact contact);
    void launchAssignContactCard(QContact contact, QContactDetail detail);
    
private:
    void removeFields(QContact& contact);
    void removeDetails(QContact& contact);

private:
    int mCurrentRequestIndex;
    QStringList m_definitionNames;
};

#endif /* CNTSERVICEHANDLER_H */
