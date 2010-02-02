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
* Description:  An own number store, contains the contacts in the MSISDN file
*                in (U)SIM
*
*/



// INCLUDE FILES
#include "COwnNumberStore.h"

#include "CPhone.h"
#include "CSingleReadCmd.h"
#include "COwnNumberWriteCmd.h"
#include "CDeleteCmd.h"
#include "TVPbkSimStoreProperty.h"
#include "CVPbkSimContact.h"
#include "CVPbkSimCntField.h"
#include "CVPbkSimCommandStore.h"
#include "CSequentialCompositeCmd.h"

// From VPbkSimStoreCommon
#include <RVPbkStreamedIntArray.h>

// From VPbkEng
#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COwnNumberStore::COwnNumberStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
COwnNumberStore::COwnNumberStore( TStoreParams& aParams )
    :   CStoreBase( aParams ),
        iETelInfoPckg( iETelStoreInfo )
    {
    }

// -----------------------------------------------------------------------------
// COwnNumberStore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void COwnNumberStore::ConstructL()
    {
    CStoreBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// COwnNumberStore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
COwnNumberStore* COwnNumberStore::NewL( TStoreParams& aParams )
    {
    COwnNumberStore* self = new( ELeave ) COwnNumberStore( aParams );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
COwnNumberStore::~COwnNumberStore()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: COwnNumberStore::~COwnNumberStore"));
    CancelRequests();
    iETelStore.Close();
    }

// -----------------------------------------------------------------------------
// COwnNumberStore::ETelName
// -----------------------------------------------------------------------------
//
const TDesC& COwnNumberStore::ETelName() const
    {
    return KETelOwnNumberStore;
    }

// ----------------------------------------------------------------------------
// COwnNumberStore::CompleteSATRefresh
// -----------------------------------------------------------------------------
//
void COwnNumberStore::CompleteSATRefresh( 
        MSimRefreshCompletion& aSimRefreshCompletion )
    {
    // It's assumed that own number store init reads only SAT defined EFs
    aSimRefreshCompletion.SatRefreshCompleted( *this, EFalse );
    }
    
// ----------------------------------------------------------------------------
// COwnNumberStore::SaveL
// -----------------------------------------------------------------------------
//
MVPbkSimStoreOperation* COwnNumberStore::SaveL( const TDesC8& aData, 
        TInt& aSimIndex, MVPbkSimContactObserver& aObserver )
    {
    COwnNumberWriteCmd* cmd = 
        COwnNumberWriteCmd::NewL( *this, aData, aSimIndex, aObserver );
    cmd->Execute();
    return cmd;
    }
    
// -----------------------------------------------------------------------------
// COwnNumberStore::DeleteL
// -----------------------------------------------------------------------------
//
MVPbkSimStoreOperation* COwnNumberStore::DeleteL( 
        RVPbkStreamedIntArray& aSimIndexes, 
        MVPbkSimContactObserver& aObserver )
    {
    CDeleteCmd* cmd = CDeleteCmd::NewL( *this, aSimIndexes, aObserver );
    cmd->Execute();
    return cmd;
    }

// -----------------------------------------------------------------------------
// COwnNumberStore::GetGsmStoreProperties
// -----------------------------------------------------------------------------
//
TInt COwnNumberStore::GetGsmStoreProperties( 
    TVPbkGsmStoreProperty& aGsmProperties ) const
    {
    aGsmProperties.iCaps = ConvertETelStoreCaps( iETelStoreInfo.iCaps );
    aGsmProperties.iMaxNumLength = iETelStoreInfo.iNumberLen;
    aGsmProperties.iMaxTextLength = iETelStoreInfo.iTextLen;
    aGsmProperties.iTotalEntries = iETelStoreInfo.iTotalEntries;
    aGsmProperties.iUsedEntries = iETelStoreInfo.iUsedEntries;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// COwnNumberStore::DestroyAllCommands
// -----------------------------------------------------------------------------
//
TInt COwnNumberStore::GetUSimStoreProperties( 
    TVPbkUSimStoreProperty& aUSimProperties ) const
    {
    aUSimProperties.iMaxAnrLength = KVPbkSimStorePropertyUndefined;
    aUSimProperties.iMaxEmailLength = KVPbkSimStorePropertyUndefined;
    aUSimProperties.iMaxGroupNameLength = KVPbkSimStorePropertyUndefined;
    aUSimProperties.iMaxNumOfAnrs = KVPbkSimStorePropertyUndefined;
    aUSimProperties.iMaxNumOfEmails = KVPbkSimStorePropertyUndefined;
    aUSimProperties.iMaxNumOfGroupNames = KVPbkSimStorePropertyUndefined;
    aUSimProperties.iMaxNumOfScndNames = KVPbkSimStorePropertyUndefined;
    aUSimProperties.iMaxScndNameLength = KVPbkSimStorePropertyUndefined;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// COwnNumberStore::ElementaryFiles
// -----------------------------------------------------------------------------
//
TUint32 COwnNumberStore::ElementaryFiles()
    {
    return KONRefresh | KONExtRefresh | KSimReset | KSimInit | 
			KSimInitFullFileChangeNotification | KUsimApplicationReset |
			K3GSessionReset;
    }

// -----------------------------------------------------------------------------
// COwnNumberStore::ETelStoreBase
// -----------------------------------------------------------------------------
//
RMobilePhoneStore& COwnNumberStore::ETelStoreBase()
    {
    return iETelStore;
    }
    
// -----------------------------------------------------------------------------
// COwnNumberStore::ETelStoreInfo
// -----------------------------------------------------------------------------
//
RMobilePhoneStore::TMobilePhoneStoreInfoV1& COwnNumberStore::ETelStoreInfo()
    {
    return iETelStoreInfo;
    }
    
// -----------------------------------------------------------------------------
// COwnNumberStore::ETelStoreInfoPackge
// -----------------------------------------------------------------------------
//
TDes8& COwnNumberStore::ETelStoreInfoPackge()
    {
    return iETelInfoPckg;
    }
    
// -----------------------------------------------------------------------------
// COwnNumberStore::OpenETelStoreL
// -----------------------------------------------------------------------------
//
void COwnNumberStore::OpenETelStoreL()
    {
    User::LeaveIfError( iETelStore.Open( Phone().ETelPhone() ) );
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobileONStore::Open h%d %S"),
        iETelStore.SubSessionHandle(),&ETelName());
    }

// -----------------------------------------------------------------------------
// COwnNumberStore::CloseETelStore
// -----------------------------------------------------------------------------
//
void COwnNumberStore::CloseETelStore()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobileONStore::Close h%d %S"),
        iETelStore.SubSessionHandle(), &ETelName());
    iETelStore.Close();
    }

