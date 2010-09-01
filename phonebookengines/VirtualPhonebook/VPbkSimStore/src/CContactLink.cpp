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
* Description:  Implements a contact link to sim contact
*
*/



// INCLUDE FILES
#include "CContactLink.h"

#include "CContactStore.h"
#include "CContact.h"
#include "CViewContact.h"
#include "CContactView.h"
#include "CVPbkSimContact.h"

#include <MVPbkContactStoreProperties.h>
#include <s32mem.h>


namespace VPbkSimStore {

const TInt KBytesToWord( 4 );
const TInt KBytesToFieldId( 4 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContactLink::CContactLink
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContactLink::CContactLink(CContactStore& aStore )
:   iStore( aStore )
    {
    }

// -----------------------------------------------------------------------------
// CContactLink::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CContactLink::ConstructL( TInt aSimIndex, TInt aFieldId )
    {
    iSimIndex = aSimIndex;
    iFieldId = aFieldId;
    }

// -----------------------------------------------------------------------------
// CContactLink::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CContactLink::ConstructL( RReadStream& aReadStream )
    {
    iSimIndex = aReadStream.ReadInt32L();
    iFieldId = aReadStream.ReadInt32L();
    }

// -----------------------------------------------------------------------------
// CContactLink::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactLink* CContactLink::NewLC( CContactStore& aStore, 
    TInt aSimIndex, TInt aFieldId )
    {
    CContactLink* self = new( ELeave ) CContactLink( aStore );
    CleanupStack::PushL( self );
    self->ConstructL( aSimIndex, aFieldId );
    return self;
    }

// -----------------------------------------------------------------------------
// CContactLink::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactLink* CContactLink::NewLC( CContactStore& aStore, 
    RReadStream& aReadStream )
    {
    CContactLink* self = new( ELeave ) CContactLink( aStore );
    CleanupStack::PushL( self );
    self->ConstructL( aReadStream );
    return self;
    }

// -----------------------------------------------------------------------------
// CContactLink::IsValid
// -----------------------------------------------------------------------------
//
TBool CContactLink::IsValid( const MVPbkContactStore& aStore,
    const MVPbkContactLink& aLink )
    {
    return &aStore == &aLink.ContactStore();
    }

// -----------------------------------------------------------------------------
// CContactLink::ContactStore
// -----------------------------------------------------------------------------
//
MVPbkContactStore& CContactLink::ContactStore() const
    {
    return iStore;
    }
  
// -----------------------------------------------------------------------------
// CContactLink::IsSame
// -----------------------------------------------------------------------------
//  
TBool CContactLink::IsSame( const MVPbkContactLink& aOther ) const
    {
    TBool result = EFalse;
    if ( &ContactStore() == &aOther.ContactStore() )
        {
        const CContactLink& other = 
            static_cast<const CContactLink&>(aOther);
        result = ( other.SimIndex() == SimIndex() && other.FieldId() == FieldId() );
        }
    return result;
    }
    
// -----------------------------------------------------------------------------
// CContactLink::RefersTo
// -----------------------------------------------------------------------------
//    
TBool CContactLink::RefersTo( const MVPbkBaseContact& aContact ) const
    {
    TBool result = EFalse;
    const CContact* contact = dynamic_cast<const CContact*>(&aContact);
    if ( contact )
        {
        if ( &ContactStore() == &contact->ParentStore() )
            {
            result = ( SimIndex() == contact->SimContact().SimIndex() );
            }
        }
    else 
        {
        const CViewContact* contact = dynamic_cast<const CViewContact*>(&aContact);
        if ( contact && (&ContactStore() == &contact->View().ContactStore()) )
            {
            result = ( SimIndex() == contact->SimIndex() );
            }
        }
    return result;
    }

const MVPbkContactLinkPacking& CContactLink::Packing() const
    {
    return *this;
    }
    
MVPbkContactLink* CContactLink::CloneLC() const
    {
    return CContactLink::NewLC( iStore, iSimIndex, iFieldId );
    }

TInt CContactLink::InternalPackedSize() const
    {
    return ( KBytesToWord + KBytesToFieldId );                        
    }

void CContactLink::PackInternalsL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iSimIndex ); // SIM index
    aStream.WriteInt32L( iFieldId ); // Field id
    }


} // namespace VPbkSimStore

//  End of File  
