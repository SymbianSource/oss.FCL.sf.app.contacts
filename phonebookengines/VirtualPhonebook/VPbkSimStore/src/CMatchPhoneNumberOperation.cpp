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
* Description:  An operation for number matching
*
*/



// INCLUDE FILES
#include "CMatchPhoneNumberOperation.h"

#include <MVPbkContactFindObserver.h>
#include <MVPbkSimStoreOperation.h>
#include <CVPbkContactLinkArray.h>
#include "CContactStore.h"
#include "CRemoteStore.h"
#include "CContactLink.h"

namespace VPbkSimStore {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMatchPhoneNumberOperation::CMatchPhoneNumberOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMatchPhoneNumberOperation::CMatchPhoneNumberOperation( 
    TInt aMaxMatchDigits, MVPbkContactFindObserver& aObserver, 
    CContactStore& aStore )
    :   iMaxMatchDigits( aMaxMatchDigits ),
        iObserver( aObserver ),
        iStore( aStore )
    {
    }

// -----------------------------------------------------------------------------
// CMatchPhoneNumberOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMatchPhoneNumberOperation::ConstructL( const TDesC& aPhoneNumber )
    {
    iPhoneNumber = aPhoneNumber.AllocL();
    }

// -----------------------------------------------------------------------------
// CMatchPhoneNumberOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMatchPhoneNumberOperation* CMatchPhoneNumberOperation::NewL( 
        const TDesC& aPhoneNumber, 
        TInt aMaxMatchDigits,
        MVPbkContactFindObserver& aObserver,
        CContactStore& aStore )
    {
    CMatchPhoneNumberOperation* self = 
        new( ELeave ) CMatchPhoneNumberOperation( aMaxMatchDigits, 
            aObserver, aStore );
    CleanupStack::PushL( self );
    self->ConstructL( aPhoneNumber );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CMatchPhoneNumberOperation::~CMatchPhoneNumberOperation()
    {       
    delete iSimOperation;
    delete iPhoneNumber;
    }

// -----------------------------------------------------------------------------
// CMatchPhoneNumberOperation::StartL
// -----------------------------------------------------------------------------
//
void CMatchPhoneNumberOperation::StartL()
    {
    iSimOperation = iStore.NativeStore().CreateMatchPhoneNumberOperationL( 
        *iPhoneNumber, iMaxMatchDigits, *this );
    }

// -----------------------------------------------------------------------------
// CMatchPhoneNumberOperation::Cancel
// -----------------------------------------------------------------------------
//
void CMatchPhoneNumberOperation::Cancel()
    {
    delete iSimOperation;
    iSimOperation = NULL;
    }

// -----------------------------------------------------------------------------
// CMatchPhoneNumberOperation::FindCompleted
// -----------------------------------------------------------------------------
//
void CMatchPhoneNumberOperation::FindCompleted( MVPbkSimCntStore& /*aStore*/,
    const RVPbkStreamedIntArray& aSimIndexArray )
    {
    TRAPD( res, HandleFindCompletedL( aSimIndexArray ) );
    if ( res != KErrNone )
        {
        iObserver.FindFailed( res );
        }
    }

// -----------------------------------------------------------------------------
// CMatchPhoneNumberOperation::FindError
// -----------------------------------------------------------------------------
//
void CMatchPhoneNumberOperation::FindError( MVPbkSimCntStore& /*aStore*/, 
    TInt aError )
    {
    iObserver.FindFailed( aError );
    }

// -----------------------------------------------------------------------------
// CMatchPhoneNumberOperation::HandleFindCompletedL
// -----------------------------------------------------------------------------
//
void CMatchPhoneNumberOperation::HandleFindCompletedL( 
    const RVPbkStreamedIntArray& aSimIndexArray )
    {
    CVPbkContactLinkArray* linkArray = NULL;
    const TInt count = aSimIndexArray.Count();
    if ( count > 0 )
        {
        linkArray = CVPbkContactLinkArray::NewLC();
        for ( TInt i = 0; i < count; ++i )
            {
            CContactLink* link = 
                CContactLink::NewLC( iStore, aSimIndexArray[i] );
            linkArray->AppendL( link );
            CleanupStack::Pop( link );
            }
        }
    
    // Pop before transferring ownership to the client.
    if ( linkArray )
        {
        CleanupStack::Pop( linkArray );
        }
        
    iObserver.FindCompleteL( linkArray );
    }
} // namespace VPbkSimStore
//  End of File  
