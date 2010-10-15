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
* Description: Asynchronously fetches and caches basic contact info
*              and icons.
*
*/

#include <hbapplication.h>
#include <qtcontacts.h>
#include <qcontactmanager.h>
#include <QTimer>
#include <cntcache.h>
#include <cntcacheitems.h>
#include <cntnamefetcher.h>
#include <cntinfofetcher.h>
#include <cnticonfetcher.h>
#include <cntdebug.h>

/*!
    \class CntContactInfo
    \brief Info about one contact, intended for list views.

   This class contains info about one contact. The info is mainly intended
   to be used in list views:
     - name: the name of the contact, formatted for displaying
     - text: secondary information like a phone number
     - icon1: the main icon
     - icon2: the smaller secondary icon

    \class CntCache
    \brief Asynchronously fetches contact info and icons.

    Singleton class that acts as a proxy to get CntContactInfo objects for
    contacts. It also implements caching for faster access. This is why the
    fetchContactInfo() function takes a row number and the full list of
    contact IDs rather than just a contact ID -- it allows precaching.

    The usage pattern for clients is to call fetchContactInfo() to get at
    least the name of the contact. If all the info is cached then it will be
    provided. If not, then the uncached info is fetched asynchronously and
    contactInfoUpdated() signals are emitted as the pieces of information
    arrive -- up to three times per contact; once for text, once for icon1
    and once for icon2.

    Internally CntCache uses three fetchers; one for names, one for info and
    one for icons.
 */

// set the singleton instance pointer to NULL
CntCache* CntCache::mInstance = NULL;

// the event for starting to process all outstanding jobs
const QEvent::Type ProcessJobsEvent = QEvent::User;

// different states of postponement 
const int JobsNotPostponed = 0;
const int JobsPostponedForDuration = 1;
const int JobsPostponedUntilResume = 2;

// number of items to read ahead into cache; this number is for one direction
const int ItemsToCacheAhead = 24;

// cache size for info items
const int InfoCacheSize = 128;

// cache size for icon items; must be larger than 2 * ItemsToCacheAhead
const int IconCacheSize = 60;

// duration of urgency mode in milliseconds
const int UrgencyModeDuration = 100;

// duration of a postponement in milliseconds
const int PostponeJobsDuration = 300;

// number of icons in a CntContactInfo object
const int IconsInCntContactInfo = 2;

// default empty text info field for a contact; it cannot be empty
// as the listview will then ignore it, causing rendering problems
const QString EmptyTextField = " ";

/*!
    Provides a pointer to the CntCache singleton instance.
    
    \param client a pointer to the client
    \param manager  
 */
CntCache* CntCache::createSession(void *client, QContactManager *manager)
{
	CNT_STATIC_ENTRY_ARGS("client =" << client << ", mngr =" << (void*) manager)

    if (!mInstance) {
        mInstance = new CntCache(manager);
    }

    // increase reference counter for cache clients
    mInstance->mClients.insert(client);

    // whenever a client requests an instance, the client will want to get all info
    // for the first screenful of contacts urgently
    mInstance->startUrgencyMode();

	CNT_EXIT_ARGS("instance =" << (void*) mInstance << ", refCount =" << mInstance->mClients.count())

    return mInstance;
}

/*!
    Disconnects from CntCache.
 */
void CntCache::closeSession(void *client)
{
	CNT_ENTRY

    // delete singleton instance if there are no more clients
    mInstance->mClients.remove(client);
    if (mInstance->mClients.count() == 0) {
    	CNT_LOG_ARGS("no more clients, so deleting singleton instance")
        mInstance = NULL;
        delete this;
    }

	CNT_EXIT
}

/*! 
    Fetches visuals for a contact: name, text (e.g. phone number or social
    status) and two icons (e.g. avatar, presence). Previously cached content,
    at the very least the name, will be returned immediately. Availability of
    more information will be checked asynchronously and sent to clients via
    contactInfoUpdated() signals.
    
    The function takes a row and a list rather than just a contact ID because
    of read ahead caching - contacts near the requested contacts are expected
    to be needed soon and are therefore precached.
    
    \param row the row of the contact to fetch
    \param idList a list with all the IDs in the list
    \return a contact with some details filled in
 */
