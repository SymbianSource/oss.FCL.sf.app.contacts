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
* Description:  A command that queries the current TSY cache status.
*
*/



// INCLUDE FILES
#include "CCacheStatusCmd.h"

#include <VPbkDebug.h>
#include "CCustomStore.h"
#include "VPbkSimStoreImplError.h"

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCacheStatusCmd::CCacheStatusCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCacheStatusCmd::CCacheStatusCmd( CCustomStore& aStore, 
        RMmCustomAPI::TPndCacheStatus& aCacheStatus )
        :   CActive( EPriorityStandard ),
            iStore( aStore ),
            iCacheStatus( aCacheStatus )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCacheStatusCmd::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCacheStatusCmd* CCacheStatusCmd::NewLC( CCustomStore& aStore, 
        RMmCustomAPI::TPndCacheStatus& aCacheStatus )
    {
    CCacheStatusCmd* self = 
        new( ELeave ) CCacheStatusCmd( aStore, aCacheStatus );
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CCacheStatusCmd::~CCacheStatusCmd()
    {
    CancelCmd();
    }

// -----------------------------------------------------------------------------
// CCacheStatusCmd::Execute
// -----------------------------------------------------------------------------
//
void CCacheStatusCmd::Execute()
    {
    __ASSERT_DEBUG( iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCCacheStatusCmdExecute ) );
        
    iStoreName.Copy(iStore.ETelName());
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMmCustomAPI::GetPndCacheStatus h%d request %S"),
        iStore.CustomAPI().SubSessionHandle(),&iStore.ETelName());
    iStore.CustomAPI().GetPndCacheStatus( iStatus, iCacheStatus, iStoreName);
    SetActive();
    }

// -----------------------------------------------------------------------------
// CCacheStatusCmd::AddObserverL
// -----------------------------------------------------------------------------
//
void CCacheStatusCmd::AddObserverL( MVPbkSimCommandObserver& aObserver )
    {
    __ASSERT_DEBUG( !iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCCacheStatusCmdAddObserverL ) );
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CCacheStatusCmd::CancelCmd
// -----------------------------------------------------------------------------
//
void CCacheStatusCmd::CancelCmd()
    {
    CActive::Cancel();
    }
        
// -----------------------------------------------------------------------------
// CCacheStatusCmd::RunL
// -----------------------------------------------------------------------------
//
void CCacheStatusCmd::RunL()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMmCustomAPI::GetPndCacheStatus h%d complete %d,\
        store %S, iCacheStatus %d"),
        iStore.CustomAPI().SubSessionHandle(), iStatus.Int(),
        &iStoreName, iCacheStatus);
    
    if ( iStatus.Int() == KErrNone )
        {
        switch ( iCacheStatus )
            {
            case RMmCustomAPI::ECacheNotReady:
                {
                // It's CCacheReadyNotificationCmd's responsibility
                // to handle cache waiting. Don't complete command because
                // it would complete the composite command. 
                break;
                }
            case RMmCustomAPI::ECacheReady:
            case RMmCustomAPI::ECacheNotUsed:
                {
                // ok, just complete the command and proceed.
                iObserver->CommandDone( *this );
                break;
                }
            case RMmCustomAPI::ECacheFailed:
                {
                // Cache failed but the reason is not known.
                iObserver->CommandError( *this, KErrUnknown );
                }
            }
        }
    else
        {
        iObserver->CommandError( *this, iStatus.Int() );
        }
    }

// -----------------------------------------------------------------------------
// CCacheStatusCmd::RunL
// -----------------------------------------------------------------------------
//    
void CCacheStatusCmd::DoCancel()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMmCustomAPI::GetPndCacheStatus h%d Cancel"),
        iStore.CustomAPI().SubSessionHandle());
    iStore.CustomAPI().CancelAsyncRequest( ECustomGetPndCacheStatusIPC );
    }

} // namespace VPbkSimStoreImpl
//  End of File  
