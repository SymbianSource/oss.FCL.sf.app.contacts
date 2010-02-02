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
* Description:  A write command for writing own number contacts
*
*/



// INCLUDE FILES
#include "COwnNumberWriteCmd.h"

#include "CVPbkSimContactBuf.h"
#include "CVPbkSimCntField.h"
#include "CStoreBase.h"
#include "MVPbkSimContactObserver.h"

namespace VPbkSimStoreImpl {
    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COwnNumberWriteCmd::COwnNumberWriteCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
COwnNumberWriteCmd::COwnNumberWriteCmd( CStoreBase& aStore, TInt& aSimIndex, 
    MVPbkSimContactObserver& aObserver )
    :   CBasicWriteCmd<RMobileONStore::TMobileONEntryV1>( aStore, aSimIndex, 
        aObserver )
    {
    }

// -----------------------------------------------------------------------------
// COwnNumberWriteCmd::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void COwnNumberWriteCmd::ConstructL( const TDesC8& aData )
    {
    // Convert contact to own number contact
    CVPbkSimContactBuf* cnt = CVPbkSimContactBuf::NewLC( aData, StoreBase() );
    
    CVPbkSimContactBase::TFieldLookup lookup = cnt->FindField( EVPbkSimName );
    if ( !lookup.EndOfLookup() )
        {
        const TDesC& data = cnt->ConstFieldAt( lookup.Index() ).Data();
        iETelContact.iText.Copy( data.Left( RMobileONStore::KOwnNumberTextSize ) );
        }

    lookup = cnt->FindField( EVPbkSimGsmNumber );
    if ( !lookup.EndOfLookup() )
        {
        const TDesC& data = cnt->ConstFieldAt( lookup.Index() ).Data();
        iETelContact.iNumber.iTelNumber.Copy( 
            data.Right( RMobilePhone::KMaxMobileTelNumberSize ) );
        }
    iETelContact.iIndex = cnt->SimIndex();
    CleanupStack::PopAndDestroy( cnt ); // cnt
    
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// COwnNumberWriteCmd::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
COwnNumberWriteCmd* COwnNumberWriteCmd::NewL( CStoreBase& aStore, 
    const TDesC8& aData, TInt& aSimIndex, 
    MVPbkSimContactObserver& aObserver )
    {
    COwnNumberWriteCmd* self = 
        new( ELeave ) COwnNumberWriteCmd( aStore, aSimIndex, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aData );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
COwnNumberWriteCmd::~COwnNumberWriteCmd()
    {
    }
} // namespace VPbkSimStoreImpl
//  End of File  