CntContactInfo* CntCache::fetchContactInfo(int row, const QList<QContactLocalId>& idList)
{
    CNT_ENTRY_ARGS(row << "/" << idList.count())

    Q_ASSERT(row >= 0 && row < idList.count());

    QString name;
    QString text = EmptyTextField;
    HbIcon icons[IconsInCntContactInfo];

    QContactLocalId contactId = idList.at(row);

    if (contactId != mLastEmittedContactId) {
        // this is a new request from the UI (rather than a response to
        // a change that the cache just emitted)
        if (!mIsInUrgencyMode) {
            postponeJobs(JobsPostponedForDuration, PostponeJobsDuration);
        }
        updateReadAheadCache(row, idList);
    }

    // fetch the contact
    if (mInfoCache.contains(contactId)) {
        // the contact's info is cached
        CntInfoCacheItem* infoItem = mInfoCache.value(contactId);
        for (int i = 0; i < IconsInCntContactInfo; ++i) {
            QString iconName = infoItem->icons[i];
            if (!iconName.isEmpty()) {
                if (mIconCache.contains(iconName)) {
                    CntIconCacheItem* iconItem = mIconCache.value(iconName);
                    if (iconItem->requestedBy.count() > 0) {
                    // icon is being fetched -> add this contact to list of requestors
                        iconItem->requestedBy << contactId;
                    }
                    iconItem->lastRequest = QTime::currentTime();
                    icons[i] = iconItem->icon;
                } else {
                    // needed icon is not in cache, so schedule it for retrieval
                    CntIconCacheItem* iconItem = createIconCacheItem(iconName);
                    iconItem->requestedBy << contactId;
                    mIconFetcher->scheduleJob(new CntIconJob(iconName), row);
                }
            }
        }

        // set return text
        text = infoItem->text;

        // update cache order
        infoItem->lastRequest = QTime::currentTime();
    } else if (contactExists(contactId)) {
        // contact exists but info is not in cache, so schedule it for retrieval
        CntInfoCacheItem* item = createInfoCacheItem(contactId);
        item->text = text;
        mInfoFetcher->scheduleJob(new CntInfoJob(contactId), row);
    } else {
        return NULL;
    }

    name = contactName(contactId);

    if (!mProcessingJobs && mJobsPostponed == JobsNotPostponed) {
        // there might be new jobs now
        mProcessingJobs = true;
        HbApplication::instance()->postEvent(this, new QEvent(ProcessJobsEvent));
    }

    CNT_EXIT_ARGS("name:" << name << "text:" << text)

    return new CntContactInfo(contactId, name, text, icons[0], icons[1]);
}

/*! 
    Creates a list of contact ids sorted according the corresponding contact names.

    \param idFilter the IDs to be returned; if NULL, all contact IDs are returned
    \return the list of ids, sorted by contact name
 */
QList<QContactLocalId> CntCache::sortIdsByName(const QSet<QContactLocalId>* idFilter) const
{
    CNT_ENTRY

    QList<QContactLocalId> sortedIds;
    
    // allocate memory in advance to avoid repeated reallocation during population
    // an extra 16 items are allocated to leave room for a few more contacts
    // before reallocation is needed
    if (!idFilter) {
        sortedIds.reserve(mSortedNames.count() + 16);
    } else {
        sortedIds.reserve(idFilter->count() + 16);
    }

    // the entries in mSortedNames are already sorted, so just pick
    // out the ids from that list in the order that they appear
    if (!idFilter) {
        foreach (CntNameCacheItem* item, mSortedNames) {
            sortedIds.append(item->contactId());
        }
    } else {
        foreach (CntNameCacheItem* item, mSortedNames) {
            if (idFilter->contains(item->contactId())) {
                sortedIds.append(item->contactId());
            }
        }
    }

    CNT_EXIT

    return sortedIds;
}

/*!
    Overloaded version of the function for string based searching of contact
    names. Currently for multi part names only space and dash variations are
    used for filtering, e.g. "A B" matches "Beta, Alfa" and "Alfa, Beta",
    but also "Gamma, Alfa-Beta" and "Gamma, Alfa Beta" and "Alfa Beta, Gamma".
    
    \param searchList list of strings to search for
    \return the list of ids, sorted by contact name
 */
