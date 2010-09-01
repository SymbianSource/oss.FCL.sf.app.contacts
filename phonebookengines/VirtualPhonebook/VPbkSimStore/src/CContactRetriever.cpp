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
* Description:  A virtual phonebook operation class that reads a contact
*
*/



// INCLUDE FILES
#include "CContactRetriever.h"

#include <MVPbkSingleContactOperationObserver.h>
#include "CContact.h"
#include "CContactStore.h"

namespace VPbkSimStore {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContactRetriever::CContactRetriever
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContactRetriever::CContactRetriever( TInt aSimIndex,
    CContactStore& aStore, MVPbkSingleContactOperationObserver& aObserver )
    :   CActive( EPriorityStandard ),
        iSimIndex ( aSimIndex ),
        iStore( aStore ),
        iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CContactRetriever::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CContactRetriever::ConstructL()
    {
    CActiveScheduler::Add( this );
    }
    
// -----------------------------------------------------------------------------
// CContactRetriever::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactRetriever* CContactRetriever::NewL( CContactStore& aStore, 
    TInt aSimIndex, MVPbkSingleContactOperationObserver& aObserver )
    {
    CContactRetriever* self = 
        new( ELeave ) CContactRetriever( aSimIndex, aStore, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CContactRetriever::~CContactRetriever
// -----------------------------------------------------------------------------
//
CContactRetriever::~CContactRetriever()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CContactRetriever::RunL
// -----------------------------------------------------------------------------
//
void CContactRetriever::RunL()
    {
    CContact* contact = iStore.ReadContactL( iSimIndex );
    iObserver.VPbkSingleContactOperationComplete( *this, contact );
    // Observer took ownership of the contact
    }

// -----------------------------------------------------------------------------
// CContactRetriever::DoCancel
// -----------------------------------------------------------------------------
//    
void CContactRetriever::DoCancel()
    {
    // Nothing to cancel
    }

// -----------------------------------------------------------------------------
// CContactRetriever::RunError
// -----------------------------------------------------------------------------
//    
TInt CContactRetriever::RunError( TInt aError )
    {
    iObserver.VPbkSingleContactOperationFailed( *this, aError );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CContactRetriever::StartL
// -----------------------------------------------------------------------------
//
void CContactRetriever::StartL()
    {
    TRequestStatus* st = &iStatus;
    User::RequestComplete( st, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CContactRetriever::Cancel
// -----------------------------------------------------------------------------
//
void CContactRetriever::Cancel()
    {
    CActive::Cancel();
    }

} // namespace VPbkSimStore

//  End of File  
