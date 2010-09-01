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
* Description:  An USIM ADN store that supports ADN specific store information
*
*/



// INCLUDE FILES
#include "CUsimAdnStore.h"
#include "CPhone.h"
#include "CGet3GPBInfoCmd.h"
#include "CMultipleReadCmd.h"
#include "CSequentialCompositeCmd.h"
#include "TVPbkSimStoreProperty.h"

#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUsimAdnStore::CUsimAdnStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUsimAdnStore::CUsimAdnStore( TStoreParams& aParams )
:   CCustomStore( aParams )
    {
    }

// -----------------------------------------------------------------------------
// CUsimAdnStore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUsimAdnStore::ConstructL()
    {
    CCustomStore::ConstructL();
    Phone().AddObserverL( *this );
    }

// -----------------------------------------------------------------------------
// CUsimAdnStore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUsimAdnStore* CUsimAdnStore::NewL( TStoreParams& aParams )
    {
    CUsimAdnStore* self = new( ELeave ) CUsimAdnStore( aParams );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CUsimAdnStore::~CUsimAdnStore()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CUsimAdnStore::~CUsimAdnStore"));
    CancelRequests();
    Phone().RemoveObserver(*this);
    }

// -----------------------------------------------------------------------------
// CUsimAdnStore::ETelName
// -----------------------------------------------------------------------------
//
const TDesC& CUsimAdnStore::ETelName() const
    {
    return KETelIccAdnPhoneBook;
    }

// -----------------------------------------------------------------------------
// CUsimAdnStore::CompleteSATRefresh
// -----------------------------------------------------------------------------
//
void CUsimAdnStore::CompleteSATRefresh( 
        MSimRefreshCompletion& aSimRefreshCompletion )
    {
    // It's assumed that USIM phonebook init read other EFs too so complete
    // with ETrue
    aSimRefreshCompletion.SatRefreshCompleted( *this, ETrue );
    }
    