QList<QContactLocalId> CntCache::sortIdsByName(const QStringList &searchList) const
{
    CNT_ENTRY_ARGS("time:" << User::FastCounter());

    QList<QContactLocalId> sortedIds;
    QSet<int> checkedNames;
    QStringList searchListSorted;

    // the given search string must be ordered to descending order according to word length
    // so the search algorithm finds the correct contacts, this prevents cases where search string
    // is e.g. "ax axx" so names starting with "axxyyz axyz" don't cause any problems for the search
    foreach (QString oneString, searchList) {
        searchListSorted.append(oneString.toLower());
    }
    qSort(searchListSorted.begin(), searchListSorted.end(), qGreater<QString>());

    for (int iter = 0; iter < mSortedNames.size(); iter++) {
        int searchIndex;
        QString currentName = (mSortedNames.at(iter))->name();
        checkedNames.clear();

        for (searchIndex = 0; searchIndex < searchListSorted.size(); searchIndex++) {
            int currentPos;
            int tempStartPos = 0;
            for (currentPos = 0; currentPos <= currentName.length(); currentPos++) {
                // at the moment only differentiating character is the space (" ")
                if (currentPos == currentName.length() || currentName.at(currentPos) == ' ') {
                    QString tempName = currentName.mid(tempStartPos, currentPos - tempStartPos);

                    if (!checkedNames.contains(tempStartPos)
                        && tempName.startsWith(searchListSorted.at(searchIndex), Qt::CaseInsensitive)) {
                        checkedNames.insert(tempStartPos);
                        break;
                    }
                    tempStartPos = ++currentPos;
                }
            }
            // if the name is parsed completely through then it can't be a match
            if (currentPos > currentName.length()) {
                break;
            }
        }
        // if the whole search parameter list is parsed, then the name must match the given search string
        if (searchIndex == searchListSorted.size()) {
            sortedIds.append(mSortedNames.at(iter)->contactId());
        }
    }

    CNT_EXIT_ARGS("time:" << User::FastCounter());

    return sortedIds;
}

/*!
    Creates the CntCache singleton instance.
 */
CntCache::CntCache(QContactManager *manager)
    : mContactManager(manager),
      mNameFetcher(new CntNameFetcher()),
      mInfoFetcher(new CntInfoFetcher(mContactManager)),
      mIconFetcher(new CntIconFetcher()),
      mProcessingJobs(false),
      mJobsPostponed(JobsNotPostponed),
      mIsInUrgencyMode(false),
      mLastEmittedContactId(-1),
      mHasModifiedNames(false),
      mAllNamesFetchStarted(false)
{
    CNT_ENTRY

    // listen to name fetcher
    QObject::connect(mNameFetcher, SIGNAL(nameFormatChanged(CntNameOrder)),
            this, SLOT(reformatNames(CntNameOrder)));
    QObject::connect(mNameFetcher, SIGNAL(databaseAccessComplete()),
            this, SLOT(resumeJobs()));
    QObject::connect(mNameFetcher, SIGNAL(namesAvailable(QList<CntNameCacheItem *>)),
            this, SLOT(setNameList(QList<CntNameCacheItem *>)));

    // listen to info fetcher
    QObject::connect(mInfoFetcher, SIGNAL(infoUpdated(QContactLocalId, const ContactInfoField &, const QString &)),
            this, SLOT(updateCachedInfo(QContactLocalId, const ContactInfoField &, const QString &)));
    QObject::connect(mInfoFetcher, SIGNAL(infoCancelled(QContactLocalId)),
            this, SLOT(cancelInfoFetch(QContactLocalId)));

    // listen to icon fetcher
    QObject::connect(mIconFetcher, SIGNAL(iconFetched(const QString &, const HbIcon &)),
            this, SLOT(updateCachedIcon(const QString &, const HbIcon &)));
    QObject::connect(mIconFetcher, SIGNAL(iconCancelled(const QString &)),
            this, SLOT(cancelIconFetch(const QString &)));

    // listen to contact manager
    QObject::connect(mContactManager, SIGNAL(contactsChanged(const QList<QContactLocalId>&)),
            this, SLOT(updateContacts(const QList<QContactLocalId>&)));
    QObject::connect(mContactManager, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)),
            this, SLOT(removeContacts(const QList<QContactLocalId>&)));
    QObject::connect(mContactManager, SIGNAL(contactsAdded(const QList<QContactLocalId>&)),
            this, SLOT(addContacts(const QList<QContactLocalId>&)));

    // listen to timer events; this is for postponing and resuming jobs
    mResumeJobsTimer.setSingleShot(true);
    QObject::connect(&mResumeJobsTimer, SIGNAL(timeout()), this, SLOT(resumeJobs()));

    // load all names to cache
    loadNames();

    CNT_EXIT
}

/*!
    Destructs the CntCache singleton instance.
 */
