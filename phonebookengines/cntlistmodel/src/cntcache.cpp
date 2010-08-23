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
* Description: Class for asynchronously fetching and caching basic
*              contact info (see CntContactInfo) for list views.
*
*/

#include <hbapplication.h>
#include <qtcontacts.h>
#include <qcontactmanager.h>
#include <cntdebug.h>
#include "cntcache.h"
#include "cntcache_p.h"
#include "cntinfoprovider.h"

// set the singleton instance pointer to NULL
CntCache* CntCache::mInstance = NULL;

// value for first cache order to be assigned
static const int CacheOrderStartValue = 1;
// for avoiding wrap around with cache orders
static const int MaxCacheOrderValue = 10000000;
// number of items to read quickly when a new instance is requested or cache is cleared
static const int ItemsToReadUrgently = 12;
// number of items to read ahead into cache; this number is for one direction
static const int ItemsToCacheAhead = 24;
// cache size for info items (name, text, icon1name, icon2name)
static const int InfoCacheSize = 128;
// cache size for icon items (iconName and HbIcon)
static const int IconCacheSize = 50;
// number of icons in a CntContactInfo object
static const int IconsInCntContactInfo = 2;
// default empty text info field for a contact; it cannot be empty
// as the listview will then ignore it, causing rendering problems
static const QString EmptyTextField = " ";

/*!
    Creates the CntCache singleton instance.
 */
CntCache::CntCache()
    : mContactManager(new QContactManager()),
      mWorker(new CntCacheThread()),
      mNextInfoCacheOrder(CacheOrderStartValue),
      mNextIconCacheOrder(CacheOrderStartValue),
      mEmittedContactId(-1),
      mUrgentContacts(0)
{
    CNT_ENTRY

    // listen to worker updates
    connect(mWorker, SIGNAL(infoFieldUpdated(int, const ContactInfoField&, const QString&)),
            this, SLOT(onNewInfo(int, const ContactInfoField&, const QString&)));
    connect(mWorker, SIGNAL(iconUpdated(const QString&, const HbIcon&)),
            this, SLOT(onNewIcon(const QString&, const HbIcon&)));
    connect(mWorker, SIGNAL(infoCancelled(int)), this, SLOT(onInfoCancelled(int)));
    connect(mWorker, SIGNAL(iconCancelled(const QString&)), this, SLOT(onIconCancelled(const QString&)));
    connect(mWorker, SIGNAL(allJobsDone()), this, SLOT(scheduleOneReadAheadItem()));

    // listen to the database for changes to contacts
    connect(mContactManager, SIGNAL(contactsChanged(const QList<QContactLocalId>&)), this, SLOT(updateContactsInCache(const QList<QContactLocalId>&)));
    connect(mContactManager, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), this, SLOT(removeContactsFromCache(const QList<QContactLocalId>&)));

    // shutdown only when the whole application shuts down
    connect(HbApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(onShutdown()));

    CNT_EXIT
}

/*!
    Destructs the CntCache singleton instance.
 */
CntCache::~CntCache()
{
    CNT_ENTRY

    delete mWorker;
    delete mContactManager;
    
    qDeleteAll(mInfoCache);
    mInfoCache.clear();
    qDeleteAll(mIconCache);
    mIconCache.clear();

    mInstance = NULL;

    CNT_EXIT
}

/*!
    Provides a pointer to the CntCache singleton instance.
 */
CntCache* CntCache::instance()
{
    if (mInstance == NULL) {
        mInstance = new CntCache();
    }

    // whenever a client requests an instance the client will want to get all info
    // for the first couple of contacts (~a screenfull) as fast as possible
    mInstance->mUrgentContacts = ItemsToReadUrgently;

    return mInstance;
}

/*! 
    Fetches information about a contact: name, text (e.g. phone number or
    social status) and two icons (e.g. avatar, presence). Previously cached
    content - at the very least the name - will be returned immediately.
    Availability of more information will be checked asynchronously and
    sent to clients via contactInfoUpdated() signals.
      
    The function takes a row and a list rather than just a contact id because
    of read ahead caching - contacts near the requested contacts are expected
    to be needed soon and are therefore also scheduled for caching.

    \param row the row of the contact to fetch
    \param idList a list with all the IDs in the list
    \return a contact with some details filled in
 */
