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
#include <sysutil.h>
#include <systemwarninglevels.hrh>

#include "PbkCompressConfig.h"
#include "CCompressionTimer.h"
#include "CDiskSpaceWatcher.h"
#include "PbkCompressUi.h"

namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_Step = 1,
    EPanicPreCond_HandleError
    };
#endif

/**
 * Time (in seconds) to wait after last database event before starting 
 * compression. The timeout tries to prevent compression from starting if there
 * is a lot of database activity. Value is specified to 30 minutes, but on udeb
 * 10 seconds is used for testing purposes.
 */
#ifdef _DEBUG
const TInt KCompressionTimeoutAfterLastEvent = 10;
#else
const TInt KCompressionTimeoutAfterLastEvent = 30 * 60;
#endif


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkDefaultCompressionStrategy");
    User::Panic(KPanicText, aReason);
    }
#endif

}


// ================= MEMBER FUNCTIONS =======================

inline CPbkDefaultCompressionStrategy::CPbkDefaultCompressionStrategy
        (CContactDatabase& aDb, 
        TDriveNumber aDbDrive,
        RFs& aFs) :
    iDb(aDb), iDbDrive(aDbDrive), iFs(aFs)
    {
    }

inline void CPbkDefaultCompressionStrategy::ConstructL()
    {
    iCompressionTimer = CCompressionTimer::NewL
        (*this, KCompressionTimeoutAfterLastEvent);
    iDbChangeNotifier = CContactChangeNotifier::NewL(iDb,this);
    iDiskSpaceWatcher = CDiskSpaceWatcher::NewL
        (*this, iFs, KDRIVECCRITICALTHRESHOLD, iDbDrive);
    }

CPbkDefaultCompressionStrategy* CPbkDefaultCompressionStrategy::NewL
        (CContactDatabase& aDb, TDriveNumber aDbDrive, RFs& aFs)
    {
    CPbkDefaultCompressionStrategy* self = 
        new(ELeave) CPbkDefaultCompressionStrategy(aDb, aDbDrive, aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkDefaultCompressionStrategy::~CPbkDefaultCompressionStrategy()
    {
    delete iDiskSpaceWatcher;
    delete iDbChangeNotifier;
    delete iCompressionTimer;
    }

void CPbkDefaultCompressionStrategy::SetCompressUi(MPbkCompressUi* aCompressUi)
    {
    iCompressUi = aCompressUi;
    // Start calls active object's cancel method when needed
    iCompressionTimer->Start();
    }

TBool CPbkDefaultCompressionStrategy::CheckCompress()
    {
    PBK_COMPRESS_LOG(PBK_COMPRESS_STRING("CPbkDefaultCompressionStrategy::CheckCompress(0x%x)"), this);
    return 
        (!iCompressor && iCompressUi && 
        (PbkCompressConfig::CompressRequired(iDb) || 
        FileSpaceLowOnDbDrive()));
    }

TBool CPbkDefaultCompressionStrategy::IsCompressionEnabled()
    {
    // Background compression requires that we have a compression UI
    return iCompressUi?ETrue:EFalse;
    }

void CPbkDefaultCompressionStrategy::CompressL()
    {
    // Start calls active object's cancel method when needed
    iCompressionTimer->Start();
    }

void CPbkDefaultCompressionStrategy::CancelCompress()
    {
    PBK_COMPRESS_LOG(PBK_COMPRESS_STRING("CPbkDefaultCompressionStrategy::CancelCompress(0x%x)"), this);
    if (iCompressor)
        {
        delete iCompressor;
        iCompressor = NULL;
        if (iCompressUi) 
            {
            iCompressUi->PbkCompressCanceled();
            }
        }
    }

void CPbkDefaultCompressionStrategy::Release()
    {
    delete this;
    }

void CPbkDefaultCompressionStrategy::HandleDatabaseEventL
        (TContactDbObserverEvent /*aEvent*/)
    {
    iCompressionTimer->Start();
    }

void CPbkDefaultCompressionStrategy::Step(TInt aStep)
    {
    PBK_COMPRESS_LOG(PBK_COMPRESS_STRING("CPbkDefaultCompressionStrategy::Step(0x%x,%d)"), this, aStep);
    __ASSERT_DEBUG(iCompressor && iCompressUi, Panic(EPanicPreCond_Step));
    if (aStep > 0)
        {
        iCompressUi->PbkCompressStep(aStep);
        }
    else
        {
        // Compression finished
        iCompressUi->PbkCompressComplete();
        delete iCompressor;
        iCompressor = NULL;
        }
    }

void CPbkDefaultCompressionStrategy::HandleError(TInt aError)
    {
    PBK_COMPRESS_LOG(PBK_COMPRESS_STRING("CPbkDefaultCompressionStrategy::HandleError(0x%x,%d)"), this, aError);
    __ASSERT_DEBUG(iCompressor && iCompressUi, Panic(EPanicPreCond_HandleError));
    iCompressUi->PbkCompressError(aError);
    delete iCompressor;
    iCompressor = NULL;
    }

/**
 * Called by CPbkDefaultCompressionStrategy::CCompressionTimer.
 */
void CPbkDefaultCompressionStrategy::DoCompressL()
    {
    if (!iCompressor)
        {
        if (iCompressUi && iCompressUi->PbkCompressCanStartNow())
            {
            // We have a compression UI and compression is ok by it
            CContactActiveCompress* compressor = iDb.CreateCompressorLC();
            iCompressUi->PbkCompressStartL(compressor->StepsTogo());
            PBK_COMPRESS_LOG(PBK_COMPRESS_STRING("CPbkDefaultCompressionStrategy::CompressL(0x%x): Started async compression"), this);
            CleanupStack::Pop();  // compressor
            iCompressor = compressor;
            iCompressor->SetObserver(this);
            }
        }
    }

TBool CPbkDefaultCompressionStrategy::FileSpaceLowOnDbDrive() const
    {
    TBool result = EFalse;
    TRAP_IGNORE(result = SysUtil::DiskSpaceBelowCriticalLevelL(&iFs,0,iDbDrive));
    if (result)
        {
        PBK_COMPRESS_LOG(PBK_COMPRESS_STRING("CPbkDefaultCompressionStrategy::FileSpaceLowOnDbDrive() returning ETrue"));
        }
    return result;
    }

//  End of File  
