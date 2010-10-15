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
* Description: Fetches icons for contacts.
*
*/

#include <thumbnailmanager_qt.h>
#include <cnticonfetcher.h>
#include <cntdebug.h>

// maximum amount of scheduled jobs; if there are more jobs, the least
// important job is cancelled
const int MaxIconJobs = 20;

// the id that states that no icon is currently pending from thumbnail manager
const int NoIconRequest = -1;

/*!
    \class CntIconJob
    \brief Holds info about one icon job.
 */

/*!
    \class CntIconFetcher
    \brief CntIconFetcher asynchronously fetches contact icons.

    CntIconFetcher queues requests for contact icons that are to be cached.
    It fetches the icons later, when asked by the client to do so.

    Internally CntIconFetcher uses thumbnail manager to fetch the icons.
 */

/*!
    Creates a CntIconFetcher object.
 */
CntIconFetcher::CntIconFetcher()
    : CntAbstractFetcher(MaxIconJobs),
      mThumbnailManager(NULL),
      mIconRequestId(NoIconRequest)
{
    CNT_ENTRY

    // create & connect the thumbnail manager
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForPerformance);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailSmall);
    connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
            this, SLOT(forwardIconToClient(QPixmap, void *, int, int)));

    CNT_EXIT
}

/*!
    Cleans up and destructs the CntIconFetcher object.
 */
CntIconFetcher::~CntIconFetcher()
{
    CNT_ENTRY

    if (mIconRequestId != NoIconRequest) {
        mThumbnailManager->cancelRequest(mIconRequestId);
    }

    delete mThumbnailManager;
    mThumbnailManager = NULL;

    CNT_EXIT
}

/*!
    Processes the next scheduled job. This function must not be
    called until the previous job has completed.
 */
void CntIconFetcher::processNextJob()
{
    CNT_ENTRY

    Q_ASSERT(mIconRequestId == NoIconRequest);

    if (hasScheduledJobs()) {
        // request icon from thumbnail manager
        CntIconJob *job = static_cast<CntIconJob *>(takeNextJob());
        mIconRequestName = job->iconName;
        mIconRequestId = mThumbnailManager->getThumbnail(mIconRequestName, NULL, 0);
        delete job;
    } else if (hasCancelledJobs()) {
        CntIconJob *job = static_cast<CntIconJob *>(takeNextCancelledJob());
        emit iconCancelled(job->iconName);
        delete job;
    }

    CNT_EXIT
}

/*!
    \return true if a job is currently being processed; otherwise returns false.
 */
bool CntIconFetcher::isProcessingJob()
{
    return (mIconRequestId != NoIconRequest);
}

/*!
    Forwards an icon from thumbnail manager to the client.
 */
void CntIconFetcher::forwardIconToClient(const QPixmap &pixmap, void *data, int id, int error)
{
    CNT_ENTRY

    Q_UNUSED(data);

    if (id != mIconRequestId) {
        // this pixmap was requested by someone else sharing the same thumbnail manager instance
        CNT_EXIT_ARGS("not our pixmap")
        return;
    }

    mIconRequestId = NoIconRequest;

    if (error == 0) {
        emit iconFetched(mIconRequestName, HbIcon(pixmap));
    } else {
        emit iconFetched(mIconRequestName, HbIcon());
    }

    CNT_EXIT
}
