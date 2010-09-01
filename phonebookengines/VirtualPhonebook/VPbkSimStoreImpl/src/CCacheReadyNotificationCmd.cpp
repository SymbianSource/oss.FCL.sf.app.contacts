/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A command that completes when TSY cache is ready
*
*/



// INCLUDE FILES
#include "CCacheReadyNotificationCmd.h"

#include "CCustomStore.h"
#include "CCacheStatusCmd.h"
#include "VPbkSimStoreImplError.h"

// From VPbkEng
#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCacheReadyNotificationCmd::CCacheReadyNotificationCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCacheReadyNotificationCmd::CCacheReadyNotificationCmd( CCustomStore& aStore,
        RMmCustomAPI::TPndCacheStatus& aCacheStatus,
        CCacheStatusCmd& aCacheStatusCmd )
        :   CActive( EPriorityStandard),
            iStore( aStore ),
            iCacheStatus( aCacheStatus ),
            iCacheStatusCmd( aCacheStatusCmd )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCacheReadyNotificationCmd::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCacheReadyNotificationCmd* CCacheReadyNotificationCmd::NewLC( 
        CCustomStore& aStore,
        RMmCustomAPI::TPndCacheStatus& aCacheStatus,
        CCacheStatusCmd& aCacheStatusCmd )
    {
    CCacheReadyNotificationCmd* self = 
        new( ELeave ) CCacheReadyNotificationCmd( aStore, aCacheStatus, 
            aCacheStatusCmd );
    CleanupStack::PushL( self );
    return self;
    }

    
// Destructor
CCacheReadyNotificationCmd::~CCacheReadyNotificationCmd()
    {
    CancelCmd();
    }

// -----------------------------------------------------------------------------
// CCacheReadyNotificationCmd::Execute
// -----------------------------------------------------------------------------
//
void CCacheReadyNotificationCmd::Execute()
    {
    __ASSERT_DEBUG( iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCCacheReadyNotificationCmdExecute ) );
    
    iStore.CustomAPI().NotifyPndCacheReady( iStatus, iETelStoreName );

    SetActive();
    }

// -----------------------------------------------------------------------------
// CCacheReadyNotificationCmd::AddObserverL
// -----------------------------------------------------------------------------
//
void CCacheReadyNotificationCmd::AddObserverL( 
        MVPbkSimCommandObserver& aObserver )
    {
    __ASSERT_DEBUG( !iObserver, VPbkSimStoreImpl::Panic( 
        EPreCondCCacheReadyNotificationCmdAddObserverL ) );
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CCacheReadyNotificationCmd::CancelCmd
// -----------------------------------------------------------------------------
//
void CCacheReadyNotificationCmd::CancelCmd()
    {
    CActive::Cancel();
    }

// -----------------------------------------------------------------------------
// CCacheReadyNotificationCmd::RunL
// -----------------------------------------------------------------------------
//
void CCacheReadyNotificationCmd::RunL()
    {
    TInt result = iStatus.Int();

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMmCustomAPI::NotifyPndCacheReady h%d complete %d,\
        for store=%S"),iStore.CustomAPI().SubSessionHandle(),
        result,&iETelStoreName);
        
    if ( iETelStoreName.Compare( iStore.ETelName() ) == 0 )
        {
        // Notification about target store, inform observer about results
        if ( result == KErrNone )
            {
            iCacheStatus = RMmCustomAPI::ECacheReady;
            iObserver->CommandDone( *this );
            }
        else
            {
            iObserver->CommandError( *this, result );
            }
        }
    else
        {
        // Notification for different store, continue listening
        Execute();
        // We must also get the current status because it can be that
        // TSY is just getting the cache ready and our new
        // NotifyPndCacheReady never completes.
        // The order of requests is important, first notification, then
        // current state because if notication jams then we get "cache ready"
        // from state request and can proceed.
        iCacheStatusCmd.CancelCmd();
        iCacheStatusCmd.Execute();
        }
    }

// -----------------------------------------------------------------------------
// CCacheReadyNotificationCmd::DoCancel
// -----------------------------------------------------------------------------
//
void CCacheReadyNotificationCmd::DoCancel()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMmCustomAPI::NotifyPndCacheReady h%d Cancel"),
        iStore.CustomAPI().SubSessionHandle());
    iStore.CustomAPI().CancelAsyncRequest( ECustomNotifyPndCacheReadyIPC );
    }
} // namespace VPbkSimStoreImpl

//  End of File  
