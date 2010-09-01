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
#include "CAdnStore.h"

#include "CPhone.h"
#include <CVPbkSimStateInformation.h>
#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAdnStore::CAdnStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAdnStore::CAdnStore( TStoreParams& aParams )
:   CCustomStore( aParams )
    {
    }

// -----------------------------------------------------------------------------
// CAdnStore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAdnStore::ConstructL()
    {
    CCustomStore::ConstructL();
    Phone().AddObserverL( *this );
    }

// -----------------------------------------------------------------------------
// CAdnStore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAdnStore* CAdnStore::NewL( TStoreParams& aParams )
    {
    CAdnStore* self = new( ELeave ) CAdnStore( aParams );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CAdnStore::~CAdnStore()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CAdnStore::~CAdnStore"));
    CancelRequests();
    Phone().RemoveObserver(*this);
    }

// -----------------------------------------------------------------------------
// CAdnStore::ETelName
// -----------------------------------------------------------------------------
//
const TDesC& CAdnStore::ETelName() const
    {
    return KETelIccAdnPhoneBook;
    }

// ----------------------------------------------------------------------------
// CAdnStore::CompleteSATRefresh
// -----------------------------------------------------------------------------
//
void CAdnStore::CompleteSATRefresh( 
        MSimRefreshCompletion& aSimRefreshCompletion )
    {
    // It's assumed that GSM ADN store init reads only SAT defined EFs
    aSimRefreshCompletion.SatRefreshCompleted( *this, EFalse );
    }
    
// ----------------------------------------------------------------------------
// CAdnStore::AllowOpeningL
// -----------------------------------------------------------------------------
//
TBool CAdnStore::AllowOpeningL()
    {
    if ( SimStateInformation().ActiveFdnBlocksAdnStoreL( Phone() ) || 
    		!CStoreBase::AllowOpeningL() )
        {
        return EFalse;
        }
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CAdnStore::ElementaryFiles
// -----------------------------------------------------------------------------
//
TUint32 CAdnStore::ElementaryFiles()
    {
    return KADNRefresh | KADNExtRefresh | KSimReset | KSimInit |
			KSimInitFullFileChangeNotification | KUsimApplicationReset |
			K3GSessionReset;
    }

// ----------------------------------------------------------------------------
// CAdnStore::PhoneOpened
// -----------------------------------------------------------------------------
//    
void CAdnStore::PhoneOpened( MVPbkSimPhone& /*aPhone*/ )
    {
    // Phone is already opened before store
    }

// ----------------------------------------------------------------------------
// CAdnStore::PhoneError
// -----------------------------------------------------------------------------
//
void CAdnStore::PhoneError( MVPbkSimPhone& /*aPhone*/, 
        TErrorIdentifier /*aIdentifier*/, TInt /*aError*/ )
    {
    // Phone is already opened before store
    }

// ----------------------------------------------------------------------------
// CAdnStore::ServiceTableUpdated
// -----------------------------------------------------------------------------
//      
void CAdnStore::ServiceTableUpdated( TUint32 /*aServiceTable*/ )
    {
    // Not interested in service table
    }
    
// ----------------------------------------------------------------------------
// CAdnStore::FixedDiallingStatusChanged
// -----------------------------------------------------------------------------
//    
void CAdnStore::FixedDiallingStatusChanged( TInt /*aFDNStatus*/ )
    {
    FUNC_ENTRY();
    TRAPD( res, HandleFDNStatusChangeL() );
    if ( res != KErrNone )
        {
        SendStoreErrorEvent( res );
        }
    FUNC_EXIT();
    }

// ----------------------------------------------------------------------------
// CAdnStore::HandleFDNStatusChangeL
// -----------------------------------------------------------------------------
//    
void CAdnStore::HandleFDNStatusChangeL()
    {
    TBool adnBlocked = SimStateInformation().ActiveFdnBlocksAdnStoreL( Phone() );
    
    // If store hasn't been open (because FDN blocking) the store must
    // be constructed.
    if ( CurrentState() == EStoreNotOpen && !adnBlocked )
        {
        ConstructStoreL();
        }
    // If store is not available (it has been constructed before)
    // send store ready event.
    else if ( CurrentState() == EStoreNotAvailable && !adnBlocked )
        {
        SendStoreReadyEvent();
        }
    // If store is open and FDN is activated and it blocks ADN
    // then set store to not available state
    else if ( CurrentState() == EStoreOpen && adnBlocked )
        {
        SendStoreUnavailableEvent();
        }
    }
} // namespace VPbkSimStoreImpl 
//  End of File  