CntCache::~CntCache()
{
    CNT_ENTRY

    QObject::disconnect(this);

    if (mHasModifiedNames) {
        mNameFetcher->writeNamesToCache(mSortedNames);
    }

    delete mNameFetcher;
    delete mInfoFetcher;
    delete mIconFetcher;

    qDeleteAll(mInfoCache);
    mInfoCache.clear();

    qDeleteAll(mIconCache);
    mIconCache.clear();

    qDeleteAll(mNameCache);
    mNameCache.clear();

    mSortedNames.clear();   // contains same data as mNameCache, so no qDeleteAll

    CNT_EXIT
}

/*!
    Postpones outstanding jobs until milliseconds ms has passed or resumeJobs() is called.
    
    \param postponement Type the type of postponement; UntilResume or ForDuration
    \param milliseconds The duration of the delay
 */
void CntCache::postponeJobs(int postponementType, int duration)
{
    CNT_ENTRY_ARGS("ms =" << duration)

    Q_ASSERT((postponementType == JobsPostponedUntilResume
             || postponementType == JobsPostponedForDuration)
             && duration >= 0);

    mJobsPostponed = postponementType;
    mResumeJobsTimer.stop();

    if (postponementType == JobsPostponedForDuration) {
        mResumeJobsTimer.start(duration);
    }

    CNT_EXIT_ARGS("type =" << mJobsPostponed)
}

/*!
    Postpones outstanding jobs until resumeJobs() is called. This must always be called after
    postponeJobs.
 */
void CntCache::resumeJobs()
{
    CNT_ENTRY

    Q_ASSERT(!mProcessingJobs && mJobsPostponed != JobsNotPostponed);

    mResumeJobsTimer.stop();
    mJobsPostponed = JobsNotPostponed;
    mProcessingJobs = true;
    HbApplication::instance()->postEvent(this, new QEvent(ProcessJobsEvent));

    CNT_EXIT
}

/*!
    Listens for ProcessJobsEvents and calls processJobs() if there is such an event.
 */
bool CntCache::event(QEvent* event)
{
    if (event->type() == ProcessJobsEvent) {
        processJobs();
        return true;
    }

    return QObject::event(event);
}

/*!
    Processes all scheduled jobs in all fetchers. The loop runs until all
    jobs are done or postponed.
 */
void CntCache::processJobs()
{
    CNT_ENTRY

    // process fetcher jobs in order of priority
    forever {
        // 1: has all jobs been postponed?
        if (mJobsPostponed != JobsNotPostponed) {
            CNT_EXIT_ARGS("jobs postponed")
            mProcessingJobs = false;
            return;

        // 2: is there a request to fetch info?
        } else if (mInfoFetcher->hasScheduledJobs()) {
            mInfoFetcher->processNextJob();

        // 3: is there a request to fetch an icon?
        } else if (mIconFetcher->hasScheduledJobs()) {
            // quit the loop; it will be started again when the icon has been fetched
            if (!mIconFetcher->isProcessingJob()) {
                mIconFetcher->processNextJob();
            }
            mProcessingJobs = false;
            CNT_EXIT_ARGS("jobs postponed until icon fetch returns")
            return;

        // 4: are there any cancelled info jobs?
        } else if (mInfoFetcher->hasCancelledJobs() && !mInfoFetcher->isProcessingJob()) {
            mInfoFetcher->processNextJob();

        // 5: are there any cancelled icon jobs?
        } else if (mIconFetcher->hasCancelledJobs() && !mIconFetcher->isProcessingJob()) {
            mIconFetcher->processNextJob();

        // 6: is there an "all names" job?
        } else if (mNameFetcher->hasScheduledJobs()) {
            // fetch all contact names from the database so that the current
            // list of names (from the file cache) can be synched with the
            // database
            if (!mNameFetcher->isProcessingJob()) {
                mNameFetcher->processNextJob();
            }
            mProcessingJobs = false;
            postponeJobs(JobsPostponedUntilResume);
            CNT_EXIT_ARGS("jobs postponed while fetching all names")
            return;

        // 7: are there contacts left to precache?
        } else if (mReadAheadCache.count() > 0) {
            int contactId = mReadAheadCache.first().first;
            int contactRow = mReadAheadCache.takeFirst().second;
            if (!mInfoCache.contains(contactId) && contactExists(contactId)) {
                // contact exists, but is not in cache, so schedule it for retrieval
                CntInfoCacheItem* item = createInfoCacheItem(contactId);
                item->text = EmptyTextField;
                mInfoFetcher->scheduleJob(new CntInfoJob(contactId), contactRow);
            }
        // nothing more to do, so exit loop
        } else {
            mProcessingJobs = false;
            CNT_EXIT_ARGS("no more jobs")
            return;
        }

        // allow events to be handled before continuing with the next job
        HbApplication::processEvents();
    }
}