CntContactInfo CntCache::fetchContactInfo(int row, const QList<QContactLocalId>& idList)
{
    CNT_ENTRY_ARGS(row << "/" << idList.count())

    Q_ASSERT(row >= 0 && row < idList.count());

    QString name;
    QString text = EmptyTextField;
    HbIcon icons[IconsInCntContactInfo];

    int contactId = idList.at(row);

    if (contactId != mEmittedContactId) {
        // this request comes from the UI when a new view is created or in response to
        // some scrolling activity; in the former case, the client should
        // have set urgencymode on, but in the latter case:
        // 1) postpone all jobs so the UI can use as much of the CPU as possible
        // 2) update read ahead cache to contain all IDs of all items near this item
        if (mUrgentContacts > 0) {
            --mUrgentContacts;
        }
        else {
            mWorker->postponeJobs();
        }
        updateReadAheadCache(row, idList);
    }

    // fetch contact
    if (mInfoCache.contains(contactId)) {
        // the item is in the cache
        CntInfoCacheItem* infoItem = mInfoCache.value(contactId);
        for (int i = 0; i < IconsInCntContactInfo; ++i) {
            QString iconName = infoItem->icons[i];
            if (!iconName.isEmpty()) {
                if (mIconCache.contains(iconName)) {
                    CntIconCacheItem* iconItem = mIconCache.value(iconName);
                    iconItem->cacheOrder = mNextIconCacheOrder++;
                    icons[i] = iconItem->icon;
                    if (!iconItem->isFetched) {
                        // if icon has not yet been received from backend, add
                        // this id to the list of contacts that want to be
                        // notified when the icon is received
                        iconItem->contactIds.insert(contactId);
                        // also reschedule it
                        mWorker->scheduleIconJob(iconName, row);
                    }
                }
                else {
                    // needed icon is not in cache, so schedule it for retrieval
                    CntIconCacheItem* iconItem = createIconCacheItem(iconName);
                    iconItem->contactIds.insert(contactId);
                    mWorker->scheduleIconJob(iconName, row);
                }
            }
        }

        // update cache order
        infoItem->cacheOrder = mNextInfoCacheOrder++;
        infoItem->latestRow = row;

        name = infoItem->name;
        text = infoItem->text;
    }
    else {
        // the item is not in cache, so fetch the name and schedule the rest
        // of the info for retrieval
        if (fetchContactName(contactId, name)) {
            // contact found, so add new entry to cache
            CntInfoCacheItem* item = createInfoCacheItem(contactId);
            item->name = name;
            item->text = text;
            item->latestRow = row;

            // ask the worker thread to fetch the information asynchronously
            mWorker->scheduleInfoJob(contactId, row);
        }
    }

    CNT_EXIT_ARGS("name:" << name << "sec:" << text)

    return CntContactInfo(contactId, name, text, icons[0], icons[1]);
}

/*! 
    Clears the cache of names (not icons). This function can be useful
    for example when the format of contact names changes.
 */
void CntCache::clearCache()
{
    CNT_ENTRY

    // clear info cache
    qDeleteAll(mInfoCache);
    mInfoCache.clear();
    mNextInfoCacheOrder = CacheOrderStartValue;
    mUrgentContacts = ItemsToReadUrgently;

    CNT_EXIT
}

/*! 
    Processes a new info field that has arrived from the worker thread.
    If the contact is in the info cache, then the info cache is updated
    accordingly.
    
    A contactInfoUpdated() signal is usually also emitted. The exception
    is if the info is the name of an icon and that icon is not in the icon
    cache. In this case the icon is fetched before a signal is emitted.
 */