// -----------------------------------------------------------------------------
// COwnNumberStore::CreateInitCommandL
// -----------------------------------------------------------------------------
//
MVPbkSimCommand* COwnNumberStore::CreateInitCommandL( 
    MVPbkSimCommandObserver& aObserver )
    {
    // Create a init command chain
    CCompositeCmdBase* cmdChain = CSequentialCompositeCmd::NewLC();
    cmdChain->AddObserverL( aObserver );
    
    // Get phonebook information to know the current state of the phonebook
    AddGetInfoCmdToChainL( *cmdChain );

    // Read all contacts after info is up to date
    CSingleONReadCmd* readCmd = 
        new( ELeave ) CSingleONReadCmd( *this );
    CleanupStack::PushL( readCmd );
    cmdChain->AddSubCommandL( readCmd );
   
    CleanupStack::Pop( 2 ); // readCmd, cmdChain
    return cmdChain;
    }

// -----------------------------------------------------------------------------
// COwnNumberStore::CreateUpdateCommandL
// -----------------------------------------------------------------------------
//
MVPbkSimCommand* COwnNumberStore::CreateUpdateCommandL( TInt aSimIndex )
    {
    // Create a update command chain
    CCompositeCmdBase* cmdChain = CSequentialCompositeCmd::NewLC();

    // Get phonebook information to know the current state of the phonebook
    AddGetInfoCmdToChainL( *cmdChain );
    
    // Read the changed contact
    CSingleONReadCmd* readCmd = 
        new( ELeave ) CSingleONReadCmd( *this, aSimIndex );
    CleanupStack::PushL( readCmd );
    cmdChain->AddSubCommandL( readCmd );
   
    CleanupStack::Pop( 2 ); // readCmd, cmdChain
    return cmdChain;
    }

// -----------------------------------------------------------------------------
// COwnNumberStore::CreateUpdateCommandUsingRefL
// -----------------------------------------------------------------------------
//
MVPbkSimCommand* COwnNumberStore::CreateUpdateCommandUsingRefL( 
        TInt& aSimIndexRef )
    {
    // Create a update command chain
    CCompositeCmdBase* cmdChain = CSequentialCompositeCmd::NewLC();

    // Get phonebook information to know the current state of the phonebook
    AddGetInfoCmdToChainL( *cmdChain );
    
    // Read the changed contact
    CSingleONReadCmd* readCmd = 
        new( ELeave ) CSingleONReadCmd( aSimIndexRef, *this );
    CleanupStack::PushL( readCmd );
    cmdChain->AddSubCommandL( readCmd );
   
    CleanupStack::Pop( 2 ); // readCmd, cmdChain
    return cmdChain;
    }
} // namespace VPbkSimStoreImpl 
//  End of File  
