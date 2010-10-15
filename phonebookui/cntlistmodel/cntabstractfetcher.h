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
* Description: Base classes for fetchers and their jobs.
*
*/

#ifndef CNTABSTRACTFETCHER_H
#define CNTABSTRACTFETCHER_H

#include <qtcontacts.h>

#define NoSuchJob -1

QTM_USE_NAMESPACE

class CntAbstractJob
{
public:
    virtual ~CntAbstractJob() {}
    virtual bool isEmpty() = 0;
    virtual bool equals(const CntAbstractJob &other) = 0;
    virtual QString toString() = 0;
};

class CntAbstractFetcher : public QObject
{
    Q_OBJECT
public:
    CntAbstractFetcher(int maxAmountOfJobs);
    virtual ~CntAbstractFetcher();

    bool scheduleJob(CntAbstractJob *job, int priority = 0);
    bool hasScheduledJobs();
    bool hasCancelledJobs();
    virtual bool isProcessingJob() = 0;
    virtual void processNextJob() = 0;

protected:
    CntAbstractJob * takeNextJob();
    CntAbstractJob * takeNextCancelledJob();
    int jobIndex(CntAbstractJob *job);
    int cancelledJobIndex(CntAbstractJob *job);

private:
    QList< QPair<CntAbstractJob *, int> > mJobs;          // list of all scheduled jobs and their priorities
    QList<CntAbstractJob *> mCancelledJobs;               // list of all cancelled jobs
    int mMaxAmountOfJobs;                                 // the maximum amount of concurrently scheduled jobs
};

#endif
