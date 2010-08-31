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

#include <QPluginLoader>
#include <QDir>

#include <qtcontacts.h>
#include <qcontactmanager.h>
#include <hbapplication.h>
#include <thumbnailmanager_qt.h>
#include <hbicon.h>
#include <QTimer>
#include "cntcache.h"
#include "cntcache_p.h"
#include <cntinfoproviderfactory.h>
#include <cntinfoprovider.h>
#include "cntdefaultinfoprovider.h"
#include "cntpresenceinfoprovider.h"
#include <cntdebug.h>

// maximum amount of info and icon jobs respectively -- if there are more jobs,
// then the oldest job is skipped and the client informed that this happened
// in this way the client can request the job again if wanted
static const int CntMaxInfoJobs = 20;
static const int CntMaxIconJobs = 20;
// amount of milliseconds to postpone the jobs if the UI is very active
static const int PostponeJobsMilliSeconds = 300;
// the event for starting to process all outstanding jobs
static const QEvent::Type ProcessJobsEvent = QEvent::User;
// the id that states that no icon is currently pending from thumbnail manager
static const int NoIconRequest = -1;
// the id that states that there is no job with that key
static const int NoSuchJob = -1;

const char *CNT_INFO_PROVIDER_EXTENSION_PLUGIN_DIRECTORY = "/resource/qt/plugins/contacts/infoproviders/";
    
// TODO: Provide a way (cenrep keys?) for UI to set which provider to use for
//       what info field (and what info fields are indeed even in use).

/*!
    Creates a new thread for fetching contact info and icons in the background.
 */
CntCacheThread::CntCacheThread()
    : mContactManager(new QContactManager()),
      mStarted(false),
      mProcessingJobs(false),
      mPostponeJobs(false),
      mIconRequestId(NoIconRequest)
{
    CNT_ENTRY

    // create static provider plugins
    mDataProviders.insert(new CntDefaultInfoProvider(), ContactInfoAllFields);
    mDataProviders.insert(new CntPresenceInfoProvider(), ContactInfoIcon2Field);

    // load dynamic provider plugins
    QDir pluginsDir(CNT_INFO_PROVIDER_EXTENSION_PLUGIN_DIRECTORY);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        // Create plugin loader
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        if ( pluginLoader.load() )
        {
            CntInfoProviderFactory *factory = qobject_cast<CntInfoProviderFactory*>(pluginLoader.instance());
            
            if (factory)
            {
                CntInfoProvider *provider = factory->infoProvider();
                mDataProviders.insert(provider, provider->supportedFields());
            }
        }
    }
    
    // connect the providers
    QMapIterator<CntInfoProvider*, ContactInfoFields> i(mDataProviders);
    while (i.hasNext()) {
        i.next();
        connect(static_cast<CntInfoProvider*>(i.key()),
                SIGNAL(infoFieldReady(CntInfoProvider*, int, ContactInfoField, const QString&)),
                this,
                SLOT(onInfoFieldReady(CntInfoProvider*, int, ContactInfoField, const QString&)));
    }

    // create & connect the thumbnail manager
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForPerformance);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailSmall);
    connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
             this, SLOT(onIconReady(QPixmap, void *, int, int)));

    CNT_EXIT
}

/*!
    Cleans up and destructs the thread.
 */
CntCacheThread::~CntCacheThread()
{
    CNT_ENTRY
    
    delete mContactManager;
    disconnect(this);

    mJobMutex.lock();
    mInfoJobs.clear();
    mCancelledInfoJobs.clear();
    mIconJobs.clear();
    mCancelledIconJobs.clear();

    if (mIconRequestId != NoIconRequest) {
        mThumbnailManager->cancelRequest(mIconRequestId);
        mIconRequestId = NoIconRequest;
    }

    delete mThumbnailManager;
    mThumbnailManager = NULL;

    qDeleteAll(mDataProviders.keys());
    mDataProviders.clear();

    mJobMutex.unlock();

    exit();
    wait();

    CNT_EXIT
}

/*!
    Starts the event loop for this thread.
 */
void CntCacheThread::run()
{
    CNT_ENTRY

    exec();

    CNT_EXIT
}

/*!
    Schedules a info to be fetched for a contact. When info has been fetched
    infoFieldUpdated() signals will be emitted, once for each field.
    
    /param contactId the contact for which the info is wanted
 */