void CntCache::onNewInfo(int contactId, const ContactInfoField& infoField, const QString& infoValue)
{
    CNT_ENTRY_ARGS( "id:" << contactId   << "infotype:" << infoField   << "infovalue:" << infoValue )

    Q_ASSERT(infoField == ContactInfoTextField || infoField == ContactInfoIcon1Field || infoField == ContactInfoIcon2Field);

    bool hasNewInfo;

    if (!mInfoCache.contains(contactId)) {
        // contact is not in cache, so nothing needs to be done
        // except notify clients that this contact has (possibly)
        // been changed
        hasNewInfo = true;
    }
    else if (infoField == ContactInfoTextField) {
        // update cache with new text for contact
        mInfoCache.value(contactId)->text = infoValue;
        hasNewInfo = true;
    }
    else {
        // update cache with new icon name for contact
        int iconIndex = (infoField == ContactInfoIcon1Field ? 0 : 1);

        CntInfoCacheItem* item = mInfoCache.value(contactId);
        QString iconName = infoValue;
        if (item->icons[iconIndex] != iconName) {
            item->icons[iconIndex] = iconName;
            if (iconName.isEmpty()) {
                hasNewInfo = true;
            }
            else if (mIconCache.contains(iconName)) {
                CntIconCacheItem* iconItem = mIconCache.value(iconName);
                if (!iconItem->isFetched) {
                    iconItem->contactIds.insert(contactId);
                    hasNewInfo = false;
                }
                else {
                    hasNewInfo = true;
                }
            }
            else if (iconName.startsWith("qtg_", Qt::CaseInsensitive)) {
                createIconCacheItem(iconName);
                onNewIcon(iconName, HbIcon(iconName)); 
                hasNewInfo = true;
            }
            else {
                CntIconCacheItem* iconItem = createIconCacheItem(iconName);
                iconItem->contactIds.insert(contactId);
				if (mInfoCache.contains(contactId)) {
                	mWorker->scheduleIconJob(iconName, mInfoCache.value(contactId)->latestRow);
				}
				else {
					// less important icon, since this contact is not in cache
                	mWorker->scheduleIconJob(iconName, 100000);
				}
                hasNewInfo = false;
            }
        }
        else {
            hasNewInfo = false;
        }
    }

    if (hasNewInfo) {
        emitContactInfoUpdated(contactId);
    }

    CNT_EXIT
}

/*! 
    Handle the case where a request for contact info is cancelled by the
    worker because of too many subsequent requests.
 */
void CntCache::onInfoCancelled(int contactId)
{
    CNT_ENTRY_ARGS( "id:" << contactId )

    if (mInfoCache.contains(contactId)) {
        CntInfoCacheItem* item = mInfoCache.take(contactId);
        delete item;
    }

    emitContactInfoUpdated(contactId);

    CNT_EXIT
}

/*! 
    Processes a new icon that has arrived from the worker thread.
    The icon cache is updated and a contactInfoUpdated() signal is
    emitted for all contacts that use this icon.
 */
void CntCache::onNewIcon(const QString& iconName, const HbIcon& icon)
{
    CNT_ENTRY_ARGS( iconName )

    QSet<int> contactsToNotify;

    if (mIconCache.contains(iconName)) {
        CntIconCacheItem* item = mIconCache.value(iconName);
        item->icon = icon;
        item->isFetched = true;
        contactsToNotify = item->contactIds;
        item->contactIds.clear();
    }

    foreach (int contactId, contactsToNotify) {
        emitContactInfoUpdated(contactId);
    }

    CNT_EXIT
}

/*! 
    Handle the case where a request for an icon is cancelled by the worker because
    of too many subsequent requests.
 */
void CntCache::onIconCancelled(const QString& iconName)
{
    CNT_ENTRY_ARGS( iconName )

    QSet<int> contactsToNotify;

    if (mIconCache.contains(iconName)) {
        CntIconCacheItem* item = mIconCache.take(iconName);
        contactsToNotify = item->contactIds;
        item->contactIds.clear();
        delete item;
    }

    foreach (int contactId, contactsToNotify) {
        emitContactInfoUpdated(contactId);
    }

    CNT_EXIT
}

/*! 
    Update contacts in cache.
    
    /param contactIds ids of the contact that will be updated
 */
void CntCache::updateContactsInCache(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY

    QString name;

    foreach (QContactLocalId contactId, contactIds) {
        if (mInfoCache.contains(contactId) && fetchContactName(contactId, name)) {
            CntInfoCacheItem* infoItem = mInfoCache.value(contactId);
            infoItem->name = name;
            mWorker->scheduleInfoJob(contactId, infoItem->latestRow);
        }
    }

    foreach (QContactLocalId contactId, contactIds) {
        emitContactInfoUpdated(contactId);
    }

    CNT_EXIT
}

/*! 
    Removes contacts from cache.
    
    /param contactIds ids of the contact that will be removed
 */
