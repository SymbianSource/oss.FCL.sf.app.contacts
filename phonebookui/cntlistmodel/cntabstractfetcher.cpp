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
* Description: Base class for cache data fetchers.
*
*/

#include <cntabstractfetcher.h>
#include <cntdebug.h>

/*!
    \class CntAbstractCacheItem
    \brief Base class for cache data for one contact.
 */

/*!
    \class CntAbstractFetcher
    \brief CntAbstractFetcher is a base class for cache data fetchers.

    CntAbstractFetcher implements a priority queue for scheduling jobs.
    A job is added using scheduleJob() and executed using processNextJob().
    If too many jobs are scheduled, the oldest jobs (not the ones with lowest
    priority) are cancelled. However, the cancelled jobs will be informed
    back to the client later so that it can reschedule them if needed.
 */

/*!
    Constructs a CntAbstractFetcher object.
 */
CntAbstractFetcher::CntAbstractFetcher(int maxAmountOfJobs)
{
    CNT_ENTRY

    // set the max amount of jobs
    mMaxAmountOfJobs = maxAmountOfJobs;

    CNT_EXIT
}

/*!
    Cleans up and destructs the CntIconFetcher object.
 */
CntAbstractFetcher::~CntAbstractFetcher()
{
    CNT_ENTRY

    disconnect(this);

    for (int i = 0; i < mJobs.count(); ++i) {
        delete mJobs.at(i).first;
    }
    mJobs.clear();

    qDeleteAll(mCancelledJobs);
    mCancelledJobs.clear();

    CNT_EXIT
}

/*!
    Schedules a job. A fetched() signal will be emitted when the job
    has been successfully completed; a failed() signal is sent if the
    job cannot be completed.

    The oldest job is cancelled if there are too many scheduled jobs; in this
    case a cancelled() signal is sent.

    \param job the job; ownership is taken
    \return true, if the job is - or already was - scheduled
 */
bool CntAbstractFetcher::scheduleJob(CntAbstractJob* job, int priority)
{
    Q_ASSERT(job != NULL);

    CNT_ENTRY_ARGS("job =" << job->toString() << "prio =" << priority)

    if (job->isEmpty()) {
        CNT_EXIT_ARGS("bad job")
        return false;
    }

    int index = jobIndex(job);
    if (index != NoSuchJob) {
        // if the job already exists, update the priority...
        if (priority < mJobs.at(index).second) {
            mJobs[index].second = priority;
        }

        // ...and notify the client that an identical job already exists
        delete job;
        CNT_EXIT_ARGS("job already exists")
        return true;
    }

    if (mJobs.count() >= mMaxAmountOfJobs) {
        // the queue of jobs is full, so remove the oldest job
        mCancelledJobs.append(mJobs.takeFirst().first);
    }

    mJobs.append(qMakePair(job, priority));

    // since this job has now been scheduled, remove it from the list of
    // cancelled jobs in case it is there
    index = cancelledJobIndex(job);
    if (index != NoSuchJob) {
        delete mCancelledJobs.takeAt(index);
    }

    CNT_EXIT

    return true;
}

/*!
    \return true if there are scheduled jobs left; otherwise returns false.
 */
bool CntAbstractFetcher::hasScheduledJobs()
{
    return (mJobs.count() > 0);
}

/*!
    \return true if there are cancelled jobs left; otherwise returns false.
 */
bool CntAbstractFetcher::hasCancelledJobs()
{
    return (mCancelledJobs.count() > 0);
}

/*!
    Picks the next job from the list of scheduled jobs, i.e. the job
    with the highest priority.

    \return the next job in the job list
 */
CntAbstractJob * CntAbstractFetcher::takeNextJob()
{
    CNT_ENTRY

    int selectionIndex = -1;
    int selectionPriority = -1;

    int jobCount = mJobs.count();
    if (jobCount == 0) {
        CNT_EXIT_ARGS("no more jobs")
        return NULL;
    }

    for (int i = 0; i < jobCount; ++i) {
        int jobPriority = mJobs.at(i).second;
        if (jobPriority < selectionPriority || selectionPriority == -1) {
            selectionIndex = i;
            selectionPriority = jobPriority;
        }
    }

    CNT_EXIT_ARGS(mJobs.at(selectionIndex).first->toString())

    return mJobs.takeAt(selectionIndex).first;
}

/*!
    \return the next job in the list of cancelled jobs
 */
CntAbstractJob * CntAbstractFetcher::takeNextCancelledJob()
{
    CNT_ENTRY

    return mCancelledJobs.takeFirst();
}


/*!
    Finds out the index of a scheduled job.

    \param job the kind of job to look for
    \return index of the job, or NoSuchJob if such a job is not scheduled.
 */
int CntAbstractFetcher::jobIndex(CntAbstractJob *job)
{
    CNT_ENTRY

    for (int i = 0; i < mJobs.count(); ++i) {
        CntAbstractJob *scheduledJob = mJobs.at(i).first;
        if (job->equals(*scheduledJob)) {
            CNT_EXIT
            return i;
        }
    }

    CNT_EXIT_ARGS("no such job")
    return NoSuchJob;
}

/*!
    Finds out the index of a cancelled job.

    \param job the kind of job to look for
    \return index of the job, or NoSuchJob if such a job is not cancelled.
 */
int CntAbstractFetcher::cancelledJobIndex(CntAbstractJob *job)
{
    CNT_ENTRY

    for (int i = 0; i < mCancelledJobs.count(); ++i) {
        CntAbstractJob *cancelledJob = mCancelledJobs.at(i);
        if (job->equals(*cancelledJob)) {
            CNT_EXIT
            return i;
        }
    }

    CNT_EXIT_ARGS("no such cancelled job")
    return NoSuchJob;
}