void CntCacheThread::scheduleInfoJob(int contactId, int priority)
{
    CNT_ENTRY_ARGS( contactId )

    if (contactId <= 0)
        return;

    mJobMutex.lock();

    int index = infoJobIndex(contactId);
    if (index != NoSuchJob) {
        // if the job already exists, update the priority
        if (priority < mInfoJobs.at(index).second) {
            mInfoJobs[index] = QPair<int,int>(contactId,priority);
        }
        mJobMutex.unlock();
        return;
    }

    if (!mStarted) {
        // starting the event loop; minimum priority is used as this thread
        // should interfere as little as possible with more time-critical tasks,
        // like updating the UI during scrolling
        start(QThread::IdlePriority);
        mStarted = true;
    }

    if (!mProcessingJobs) {
        // new job => start processing jobs
        mProcessingJobs = true;
        HbApplication::instance()->postEvent(this, new QEvent(ProcessJobsEvent));
    }
    
    if (mInfoJobs.count() >= CntMaxInfoJobs) {
        // the queue of jobs is full, so remove the oldest job
        mCancelledInfoJobs.append(mInfoJobs.takeFirst().first);
        CNT_LOG_ARGS( mCancelledInfoJobs.last() << "removed from joblist" )
    }

    mInfoJobs.append(QPair<int,int>(contactId, priority));
    CNT_LOG_ARGS( contactId << "(prio:" << priority << ") appended @" << (mInfoJobs.count() - 1) );

    // since this job has now been scheduled, remove it from the list of
    // cancelled jobs in case it is there
    mCancelledInfoJobs.removeOne(contactId);

    mJobMutex.unlock();

    CNT_EXIT
}

/*!
    Schedules an icon to be fetched. An iconUpdated() signal will be emitted when the icon
    has been fetched.
    
    /param iconName the name of the icon to be fetched
 */
void CntCacheThread::scheduleIconJob(const QString& iconName, int priority)
{
    CNT_ENTRY_ARGS( iconName )

    if (iconName.isEmpty())
        return;

    mJobMutex.lock();

    int index = iconJobIndex(iconName);
    if (index != NoSuchJob) {
        // if the job already exists, update the priority
        if (priority < mIconJobs.at(index).second) {
            mIconJobs[index] = QPair<QString,int>(iconName,priority);
        }
        mJobMutex.unlock();
        return;
    }

    if (!mProcessingJobs) {
        // new job, so restart job loop
        mProcessingJobs = true;
        HbApplication::instance()->postEvent(this, new QEvent(ProcessJobsEvent));
    }

    if (mIconJobs.count() >= CntMaxIconJobs) {
        // the queue of jobs is full, so remove the oldest job
        mCancelledIconJobs.append(mIconJobs.takeLast().first);
        CNT_LOG_ARGS( mCancelledIconJobs.last() << "removed from joblist" );
    }

    mIconJobs.append(QPair<QString,int>(iconName, priority));
    CNT_LOG_ARGS( iconName << "(prio:" << priority << ") appended @" << (mIconJobs.count() - 1) );

    // since this job has now been rescheduled, remove it from the list of
    // cancelled jobs in case it is there
    mCancelledIconJobs.removeOne(iconName);

    mJobMutex.unlock();

    CNT_EXIT
}

/*!
    Postpones outstanding jobs for a few tenths of a second. This should be called if
    the client wants to reserve more CPU time for some urgent task.
 */
void CntCacheThread::postponeJobs()
{
    CNT_ENTRY
    
    mPostponeJobs = true;
    
    CNT_EXIT
}

/*!
    Handles a class-specific event that is sent by the scheduleOrUpdate functions
    when there are jobs.
 */
bool CntCacheThread::event(QEvent* event)
{
    if (event->type() == ProcessJobsEvent) {
        processJobs();
        return true;
    }

    return QThread::event(event);
}

/*!
    Processes all scheduled jobs. The loop runs until all jobs are done.
    It pauses for a while if new info jobs appear -- this means that the
    UI is updating and so the CPU is yielded to the UI. If there are
    again new jobs after the pause, then it pauses again, and so on.
 */
void CntCacheThread::processJobs()
{
    CNT_ENTRY

    forever {
        mJobMutex.lock();
        int infoJobs = mInfoJobs.count();
        int iconJobs = mIconJobs.count();
        int totalJobs = infoJobs + iconJobs + mCancelledInfoJobs.count() + mCancelledIconJobs.count();

        if (totalJobs == 0 || totalJobs == iconJobs && mIconRequestId != NoIconRequest || mPostponeJobs) {
            if (mPostponeJobs) {
                // client has requested a pause in activies (e.g. due to high UI activity)
                mPostponeJobs = false;
                if (totalJobs > 0) {
                    QTimer::singleShot(PostponeJobsMilliSeconds, this, SLOT(processJobs()));
                }
                else {
                    mProcessingJobs = false;
                }
            }
            else {
                mProcessingJobs = false;
            }

            mJobMutex.unlock();

            if (totalJobs == 0) {
                emit allJobsDone();
            }

            break;
        }

        bool doInfoJob = infoJobs > 0; // && (iconJobs == 0 || mIconRequestId != NoIconRequest || qrand() % (infoJobs + iconJobs) < infoJobs);
        
        if (doInfoJob) {
            // get next job
            int contactId = takeNextInfoJob();
            mJobMutex.unlock();
    
            // fetch qcontact
            QContactFetchHint restrictions;
            restrictions.setOptimizationHints(QContactFetchHint::NoRelationships);
			QContact contact = mContactManager->contact(contactId, restrictions);

            // request contact info from providers
            QMapIterator<CntInfoProvider*, ContactInfoFields> i(mDataProviders);
            while (i.hasNext()) {
                i.next();
                if (i.value() != 0) {
                    i.key()->requestInfo(contact, i.value());
                }
            }
        }
        else if (iconJobs > 0 && mIconRequestId == NoIconRequest) {
            // request icon from thumbnail manager
            QString iconName  = takeNextIconJob();
            mIconRequestId = mThumbnailManager->getThumbnail(iconName, NULL, 0);
            mIconRequestName = iconName;
            mJobMutex.unlock();
        }
        else {
            if (mCancelledInfoJobs.count() > 0) {
                int contactId = mCancelledInfoJobs.takeLast();
                mJobMutex.unlock();
                emit infoCancelled(contactId);
            }
            else if (mCancelledIconJobs.count() > 0) {
                QString iconName = mCancelledIconJobs.takeFirst();
                mJobMutex.unlock();
                emit iconCancelled(iconName);
            }
        }
    
        // allow signals to be passed from providers and from the client
        HbApplication::processEvents();
    }

    CNT_EXIT
}

