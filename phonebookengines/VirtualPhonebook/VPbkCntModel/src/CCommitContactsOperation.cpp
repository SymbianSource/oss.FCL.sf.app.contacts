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
* Description:  The virtual phonebook commit contacts operation
*
*/


#include "CCommitContactsOperation.h"
#include "CContactStore.h"
#include "CContact.h"


#include <MVPbkBatchOperationObserver.h>

#include <cntdef.h>
#include <cntdb.h>

namespace VPbkCntModel {

const TInt KStepSize = 1;


CCommitContactsOperation::CCommitContactsOperation(
        CContactStore& aContactStore, 
        MVPbkBatchOperationObserver& aObserver ) :
    CActive( CActive::EPriorityIdle ),
    iContactStore( aContactStore ),
    iObserver( aObserver ),
    iState( ECommit )
    {
    CActiveScheduler::Add( this );
    }

inline void CCommitContactsOperation::ConstructL( const TArray<CContact*>& 
                                                    aContacts )
    {
    const TInt count = aContacts.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iContacts.Append( aContacts[i] );
        }
    }

CCommitContactsOperation* CCommitContactsOperation::NewL(
        CContactStore& aContactStore, 
        const TArray<CContact*> aContacts, 
        MVPbkBatchOperationObserver& aObserver )
    {
    CCommitContactsOperation* self = new ( ELeave ) CCommitContactsOperation
                                                ( aContactStore, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aContacts );
    CleanupStack::Pop( self );
    return self;
    }

CCommitContactsOperation::~CCommitContactsOperation()
    {
    Cancel();
    iContacts.Reset();
    }

void CCommitContactsOperation::RunL()
    {
    NextStepL();
    }

TInt CCommitContactsOperation::RunError( TInt aError )
    {
    if ( iObserver.StepFailed( *this, KStepSize, aError ) )
        {
        IssueRequest();    
        }    
    return KErrNone;
    }

void CCommitContactsOperation::DoCancel()
    {
    // Do nothing
    }

void CCommitContactsOperation::NextStepL()
    {
    if (iContacts.Count() > 0)
        {
        CContact* contact = iContacts[0];
        iContacts.Remove( 0 );
        contact->CommitL( *this );
        }
    else
        {
        iObserver.OperationComplete( *this );
        }
    }

void CCommitContactsOperation::IssueRequest()
    {
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

void CCommitContactsOperation::StartL()
    {
    IssueRequest();
    }

void CCommitContactsOperation::Cancel()
    {
    CActive::Cancel();
    }

void CCommitContactsOperation::ContactOperationCompleted
    ( TContactOpResult /* aResult */ )
    {
    iObserver.StepComplete( *this, KStepSize );
    IssueRequest();
    }

void CCommitContactsOperation::ContactOperationFailed
        ( TContactOp /* aOpCode */, 
          TInt aErrorCode, 
          TBool /* aErrorNotified */ )
    {
    if ( iObserver.StepFailed( *this, KStepSize, aErrorCode ) )
        {
        // Continue only if ETrue is returned. Otherwise this operation might
        // be already deleted.
        IssueRequest();        
        }
    }

} // namespace VPbkCntModel
//End of file
