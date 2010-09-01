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
* Description:  A store that uses ETel custom API due to cache in TSY
*
*/



// INCLUDE FILES
#include "CCustomStore.h"

#include <VPbkDebug.h>
#include "CPhone.h"
#include "CCacheReadyNotificationCmd.h"
#include "CCacheStatusCmd.h"
#include "CSequentialCompositeCmd.h"
#include "CParallelCompositeCmd.h"

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCustomStore::CCustomStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCustomStore::CCustomStore( TStoreParams& aParams )
:   CBasicStore( aParams )
    {
    }

// -----------------------------------------------------------------------------
// CCustomStore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCustomStore::ConstructL()
    {
    CBasicStore::ConstructL();
    }
    
// Destructor
CCustomStore::~CCustomStore()
    {
    CancelRequests();
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMmCustomAPI::Close"));
    iCustomAPI.Close();
    }

// -----------------------------------------------------------------------------
// CCustomStore::OpenETelStoreL
// -----------------------------------------------------------------------------
//
void CCustomStore::OpenETelStoreL()
    {
    CBasicStore::OpenETelStoreL();
    User::LeaveIfError( iCustomAPI.Open( Phone().ETelPhone() ) );
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMmCustomAPI::Open handle %d"),
        iCustomAPI.SubSessionHandle());
    }

void CCustomStore::CloseETelStore()
    {
    CBasicStore::CloseETelStore();
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMmCustomAPI::Close handle %d"),
        iCustomAPI.SubSessionHandle());
    iCustomAPI.Close();
    }

// -----------------------------------------------------------------------------
// CCustomStore::CreateInitCommandL
// -----------------------------------------------------------------------------
//
MVPbkSimCommand* CCustomStore::CreateInitCommandL( 
        MVPbkSimCommandObserver& aObserver )
    {
    // Create a init command chain
    CCompositeCmdBase* cmdChain = CSequentialCompositeCmd::NewLC();
    cmdChain->AddObserverL( aObserver );
    
    // Add notication command that waits until the TSY cache is ready if
    // the cache is not ready
    AddTSYCacheReadyCmdToChainL( *cmdChain );

    // Get phonebook information to know the current state of the phonebook
    AddGetInfoCmdToChainL( *cmdChain );

    // Start reading contacts after the total amount of contacts is known
    AddMultipleReadCmdToChainL( *cmdChain, KErrNotFound, KErrNotFound );

    CleanupStack::Pop( cmdChain );
    return cmdChain;
    }

// -----------------------------------------------------------------------------
// CCustomStore::AddTSYCacheReadyCmdToChainL
// -----------------------------------------------------------------------------
//
void CCustomStore::AddTSYCacheReadyCmdToChainL( CCompositeCmdBase& aCmdChain )
    {
    // Create a parallel composite to ensure that we don't jam
    // to TSY cache waiting. We must use both cache status command and
    // cache ready notification command and they must also co-operate.
    // The composite completes when the first subcommand tells it that
    // cache is ready.
    CCompositeCmdBase* composite = CParallelCompositeCmd::NewLC(
            CParallelCompositeCmd::EFirstSubCommandDone );
    
    CCacheStatusCmd* statusCmd = CCacheStatusCmd::NewLC( *this, iCacheStatus );
    // Notification command needs to re-execute CCacheStatusCmd in some
    // situation.
    CCacheReadyNotificationCmd* notifyCmd = 
        CCacheReadyNotificationCmd::NewLC( *this, iCacheStatus, *statusCmd );
    // The order of subcommand has a meaning. TSY must get notification
    // request before cache status request. Otherwise there is a possiblity
    // that we miss the notification and the request never completes.
    composite->AddSubCommandL( notifyCmd );
    CleanupStack::Pop( notifyCmd );
    composite->AddSubCommandL( statusCmd );
    CleanupStack::Pop( statusCmd );
    
    aCmdChain.AddSubCommandL( composite );
    CleanupStack::Pop( composite );
    }
} // namespace VPbkSimStoreImpl
//  End of File  