// -----------------------------------------------------------------------------
// CUsimAdnStore::GetUSimStoreProperties
// -----------------------------------------------------------------------------
//
TInt CUsimAdnStore::GetUSimStoreProperties( 
    TVPbkUSimStoreProperty& aUSimProperties ) const
    {    
    aUSimProperties.iMaxAnrLength = i3GPBInfo.iMaxLenAnr;
    aUSimProperties.iMaxEmailLength = i3GPBInfo.iMaxLenEmail;
    aUSimProperties.iMaxGroupNameLength = i3GPBInfo.iMaxLenGroupName;
    aUSimProperties.iMaxNumOfAnrs = i3GPBInfo.iMaxNumAnr;
    aUSimProperties.iMaxNumOfEmails = i3GPBInfo.iMaxNumEmail;
    aUSimProperties.iMaxNumOfGroupNames = i3GPBInfo.iMaxNumGroupName;
    aUSimProperties.iMaxNumOfScndNames = i3GPBInfo.iMaxNumSne;
    aUSimProperties.iMaxScndNameLength = i3GPBInfo.iMaxLenSne;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUsimAdnStore::CreateInitCommandL
// -----------------------------------------------------------------------------
//
MVPbkSimCommand* CUsimAdnStore::CreateInitCommandL( 
    MVPbkSimCommandObserver& aObserver )
    {
    // Create a init command chain
    CCompositeCmdBase* cmdChain = CSequentialCompositeCmd::NewLC();
    cmdChain->AddObserverL( aObserver );
    
    // Wait TSY cache first.
    AddTSYCacheReadyCmdToChainL( *cmdChain );

    // Get phonebook information to know the current state of the phonebook
    AddGetInfoCmdToChainL( *cmdChain );

    // Get 3G ADN phonebook information
    Add3GPBInfoCmdToChainL( *cmdChain );

    // Start reading contacts after the total amount of contacts is known
    AddMultipleReadCmdToChainL( *cmdChain, KErrNotFound, KErrNotFound );

    CleanupStack::Pop( cmdChain );
    return cmdChain;
    }

// -----------------------------------------------------------------------------
// CUsimAdnStore::CreateInitCommandL
// -----------------------------------------------------------------------------
//
MVPbkSimCommand* CUsimAdnStore::CreateUpdateCommandL( TInt aSimIndex )
    {
    // Create a update command chain
    CCompositeCmdBase* cmdChain = CSequentialCompositeCmd::NewLC();

    // Get phonebook information
    AddGetInfoCmdToChainL( *cmdChain );
    
    // Get 3G ADN phonebook information
    Add3GPBInfoCmdToChainL( *cmdChain );

    // Read the changed contact
    AddMultipleReadCmdToChainL( *cmdChain, aSimIndex, aSimIndex );
    
    CleanupStack::Pop( cmdChain );
    return cmdChain;
    }

// -----------------------------------------------------------------------------
// CUsimAdnStore::::CreateUpdateCommandUsingRefL
// -----------------------------------------------------------------------------
//
MVPbkSimCommand* CUsimAdnStore::CreateUpdateCommandUsingRefL( 
    TInt& aSimIndexRef )
    {
    // Create a update command chain
    CCompositeCmdBase* cmdChain = CSequentialCompositeCmd::NewLC();

    // Read the changed contact
    cmdChain->AddSubCommandL( 
        CMultipleReadCmd::NewLC( *this, aSimIndexRef ) );
    CleanupStack::Pop(); // CMultipleReadCmd
    
    // Get phonebook information to know the current state of the phonebook
    AddGetInfoCmdToChainL( *cmdChain );
    
    // Get 3G ADN phonebook information
    Add3GPBInfoCmdToChainL( *cmdChain );
    
    CleanupStack::Pop( cmdChain );
    return cmdChain;
    }

// ----------------------------------------------------------------------------
// CUsimAdnStore::ElementaryFiles
// -----------------------------------------------------------------------------
//
TUint32 CUsimAdnStore::ElementaryFiles()
    {
    return KADNRefresh | KADNExtRefresh | KSimReset | KSimInit | 
            KSimInitFullFileChangeNotification | KUsimApplicationReset |
            K3GSessionReset;
    }
    
// ----------------------------------------------------------------------------
// CUsimAdnStore::PhoneOpened
// -----------------------------------------------------------------------------
//    
void CUsimAdnStore::PhoneOpened( MVPbkSimPhone& /*aPhone*/ )
    {
    // Phone is already opened before store
    }

// ----------------------------------------------------------------------------
// CAdnStore::PhoneError
// -----------------------------------------------------------------------------
//
void CUsimAdnStore::PhoneError( MVPbkSimPhone& /*aPhone*/, 
        TErrorIdentifier /*aIdentifier*/, TInt /*aError*/ )
    {
    // Phone is already opened before store
    }

// ----------------------------------------------------------------------------
// CUsimAdnStore::ServiceTableUpdated
// -----------------------------------------------------------------------------
//      
void CUsimAdnStore::ServiceTableUpdated( TUint32 /*aServiceTable*/ )
    {
    // Not interested in service table
    }
    
// ----------------------------------------------------------------------------
// CUsimAdnStore::FixedDiallingStatusChanged
// -----------------------------------------------------------------------------
//    
void CUsimAdnStore::FixedDiallingStatusChanged( TInt aFDNStatus )
    {
    VPBK_DEBUG_PRINT( VPBK_DEBUG_STRING( "CUsimAdnStore::FixedDiallingStatusChanged" ) );

    TStoreState state = CurrentState();

    if ( aFDNStatus != MVPbkSimPhone::EFdnIsActive && 
         aFDNStatus != MVPbkSimPhone::EFdnIsPermanentlyActive &&
         ( state == EStoreNotOpen || state == EStoreNotAvailable ) )
        {
        if ( state == EStoreNotAvailable )
            {
            VPBK_DEBUG_PRINT( VPBK_DEBUG_STRING( "CUsimAdnStore::FixedDiallingStatusChanged - reset store") );
            ResetStore();
            }
        
        VPBK_DEBUG_PRINT( VPBK_DEBUG_STRING( "CUsimAdnStore::FixedDiallingStatusChanged - construct store") );
        TRAPD( res, ConstructStoreL() );

        if ( res != KErrNone )
            {
            SendStoreErrorEvent( res );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUsimAdnStore::Add3GPBInfoCmdToChainL
// -----------------------------------------------------------------------------
//
void CUsimAdnStore::Add3GPBInfoCmdToChainL( CCompositeCmdBase& aCmdChain )
    {
    CGet3GPBInfoCmd* cmd = CGet3GPBInfoCmd::NewLC( *this );
    aCmdChain.AddSubCommandL( cmd );
    CleanupStack::Pop( cmd );
    }
} // namespace VPbkSimStoreImpl 
//  End of File  
