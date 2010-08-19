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
* Description:  An API for sim contacts
*
*/



// INCLUDE FILES
#include "CVPbkSimContactBase.h"

#include "MVPbkSimCntStore.h"
#include "CVPbkSimCntField.h"
#include <RVPbkStreamedIntArray.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVPbkSimContactBase::CVPbkSimContactBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVPbkSimContactBase::CVPbkSimContactBase( MVPbkSimCntStore& aStore )
:   iStore( aStore )
    {
    }

// -----------------------------------------------------------------------------
// CVPbkSimContactBase::ParentStore
// -----------------------------------------------------------------------------
//
MVPbkSimCntStore& CVPbkSimContactBase::ParentStore() const
    {
    return iStore;
    }
    
// -----------------------------------------------------------------------------
// CVPbkSimContactBase::DeleteL
// -----------------------------------------------------------------------------
//
MVPbkSimStoreOperation* CVPbkSimContactBase::DeleteL( 
        MVPbkSimContactObserver& aObserver )
    {
    RVPbkStreamedIntArray indexArray;
    CleanupClosePushL( indexArray );
    indexArray.AppendIntL( SimIndex() );
    MVPbkSimStoreOperation* op = iStore.DeleteL( indexArray, aObserver );
    CleanupStack::PopAndDestroy();  // indexArray
    return op;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContactBase::FindField
// -----------------------------------------------------------------------------
//
MVPbkSimContact::TFieldLookup CVPbkSimContactBase::FindField( 
    TVPbkSimCntFieldType aType ) const
    {
    MVPbkSimContact::TFieldLookup lookup;
    lookup.SetType( aType );
    const TInt firstPosition = 0;
    lookup.SetPos( firstPosition );
    FindNextField( lookup );
    return lookup;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContactBase::FindNextField
// -----------------------------------------------------------------------------
//
void CVPbkSimContactBase::FindNextField( 
    MVPbkSimContact::TFieldLookup& aLookup ) const
    {
    const TInt count = FieldCount();
    TInt i = aLookup.Pos();
    aLookup.SetPos( KErrNotFound );
    for ( ; i < count; ++i )
        {
        if( aLookup.Type() == EVPbkSimAdditionalNumber )
            {
            const CVPbkSimCntField& field = ConstFieldAt( i );
            if ( field.Type() == EVPbkSimAdditionalNumber1 
                    || field.Type() == EVPbkSimAdditionalNumber2
                    || field.Type() == EVPbkSimAdditionalNumber3
                    || field.Type() == EVPbkSimAdditionalNumberLast ) // the same as EVPbkSimAdditionalNumber
                {
                aLookup.SetIndex( i );
                aLookup.SetPos( i + 1 );
                break;
                }
            }
        else
            {
            if ( ConstFieldAt( i ).Type() == aLookup.Type() )
                {
                aLookup.SetIndex( i );
                aLookup.SetPos( i + 1 );
                break;
                }
            }
        }
    }
//  End of File  
