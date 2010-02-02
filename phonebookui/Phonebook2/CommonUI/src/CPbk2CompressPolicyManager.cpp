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
* Description:  Phonebook 2 compress policy manager.
*
*/


#include "CPbk2CompressPolicyManager.h"

// Phonebook 2
#include "CPbk2CompressPolicyTimer.h"
#include "CPbk2CompressPolicyDiskSpace.h"
#include <MPbk2StoreObservationRegister.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactOperationBase.h>

// System includes
#include <systemwarninglevels.hrh>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

 /// Time (in seconds) to wait after last database event before starting
 /// compression. The timeout tries to prevent compression from starting
 /// if there is a lot of database activity.
const TInt KIntervalInSeconds(10);

#ifdef _DEBUG

enum TPanic
    {
    EPreCondPanic_OperationComplete = 1,
    EPreCondPanic_AlreadyExist
    };

_LIT(KPanicCat, "CPbk2CompressPolicyManager");

void Panic(TPanic aPanic)
    {
    User::Panic(KPanicCat, aPanic);
    }

#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2CompressPolicyManager::CPbk2CompressPolicyManager
// --------------------------------------------------------------------------
//
CPbk2CompressPolicyManager::CPbk2CompressPolicyManager
        ( MPbk2StoreObservationRegister& aStoreObservationRegister,
          CVPbkContactManager& aContactManager ) :
            iStoreObservationRegister( aStoreObservationRegister ),
            iContactManager( aContactManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyManager::~CPbk2CompressPolicyManager
// --------------------------------------------------------------------------
//
CPbk2CompressPolicyManager::~CPbk2CompressPolicyManager()
    {
    iPolicyArray.ResetAndDestroy();
    delete iCompressOperation;
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyManager::NewL
// --------------------------------------------------------------------------
//
CPbk2CompressPolicyManager* CPbk2CompressPolicyManager::NewL
        ( MPbk2StoreObservationRegister& aStoreObservationRegister,
          CVPbkContactManager& aContactManager )
    {
    CPbk2CompressPolicyManager* self =
        new ( ELeave ) CPbk2CompressPolicyManager
            ( aStoreObservationRegister, aContactManager );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyManager::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyManager::ConstructL()
    {
    ConstructPoliciesL();
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyManager::CompressStores
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyManager::CompressStores()
    {
    __ASSERT_DEBUG( !iCompressOperation, Panic(EPreCondPanic_AlreadyExist));
    
    TRAPD( err,
        iCompressOperation = iContactManager.CompressStoresL( *this ) );

    if (err != KErrNone)
        {
        // if error occured here, restart all policies
        StartAllPolicies();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyManager::CancelCompress
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyManager::CancelCompress()
    {
    delete iCompressOperation;
    iCompressOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyManager::StartAllPolicies
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyManager::StartAllPolicies()
    {
    for (TInt i = 0; i < iPolicyArray.Count(); ++i)
        {
        MPbk2CompressPolicy* policy = iPolicyArray[i];
        policy->Start();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyManager::StopAllPolicies
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyManager::StopAllPolicies()
    {
    CancelCompress();
    for (TInt i = 0; i < iPolicyArray.Count(); ++i)
        {
        iPolicyArray[i]->Stop();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyManager::StepComplete
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyManager::StepComplete
        ( MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyManager::StepFailed
// --------------------------------------------------------------------------
//
TBool CPbk2CompressPolicyManager::StepFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/,
          TInt /*aError*/ )
    {
    // The process should be stopped
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyManager::OperationComplete
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyManager::OperationComplete
        ( MVPbkContactOperationBase& PBK2_DEBUG_ONLY( aOperation ) )
    {
    __ASSERT_DEBUG(iCompressOperation == &aOperation,
        Panic(EPreCondPanic_OperationComplete));
    delete iCompressOperation;
    iCompressOperation = NULL;

    // After compress operation complete, restart the policies
    StartAllPolicies();
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyManager::ConstructPoliciesL
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyManager::ConstructPoliciesL()
    {
    // Timer policy
    CPbk2CompressPolicyTimer* timerPolicy = CPbk2CompressPolicyTimer::NewL
        ( iStoreObservationRegister, *this, KIntervalInSeconds );
    CleanupStack::PushL(timerPolicy);
    iPolicyArray.AppendL(timerPolicy);
    CleanupStack::Pop(timerPolicy);

    // Disk space policy
    CPbk2CompressPolicyDiskSpace* diskspacePolicy =
        CPbk2CompressPolicyDiskSpace::NewL
            ( *this, iContactManager.FsSession(), KDRIVECCRITICALTHRESHOLD );
    CleanupStack::PushL(diskspacePolicy);
    iPolicyArray.AppendL(diskspacePolicy);
    CleanupStack::Pop(diskspacePolicy);
    }

// End of File