/*! 
    Processes a new info field that has arrived from the info fetcher.
    If the contact is in the info cache, then the info cache is updated
    accordingly.
    
    A contactInfoUpdated() signal is usually also emitted. The exception is if
    the info is the name of an icon and that icon is not in the icon cache. In
    this case the icon is scheduled to be fetched and a signal will eventually
    be emitted when the icon has been fetched (or cancelled).
 */
void CntCache::updateCachedInfo(QContactLocalId contactId, const ContactInfoField& infoField, const QString& infoValue)
{
    CNT_ENTRY_ARGS( "id:" << contactId   << "infotype:" << infoField   << "infovalue:" << infoValue )

    Q_ASSERT(infoField == ContactInfoTextField || infoField == ContactInfoIcon1Field || infoField == ContactInfoIcon2Field);

    bool hasNewInfo;

    if (!mInfoCache.contains(contactId)) {
        // contact is not in cache, so nothing needs to be done except notify
        // clients that this contact has (possibly) been changed
        hasNewInfo = true;
    } else if (infoField == ContactInfoTextField) {
        // update cache with new text for contact
        mInfoCache.value(contactId)->text = infoValue;
        hasNewInfo = true;
    } else {
        // update cache with new icon name for contact
        int iconIndex = (infoField == ContactInfoIcon1Field ? 0 : 1);
        CntInfoCacheItem* item = mInfoCache.value(contactId);
        QString iconName = infoValue;
        if (item->icons[iconIndex] != iconName) {
            item->icons[iconIndex] = iconName;
            if (iconName.isEmpty()) {
                hasNewInfo = true;
            } else if (mIconCache.contains(iconName)) {
                hasNewInfo = true;
            } else if (iconName.startsWith("qtg_", Qt::CaseInsensitive)) {
                CntIconCacheItem* iconItem = createIconCacheItem(iconName);
                iconItem->icon = HbIcon(iconName);
                hasNewInfo = true;
            } else {
                CntIconCacheItem* iconItem = createIconCacheItem(iconName);
                iconItem->requestedBy << contactId;
                QList<CntNameCacheItem*>::iterator pos = qLowerBound(mSortedNames.begin(), mSortedNames.end(), mNameCache.value(contactId), CntNameFetcher::compareNames);
                while (pos != mSortedNames.end() && (*pos)->contactId() != contactId) {
                    ++pos;
                }
                mIconFetcher->scheduleJob(new CntIconJob(iconName), pos - mSortedNames.begin());
                hasNewInfo = false;
            }
        } else {
            hasNewInfo = false;
        }
    }

    if (hasNewInfo) {
        emitContactInfoUpdated(contactId);
    }

    if (!mProcessingJobs && mJobsPostponed == JobsNotPostponed) {
        // there might be new jobs now
        mProcessingJobs = true;
        HbApplication::instance()->postEvent(this, new QEvent(ProcessJobsEvent));
    }

    CNT_EXIT
}

/*! 
    Handle the case where a request for contact info is cancelled by the
    info fetcher because of too many scheduled jobs.
 */
void CntCache::cancelInfoFetch(QContactLocalId contactId)
{
    CNT_ENTRY_ARGS( "cid =" << contactId )

    if (mInfoCache.contains(contactId)) {
        delete mInfoCache.take(contactId);
    }

    emitContactInfoUpdated(contactId);

    CNT_EXIT
}

/*! 
    Processes a new icon that has arrived from the icon fetcher.
    The icon cache is updated and a contactInfoUpdated() signal is
    emitted.
 */
void CntCache::updateCachedIcon(const QString& iconName, const HbIcon& icon)
{
    CNT_ENTRY_ARGS( "icon =" << iconName )

    if (mIconCache.contains(iconName)) {
        CntIconCacheItem* item = mIconCache.value(iconName);
        item->icon = icon;
        foreach (QContactLocalId contactId, item->requestedBy) {
            emitContactInfoUpdated(contactId);
        }
        item->requestedBy.clear();

        if (!mProcessingJobs && mJobsPostponed == JobsNotPostponed) {
            // there might still be unfinished icon jobs; only one icon job is
            // done at a time
            mProcessingJobs = true;
            HbApplication::instance()->postEvent(this, new QEvent(ProcessJobsEvent));
        }
    }

    CNT_EXIT
}

