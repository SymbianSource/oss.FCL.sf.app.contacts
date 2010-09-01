/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for handling connections over application server.
*
*/


#include <s32mem.h>
#include <apgtask.h>  // TApaTask
#include <apgcli.h>   // RApaLsSession

#include "ccaclientheaders.h"

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_DoLaunch = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "RCCAClientUIService");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} /// namespace


// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// RCCAClientUIService::RCCAClientUIService
// --------------------------------------------------------------------------
//
RCCAClientUIService::RCCAClientUIService()        
    {
    }


// --------------------------------------------------------------------------
// RCCAClientUIService::DoLaunch
// --------------------------------------------------------------------------
//
void RCCAClientUIService::DoLaunch
        ( TRequestStatus& aStatus,  TPtr8& aMessagePtr )
    {
    __ASSERT_DEBUG(aStatus == KRequestPending && &aMessagePtr, Panic(EPanicPreCond_DoLaunch));   
    
    CCA_DP(KCCAClientLoggerFile, CCA_L( "RCCAClientUIService::DoLaunch()"));    
    SendReceive( ECCALaunchApplication, TIpcArgs( &aMessagePtr ), aStatus);       
    CCA_DP(KCCAClientLoggerFile, CCA_L( "RCCAClientUIService::DoLaunch(): Done."));    
    
    return; 
    }


// --------------------------------------------------------------------------
// RCCAClientUIService::DoCloseApplication
// --------------------------------------------------------------------------
//
void RCCAClientUIService::DoCloseApplication()
    {
    if ( Handle() )
        {
        RWsSession& wsSession = CCoeEnv::Static()->WsSession();
        TApaTask task( wsSession );
        TInt windowGroupId = GetAppWindowGroupId();
        task.SetWgId( windowGroupId );

        task.EndTask();
        }
    }

// --------------------------------------------------------------------------
// RCCAClientUIService::ConnectL
// --------------------------------------------------------------------------
//
void RCCAClientUIService::DoConnectL()
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "RCCAClientUIService::DoConnectL()"));    

    //Check if connection is already done. 
    if ( KNullHandle == Handle() )
        {      
        ConnectChainedAppL( TUid::Uid( KCCAAppUID ) );    
        }

    CCA_DP(KCCAClientLoggerFile, CCA_L( "RCCAClientUIService::DoConnectL(): Done."));    
    
    return; 
    }
    


// --------------------------------------------------------------------------
// RCCAClientUIService::ServiceUid
// --------------------------------------------------------------------------
//
TUid RCCAClientUIService::ServiceUid() const
    {
    return TUid::Uid( KCCAClientServiceType );
    }



// --------------------------------------------------------------------------
// RCCAClientUIService::ServiceUid
// --------------------------------------------------------------------------
//
TInt RCCAClientUIService::GetAppWindowGroupId()
    {
    CCA_DP(KCCAClientLoggerFile, CCA_L( "RCCAClientUIService::GetAppWindowGroupId()"));    

    // Create buffer for window group id.
    // Local variable can be used since function in synchronous and variable
    // does not need longer lifetime than the function.
    TInt wgId;
    TPckg<TInt> ptr( wgId );
    
    SendReceive( ECCAGetWindowGroupId, TIpcArgs( &ptr ) );

    CCA_DP(KCCAClientLoggerFile, CCA_L( "RCCAClientUIService::GetAppWindowGroupId(): Done."));    
    
    return wgId;
    }

// End of File
