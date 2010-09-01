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
#include "CCompressionTimer.h"
#include "PbkCompressConfig.h"


// ================= MEMBER FUNCTIONS =======================

inline CPbkDefaultCompressionStrategy::CCompressionTimer::CCompressionTimer(
    CPbkDefaultCompressionStrategy& aCompressionStrategy,
    TTimeIntervalSeconds aTimeout) :
    CTimer(CActive::EPriorityIdle),
    iCompressionStrategy(aCompressionStrategy),
    iTimeout(1000000*aTimeout.Int()) /* 1 sec. timeout */
    {
    CActiveScheduler::Add(this);
    }

inline void CPbkDefaultCompressionStrategy::CCompressionTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

CPbkDefaultCompressionStrategy::CCompressionTimer*
    CPbkDefaultCompressionStrategy::CCompressionTimer::NewL
        (CPbkDefaultCompressionStrategy& aCompressionStrategy,
        TTimeIntervalSeconds aTimeout)
    {
    CCompressionTimer* self = new(ELeave) CCompressionTimer
        (aCompressionStrategy, aTimeout);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CPbkDefaultCompressionStrategy::CCompressionTimer::Start()
    {
    if (!iCompressionStrategy.IsCompressionEnabled())
        {
        return;
        }

    if (IsActive())
        {
        // Timer already running -> cancel and restart
        Cancel();
        PBK_COMPRESS_LOG(PBK_COMPRESS_STRING(
            "CCompressionTimer::Start(0x%x): restarted"), this);
        }

    // Start timer
    After(iTimeout);
    PBK_COMPRESS_LOG(PBK_COMPRESS_STRING(
        "CCompressionTimer::Start(0x%x): started"), this);
    }

void CPbkDefaultCompressionStrategy::CCompressionTimer::RunL()
    {
    PBK_COMPRESS_LOG(PBK_COMPRESS_STRING("CCompressionTimer::RunL(0x%x)"),
        this);

    // Timer elapsed  -> check and start compression if required
    if (iCompressionStrategy.CheckCompress())
        {
        iCompressionStrategy.DoCompressL();
        }
    }

TInt CPbkDefaultCompressionStrategy::CCompressionTimer::RunError(TInt aError)
    {
    PBK_COMPRESS_LOG(PBK_COMPRESS_STRING(
        "CCompressionTimer::RunError(0x%x,%d)"), this, aError);

    // Compression is a silent background operation -> ignore all errors
    return KErrNone;
    }


//  End of File
