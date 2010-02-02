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
* Description:  A find operation that loops the contact data
*
*/



// INCLUDE FILES
#include "CFindOperation.h"

#include "CVPbkSimContact.h"
#include "CVPbkSimCntField.h"
#include "CStoreBase.h"
#include "CContactArray.h"
#include "MVPbkSimFindObserver.h"
#include <CVPbkContactFindPolicy.h>


namespace VPbkSimStoreImpl {

// CONSTANTS
/// The amount of contacts to loop through in on cycle
const TInt KContactsToCheckInOneCycle = 50;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFindOperation::CFindOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFindOperation::CFindOperation( MVPbkSimFindObserver& aObserver,
    CStoreBase& aStore )
    :   CActive( EPriorityStandard ),
        iObserver( aObserver ),
        iStore( aStore )
    {
    }

// -----------------------------------------------------------------------------
// CFindOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFindOperation::ConstructL( const TDesC& aStringToFind, 
    RVPbkSimFieldTypeArray& aFieldTypes )
    {
    CActiveScheduler::Add( this );
    iStringToFind = aStringToFind.AllocL();
    const TInt count = aFieldTypes.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iFieldTypes.AppendL( aFieldTypes[i] );
        }

    // Create a find policy object via ECom
    iFindPolicy = CVPbkContactFindPolicy::NewL();
    }

// -----------------------------------------------------------------------------
// CFindOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFindOperation* CFindOperation::NewL( MVPbkSimFindObserver& aObserver,
    CStoreBase& aStore, const TDesC& aStringToFind, 
    RVPbkSimFieldTypeArray& aFieldTypes )
    {
    CFindOperation* self = new( ELeave ) CFindOperation( aObserver, aStore );
    CleanupStack::PushL( self );
    self->ConstructL( aStringToFind, aFieldTypes );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CFindOperation::~CFindOperation()
    {
    Cancel();
    delete iFindPolicy;
    delete iStringToFind;
    iFieldTypes.Close();
    iResults.Close();
    // Find policy is loaded as ECOM plug-in so clean ECOM server
    REComSession::FinalClose();
    }

// -----------------------------------------------------------------------------
// CFindOperation::Execute
// -----------------------------------------------------------------------------
//
void CFindOperation::Execute()
    {
    // Start from the first sim index
    iNextSimIndex = 1;
    iResults.Reset();
    NextCycle();
    }

// -----------------------------------------------------------------------------
// CFindOperation::RunL
// -----------------------------------------------------------------------------
//
void CFindOperation::RunL()
    {
    TInt lastIndex = iNextSimIndex + KContactsToCheckInOneCycle;
    TInt cntArraySize = iStore.Contacts().Size();
    if ( lastIndex > cntArraySize )
        {
        lastIndex = cntArraySize;
        }
        
    while ( iNextSimIndex <= lastIndex )
        {
        MVPbkSimContact* cnt = iStore.Contacts().At( iNextSimIndex );
        if ( cnt && IsMatch( *cnt ) )
            {
            iResults.AppendIntL( cnt->SimIndex() );
            }
        ++iNextSimIndex;
        }
    
    if ( iNextSimIndex > lastIndex )
        {
        iObserver.FindCompleted( iStore, iResults );
        }
    else
        {
        NextCycle();
        }
    }

// -----------------------------------------------------------------------------
// CFindOperation::DoCancel
// -----------------------------------------------------------------------------
//
void CFindOperation::DoCancel()
    {
    // Nothing to cancel here
    }

// -----------------------------------------------------------------------------
// CFindOperation::RunError
// -----------------------------------------------------------------------------
//
TInt CFindOperation::RunError( TInt aError )
    {
    iObserver.FindError( iStore, aError );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFindOperation::NextCycle
// -----------------------------------------------------------------------------
//
void CFindOperation::NextCycle()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CFindOperation::IsMatch
// -----------------------------------------------------------------------------
//
TBool CFindOperation::IsMatch( MVPbkSimContact& aContact )
    {
    const TInt count = iFieldTypes.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        MVPbkSimContact::TFieldLookup lookup = 
            aContact.FindField( iFieldTypes[i] );
        while ( !lookup.EndOfLookup() )
            {
            if ( IsMatch( aContact.ConstFieldAt( lookup.Index() ).Data() ) )
                {
                return ETrue;
                }
            aContact.FindNextField( lookup );
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CFindOperation::IsMatch
// -----------------------------------------------------------------------------
//
TBool CFindOperation::IsMatch( const TDesC& aString )
    {
    // uses virtual phonebook find policy for finding
    TBool ret = EFalse;
    
    // ignore find errors and return a false match
    TRAP_IGNORE(
        ret = iFindPolicy->MatchRefineL(aString, *iStringToFind));
    return ret;
    }

} // namespace VPbkSimStoreImpl

//  End of File  
