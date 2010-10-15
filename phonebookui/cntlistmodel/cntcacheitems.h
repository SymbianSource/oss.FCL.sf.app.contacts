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
* Description: Classes for each of the items (name, info and icon) in cache.
*
*/

#ifndef CNTCACHEITEMS_H
#define CNTCACHEITEMS_H

#include <HbIcon>
#include <qtcontacts.h>
#include <cntuids.h>

QTM_USE_NAMESPACE

class CntNameCacheItem
{
public:
    CntNameCacheItem(QContactLocalId id, const QString &firstName, const QString &lastName, CntNameOrder nameFormat);
    ~CntNameCacheItem();
    QContactLocalId contactId() const { return mContactId; }
    QString name() const { return mName; }
    QString firstName() const;
    QString lastName() const;
    void setNameFormat(CntNameOrder newFormat);
    void operator=(const CntNameCacheItem &other);
    void externalize(QDataStream &stream);
    static CntNameCacheItem* internalize(QDataStream &stream, CntNameOrder nameFormat);

private:
    void setFormattedName(const QString &firstName, const QString &lastName, CntNameOrder nameFormat);

private:
    QContactLocalId mContactId; // database contact id for this name
    int mFirstNamePosition;     // length << 16 | offset
    int mLastNamePosition;      // length << 16 | offset
    QString mName;              // formatted name
};

class CntInfoCacheItem
{
public:
    QContactLocalId contactId;
    QString text;
    QString icons[2];
    QTime lastRequest;
};

class CntIconCacheItem
{
public:
    QString iconName;
    HbIcon icon;
    QList<QContactLocalId> requestedBy;
    QTime lastRequest;
};

#endif
