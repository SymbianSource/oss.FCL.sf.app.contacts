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
* Description:  A virtual phonebook operation for find feature
*
*/



// INCLUDE FILES
#include "CFindOperation.h"

#include <MVPbkFieldType.h>
#include <MVPbkContactFindObserver.h>
#include <CVPbkContactLinkArray.h>
#include "CContactStore.h"
#include "CRemoteStore.h"
#include "CSupportedFieldTypes.h"
#include "CFieldTypeMappings.h"
#include "CContactLink.h"

namespace VPbkSimStore {

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFindOperation::CFindOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFindOperation::CFindOperation( CContactStore& aStore,
    MVPbkContactFindObserver& aObserver ) 
    :   CActive( EPriorityStandard ),
        iStore( aStore ),
        iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CFindOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFindOperation::ConstructL( const TDesC& aStringToFind,
    const MVPbkFieldTypeList& aFieldTypes )
    {
    CActiveScheduler::Add( this );
    iStringToFind = aStringToFind.AllocL();
    iLinkArray = CVPbkContactLinkArray::NewL();
    const TInt count = aFieldTypes.FieldTypeCount();
    for ( TInt i = 0; i < count; ++i )
        {
        const MVPbkFieldType& type = aFieldTypes.FieldTypeAt( i );
        if ( iStore.SupportedFieldTypes().ContainsSame( type ) )
            {
            iFieldTypes.AppendL( iStore.FieldTypeMappings().Match( type ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CFindOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFindOperation* CFindOperation::NewL( CContactStore& aStore, 
    MVPbkContactFindObserver& aObserver, const TDesC& aStringToFind, 
    const MVPbkFieldTypeList& aFieldTypes )
    {
    CFindOperation* self = new( ELeave ) CFindOperation( aStore, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aStringToFind, aFieldTypes );
    CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
CFindOperation::~CFindOperation()
    {
    Cancel();
    iFieldTypes.Close();
    delete iStringToFind;
    delete iSimOperation;
    delete iLinkArray;
    }

// -----------------------------------------------------------------------------
// CFindOperation::RunL
// -----------------------------------------------------------------------------
//
void CFindOperation::RunL()
    {
    CompleteL();
    }

// -----------------------------------------------------------------------------
// CFindOperation::DoCancel
// -----------------------------------------------------------------------------
//
void CFindOperation::DoCancel()
    {
    delete iSimOperation;
    iSimOperation = NULL;
    }

// -----------------------------------------------------------------------------
// CFindOperation::RunError
// -----------------------------------------------------------------------------
//
TInt CFindOperation::RunError( TInt aError )
    {
    iObserver.FindFailed( aError );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CFindOperation::StartL
// -----------------------------------------------------------------------------
//
void CFindOperation::StartL()
    {
    if ( iFieldTypes.Count() > 0 )
        {
        iSimOperation = iStore.NativeStore().CreateFindOperationL( 
            *iStringToFind, iFieldTypes, *this );    
        }
    else
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNotFound );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CFindOperation::Cancel
// -----------------------------------------------------------------------------
//
void CFindOperation::Cancel()
    {
    CActive::Cancel();
    }

// -----------------------------------------------------------------------------
// CFindOperation::FindCompleted
// -----------------------------------------------------------------------------
//
void CFindOperation::FindCompleted( MVPbkSimCntStore& /*aStore*/,
    const RVPbkStreamedIntArray& aSimIndexArray )
    {
    TRAPD( res, HandleFindCompletedL( aSimIndexArray ) );
    if ( res != KErrNone )
        {
        iObserver.FindFailed( res );
        }
    }

// -----------------------------------------------------------------------------
// CFindOperation::FindError
// -----------------------------------------------------------------------------
//
void CFindOperation::FindError( MVPbkSimCntStore& /*aStore*/, 
    TInt aError )
    {
    iObserver.FindFailed( aError );
    }

// -----------------------------------------------------------------------------
// CFindOperation::FindError
// -----------------------------------------------------------------------------
//
void CFindOperation::HandleFindCompletedL( 
    const RVPbkStreamedIntArray& aSimIndexArray )
    {
    const TInt count = aSimIndexArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        CContactLink* link = 
            CContactLink::NewLC( iStore, aSimIndexArray[i] );
        iLinkArray->AppendL( link );
        CleanupStack::Pop( link );
        }
    CompleteL();
    }

// -----------------------------------------------------------------------------
// CFindOperation::CompleteL
// -----------------------------------------------------------------------------
//
void CFindOperation::CompleteL()
    {
    CVPbkContactLinkArray* results = iLinkArray;
    iLinkArray = NULL;
    iObserver.FindCompleteL( results );    
    }
} //namespace VPbkSimStore
//  End of File  
