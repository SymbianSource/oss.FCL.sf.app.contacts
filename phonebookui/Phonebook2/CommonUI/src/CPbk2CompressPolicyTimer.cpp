/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Phonebook 2 database compression timeout timer.
*
*/


// INCLUDE FILES
#include "CPbk2CompressPolicyTimer.h"

// Phonebook 2
#include "MPbk2CompressPolicyManager.h"
#include <MPbk2StoreObservationRegister.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KOneSecond( 1000000 );

} /// namespace


// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::CPbk2CompressPolicyTimer
// --------------------------------------------------------------------------
//
CPbk2CompressPolicyTimer::CPbk2CompressPolicyTimer(
        MPbk2StoreObservationRegister& aStoreObservationRegister,
        MPbk2CompressPolicyManager& aManager,
        TTimeIntervalSeconds aTimeout) :
            CTimer(CActive::EPriorityIdle),
            iStoreObservationRegister(aStoreObservationRegister),
            iManager(aManager),
            iTimeout(KOneSecond * aTimeout.Int()) // 1 sec. timeout
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::~CPbk2CompressPolicyTimer
// --------------------------------------------------------------------------
//
CPbk2CompressPolicyTimer::~CPbk2CompressPolicyTimer()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::NewL
// --------------------------------------------------------------------------
//
CPbk2CompressPolicyTimer*  CPbk2CompressPolicyTimer::NewL
        ( MPbk2StoreObservationRegister& aStoreObservationRegister,
          MPbk2CompressPolicyManager& aManager,
          TTimeIntervalSeconds aTimeout)
    {
    CPbk2CompressPolicyTimer* self = new ( ELeave ) CPbk2CompressPolicyTimer
        ( aStoreObservationRegister, aManager, aTimeout );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2CompressPolicyTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::Start
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyTimer::Start()
    {
    RegisterToListenEventsSafely();
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::Stop
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyTimer::Stop()
    {
    iStoreObservationRegister.DeregisterStoreEvents( *this );
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyTimer::StoreReady
        ( MVPbkContactStore& /*aContactStore*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyTimer::StoreUnavailable(
        MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyTimer::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/,
        TVPbkContactStoreEvent /*aStoreEvent*/)
    {
    StartTimer();
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::RunL
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyTimer::RunL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2CompressPolicyTimer::RunL(0x%x)"),
        this);

    if (iRestarted)
        {
        // If timer was restarted just renew it
        iRestarted = EFalse;
        After(iTimeout);
        }
    else
        {
        // Timer elapsed without restarting
        iManager.CompressStores();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2CompressPolicyTimer::RunError(TInt aError)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2CompressPolicyTimer::RunError(0x%x,%d)"), this, aError);

    // Compression is a silent background operation -> ignore all errors
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::StartTimer
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyTimer::StartTimer()
    {
    if (IsActive())
        {
        // Timer already running -> set restarted flag
        iRestarted = ETrue;
        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
            "CPbk2CompressPolicyTimer::StartTimer(0x%x): restarted"), this);
        }
    else
        {
        // Start timer
        After(iTimeout);
        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
            "CPbk2CompressPolicyTimer::StartTimer(0x%x): started"), this);
        }
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyTimer::RegisterToListenEventsSafely
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyTimer::RegisterToListenEventsSafely()
    {
    // Make sure that you are not registered twice
    iStoreObservationRegister.DeregisterStoreEvents( *this );
    TRAP_IGNORE( iStoreObservationRegister.RegisterStoreEventsL( *this ) );
    }

//  End of File