void CntCache::removeContactsFromCache(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY

    foreach (QContactLocalId contactId, contactIds) {
        if (mInfoCache.contains(contactId)) {
            CntInfoCacheItem* item = mInfoCache.take(contactId);
            delete item;
        }
    }

    foreach (QContactLocalId contactId, contactIds) {
        emitContactInfoUpdated(contactId);
    }

    CNT_EXIT
}

/*! 
    Uses an optimized function to fetch the name of a contact from
    the database.

    /param contactId the id of the contact to fetch
    /param contactName the name will be stored here if the function is successful
    /return true if the name was fetched successfully
 */
bool CntCache::fetchContactName(int contactId, QString& contactName)
{
    CNT_ENTRY_ARGS( contactId )

    bool foundContact = false;

    QContactFetchHint nameOnlyFetchHint;
    /*QStringList details;
    details << QContactDisplayLabel::DefinitionName;
    nameOnlyFetchHint.setDetailDefinitionsHint(details);*/
    nameOnlyFetchHint.setOptimizationHints(QContactFetchHint::NoRelationships);
    QContact contact = mContactManager->contact(contactId, nameOnlyFetchHint);
    
    if (mContactManager->error() == QContactManager::NoError) {
        contactName = contact.displayLabel();
        foundContact = true;
    }
    
    CNT_EXIT_ARGS( foundContact )
    
    return foundContact;
}

/*! 
    Collects all contact IDs near the latest fetch from the UI. These will be fetched and
    precached when UI activity slows down.

    \param mostRecentRow the row of the contact that was most recently fetched
    \param idList a list with all the IDs in the list
 */
void CntCache::updateReadAheadCache(int mostRecentRow, const QList<QContactLocalId>& idList)
{
    CNT_ENTRY_ARGS( mostRecentRow )

    int row;

    mReadAheadCache.clear();
    
    // step through the area near to last fetch item and make sure all
    // contacts in it are also in cache or in the read ahead list
    for (int i = 1; i <= ItemsToCacheAhead; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (j == 0) {
                row = mostRecentRow - i;
                if (row <= 0) {
                    continue;
                }
            }
            else {
                row = mostRecentRow + i;
                if (row >= idList.count()) {
                    continue;
                }
            }
            
            int contactId = idList.at(row);
            if (!mInfoCache.contains(contactId)) {
                // contact is not in cache, so put the id to items to read into cache
                mReadAheadCache.append(QPair<int,int>(contactId,row));
            }
            else {
                // contact is in cache; update cache order as we want to keep this item in cache
                mInfoCache.value(contactId)->cacheOrder = mNextInfoCacheOrder++;
            }
        }
    }

   CNT_EXIT
}

/*! 
    Schedules one uncached item in the read-ahead list for retrieval.
 */
void CntCache::scheduleOneReadAheadItem()
{
    CNT_ENTRY

    QString name;

    // pick the first contact from the read ahead cache and schedule it
    while (mReadAheadCache.count() > 0) {
        int contactId = mReadAheadCache.first().first;
        int contactRow = mReadAheadCache.takeFirst().second;
        if (!mInfoCache.contains(contactId)) {
            // contact is not in cache, so schedule it for retreival
            if (fetchContactName(contactId, name)) {
                // contact found, so add new entry to cache
                CntInfoCacheItem* item = createInfoCacheItem(contactId);
                item->name = name;
                item->text = EmptyTextField;
                item->latestRow = contactRow;
    
                // schedule the info for retrieval
                mWorker->scheduleInfoJob(contactId, contactRow);
                break;
            }
        }
    }

    CNT_EXIT
}

/*! 
    Creates a new item in the info cache. If the cache is full,
    then the least recently accessed item is removed from cache.
    
    /param contactId id of contact for which to create the new cache item
    /return the newly created cache item
 */
