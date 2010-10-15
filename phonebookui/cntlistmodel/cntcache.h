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
* Description: Asynchronously fetches and caches visual contact info for
*              e.g. list views.
*
*/

#ifndef CNTCACHE_H
#define CNTCACHE_H

#include <QObject>
#include <QTimer>
#include <QSet>
#include <HbIcon>
#include <cntuids.h>
#include <qcontactmanager.h>
#include <cntinfoprovider.h>

class CntNameFetcher;
class CntInfoFetcher;
class CntIconFetcher;
class CntNameCacheItem;
class CntInfoCacheItem;
class CntIconCacheItem;

QTM_USE_NAMESPACE

class CntContactInfo : public QObject
{
    Q_OBJECT
public:
    CntContactInfo(QContactLocalId id,
                   const QString& name,
                   const QString& text,
                   const HbIcon& icon1,
                   const HbIcon& icon2)
    {
        mId = id;
        mName = name;
        mText = text;
        mIcon1 = icon1;
        mIcon2 = icon2;
    }
    ~CntContactInfo() {}

    int id() const { return mId; }
    QString name() const { return mName; }
    QString text() const { return mText; }
    HbIcon icon1() const { return mIcon1; }
    HbIcon icon2() const { return mIcon2; }

private:
    QContactLocalId mId;
    QString mName;
    QString mText;
    HbIcon mIcon1;
    HbIcon mIcon2;
};

class CntCache : public QObject
{
    Q_OBJECT
public:
    static CntCache* createSession(void *client, QContactManager *manager);
    void closeSession(void *client);

    CntContactInfo* fetchContactInfo(int row, const QList<QContactLocalId>& idList);
    QList<QContactLocalId> sortIdsByName(const QSet<QContactLocalId>* idFilter = NULL) const;
    QList<QContactLocalId> sortIdsByName(const QStringList &searchList) const;

    bool event(QEvent *event);

signals:
    void contactInfoUpdated(QContactLocalId contactId);
    void contactsChanged(const QList<QContactLocalId> &changedContacts);
    void contactsRemoved(const QList<QContactLocalId> &removedContacts);
    void contactsAdded(const QList<QContactLocalId> &addedContacts);
    void dataChanged();

private slots:
    void startUrgencyMode();
    void stopUrgencyMode();
    void processJobs();
    void postponeJobs(int postponementType, int duration = 0);
    void resumeJobs();

    void updateCachedInfo(QContactLocalId contactId, const ContactInfoField &infoField, const QString &infoValue);
    void cancelInfoFetch(QContactLocalId contactId);
    void updateCachedIcon(const QString &iconName, const HbIcon &icon);
    void cancelIconFetch(const QString &iconName);

    void updateContacts(const QList<QContactLocalId> &changedContacts);
    void removeContacts(const QList<QContactLocalId> &removedContacts);
    void addContacts(const QList<QContactLocalId> &addedContacts);

    void reformatNames(CntNameOrder newFormat);
    void setNameList(QList<CntNameCacheItem *> newSortedNames);

private:
    CntCache(QContactManager *manager);
    ~CntCache();
    void loadNames();
    bool contactExists(QContactLocalId contactId) const;
    QString contactName(QContactLocalId contactId) const;
    CntInfoCacheItem* createInfoCacheItem(QContactLocalId contactId);
    CntIconCacheItem* createIconCacheItem(const QString &iconName);
    void updateReadAheadCache(int mostRecentRow, const QList<QContactLocalId> &idList);
    void emitContactInfoUpdated(QContactLocalId contactId);

private:
    static CntCache *mInstance;                 // the one and only instance of CntCache
    QSet<void*> mClients;                       // the current clients of cache

    QContactManager *mContactManager;           // for getting notifications about changes to contacts
    CntNameFetcher *mNameFetcher;               // fetches contact names
    CntInfoFetcher *mInfoFetcher;               // fetches secondary text and icon paths
    CntIconFetcher *mIconFetcher;               // fetches icons

    QList<CntNameCacheItem *> mSortedNames;                                 // list of all contact names, in sorted order
    QHash<QContactLocalId, CntNameCacheItem *> mNameCache;                  // cache with all contact names, indexed by contact id
    QHash<QContactLocalId, CntInfoCacheItem *> mInfoCache;                  // cache with recent contact info, indexed by contact id
    QHash<QString, CntIconCacheItem *> mIconCache;                          // cache with recent icons, indexed by contact id and icon name
    QList<QPair<QContactLocalId, int> > mReadAheadCache;                    // cache with contacts to prefetch (they are likely to be needed soon)

    QTimer mResumeJobsTimer;                    // timer used to resume postponed jobs
    bool mProcessingJobs;                       // true from when job loop event has been posted until job loop exits
    int mJobsPostponed;                         // are jobs postponed (no / for some time / until further notice)
    bool mIsInUrgencyMode;                      // true if cache is in urgency mode; secondary info is fetched immediately
    int mLastEmittedContactId;                  // id of the last contact emitted to UI
    bool mHasModifiedNames;                     // monitors whether any names have changed since file cache was last updated
    bool mAllNamesFetchStarted;                 // false until the asynch fetching of all names from the DB has started;
                                                // this operation is done only once
    friend class TestCntCache;
};

#endif
