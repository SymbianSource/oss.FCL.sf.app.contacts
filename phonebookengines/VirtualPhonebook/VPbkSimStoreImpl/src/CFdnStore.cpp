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
* Description:  An FDN store
*
*/



// INCLUDE FILES
#include "CFdnStore.h"

#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFdnStore::CFdnStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFdnStore::CFdnStore( TStoreParams& aParams )
:   CCustomStore( aParams )
    {
    }

// -----------------------------------------------------------------------------
// CFdnStore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFdnStore::ConstructL()
    {
    CCustomStore::ConstructL();
    }

// -----------------------------------------------------------------------------
// CFdnStore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFdnStore* CFdnStore::NewL( TStoreParams& aParams )
    {
    CFdnStore* self = new( ELeave ) CFdnStore( aParams );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CFdnStore::~CFdnStore()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CFdnStore::~CFdnStore"));
    CancelRequests();
    }

// -----------------------------------------------------------------------------
// CFdnStore::ETelName
// -----------------------------------------------------------------------------
//
const TDesC& CFdnStore::ETelName() const
    {
    return KETelIccFdnPhoneBook;
    }

// ----------------------------------------------------------------------------
// CFdnStore::CompleteSATRefresh
// -----------------------------------------------------------------------------
//
void CFdnStore::CompleteSATRefresh( 
        MSimRefreshCompletion& aSimRefreshCompletion )
    {
    // It's assumed that fixed dialling store init reads only SAT defined EFs
    aSimRefreshCompletion.SatRefreshCompleted( *this, EFalse );
    }

// ----------------------------------------------------------------------------
// CFdnStore::ElementaryFiles
// -----------------------------------------------------------------------------
//
TUint32 CFdnStore::ElementaryFiles()
    {
    return KFDNRefresh | KFDNExtRefresh | KSimReset | KSimInit |
    		KSimInitFullFileChangeNotification | KUsimApplicationReset |
    		K3GSessionReset;
    }
} // namespace VPbkSimStoreImpl 
//  End of File  
