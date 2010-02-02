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
* Description:  A command that finds the contacts that matches to given number
*
*/



// INCLUDE FILES
#include "CNumberMatchOperation.h"

#include <VPbkPhoneNumberIndex.h>
#include "CVPbkSimContact.h"
#include "CVPbkSimCntField.h"
#include "CSimPhoneNumberIndex.h"
#include "RVPbkStreamedIntArray.h"
#include "MVPbkSimFindObserver.h"
#include "MVPbkSimCommandObserver.h"

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNumberMatchOperation::CNumberMatchOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNumberMatchOperation::CNumberMatchOperation( MVPbkSimFindObserver& aObserver,
    CSimPhoneNumberIndex& aSimPhoneNumberIndex, TInt aNumDigits, 
    MVPbkSimCntStore& aStore )
    :   CActive( EPriorityStandard ),
        iObserver( aObserver ),
        iNumberIndex( aSimPhoneNumberIndex ),
        iNumDigits( aNumDigits ),
        iStore( aStore )
    {
    }

// -----------------------------------------------------------------------------
// CNumberMatchOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNumberMatchOperation::ConstructL( const TDesC& aPhoneNumber )
    {
    CActiveScheduler::Add( this );
    iPhoneNumber = aPhoneNumber.Right( iNumDigits ).AllocL();
    }

// -----------------------------------------------------------------------------
// CNumberMatchOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNumberMatchOperation* CNumberMatchOperation::NewL( MVPbkSimFindObserver& aObserver,
    CSimPhoneNumberIndex& aSimPhoneNumberIndex,
    const TDesC& aPhoneNumber, TInt aNumDigits, MVPbkSimCntStore& aStore )
    {
    CNumberMatchOperation* self = new( ELeave ) CNumberMatchOperation( aObserver, 
        aSimPhoneNumberIndex, aNumDigits, aStore );
    CleanupStack::PushL( self );
    self->ConstructL( aPhoneNumber );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CNumberMatchOperation::~CNumberMatchOperation()
    {
    delete iPhoneNumber;
    Cancel();
    }

// -----------------------------------------------------------------------------
// CNumberMatchOperation::RunL
// -----------------------------------------------------------------------------
//
void CNumberMatchOperation::RunL()
    {
    VPbkEngUtils::RPhoneNumberLookup<MVPbkSimContact>lookup = 
        iNumberIndex.Index().StartLookupL( *iPhoneNumber, iNumDigits );
    CleanupClosePushL( lookup );

    RVPbkStreamedIntArray indexArray;
    CleanupClosePushL( indexArray );
    
    while ( !lookup.AtEnd() )
        {
        MVPbkSimContact& cnt = lookup.Current();
        TInt simIndex = cnt.SimIndex();
        if ( indexArray.Find( simIndex ) == KErrNotFound )
            {
            // Add sim index only if it hasn't been added
            indexArray.AppendIntL( simIndex );
            }
        iNumberIndex.Index().LookupNext( lookup );
        }
    iObserver.FindCompleted( iStore, indexArray );
    CleanupStack::PopAndDestroy(2); // indexArray, lookup
    }

// -----------------------------------------------------------------------------
// CNumberMatchOperation::DoCancel
// -----------------------------------------------------------------------------
//
void CNumberMatchOperation::DoCancel()
    {
    // Nothing to cancel here
    }

// -----------------------------------------------------------------------------
// CNumberMatchOperation::RunError
// -----------------------------------------------------------------------------
//
TInt CNumberMatchOperation::RunError( TInt aError )
    {
    iObserver.FindError( iStore, aError );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNumberMatchOperation::Execute
// -----------------------------------------------------------------------------
//
void CNumberMatchOperation::Execute()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }
} // namespace VPbkSimStoreImpl
//  End of File  