/*!
    Passes an info field from a data provider up to the client via signals. The
    client is not in the same thread, so Qt passes the signal as an event.
 */
void CntCacheThread::onInfoFieldReady(CntInfoProvider* sender, int contactId,
                                      ContactInfoField field, const QString& text)
{
    CNT_ENTRY

    // there can be 3rd party providers, so we cannot blindly trust them;
    // info is emitted only if:
    // 1) the sender is in the list of providers
    // 2) exactly one field bit is set in parameter 'field'
    // 3) the field bit has been assigned to this provider
    if (mDataProviders.contains(sender)
        && ((field & (field - 1)) == 0)
        && ((field & mDataProviders.value(sender)) != 0)) {
        emit infoFieldUpdated(contactId, field, text);
    }

    CNT_EXIT
}

/*!
    Passes an icon from thumbnail manager up to the client via a signal. The
    client is not in the same thread, so Qt passes the signal as an event.
 */
void CntCacheThread::onIconReady(const QPixmap& pixmap, void *data, int id, int error)
{
    CNT_ENTRY

    Q_UNUSED(id);
    Q_UNUSED(data);

    mJobMutex.lock();
    Q_ASSERT(id == mIconRequestId && !mIconRequestName.isEmpty());
    if (!mProcessingJobs) {
        // job loop quit while waiting for this icon, so restart it
        mProcessingJobs = true;
        HbApplication::instance()->postEvent(this, new QEvent(ProcessJobsEvent));
    }
    mIconRequestId = NoIconRequest;
    mJobMutex.unlock();

    if (error == 0) {
        emit iconUpdated(mIconRequestName, HbIcon(pixmap));
    }

    CNT_EXIT
}

/*!
    Finds out the index of an info job in the job list. Job mutex must be held when calling this function.

    \return index of the contact in the job list, or NoSuchJob if no job is scheduled for the contact
 */
int CntCacheThread::infoJobIndex(int contactId)
{
    int jobCount = mInfoJobs.count();
    for (int i = 0; i < jobCount; ++i) {
        if (mInfoJobs.at(i).first == contactId) {
            return i;
        }
    }
    
    return NoSuchJob;
}

/*!
    Picks the next job from the info job list (the one with the highest priority).

    \return the id of the contact for which the info should be fetched
 */
int CntCacheThread::takeNextInfoJob()
{
    int selectionIndex = -1;
    int selectionPriority = -1;

    int jobCount = mInfoJobs.count();
    if (jobCount == 0) {
        return NoSuchJob;
    }

    for (int i = 0; i < jobCount; ++i) {
        int jobPriority = mInfoJobs.at(i).second;
        if (jobPriority < selectionPriority || selectionPriority == -1) {
            selectionIndex = i;
            selectionPriority = jobPriority;
        }
    }
    
    return mInfoJobs.takeAt(selectionIndex).first;
}

/*!
    Picks the next job from the icon job list (the one with the highest priority).

    \return the name of the icon that should be fetched
 */
QString CntCacheThread::takeNextIconJob()
{
    int selectionIndex = -1;
    int selectionPriority = -1;

    int jobCount = mIconJobs.count();
    if (jobCount == 0) {
        return QString();
    }

    for (int i = 0; i < jobCount; ++i) {
        int jobPriority = mIconJobs.at(i).second;
        if (jobPriority < selectionPriority || selectionPriority == -1) {
            selectionIndex = i;
            selectionPriority = jobPriority;
        }
    }
    
    return mIconJobs.takeAt(selectionIndex).first;
}

/*!
    Finds out the index of an icon job in the job list. Job mutex must be held when calling this function.

    \return index of the icon in the job list, or NoSuchJob if a job for the icon is not scheduled.
 */
int CntCacheThread::iconJobIndex(QString iconName)
{
    int jobCount = mIconJobs.count();
    for (int i = 0; i < jobCount; ++i) {
        if (mIconJobs.at(i).first == iconName) {
            return i;
        }
    }
    
    return NoSuchJob;
}
