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
* Description:  A store that doesn't have a cache in the TSY side.
*                Uses ETel RMobilePhoneBookStore 
*
*/



// INCLUDE FILES
#include "CBasicStore.h"

#include "VPbkSimStoreImplError.h"
#include "CPhone.h"
#include "CMultipleReadCmd.h"
#include "CWriteCmd.h"
#include "CDeleteCmd.h"
#include "TVPbkSimStoreProperty.h"
#include "CVPbkSimCommandStore.h"
#include "CSequentialCompositeCmd.h"

// From VPbkSimStoreCommon
#include <RVPbkStreamedIntArray.h>

// System includes
#include <etelmm.h>

// From VPbkEng
#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBasicStore::CBasicStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBasicStore::CBasicStore( TStoreParams& aParams )
:   CStoreBase( aParams ),
    iETelInfoPckg( iETelStoreInfo )
    {
    }

// -----------------------------------------------------------------------------
// CBasicStore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBasicStore::ConstructL()
    {
    CStoreBase::ConstructL();
    }

// Destructor
CBasicStore::~CBasicStore()
    {
    CancelRequests();
    iETelStore.Close();
    }

// -----------------------------------------------------------------------------
// CBasicStore::SaveL
// -----------------------------------------------------------------------------
//
MVPbkSimStoreOperation* CBasicStore::SaveL( const TDesC8& aData, 
        TInt& aSimIndex, MVPbkSimContactObserver& aObserver )
    {
    CWriteCmd* cmd = CWriteCmd::NewL( *this, aData, aSimIndex, aObserver );
    cmd->Execute();
    return cmd;
    }

// -----------------------------------------------------------------------------
// CBasicStore::DeleteL
// -----------------------------------------------------------------------------
//
MVPbkSimStoreOperation* CBasicStore::DeleteL( 
        RVPbkStreamedIntArray& aSimIndexes, 
        MVPbkSimContactObserver& aObserver )
    {
    CDeleteCmd* cmd = CDeleteCmd::NewL( *this, aSimIndexes, aObserver );
    cmd->Execute();
    return cmd;
    }
    
// -----------------------------------------------------------------------------
// CBasicStore::GetGsmStoreProperties
// -----------------------------------------------------------------------------
//
TInt CBasicStore::GetGsmStoreProperties( 
    TVPbkGsmStoreProperty& aGsmProperties ) const
    {
    aGsmProperties.iCaps = ConvertETelStoreCaps( iETelStoreInfo.iCaps );
    aGsmProperties.iMaxNumLength = iETelStoreInfo.iMaxNumLength;
    aGsmProperties.iMaxTextLength = iETelStoreInfo.iMaxTextLength;
    aGsmProperties.iTotalEntries = iETelStoreInfo.iTotalEntries;
    aGsmProperties.iUsedEntries = iETelStoreInfo.iUsedEntries;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBasicStore::DestroyAllCommands
// -----------------------------------------------------------------------------
//
TInt CBasicStore::GetUSimStoreProperties( 
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

// -----------------------------------------------------------------------------
// CBasicStore::ETelStoreBase
// -----------------------------------------------------------------------------
//
RMobilePhoneStore& CBasicStore::ETelStoreBase()
    {
    return iETelStore;
    }
    
// -----------------------------------------------------------------------------
// CBasicStore::ETelStoreInfo
// -----------------------------------------------------------------------------
//
RMobilePhoneStore::TMobilePhoneStoreInfoV1& CBasicStore::ETelStoreInfo()
    {
    return iETelStoreInfo;
    }
    
// -----------------------------------------------------------------------------
// CBasicStore::ETelStoreInfoPackge
// -----------------------------------------------------------------------------
//
TDes8& CBasicStore::ETelStoreInfoPackge()
    {
    return iETelInfoPckg;
    }
    
// -----------------------------------------------------------------------------
// CBasicStore::OpenETelStoreL
// -----------------------------------------------------------------------------
//
void CBasicStore::OpenETelStoreL()
    {
    // Opens the RMobilePhoneBookStore subsubsession.
    // params: phone, store name and store type.
    User::LeaveIfError( iETelStore.Open( Phone().ETelPhone(), 
        ETelName() /*, ETelStoreType( Identifier() )*/ ) );
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneBookStore::Open h%d %S"),
        iETelStore.SubSessionHandle(),&ETelName());
    }

// -----------------------------------------------------------------------------
// CBasicStore::OpenETelStoreL
// -----------------------------------------------------------------------------
//
void CBasicStore::CloseETelStore()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneBookStore::Close h%d %S"),
        iETelStore.SubSessionHandle(), &ETelName());
    iETelStore.Close();
    }

