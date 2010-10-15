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
* Description: Private data and helper classes used by class CntCache.
*
*/

#ifndef CNTICONFETCHER_H
#define CNTICONFETCHER_H

#include <HbIcon>
#include <cntabstractfetcher.h>

class ThumbnailManager;

QTM_USE_NAMESPACE

class CntIconJob : public CntAbstractJob
{
public:
    CntIconJob(QString iconName) { this->iconName = iconName; }
    bool isEmpty() { return iconName.isEmpty(); }
    bool equals(const CntAbstractJob &other) { const CntIconJob *o = static_cast<const CntIconJob *>(&other); return (iconName == o->iconName); }
    QString toString() { return QString("iconName = '%1'").arg(iconName); }

public:
    QString iconName;               // set by requester
    HbIcon icon;                    // set by icon fetcher
};

class CntIconFetcher : public CntAbstractFetcher
{
    Q_OBJECT
public:
    CntIconFetcher();
    ~CntIconFetcher();

    bool isProcessingJob();
    void processNextJob();

signals:
    void iconFetched(const QString &iconName, const HbIcon &icon);
    void iconCancelled(const QString &iconName);

private slots:
    void forwardIconToClient(const QPixmap &pixmap, void *data, int id, int error);

private:
    ThumbnailManager *mThumbnailManager;                    // manager that fetches the icons; owned
    int mIconRequestId;                                     // the id of the last request to thumbnail manager
    QString mIconRequestName;                               // the name of the icon last requested from thumbnail manager

    friend class TestIconFetcher;
};

#endif
