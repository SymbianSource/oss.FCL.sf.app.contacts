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
* Description:  A read-only sim contact buffer
*
*/



// INCLUDE FILES
#include "CVPbkSimContactBuf.h"

#include "VPbkSimStoreTemplateFunctions.h"
#include "CVPbkSimCntField.h"
#include "MVPbkSimCntStore.h"
#include "VPbkSimStoreImplError.h"
#include "CVPbkETelCntConverter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVPbkSimContactBuf::CVPbkSimContactBuf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVPbkSimContactBuf::CVPbkSimContactBuf( MVPbkSimCntStore& aSimStore )
:   CVPbkSimContactBase( aSimStore )
    {
    }

// -----------------------------------------------------------------------------
// CVPbkSimContactBuf::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVPbkSimContactBuf::ConstructL( const TDesC8* aETelContact )
    {
    if ( aETelContact )
        {
        iData = aETelContact->AllocL();
        }
    iCurrentField = CVPbkSimCntField::NewL( EVPbkSimUnknownType, *this );
    }

// -----------------------------------------------------------------------------
// CVPbkSimContactBuf::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimContactBuf* CVPbkSimContactBuf::NewL( 
    const TDesC8& aETelContact, MVPbkSimCntStore& aSimStore )
    {
    CVPbkSimContactBuf* self = NewLC( aETelContact, aSimStore );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContactBuf::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimContactBuf* CVPbkSimContactBuf::NewL( 
    MVPbkSimCntStore& aSimStore )
    {
    CVPbkSimContactBuf* self = NewLC( aSimStore );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContactBuf::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimContactBuf* CVPbkSimContactBuf::NewLC( 
    const TDesC8& aETelContact, MVPbkSimCntStore& aSimStore )
    {
    CVPbkSimContactBuf* self = new( ELeave ) CVPbkSimContactBuf( aSimStore );
    CleanupStack::PushL( self );
    self->ConstructL( &aETelContact );
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContactBuf::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimContactBuf* CVPbkSimContactBuf::NewLC( 
    MVPbkSimCntStore& aSimStore )
    {
    CVPbkSimContactBuf* self = new( ELeave ) CVPbkSimContactBuf( aSimStore );
    CleanupStack::PushL( self );
    self->ConstructL( NULL );
    return self;
    }

// Destructor
CVPbkSimContactBuf::~CVPbkSimContactBuf()
    {
    delete iData;
    delete iCurrentField;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContactBuf::ConstFieldAt
// -----------------------------------------------------------------------------
//
const CVPbkSimCntField& CVPbkSimContactBuf::ConstFieldAt( TInt aIndex ) const
    {
    if ( iData )
        {
        TPtr8 ptr( iData->Des() );
        iStore.ContactConverter().FieldAt( ptr, aIndex, *iCurrentField );
        }
    return *iCurrentField;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContactBuf::FieldAt
// -----------------------------------------------------------------------------
//
TInt CVPbkSimContactBuf::FieldCount() const
    {
    if ( iData )
        {
        TPtr8 ptr( iData->Des() );
        TInt count = iStore.ContactConverter().FieldCount( ptr );
        __ASSERT_DEBUG( count >= 0, VPbkSimStoreImpl::Panic( 
            VPbkSimStoreImpl::EInvalidETelContactForFieldCount ) );
        return count;
        }
    // if data buffer is not allocated then there are no fields
    return 0;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContactBuf::SimIndex
// -----------------------------------------------------------------------------
//
TInt CVPbkSimContactBuf::SimIndex() const
    {
    if ( iData )
        {
        TPtr8 ptr( iData->Des() );
        return iStore.ContactConverter().SimIndex( ptr );
        }
    return KVPbkSimStoreFirstFreeIndex;
    }

// -----------------------------------------------------------------------------
// CVPbkSimContactBuf::ETelContactL
// -----------------------------------------------------------------------------
//
const TDesC8& CVPbkSimContactBuf::ETelContactL() const
    {
    return *iData;
    }
  
// -----------------------------------------------------------------------------
// CVPbkSimContactBuf::SetL
// -----------------------------------------------------------------------------
// 
void CVPbkSimContactBuf::SetL( const TDesC8& aETelContact )
    {
    TPtr8 ptr( NULL, 0 );
    VPbkSimStoreImpl::CheckAndUpdateBufferSizeL( iData, ptr, 
        aETelContact.Length() );
    ptr.Copy( aETelContact );
    }
//  End of File  