// -----------------------------------------------------------------------------
// CBasicStore::CreateInitCommandL
// -----------------------------------------------------------------------------
//
MVPbkSimCommand* CBasicStore::CreateInitCommandL( 
        MVPbkSimCommandObserver& aObserver )
    {
    // Create a init command chain
    CCompositeCmdBase* cmdChain = CSequentialCompositeCmd::NewLC();
    cmdChain->AddObserverL( aObserver );
    
    // Get phonebook information to know the current state of the phonebook
    AddGetInfoCmdToChainL( *cmdChain );

    // Start reading contacts after the total amount of contacts is known
    AddMultipleReadCmdToChainL( *cmdChain, KErrNotFound, KErrNotFound );

    CleanupStack::Pop( cmdChain );
    return cmdChain;
    }

// -----------------------------------------------------------------------------
// CBasicStore::CreateUpdateCommandL
// -----------------------------------------------------------------------------
//
MVPbkSimCommand* CBasicStore::CreateUpdateCommandL( TInt aSimIndex )
    {
    // Create a update command chain
    CCompositeCmdBase* cmdChain = CSequentialCompositeCmd::NewLC();
    
    // Read the changed contact
    AddMultipleReadCmdToChainL( *cmdChain, aSimIndex, aSimIndex );
    
    // Get phonebook information to know the current state of the phonebook
    AddGetInfoCmdToChainL( *cmdChain );
    
    CleanupStack::Pop( cmdChain );
    return cmdChain;
    }

// -----------------------------------------------------------------------------
// CBasicStore::CreateUpdateCommandUsingRefL
// -----------------------------------------------------------------------------
//
MVPbkSimCommand* CBasicStore::CreateUpdateCommandUsingRefL( TInt& aSimIndexRef )
    {
    // Create a update command chain
    CCompositeCmdBase* cmdChain = CSequentialCompositeCmd::NewLC();
    
    // Read the changed contact
    cmdChain->AddSubCommandL( 
        CMultipleReadCmd::NewLC( *this, aSimIndexRef ) );
    CleanupStack::Pop(); // CMultipleReadCmd
    
    // Get phonebook information to know the current state of the phonebook
    AddGetInfoCmdToChainL( *cmdChain );
    
    CleanupStack::Pop( cmdChain );
    return cmdChain;
    }
    
// -----------------------------------------------------------------------------
// CBasicStore::AddMultipleReadCmdToChainL
// -----------------------------------------------------------------------------
//
void CBasicStore::AddMultipleReadCmdToChainL( CCompositeCmdBase& aCmdChain,
    TInt aFromSimIndex, TInt aToSimIndex )
    {
    CMultipleReadCmd* readCmd = NULL;
    if ( aFromSimIndex != KErrNotFound && aToSimIndex != KErrNotFound  )
        {
        readCmd = CMultipleReadCmd::NewLC( *this, aFromSimIndex, 
            aToSimIndex );
        }
    else
        {
        readCmd = CMultipleReadCmd::NewLC( *this );
        }
        
    aCmdChain.AddSubCommandL( readCmd );
    CleanupStack::Pop( readCmd );
    }
} // namespace VPbkSimStoreImpl
//  End of File  
