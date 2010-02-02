/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An operation for find for store
*
*/


#include <CVPbkContactFindFromStoresOperation.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactLinkArray.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CVPbkContactFindFromStoresOperation::CVPbkContactFindFromStoresOperation
// ---------------------------------------------------------------------------
//
CVPbkContactFindFromStoresOperation::CVPbkContactFindFromStoresOperation(
        MVPbkContactFindFromStoresObserver& aObserver )
        :   iObserver( aObserver )
    {
    }


// ---------------------------------------------------------------------------
// CVPbkContactFindFromStoresOperation::ConstructL
// ---------------------------------------------------------------------------
//
void CVPbkContactFindFromStoresOperation::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CVPbkContactFindFromStoresOperation::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactFindFromStoresOperation* 
        CVPbkContactFindFromStoresOperation::NewLC( 
            MVPbkContactFindFromStoresObserver& aObserver )
    {
    CVPbkContactFindFromStoresOperation* self = 
        new( ELeave ) CVPbkContactFindFromStoresOperation( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CVPbkContactFindFromStoresOperation::~CVPbkContactFindFromStoresOperation
// ---------------------------------------------------------------------------
//
CVPbkContactFindFromStoresOperation::~CVPbkContactFindFromStoresOperation()
    {
    }

// ---------------------------------------------------------------------------
// CVPbkContactFindFromStoresOperation::HandleZeroSuboperations
// ---------------------------------------------------------------------------
//
void CVPbkContactFindFromStoresOperation::HandleZeroSuboperations()
    {
    iObserver.FindFromStoresOperationComplete();
    }
    
// ---------------------------------------------------------------------------
// CVPbkContactFindFromStoresOperation::FindFromStoreSucceedL
// ---------------------------------------------------------------------------
//
void CVPbkContactFindFromStoresOperation::FindFromStoreSucceededL( 
        MVPbkContactStore& aStore, 
        MVPbkContactLinkArray* aResultsFromStore )
    {
    // Client takes ownership of aResultsFromStore immediately
    iObserver.FindFromStoreSucceededL( aStore, aResultsFromStore );
    const TDesC16& check = aStore.StoreProperties().Uri().UriDes();
    }

// ---------------------------------------------------------------------------
// CVPbkContactFindFromStoresOperation::FindFromStoreFailed
// ---------------------------------------------------------------------------
//    
void CVPbkContactFindFromStoresOperation::FindFromStoreFailed( 
        MVPbkContactStore& aStore, TInt aError )
    {
    iObserver.FindFromStoreFailed( aStore, aError );
    }

// ---------------------------------------------------------------------------
// CVPbkContactFindFromStoresOperation::FindFromStoreSucceed
// ---------------------------------------------------------------------------
//
void CVPbkContactFindFromStoresOperation::FindFromStoresOperationComplete()
    {
    ++iNumberOfCompleted;
    if ( iNumberOfCompleted >= SubOperationCount() )
        {
        iObserver.FindFromStoresOperationComplete();
        }
    }

