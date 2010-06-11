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
* Description: Class for asynchronously fetching and caching
*              basic contact info for list views.
*
*/

#ifndef CNTCACHE_H
#define CNTCACHE_H

#include <QObject>
#include <QSharedData>
#include <HbIcon>
#include <qcontactmanager.h>
#include <cntinfoprovider.h>

class CntContactInfoData;
class CntCacheThread;
class CntInfoCacheItem;
class CntIconCacheItem;

QTM_USE_NAMESPACE

/*
   Info about one contact that can be used by listviews:
   - the id
   - the full name, properly formatted
   - text, secondary information like phone number
   - icon1, the main icon
   - icon2, a secondary icon
 */
class CntContactInfo : public QObject
{
    Q_OBJECT
public:
    CntContactInfo();
    CntContactInfo(int id, const QString& name, const QString& text, const HbIcon& icon1, const HbIcon& icon2);
    ~CntContactInfo();

    CntContactInfo(const CntContactInfo& other);
    CntContactInfo& operator=(const CntContactInfo& other);

    int id() const;
    QString name() const;
    QString text() const;
    HbIcon icon1() const;
    HbIcon icon2() const;

private:
    QSharedDataPointer<CntContactInfoData> d;
};

/*
   Singleton class that acts as a proxy to get CntContactInfo objects for contacts.
   It also implements caching for faster access. This is why the fetchContactInfo()
   function takes a row number and the full list of contact IDs rather than just a
   contact ID -- the former allows caching ahead.
   
   The usage pattern for clients is to call fetchContactInfo() to get at least the
   name of the contact. If all the info is cached then it will be provided. If not,
   then the uncached info is fetched asynchronously and contactInfoUpdated signals
   are emitted as the pieces of information arrive -- up to three times per contact;
   once for text, once for icon1 and once for icon2.
 */
class CntCache : public QObject
{
    Q_OBJECT
public:
    static CntCache* instance();
    CntContactInfo fetchContactInfo(int row, const QList<QContactLocalId>& idList);

public slots:
    void clearCache();

signals:
    void contactInfoUpdated(QContactLocalId contactId);

private:
    CntCache();
    ~CntCache();
    bool fetchContactName(int contactId, QString& contactName);
    void updateReadAheadCache(int mostRecentRow, const QList<QContactLocalId>& idList);
    CntInfoCacheItem* createInfoCacheItem(int contactId);
    CntIconCacheItem* createIconCacheItem(const QString& iconName);
    void emitContactInfoUpdated(int contactId);

private slots:
    void onNewInfo(int contactId, const ContactInfoField& infoField, const QString& infoValue);
    void onInfoCancelled(int contactId);
    void onNewIcon(const QString& iconName, const HbIcon& icon);
    void onIconCancelled(const QString& iconName);
    void onShutdown();
    void removeContactsFromCache(const QList<QContactLocalId>& contactIds);
    void scheduleOneReadAheadItem();

private:
    static CntCache* mInstance;                   // the one and only instance of CntCache
    QContactManager* mContactManager;             // for fetching contact names and for getting
                                                  // notifications about changes to contacts
    CntCacheThread* mWorker;                      // the background thread that does the actual fetching
    QList<int> mReadAheadCache;                   // cache with set of IDs to prefetch (they are likely to be needed soon)
    QHash<int,CntInfoCacheItem*> mInfoCache;      // cache with contact info, indexed by contact ids
    QHash<QString,CntIconCacheItem*> mIconCache;  // cache with icons, indexed by icon name
    int mNextInfoCacheOrder;                      // cache order for the next item to be updated/inserted in info cache
    int mNextIconCacheOrder;                      // cache order for the next item to be updated/inserted in icon cache
    int mEmittedContactId;                        // id of the last contact emitted to UI
};

#endif
