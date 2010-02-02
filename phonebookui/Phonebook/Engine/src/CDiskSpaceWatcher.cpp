/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       Database compression timeout timer
*
*/


// INCLUDE FILES
#include "CPbkDefaultCompressionStrategy.h"
#include "CDiskSpaceWatcher.h" 
#include "PbkCompressConfig.h"


// ================= MEMBER FUNCTIONS =======================

inline CPbkDefaultCompressionStrategy::CDiskSpaceWatcher::CDiskSpaceWatcher
        (MPbkCompressionStrategy& aCompressionStrategy, 
        RFs& aFs, TInt64 aThreshold, TInt aDrive) :
    CActive(CActive::EPriorityIdle),
    iCompressionStrategy(aCompressionStrategy),
    iFs(aFs), iThreshold(aThreshold), iDrive(aDrive)
    {
    CActiveScheduler::Add(this);
    RequestNotification();
    }

CPbkDefaultCompressionStrategy::CDiskSpaceWatcher* 
    CPbkDefaultCompressionStrategy::CDiskSpaceWatcher::NewL
        (MPbkCompressionStrategy& aCompressionStrategy, 
        RFs& aFs, TInt64 aThreshold, TInt aDrive)
    {
    CDiskSpaceWatcher* self = new(ELeave) CDiskSpaceWatcher
        (aCompressionStrategy, aFs, aThreshold, aDrive);
    return self;
    }

CPbkDefaultCompressionStrategy::CDiskSpaceWatcher::~CDiskSpaceWatcher()
    {
    Cancel();
    }

void CPbkDefaultCompressionStrategy::CDiskSpaceWatcher::DoCancel()
    {
    iFs.NotifyDiskSpaceCancel();
    }

void CPbkDefaultCompressionStrategy::CDiskSpaceWatcher::RunL()
    {
    PBK_COMPRESS_LOG(PBK_COMPRESS_STRING("CDiskSpaceWatcher::RunL() executed"));

    // Leave if NotifyDiskSpace signals error
    User::LeaveIfError(iStatus.Int());

    // Disk space is below CL -> initiate a compress
    iCompressionStrategy.CompressL();
    // Renew notification request
    RequestNotification();
    }

TInt CPbkDefaultCompressionStrategy::CDiskSpaceWatcher::RunError(TInt /*aError*/)
    {
    // Compression is a silent background operation -> ignore all errors
    return KErrNone;
    }

void CPbkDefaultCompressionStrategy::CDiskSpaceWatcher::RequestNotification()
    {
    Cancel();
    iFs.NotifyDiskSpace(iThreshold,iDrive,iStatus);
    SetActive();
    }


//  End of File  