/*! 
    Handle the case where a request for an icon is cancelled by the icon
    fetcher because of too many scheduled jobs.
 */
void CntCache::cancelIconFetch(const QString& iconName)
{
    CNT_ENTRY_ARGS(iconName)

    if (mIconCache.contains(iconName)) {
        CntIconCacheItem* item = mIconCache.take(iconName);
        foreach (QContactLocalId contactId, item->requestedBy) {
            emitContactInfoUpdated(contactId);
        }
        delete item;
    }

    CNT_EXIT
}

/*! 
    Creates a new item in the info cache. If the cache is full,
    then the least recently accessed item is removed from cache.
    
    /param contactId id of contact for which to create the new cache item
    /return the newly created cache item
 */
CntInfoCacheItem * CntCache::createInfoCacheItem(QContactLocalId contactId)
{
    CNT_ENTRY_ARGS(contactId)

    if (mInfoCache.count() >= InfoCacheSize) {
        // cache is full, so remove the oldest contact
        CntInfoCacheItem* oldestItem = NULL;
        QTime oldestRequest;

        foreach (CntInfoCacheItem* i, mInfoCache) {
            if (oldestItem == NULL || i->lastRequest < oldestRequest) {
                oldestRequest = i->lastRequest;
                oldestItem = i;
            }
        }

        if (oldestItem != NULL) {
            mInfoCache.remove(oldestItem->contactId);
            delete oldestItem;
        }
    }
    
    // create and insert the new item
    CntInfoCacheItem* item = new CntInfoCacheItem();
    item->contactId = contactId;
    item->lastRequest = QTime::currentTime();
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
    CNT_ENTRY_ARGS(iconName)

    if (mIconCache.count() >= IconCacheSize) {
        // cache is full, so remove the oldest icon
        CntIconCacheItem* oldestItem = NULL;
        QTime oldestRequest;

        foreach (CntIconCacheItem* i, mIconCache) {
            if (oldestItem == NULL || i->lastRequest < oldestRequest) {
                oldestRequest = i->lastRequest;
                oldestItem = i;
            }
        }

        if (oldestItem) {
            mIconCache.remove(oldestItem->iconName);
            delete oldestItem;
        }
    }

    // create and insert the new item
    CntIconCacheItem* item = new CntIconCacheItem();
    item->iconName = iconName;
    item->lastRequest = QTime::currentTime();
    mIconCache.insert(iconName, item);

    CNT_EXIT

    return item;
}

/*!
    Notifies clients that a contact might have changed.
    Clients can then request the info via fetchContactInfo() 
    if they are interested.
 */
void CntCache::emitContactInfoUpdated(QContactLocalId contactId)
{
	CNT_ENTRY_ARGS(contactId)

    mLastEmittedContactId = contactId;
    emit contactInfoUpdated(contactId);
    mLastEmittedContactId = -1;

	CNT_EXIT
}

/*! 
    Collects all contact IDs near the latest fetch from the UI. These will be fetched and
    precached when UI activity slows down.

    \param mostRecentRow the row of the contact that was most recently fetched
    \param idList a list with all the IDs in the list
 */
void CntCache::updateReadAheadCache(int mostRecentRow, const QList<QContactLocalId>& idList)
{
    CNT_ENTRY_ARGS(mostRecentRow)

    int row;

    mReadAheadCache.clear();

    // step through the area near to last fetch item and make sure all
    // contacts in it are also in cache or in the read ahead list
    for (int i = 1; i <= ItemsToCacheAhead; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (j == 0) {
                row = mostRecentRow - i;
                if (row < 0) {
                    continue;
                }
            } else {
                row = mostRecentRow + i;
                if (row >= idList.count()) {
                    continue;
                }
            }
            
            int contactId = idList.at(row);
            if (!mInfoCache.contains(contactId)) {
                // contact is not in cache, so put the id to items to read into cache
                mReadAheadCache.append(QPair<QContactLocalId, int>(contactId, row));
            } else {
                // contact is in cache; update lastRequest as we want to keep this item in cache
                mInfoCache.value(contactId)->lastRequest = QTime::currentTime();
            }
        }
    }

    CNT_EXIT
}

