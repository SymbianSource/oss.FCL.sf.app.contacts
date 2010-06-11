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

#include <qtcontacts.h>
#include <qcontactmanager.h>
#include <hbapplication.h>
#include <thumbnailmanager_qt.h>
#include <hbicon.h>
#include <QTimer>
#include "cntcache.h"
#include "cntcache_p.h"
#include "cntinfoprovider.h"
#include "cntdefaultinfoprovider.h"

// maximum amount of info and icon jobs respectively -- if there are more jobs,
// then the oldest job is skipped and the client informed that this happened
// in this way the client can request the job again if wanted
static const int CntMaxInfoJobs = 20;
static const int CntMaxIconJobs = 20;
// amount of milliseconds to postpone the jobs if the UI is very active
static const int PostponeJobsMilliSeconds = 300;
// the event for starting to do all the outstanding jobs
static const QEvent::Type DoAllJobsEvent = QEvent::User;
// the id that states that no icon is currently pending from thumbnail manager
static const int NoIconRequest = -1;
    
// TODO: Provide a way (cenrep keys?) for UI to set which provider to use for
//       what info field (and what info fields are indeed even in use).

/*!
    Creates a new thread for fetching contact info and icons in the background.
 */
CntCacheThread::CntCacheThread()
    : mContactManager(new QContactManager()),
      mJobLoopRunning(false),
      mPostponeJobs(false),
      mIconRequestId(NoIconRequest)
{
    DP_IN("CntCacheThread::CntCacheThread()");

    // create static provider plugins
    mDataProviders.insert(new CntDefaultInfoProvider(), ContactInfoAllFields);
    // TODO: create more static provider plugins

    // TODO: load dynamic provider plugins using QPluginLoader

    // connect the providers
    QMapIterator<CntInfoProvider*, ContactInfoFields> i(mDataProviders);
    while (i.hasNext()) {
        i.next();
        connect(qobject_cast<CntInfoProvider*>(i.key()),
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

    // this thread should interfere as little as possible with more time-critical tasks,
    // like updating the UI during scrolling
    start(QThread::IdlePriority);

    DP_OUT("CntCacheThread::CntCacheThread()");
}

/*!
    Cleans up and destructs the thread.
 */
CntCacheThread::~CntCacheThread()
{
    DP_IN("CntCacheThread::~CntCacheThread()");

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

    QMapIterator<CntInfoProvider*, ContactInfoFields> i(mDataProviders);
    while (i.hasNext()) {
        i.next();
        delete i.key();
    }
    mDataProviders.clear();

    mJobMutex.unlock();

    exit();
    wait();

    DP_OUT("CntCacheThread::~CntCacheThread()");
}

/*!
    Starts the event loop for this thread.
 */
void CntCacheThread::run()
{
    DP_IN("CntCacheThread::run()");
    exec();
    DP_OUT("CntCacheThread::run()");
}

/*!
    Schedules a info to be fetched for a contact. When info has been fetched
    infoFieldUpdated() signals will be emitted, once for each field.
    
    /param contactId the contact for which the info is wanted
 */
void CntCacheThread::scheduleInfoJob(int contactId)
{
    DP_IN("CntCacheThread::scheduleInfoJob(" << contactId << ")");

    Q_ASSERT(contactId > 0 && !mInfoJobs.contains(contactId));

    mJobMutex.lock();

    if (!mJobLoopRunning) {
        // new job => restart job loop
        mJobLoopRunning = true;
        HbApplication::instance()->postEvent(this, new QEvent(DoAllJobsEvent));
    }
    
    if (mInfoJobs.count() >= CntMaxInfoJobs) {
        // the queue of jobs is full, so remove the oldest job
        mCancelledInfoJobs.append(mInfoJobs.takeFirst());
        DP("CntCacheThread::scheduleInfoJob() :" << mCancelledInfoJobs.last() << "removed from joblist");
    }

    mInfoJobs.append(contactId);
    DP("CntCacheThread::scheduleInfoJob() :" << contactId << "appended @" << mInfoJobs.indexOf(contactId));

    // since this job has now been scheduled, remove it from the list of
    // cancelled jobs in case it is there
    mCancelledInfoJobs.removeOne(contactId);

    mJobMutex.unlock();

    DP_OUT("CntCacheThread::scheduleInfoJob(" << contactId << ")");
}

/*!
    Schedules an icon to be fetched. An iconUpdated() signal will be emitted when the icon
    has been fetched.
    
    /param iconName the name of the icon to be fetched
 */
void CntCacheThread::scheduleIconJob(const QString& iconName)
{
    DP_IN("CntCacheThread::scheduleIconJob(" << iconName << ")");

    mJobMutex.lock();

    Q_ASSERT(!iconName.isEmpty() && !mIconJobs.contains(iconName));

    if (!mJobLoopRunning) {
        // new job, so restart job loop
        mJobLoopRunning = true;
        HbApplication::instance()->postEvent(this, new QEvent(DoAllJobsEvent));
    }

    if (mIconJobs.count() >= CntMaxIconJobs) {
        // the queue of jobs is full, so remove the oldest job
        mCancelledIconJobs.append(mIconJobs.takeLast());
        DP("CntCacheThread::scheduleIconJob() :" << mCancelledIconJobs.last() << "removed from joblist");
    }

    mIconJobs.append(iconName);
    DP("CntCacheThread::scheduleIconJob() :" << iconName << "appended @" << mIconJobs.indexOf(iconName));

    // since this job has now been rescheduled, remove it from the list of
    // cancelled jobs in case it is there
    mCancelledIconJobs.removeOne(iconName);

    mJobMutex.unlock();

    DP_OUT("CntCacheThread::scheduleIconJob(" << iconName << ")");
}

/*!
    Postpones outstanding jobs for a few tenths of a second. This should be called if
    the client wants to reserve more CPU time for some urgent task.
 */
void CntCacheThread::postponeJobs()
{
    DP_IN("CntCacheThread::postponeJobs()");
    
    mPostponeJobs = true;
    
    DP_OUT("CntCacheThread::postponeJobs()");
}

/*!
    Handles a class-specific event that is sent by the scheduleOrUpdate functions
    when there are jobs.
 */
bool CntCacheThread::event(QEvent* event)
{
    if (event->type() == DoAllJobsEvent) {
        doAllJobs();
        return true;
    }

    return QThread::event(event);
}

/*!
    Does the jobs. The loop runs until all jobs are done. It pauses
    for a while if new info jobs appear -- this means that the UI is
    updating and so the CPU is yielded to the UI. If there are again
    new jobs after the pause, then it pauses again, and so on.
 */
void CntCacheThread::doAllJobs()
{
    DP_IN("CntCacheThread::doAllJobs()");

    forever {
        mJobMutex.lock();
        int infoJobs = mInfoJobs.count();
        int iconJobs = mIconJobs.count();
        int totalJobs = infoJobs + iconJobs + mCancelledInfoJobs.count() + mCancelledIconJobs.count();
        DP_IN("CntCacheThread::doAllJobs() : infojobs=" << infoJobs << ", iconjobs=" << iconJobs << ",icon_request=" << mIconRequestId << ", cancelledinfojobs=" << mCancelledInfoJobs.count() << ", cancellediconjobs=" << mCancelledIconJobs.count());

        if (totalJobs == 0 || totalJobs == iconJobs && mIconRequestId != NoIconRequest || mPostponeJobs) {
            if (mPostponeJobs) {
                // client has requested a pause in activies (e.g. due to high UI activity)
                mPostponeJobs = false;
                if (totalJobs > 0) {
                    QTimer::singleShot(PostponeJobsMilliSeconds, this, SLOT(doAllJobs()));
                    DP("CntCacheThread::doAllJobs() : postponing for" << PostponeJobsMilliSeconds << "ms");
                }
                else {
                    mJobLoopRunning = false;
                }
            }
            else {
                mJobLoopRunning = false;
            }

            mJobMutex.unlock();

            if (totalJobs == 0) {
                DP("CntCacheThread::doAllJobs() : emitting all jobs done");
                emit allJobsDone();
            }

            break;
        }

        bool doInfoJobs = infoJobs > 0 && (iconJobs == 0 || mIconRequestId != NoIconRequest || qrand() % (infoJobs + iconJobs) < infoJobs);
        
        if (doInfoJobs) {
            // get next job
            int contactId = mInfoJobs.takeLast();
            mJobMutex.unlock();
    
            // fetch qcontact
            QStringList definitionRestrictions;
            definitionRestrictions.append(QContactName::DefinitionName);
            definitionRestrictions.append(QContactAvatar::DefinitionName);
            definitionRestrictions.append(QContactPhoneNumber::DefinitionName);
            definitionRestrictions.append(QContactOrganization::DefinitionName);
            QContactFetchHint restrictions;
            restrictions.setDetailDefinitionsHint(definitionRestrictions);
            restrictions.setOptimizationHints(QContactFetchHint::NoRelationships);
			QContact contact = mContactManager->contact(contactId, restrictions);

            // request contact info from providers
            DP("CntCacheThread::doAllJobs() : fetching info for" << contact.displayLabel() << " (id=" << contactId << ")");
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
            QString iconName = mIconJobs.takeFirst();
            DP("CntCacheThread::doAllJobs() : fetching icon" << iconName);
            mIconRequestId = mThumbnailManager->getThumbnail(iconName, NULL, 0);
            mIconRequestName = iconName;
            mJobMutex.unlock();
        }
        else {
            if (mCancelledInfoJobs.count() > 0) {
                int contactId = mCancelledInfoJobs.takeLast();
                mJobMutex.unlock();
                DP("CntCacheThread::doAllJobs() : emitting cancelled info job" << contactId);
                emit infoCancelled(contactId);
            }
            else if (mCancelledIconJobs.count() > 0) {
                QString iconName = mCancelledIconJobs.takeFirst();
                mJobMutex.unlock();
                DP("CntCacheThread::doAllJobs() : emitting cancelled icon job" << iconName);
                emit iconCancelled(iconName);
            }
        }
    
        // allow signals to be passed from providers and from the client
        HbApplication::processEvents();
    }

    DP_OUT("CntCacheThread::doAllJobs()");
}

/*!
    Passes an info field from a data provider up to the client via signals. The
    client is not in the same thread, so Qt passes the signal as an event.
 */
void CntCacheThread::onInfoFieldReady(CntInfoProvider* sender, int contactId,
                                      ContactInfoField field, const QString& text)
{
    DP_IN("CntCacheThread::onInfoFieldReady( CntInfoProvider*," << contactId << "," << field << "," << text << ")");

    // there can be 3rd party providers, so we cannot blindly trust them;
    // info is emitted only if:
    // 1) the sender is in the list of providers
    // 2) exactly one field bit is set in parameter 'field'
    // 3) the field bit has been assigned to this provider
    if (mDataProviders.contains(sender)
        && ((field & (field - 1)) == 0)
        && ((field & mDataProviders.value(sender)) != 0)) {
        DP("CntCacheThread::onInfoFieldReady(" << contactId << "," << field << "," << text << ") : emitting infoFieldUpdated()");
        emit infoFieldUpdated(contactId, field, text);
    }

    DP_OUT("CntCacheThread::onInfoFieldReady(" << contactId << "," << field << "," << text << ")");
}

/*!
    Passes an icon from thumbnail manager up to the client via a signal. The
    client is not in the same thread, so Qt passes the signal as an event.
 */
void CntCacheThread::onIconReady(const QPixmap& pixmap, void *data, int id, int error)
{
    DP_IN("CntCacheThread::onIconReady( QPixMap, void*, " << id << "," << error << ")");
    Q_UNUSED(data);

    mJobMutex.lock();
    Q_ASSERT(id == mIconRequestId && !mIconRequestName.isEmpty());
    if (!mJobLoopRunning) {
        // job loop quit while waiting for this icon, so restart it
        mJobLoopRunning = true;
        HbApplication::instance()->postEvent(this, new QEvent(DoAllJobsEvent));
    }
    mIconRequestId = NoIconRequest;
    mJobMutex.unlock();

    if (error == 0) {
        DP("CntCacheThread::onIconReady() : emitting iconUpdated(" << mIconRequestName << ")");
        emit iconUpdated(mIconRequestName, HbIcon(pixmap));
    }

    DP_OUT("CntCacheThread::onIconReady( QPixMap, void*, " << id << "," << error << ")");
}
