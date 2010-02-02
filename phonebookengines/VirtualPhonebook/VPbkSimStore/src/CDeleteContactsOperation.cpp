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
* Description:  A contact operation for deleting multiple contacts
*
*/



// INCLUDE FILES
#include "CDeleteContactsOperation.h"

#include <MVPbkBatchOperationObserver.h>
#include "CContactStore.h"
#include "CRemoteStore.h"
#include "VPbkSimStoreError.h"

namespace VPbkSimStore {

// CONSTANTS

// Contacts are delete one contact at a time
const TInt KStepSize = 1;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SendStepFailed
// Send step failed event to given observer.
// -----------------------------------------------------------------------------
//
inline TBool SendStepFailed( CDeleteContactsOperation& aOperation, TInt aError,
    MVPbkBatchOperationObserver& aObserver )
    { 
    return aObserver.StepFailed( aOperation, KStepSize, aError );
    }

// -----------------------------------------------------------------------------
// SendStepComplete
// Send step completed event to given observer.
// -----------------------------------------------------------------------------
//
inline void SendStepComplete( CDeleteContactsOperation& aOperation,
    MVPbkBatchOperationObserver& aObserver )
    {
    aObserver.StepComplete( aOperation, KStepSize );
    }

// -----------------------------------------------------------------------------
// SendOperationComplete
// Send operation completed event to given observer.
// -----------------------------------------------------------------------------
//
inline void SendOperationComplete( CDeleteContactsOperation& aOperation,
    MVPbkBatchOperationObserver& aObserver )
    {
    aObserver.OperationComplete( aOperation );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::CDeleteContactsOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDeleteContactsOperation::CDeleteContactsOperation( CContactStore& aStore, 
    MVPbkBatchOperationObserver& aObserver )
    :   iStore( aStore ),
        iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::ConstructL( 
    const TArray<TInt>& aSimIndexes )
    {
    const TInt count = aSimIndexes.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iSimIndexes.AppendIntL( aSimIndexes[i] );
        }
        
    __ASSERT_DEBUG( iSimIndexes.Count() > 0,
        VPbkSimStore::Panic( VPbkSimStore::EZeroIndexesInDeleteOperation ) );
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDeleteContactsOperation* CDeleteContactsOperation::NewL( 
    CContactStore& aStore, MVPbkBatchOperationObserver& aObserver,
    const TArray<TInt>& aSimIndexes )
    {
    CDeleteContactsOperation* self = 
        new( ELeave ) CDeleteContactsOperation( aStore, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aSimIndexes );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CDeleteContactsOperation::~CDeleteContactsOperation()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::StartL
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::StartL()
    {
    // Open store because the operation must listen to store events.
    iStore.OpenL( *this );
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::Cancel
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::Cancel()
    {
    delete iSimOperation;
    iSimOperation = NULL;
    iStore.Close( *this );
    iSimIndexes.Close();
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::ContactEventComplete
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::ContactEventComplete( TEvent /*aEvent*/, 
    CVPbkSimContact* /*aContact*/ )
    {
    // All the contacts have been deleted. Complete operation.
    SendOperationComplete( *this, iObserver );  
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::ContactEventError
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::ContactEventError( TEvent /*aEvent*/, 
    CVPbkSimContact* /*aContact*/, TInt aError )
    {
    // An error happened, the operatio can not continue.
    CompleteWithError( aError );
    }
            
// -----------------------------------------------------------------------------
// CDeleteContactsOperation::ContactEventError
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::StoreReady(MVPbkContactStore& /*aContactStore*/)
    {
    TRAPD( res, iSimOperation = iStore.NativeStore().DeleteL( 
        iSimIndexes, *this ) );
    if ( res != KErrNone )
        {
        CompleteWithError( res );
        }
    }
    
// -----------------------------------------------------------------------------
// CDeleteContactsOperation::ContactEventError
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::StoreUnavailable(
        MVPbkContactStore& /*aContactStore*/, 
        TInt aReason)
    {
    CompleteWithError( aReason );
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::ContactEventError
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/, 
        TVPbkContactStoreEvent aStoreEvent)
    {
    if ( aStoreEvent.iEventType == TVPbkContactStoreEvent::EContactDeleted )
        {
        SendStepComplete( *this, iObserver );
        }
    }

// -----------------------------------------------------------------------------
// CDeleteContactsOperation::CompleteWithError
// -----------------------------------------------------------------------------
//
void CDeleteContactsOperation::CompleteWithError( TInt aError )
    {
    // Delete the async operation so that it doesn't send events after error
    delete iSimOperation;
    iSimOperation = NULL;
    // Close store so that the operation doesn't get store events after error.
    iStore.Close( *this );
    // Send error code to client.
    if ( SendStepFailed( *this, aError, iObserver ) )
        {
        // If client does continue call OperationComplete() for it.
        SendOperationComplete( *this, iObserver );        
        }
    }    
} // namespace VPbkSimStore
//  End of File  