CntInfoCacheItem* CntCache::createInfoCacheItem(int contactId)
{
    CNT_ENTRY_ARGS( contactId )

    if (mInfoCache.count() >= InfoCacheSize) {
        // cache is full, so remove the oldest contact
        int minCacheOrder = mNextInfoCacheOrder;
        CntInfoCacheItem* oldestItem = NULL;
        foreach (CntInfoCacheItem* i, mInfoCache) {
            if (i->cacheOrder < minCacheOrder) {
                minCacheOrder = i->cacheOrder;
                oldestItem = i;
            }
        }
        
        if (oldestItem) {
            mInfoCache.remove(oldestItem->contactId);
            delete oldestItem;
        }
        
        // cache maintenance: if the cache ids become too large,
        // reduce all of them by MaxCacheOrderValue
        if (mNextInfoCacheOrder >= MaxCacheOrderValue) {
            mNextInfoCacheOrder -=  MaxCacheOrderValue;
            foreach (CntInfoCacheItem* i, mInfoCache) {
                i->cacheOrder -= MaxCacheOrderValue;
            }
        }
    }
    
    // create and insert the new item
    CntInfoCacheItem* item = new CntInfoCacheItem();
    item->cacheOrder = mNextInfoCacheOrder++;
    item->contactId = contactId;
    mInfoCache.insert(contactId, item);
    
    CNT_EXIT

    return item;
}

/*! 
    Creates a new item in the icon cache. If the cache is full,
    then the least recently accessed item is removed from cache.
    
    /param iconName name of the icon for which to create the new cache item
    /return the newly created cache item
 */
CntIconCacheItem* CntCache::createIconCacheItem(const QString& iconName)
{
    CNT_ENTRY_ARGS( iconName )

    if (mIconCache.count() >= IconCacheSize) {
        // cache is full, so remove the oldest icon
        int minCacheOrder = mNextIconCacheOrder;
        CntIconCacheItem* oldestItem = NULL;
        foreach (CntIconCacheItem* i, mIconCache) {
            if (i->cacheOrder < minCacheOrder) {
                minCacheOrder = i->cacheOrder;
                oldestItem = i;
            }
        }
        mIconCache.remove(oldestItem->iconName);
        delete oldestItem;

        // cache maintenance: if the cache orders become too large,
        // reduce all of them by MaxCacheOrderValue
        if (mNextIconCacheOrder >= MaxCacheOrderValue) {
            mNextIconCacheOrder -=  MaxCacheOrderValue;
            foreach (CntIconCacheItem* i, mIconCache) {
                i->cacheOrder -=  MaxCacheOrderValue;
            }
        }
    }

    // create and insert the new item
    CntIconCacheItem* item = new CntIconCacheItem();
    item->cacheOrder = mNextIconCacheOrder++;
    item->iconName = iconName;
    item->isFetched = false;
    mIconCache.insert(iconName, item);

    CNT_EXIT

    return item;
}

/*! 
    Notifies clients that a contact might have changed.
    Clients can then request the info via fetchContactInfo() 
    if they are interested.
 */
void CntCache::emitContactInfoUpdated(int contactId)
{
	CNT_ENTRY_ARGS( contactId )

    mEmittedContactId = contactId;
    emit contactInfoUpdated(contactId);
    mEmittedContactId = -1;

	CNT_EXIT
}

/*! 
    Deletes the cache.
 */
void CntCache::onShutdown()
{
	CNT_ENTRY

    deleteLater();

	CNT_EXIT
}


/*! 
    Creates an empty object.
 */
CntContactInfo::CntContactInfo()
    : d(new CntContactInfoData())
{
}

/*! 
    Creates an object with all info fields set.
 */
CntContactInfo::CntContactInfo(int id, const QString& name, const QString& text, const HbIcon& icon1, const HbIcon& icon2)
    : d(new CntContactInfoData())
{
      d->id = id;
      d->name = name;
      d->text = text;
      d->icon1 = icon1;
      d->icon2 = icon2;
}

/*! 
    Destroys the object.
 */
CntContactInfo::~CntContactInfo()
{
}

/*! 
    Copy constructor.
 */
CntContactInfo::CntContactInfo(const CntContactInfo& other)
    : d(other.d)
{
}

/*! 
    Assignment operator.
 */
CntContactInfo& CntContactInfo::operator=(const CntContactInfo& other)
{
    d = other.d;
    return *this;
}

/*! 
    Getter function for the id.
 */
int CntContactInfo::id() const
{
    return d->id;
}

/*! 
    Getter function for the name.
 */
QString CntContactInfo::name() const
{
    return d->name;
}

/*! 
    Getter function for the text.
 */
QString CntContactInfo::text() const
{
    return d->text;
}

/*! 
    Getter function for the first icon.
 */
HbIcon CntContactInfo::icon1() const
{
    return d->icon1;
}

/*! 
    Getter function for the second icon.
 */
HbIcon CntContactInfo::icon2() const
{
    return d->icon2;
}

