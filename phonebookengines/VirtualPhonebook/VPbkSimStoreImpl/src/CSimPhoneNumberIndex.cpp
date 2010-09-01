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
* Description:  An index for sim phone numbers
*
*/



// INCLUDE FILES
#include "CSimPhoneNumberIndex.h"

#include <VPbkPhoneNumberIndex.h>
#include "MVPbkSimContact.h"
#include "CVPbkSimCntField.h"
#include "CStoreBase.h"

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSimPhoneNumberIndex::CSimPhoneNumberIndex
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSimPhoneNumberIndex::CSimPhoneNumberIndex( CStoreBase& aParentStore )
:   iParentStore( aParentStore )
    {
    }

// -----------------------------------------------------------------------------
// CSimPhoneNumberIndex::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSimPhoneNumberIndex::ConstructL()
    {
    iNumberIndex = 
        VPbkEngUtils::CPhoneNumberIndex<MVPbkSimContact>::NewL();
    ResetAndBuildIndexL();
    }

// -----------------------------------------------------------------------------
// CSimPhoneNumberIndex::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSimPhoneNumberIndex* CSimPhoneNumberIndex::NewL( CStoreBase& aParentStore )
    {
    CSimPhoneNumberIndex* self = 
        new( ELeave ) CSimPhoneNumberIndex( aParentStore );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
CSimPhoneNumberIndex::~CSimPhoneNumberIndex()
    {
    delete iNumberIndex;
    }

// -----------------------------------------------------------------------------
// CSimPhoneNumberIndex::ContactAdded
// -----------------------------------------------------------------------------
//
void CSimPhoneNumberIndex::ContactAdded( MVPbkSimContact& aSimContact )
    {
    /// There is no one to notify in error case. The worst case here is that
    /// index will not be updated if there is no memory and clients possibly
    /// miss a match
    TRAP_IGNORE( AppendNumbersL( aSimContact ) );
    }

// -----------------------------------------------------------------------------
// CSimPhoneNumberIndex::ContactRemoved
// -----------------------------------------------------------------------------
//
void CSimPhoneNumberIndex::ContactRemoved( MVPbkSimContact& aSimContact )
    {
    iNumberIndex->RemoveAll( aSimContact );
    }

// -----------------------------------------------------------------------------
// CSimPhoneNumberIndex::ContactChanged
// -----------------------------------------------------------------------------
//
void CSimPhoneNumberIndex::ContactChanged( MVPbkSimContact& aOldContact,
    MVPbkSimContact& aNewContact )
    {
    ContactRemoved( aOldContact );
    ContactAdded( aNewContact );
    }
    
// -----------------------------------------------------------------------------
// CSimPhoneNumberIndex::ReBuildL
// -----------------------------------------------------------------------------
//
void CSimPhoneNumberIndex::ReBuildL()
    {
    ResetAndBuildIndexL();
    }

// -----------------------------------------------------------------------------
// CSimPhoneNumberIndex::ResetAndBuildIndexL
// -----------------------------------------------------------------------------
//
void CSimPhoneNumberIndex::ResetAndBuildIndexL()
    {
    iNumberIndex->Reset();
    CContactArray& contacts = iParentStore.Contacts();
    const TInt count = contacts.Size();
    for ( TInt i = 1; i <= count; ++i )
        {
        MVPbkSimContact* cnt = contacts.At( i );
        if ( cnt )
            {
            AppendNumbersL( *cnt );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSimPhoneNumberIndex::AppendNumbersL
// -----------------------------------------------------------------------------
//
void CSimPhoneNumberIndex::AppendNumbersL( MVPbkSimContact& aContact )
    {
    MVPbkSimContact::TFieldLookup lookup = 
        aContact.FindField( EVPbkSimGsmNumber );
    if ( !lookup.EndOfLookup() )
        {
        iNumberIndex->AddL( aContact.ConstFieldAt( lookup.Index() ).Data(), 
            aContact );
        }
    lookup = aContact.FindField( EVPbkSimAdditionalNumber );
    while ( !lookup.EndOfLookup() )
        {
        iNumberIndex->AddL( aContact.ConstFieldAt( lookup.Index() ).Data(), 
            aContact );
        aContact.FindNextField( lookup );
        }
    }
} // namespace VPbkSimStore
//  End of File  
