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
* Description:  An array of sim contacts
*
*/



// INCLUDE FILES
#include "CContactArray.h"
#include "VPbkSimStoreImplError.h"
#include "CVPbkSimContactBase.h"
#include "CVPbkSimCntField.h"
#include <VPbkSimStoreTemplateFunctions.h>
#include <VPbkSimStoreCommon.h>

namespace VPbkSimStoreImpl {

// CONSTANTS

// expands memory with this amount if array gets too small
const TInt KCntArrayGranularity = 50;

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContactArray::CContactArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContactArray::CContactArray() : iCntArray( KCntArrayGranularity )
    {
    }
    
// Destructor
CContactArray::~CContactArray()
    {
    iObservers.Close();
    iCntArray.ResetAndDestroy();
    iCntArray.Close();
    }

// -----------------------------------------------------------------------------
// CContactArray::AddOrReplaceL
// -----------------------------------------------------------------------------
//
void CContactArray::AddOrReplaceL( MVPbkSimContact* aSimContact )
    {
    // SIM contacts must have a valid SIM index because this array is ordered
    // by the SIM index.
    __ASSERT_DEBUG( aSimContact && aSimContact->SimIndex() >= 
        KVPbkSimStoreFirstETelIndex,
        VPbkSimStoreImpl::Panic( ECContactArrayAddOrReplaceL_NullSimContact ) );
    if ( !aSimContact || 
         aSimContact->SimIndex() < KVPbkSimStoreFirstETelIndex )
        {
        User::Leave( KErrArgument );
        }
    
    // If array is not big enough, enlarge it.
    ReserveL( aSimContact->SimIndex() );
    TInt arrayIndex = aSimContact->SimIndex() - 1;

    // Take a pointer to the possible old contact. There must not be
    // any leaving operations after this.
    MVPbkSimContact* cnt = iCntArray[arrayIndex];
    // Replace the possible existing contact
    iCntArray[arrayIndex] = aSimContact;
    if ( cnt )
        {
        SendObserverMessageRR( iObservers,
            &MContactArrayObserver::ContactChanged, *cnt, *aSimContact );
        // Delete old contact
        delete cnt;
        }
    else
        {
        SendObserverMessageR( iObservers, &MContactArrayObserver::ContactAdded,
            *aSimContact );
        }        
    }

// -----------------------------------------------------------------------------
// CContactArray::Delete
// -----------------------------------------------------------------------------
//
void CContactArray::Delete( TInt aFromSimIndex, TInt aToSimIndex )
    {
    TInt indexFrom = aFromSimIndex - 1;
    TInt indexTo = aToSimIndex - 1;
    if ( indexTo < indexFrom )
        {
        indexTo = indexFrom;
        }

    TInt size = Size();
    TInt simIndex = aFromSimIndex;
    for ( TInt i = indexFrom; i < size && i <= indexTo; 
        ++i, ++simIndex )
        {
        if ( iCntArray[i] )
            {
            // There must not be any leaving operation after this
            MVPbkSimContact* cnt = iCntArray[i];
            iCntArray[i] = NULL;
            SendObserverMessageR( iObservers, 
                &MContactArrayObserver::ContactRemoved, *cnt );
            delete cnt;
            }
        }
    }

// -----------------------------------------------------------------------------
// CContactArray::At
// -----------------------------------------------------------------------------
//
MVPbkSimContact* CContactArray::At( TInt aSimIndex ) const
    {
    // indexes start from 0 in the array and from 1 in the sim
    TInt arrayIndex = aSimIndex - 1;
    __ASSERT_DEBUG( arrayIndex < iCntArray.Count(),
        VPbkSimStoreImpl::Panic( EInvalidSimIndex ) );
    
    if ( arrayIndex < iCntArray.Count() )
        {
        return iCntArray[arrayIndex];
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactArray::ResetAndCompress
// -----------------------------------------------------------------------------
//
void CContactArray::DeleteAll()
    {
    const TInt firstSimIndex = 1;
    const TInt lastSimIndex = iCntArray.Count();
    Delete( firstSimIndex, lastSimIndex );
    }

// -----------------------------------------------------------------------------
// CContactArray::AddObserverL
// -----------------------------------------------------------------------------
//
void CContactArray::AddObserverL( MContactArrayObserver& aObserver )
    {
    iObservers.AppendL( &aObserver );
    }

// -----------------------------------------------------------------------------
// CContactArray::AddObserverL
// -----------------------------------------------------------------------------
//
void CContactArray::RemoveObserver( MContactArrayObserver& aObserver )
    {
    TInt pos = iObservers.Find( &aObserver );
    if ( pos >= 0 )
        {
        iObservers.Remove( pos );
        }
    }

// -----------------------------------------------------------------------------
// CContactArray::ReserveL
// -----------------------------------------------------------------------------
//
void CContactArray::ReserveL( TInt aSize )
    {
    while ( iCntArray.Count() < aSize )
        {
        iCntArray.AppendL( NULL );
        }
    }

// -----------------------------------------------------------------------------
// CContactArray::FirstEmptySlot
// -----------------------------------------------------------------------------
//
TInt CContactArray::FirstEmptySlot() const
    {
    TInt slot = iCntArray.Find(NULL);
    if ( slot != KErrNotFound )
        {
        // SIM indexes start from 1.
        ++slot;
        }
    return slot;
    }

} // namespace VPbkSimStoreImpl
//  End of File  
