/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A contact operation for commtting multiple contacts
*
*/


// INCLUDE FILES
#include "CCommitContactsOperation.h"

#include <MVPbkBatchOperationObserver.h>
#include <MVPbkStoreContact.h>
#include "VPbkSimStoreError.h"

namespace VPbkSimStore {

// CONSTANTS

// Contacts are commited 1 contact at a time
const TInt KStepSize = 1;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SendStepFailed
// Send step failed event to given observer.
// -----------------------------------------------------------------------------
//
inline TBool SendStepFailed( CCommitContactsOperation& aOperation, TInt aError,
    MVPbkBatchOperationObserver& aObserver )
    { 
    return aObserver.StepFailed( aOperation, KStepSize, aError );
    }

// -----------------------------------------------------------------------------
// SendStepComplete
// Send step completed event to given observer.
// -----------------------------------------------------------------------------
//
inline void SendStepComplete( CCommitContactsOperation& aOperation,
    MVPbkBatchOperationObserver& aObserver )
    {
    aObserver.StepComplete( aOperation, KStepSize );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCommitContactsOperation::CCommitContactsOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommitContactsOperation::CCommitContactsOperation( CContactStore& aStore,
    MVPbkBatchOperationObserver& aObserver )
    :   iStore( aStore ),
        iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CCommitContactsOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCommitContactsOperation::ConstructL( 
    const TArray<MVPbkStoreContact*>& aContacts )
    {
    const TInt count = aContacts.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iStoreContacts.AppendL( aContacts[i] );
        }
    }

// -----------------------------------------------------------------------------
// CCommitContactsOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCommitContactsOperation* CCommitContactsOperation::NewL( 
    CContactStore& aStore, 
    MVPbkBatchOperationObserver& aObserver,
    const TArray<MVPbkStoreContact*>& aContacts )
    {
    CCommitContactsOperation* self = 
        new( ELeave ) CCommitContactsOperation( aStore, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aContacts );
    CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
CCommitContactsOperation::~CCommitContactsOperation()
    {
    iStoreContacts.Close();
    }

// -----------------------------------------------------------------------------
// CCommitContactsOperation::StartL
// -----------------------------------------------------------------------------
//
void CCommitContactsOperation::StartL()
    {
    __ASSERT_DEBUG( iStoreContacts.Count() > 0,
        VPbkSimStore::Panic( VPbkSimStore::EZeroCommitContacts ) );
    iCommitCounter = 0;
    User::LeaveIfError( NextCycle() );
    }

// -----------------------------------------------------------------------------
// CCommitContactsOperation::Cancel
// -----------------------------------------------------------------------------
//
void CCommitContactsOperation::Cancel()
    {
    iCommitCounter = iStoreContacts.Count();
    }

// -----------------------------------------------------------------------------
// CCommitContactsOperation::ContactOperationCompleted
// -----------------------------------------------------------------------------
//
void CCommitContactsOperation::ContactOperationCompleted( 
    TContactOpResult /*aResult*/ )
    {
    if ( iCommitCounter >= iStoreContacts.Count() )
        {
        iObserver.OperationComplete( *this );
        }
    else
        {
        SendStepComplete( *this, iObserver );
        TryCommitNext();
        }
    }
  
// -----------------------------------------------------------------------------
// CCommitContactsOperation::ContactOperationFailed
// -----------------------------------------------------------------------------
//   
void CCommitContactsOperation::ContactOperationFailed( TContactOp /*aOpCode*/, 
    TInt aErrorCode, TBool /*aErrorNotified*/ )
    {
    if ( SendStepFailed( *this, aErrorCode, iObserver ) )
        {
        // Continue only if ETrue is returned. Otherwise this operation might
        // be already deleted.
        TryCommitNext();        
        }
    }

// -----------------------------------------------------------------------------
// CCommitContactsOperation::NextCycle
// -----------------------------------------------------------------------------
//
TInt CCommitContactsOperation::NextCycle()
    {
    TRAPD( result, iStoreContacts[iCommitCounter]->CommitL( *this ) );
    ++iCommitCounter;
    return result;
    }

// -----------------------------------------------------------------------------
// CCommitContactsOperation::TryCommitNext
// -----------------------------------------------------------------------------
//
void CCommitContactsOperation::TryCommitNext()
    {
    const TInt totalCount = iStoreContacts.Count(); 
    TInt result = KErrNone;
    TBool abortedByClient = EFalse;
    do
        {
        result = NextCycle();
        if ( result != KErrNone )
            {
            if ( !SendStepFailed( *this, result, iObserver ) )
                {
                // Set aborted, we can't continue because this operation might
                // be already deleted.
                abortedByClient = ETrue;
                }
            }
        } while ( !abortedByClient &&           // Client is not aborted this operation
                  result != KErrNone && 
                  iCommitCounter < totalCount );
                  
    if ( !abortedByClient && result != KErrNone && iCommitCounter >= totalCount )
        {
        iObserver.OperationComplete( *this );
        }
    }
} // namespace VPbkSimStore
//  End of File  

