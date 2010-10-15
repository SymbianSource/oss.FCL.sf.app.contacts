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

#ifndef CNTNAMEFETCHER_H
#define CNTNAMEFETCHER_H

#include <QObject>
#include <QThread>
#include <QDataStream>
#include <QEvent>
#include <qcontactid.h>
#include <cntuids.h>
#include <cntabstractfetcher.h>

QTM_USE_NAMESPACE

class CntSrvSession;
class CntNameCacheItem;
class XQSettingsManager;
class XQSettingsKey;

class CntSrvConnection : public QObject
{
    Q_OBJECT
public:
    CntSrvConnection();
    ~CntSrvConnection();
    void setAsynchronous();
    bool executeSqlQuery(const QString &sqlQuery, CntNameOrder nameFormat, int sizeHintKB);
    QList<CntNameCacheItem *> names() { return mNames; }

protected:
    bool event(QEvent *event);

signals:
    void namesRead();
    void namesSorted();

private:
    QThread mThread;
    QList<CntNameCacheItem *> mNames;
    CntSrvSession *mSession;
    QString mSqlQuery;
    CntNameOrder mNameFormat;
    int mSizeHintKB;
    bool mIsAsynchronous;
};

class CntAllNamesJob : public CntAbstractJob
{
public:
    virtual bool isEmpty() { return false; }
    virtual bool equals(const CntAbstractJob &/*other*/) { return true; }
    QString toString() { return QString(); }
};

class CntNameFetcher : public CntAbstractFetcher
{
    Q_OBJECT
public:
    CntNameFetcher();
    ~CntNameFetcher();

    // functions for the asynchronous fetching of all names
    bool isProcessingJob();
    void processNextJob();

    // synchronous functions
    CntNameCacheItem * fetchOneName(QContactLocalId contactId) const;
    bool readNamesFromCache(QList<CntNameCacheItem *> &names);
    bool writeNamesToCache(const QList<CntNameCacheItem *> &names) const;
    void sortNames(QList<CntNameCacheItem *> &names) const;
    static bool compareNames(const CntNameCacheItem *a, const CntNameCacheItem *b);

signals:
    void nameFormatChanged(CntNameOrder nameFormat);
    void databaseAccessComplete();
    void namesAvailable(QList<CntNameCacheItem *> contactNames);

private slots:
    void setNameFormat(const XQSettingsKey &key, const QVariant &value);
    void sendCompletionSignal();

private:
    CntSrvConnection *mDbConnection;
    CntSrvConnection *mAsynchDbConnection;
    XQSettingsManager *mSettingsManager;
    XQSettingsKey *mNameFormatSetting;
    CntNameOrder mNameFormat;
    int mBufferSizeEstimate;

    friend class TestNameFetcher;
};

#endif