/*!
    Starts the urgency mode, where all contact info is fetched immediately,
    regardless of whether there is activity in the UI or not.
 */
void CntCache::startUrgencyMode()
{
    CNT_ENTRY

    mIsInUrgencyMode = true;
    QTimer::singleShot(UrgencyModeDuration, this, SLOT(stopUrgencyMode()));

    CNT_EXIT
}

/*!
    Starts the urgency mode, where all contact info is fetched immediately,
    regardless of whether there is activity in the UI or not.
 */
void CntCache::stopUrgencyMode()
{
    CNT_ENTRY

    mIsInUrgencyMode = false;

    CNT_EXIT
}

/*!
    Fetch the names of all contacts.
 */
void CntCache::loadNames()
{
    CNT_ENTRY
    
    // read names from file cache
    mNameFetcher->readNamesFromCache(mSortedNames);

    // insert the names into the id-to-name map
    foreach (CntNameCacheItem* item, mSortedNames) {
        mNameCache.insert(item->contactId(), item);
    }

    // schedule the job for reading all names from the database; it will be processed once
    // info and icons for the first screenful of contacts have been read
    mNameFetcher->scheduleJob(new CntAllNamesJob());
    mProcessingJobs = true;
    HbApplication::instance()->postEvent(this, new QEvent(ProcessJobsEvent));

    CNT_EXIT
}

/*!
    Checks whether a contact exists.
 */
bool CntCache::contactExists(QContactLocalId contactId) const
{
    CNT_ENTRY_ARGS(contactId)
    CNT_EXIT_ARGS(mNameCache.contains(contactId))

    return mNameCache.contains(contactId);
}

/*!
    Fetch the name of one contact.
 */
QString CntCache::contactName(QContactLocalId contactId) const
{
    CNT_ENTRY_ARGS(contactId)

    QString name;

    QHash<QContactLocalId, CntNameCacheItem*>::const_iterator i = mNameCache.find(contactId);
    if (i != mNameCache.end()) {
        name = i.value()->name();
    }

    CNT_EXIT_ARGS(name)

    return name;
}

/*! 
    Updates the names in cache according to newFormat.

    \param newFormat the new name format, e.g. "Lastname, Firstname"
 */
void CntCache::reformatNames(CntNameOrder newFormat)
{
	CNT_ENTRY

    foreach (CntNameCacheItem* item, mSortedNames) {
        item->setNameFormat(newFormat);
    }

    mNameFetcher->sortNames(mSortedNames);
    mNameFetcher->writeNamesToCache(mSortedNames);
    mHasModifiedNames = false;

    emit dataChanged();

	CNT_EXIT
}

/*! 
    Replaces the names in cache with the ones in this list.
    
    \param newSortedNames the sorted list with names; this list will be cleared and
                          ownership will be taken of the items in the list
 */
void CntCache::setNameList(QList<CntNameCacheItem *> newSortedNames)
{
    CNT_ENTRY
    
    bool hasModifiedContacts = false;
    int count = newSortedNames.count();

    CNT_LOG_ARGS("curr_count=" << mSortedNames.count() << "db_count=" << count);

    // check if there have been any changes
    if (mSortedNames.count() != count) {
        hasModifiedContacts = true;
    } else {
        for (int i = 0; i < count; ++i) {
            CntNameCacheItem *oldItem = mSortedNames.at(i);
            CntNameCacheItem *newItem = newSortedNames.at(i);
            CNT_LOG_ARGS("name=" << oldItem->name());
            if (oldItem->contactId() != newItem->contactId() || oldItem->name() != newItem->name()) {
                hasModifiedContacts = true;
                break;
            }
        }
    }

    // the list has changed, so use the new list instead
    if (hasModifiedContacts) {
    	CNT_LOG_ARGS("has modified contacts -> use new list")
        qDeleteAll(mSortedNames);
        mNameCache.clear();
        mSortedNames.clear();
        
        foreach (CntNameCacheItem* item, newSortedNames) {
            mSortedNames.append(item);
            mNameCache.insert(item->contactId(), item);
        }
        
        // write names to file cache
        mNameFetcher->writeNamesToCache(mSortedNames);
        
        // notify clients that the list of names has changed
        emit dataChanged();
    } else {
        qDeleteAll(newSortedNames);
    }
    
    CNT_EXIT
}

/*! 
    Updates data in response to some contacts having changed and
    then notifies observers that these contacts have changed.
 */
