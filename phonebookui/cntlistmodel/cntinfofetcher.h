/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Fetches visual info about contacts.
*
*/

#ifndef CNTINFOFETCHER_H
#define CNTINFOFETCHER_H

#include <qcontactmanager.h>
#include <qcontactid.h>
#include <cntinfoprovider.h>
#include <cntabstractfetcher.h>

QTM_USE_NAMESPACE

class CntInfoJob : public CntAbstractJob
{
public:
    CntInfoJob(QContactLocalId contactId) { this->contactId = contactId; }
    virtual bool isEmpty() { return (contactId == 0); }
    virtual bool equals(const CntAbstractJob &other) { return (contactId == static_cast<const CntInfoJob *>(&other)->contactId); }
    QString toString() { return QString("cid = %1").arg(contactId); }

public:
    QContactLocalId contactId;
};

class CntInfoFetcher : public CntAbstractFetcher
{
    Q_OBJECT
public:
    CntInfoFetcher(QContactManager *contactManager);
    ~CntInfoFetcher();

    bool isProcessingJob();
    void processNextJob();

signals:
    void infoUpdated(QContactLocalId contactId, ContactInfoField field, const QString &text);
    void infoCancelled(QContactLocalId contactId);

private slots:
    void forwardInfoToClient(CntInfoProvider* sender, int contactId,
                             ContactInfoField field, const QString& text);

private:
    QContactManager *mContactManager;                           // manager that fetches QContact objects; not owned
    QMap<CntInfoProvider *, ContactInfoFields> mInfoProviders;   // maps info providers to their responsibilities

    friend class TestInfoFetcher;
};

#endif
