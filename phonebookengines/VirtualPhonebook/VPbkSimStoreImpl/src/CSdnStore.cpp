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
* Description:  An ADN store
*
*/



// INCLUDE FILES
#include "CSdnStore.h"

#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSdnStore::CSdnStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSdnStore::CSdnStore( TStoreParams& aParams )
:   CBasicStore( aParams )
    {
    }

// -----------------------------------------------------------------------------
// CSdnStore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdnStore::ConstructL()
    {
    CBasicStore::ConstructL();
    }

// -----------------------------------------------------------------------------
// CSdnStore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSdnStore* CSdnStore::NewL( TStoreParams& aParams )
    {
    CSdnStore* self = new( ELeave ) CSdnStore( aParams );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CSdnStore::~CSdnStore()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CSdnStore::~CSdnStore"));
    CancelRequests();
    }

// -----------------------------------------------------------------------------
// CSdnStore::ETelName
// -----------------------------------------------------------------------------
//
const TDesC& CSdnStore::ETelName() const
    {
    return KETelIccSdnPhoneBook;
    }

// ----------------------------------------------------------------------------
// CSdnStore::CompleteSATRefresh
// -----------------------------------------------------------------------------
//
void CSdnStore::CompleteSATRefresh( 
        MSimRefreshCompletion& aSimRefreshCompletion )
    {
    // It's assumed that service dialling store init reads only SAT defined EFs
    aSimRefreshCompletion.SatRefreshCompleted( *this, EFalse );
    }
    
// ----------------------------------------------------------------------------
// CSdnStore::ElementaryFiles
// -----------------------------------------------------------------------------
//
TUint32 CSdnStore::ElementaryFiles()
    {
    return KSDNRefresh | KSDNExtRefresh | KSimReset | KSimInit | 
			KSimInitFullFileChangeNotification | KUsimApplicationReset |
			K3GSessionReset;
    }
} // namespace VPbkSimStoreImpl 
//  End of File  