void CntCache::updateContacts(const QList<QContactLocalId> &changedContacts)
{
	CNT_ENTRY

    QString name;
    QList<CntNameCacheItem*> items;

    // reloads the names of the changed contacts and updates the
    // list of sorted names accordingly
    foreach (QContactLocalId contactId, changedContacts) {
        CntNameCacheItem *newItem = mNameFetcher->fetchOneName(contactId);
        if (newItem != NULL) {
            CntNameCacheItem *oldItem = mNameCache.value(contactId);
            if (oldItem->name() != newItem->name()) {
                QList<CntNameCacheItem*>::iterator oldPos = qLowerBound(mSortedNames.begin(), mSortedNames.end(), oldItem, CntNameFetcher::compareNames);
                while (oldPos != mSortedNames.end() && *oldPos != oldItem) {
                     ++oldPos;
                }
                QList<CntNameCacheItem*>::iterator newPos = qUpperBound(mSortedNames.begin(), mSortedNames.end(), newItem, CntNameFetcher::compareNames);
                if (oldPos < newPos) {
                    mSortedNames.move(oldPos - mSortedNames.begin(), (newPos - mSortedNames.begin()) - 1);
                } else {
                    mSortedNames.move(oldPos - mSortedNames.begin(), newPos - mSortedNames.begin());
                }
                *oldItem = *newItem;
                mHasModifiedNames = true;
            }
        }
    }

    // if any of the changed items have cached info, the info
    // is scheduled for refreshing
    foreach (QContactLocalId contactId, changedContacts) {
        if (mInfoCache.contains(contactId)) {
            QList<CntNameCacheItem*>::iterator pos = qLowerBound(mSortedNames.begin(), mSortedNames.end(), mNameCache.value(contactId), CntNameFetcher::compareNames);
            while (pos != mSortedNames.end() && (*pos)->contactId() != contactId) {
                ++pos;
            }
            mInfoFetcher->scheduleJob(new CntInfoJob(contactId), pos - mSortedNames.begin());
        }
    }

    // inform clients about these changes
    emit contactsChanged(changedContacts);

    if (!mProcessingJobs && mJobsPostponed == JobsNotPostponed) {
        // there might be new jobs now
        mProcessingJobs = true;
        HbApplication::instance()->postEvent(this, new QEvent(ProcessJobsEvent));
    }

	CNT_EXIT
}

/*! 
    Updates data in response to some contacts having been removed
    and then notifies observers that the contacts have been removed.
 */
void CntCache::removeContacts(const QList<QContactLocalId> &removedContacts)
{
	CNT_ENTRY

    // removed the deleted contacts from the name cache and from the
    // list of sorted names
    foreach (QContactLocalId contactId, removedContacts) {
        if (mNameCache.contains(contactId)) {
            CntNameCacheItem *item = mNameCache.take(contactId);
            QList<CntNameCacheItem*>::iterator pos = qLowerBound(mSortedNames.begin(), mSortedNames.end(), item, CntNameFetcher::compareNames);
            while (*pos != item && pos != mSortedNames.end()) {
                ++pos;
            }
            mSortedNames.erase(pos);
            delete item;
            mHasModifiedNames = true;
        }
    }

    // info for these deleted items should be removed from cache
    foreach (QContactLocalId contactId, removedContacts) {
        if (mInfoCache.contains(contactId)) {
            CntInfoCacheItem* item = mInfoCache.take(contactId);
            delete item;
        }
    }

    // inform clients about these deleted contacts
    emit contactsRemoved(removedContacts);

	CNT_EXIT
}

/*! 
    Updates data in response to some contacts having been added
    and then notifies observers that the contacts have been added.
 */
void CntCache::addContacts(const QList<QContactLocalId> &addedContacts)
{
	CNT_ENTRY

    // add the new contacts to the name cache and to the
    // list of sorted names
    foreach (QContactLocalId contactId, addedContacts) {
        CntNameCacheItem *item = mNameFetcher->fetchOneName(contactId);
        if (item != NULL) {
            mNameCache.insert(contactId, item);
            QList<CntNameCacheItem*>::iterator i = qUpperBound(mSortedNames.begin(), mSortedNames.end(), item, CntNameFetcher::compareNames);
            mSortedNames.insert(i, item);
            mHasModifiedNames = true;
        }
    }

    // inform clients about the new contacts
    emit contactsAdded(addedContacts);

	CNT_EXIT
}
