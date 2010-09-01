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
* Description:  The main class of the server.
*
*/



// INCLUDE FILES
#include "CVPbkSimServer.h"

// From VirtualPhonebook
#include "VPbkSimServerCommon.h"
#include "VPbkSimServerOpCodes.h"
#include "CVPbkStoreSession.h"
#include <CVPbkSimStoreManager.h>
#include <CVPbkSimCommandStore.h>
#include <MVPbkSimStoreObserver.h>
#include <MVPbkSimCntStore.h>
#include <MVPbkSimPhone.h>
#include <VPbkDebug.h>
#include <ecom/ecom.h>

// System includes
#include <featmgr.h> // FeatureManager

namespace {

// CONSTANTS

// The number of security message ranges
const TUint KMessageRangeCount = 3;

// An array of ranges that have element indexes below
const TInt KMessageRanges[KMessageRangeCount] =
    {
    0, // range is 0 - 2
    4, // range is 4 - 400
    401 // range is 401 - KMaxInt
    };

// An array that defines which security element each range uses
const TUint8 KSecurityElementsIndex[KMessageRangeCount] =
    {
    // First range for calls called before store is open.
    0, // pass
    // Custom check for range 1 to handle custom security check
    CPolicyServer::ECustomCheck,
    // last range for phone calls.
    0 // pass
    };

// Security elements
const CPolicyServer::TPolicyElement KSecurityElements[] =
    {
    _INIT_SECURITY_POLICY_C2( ECapabilityReadUserData,
    ECapabilityWriteUserData )
    };

// Defines the security policy of this server
const CPolicyServer::TPolicy KSecurityPolicy =
    {
    0,  // do capability check in the connection
    KMessageRangeCount,
    KMessageRanges,
    KSecurityElementsIndex,
    KSecurityElements
    };

// ============================= LOCAL FUNCTIONS ===============================

TInt StopActiveScheduler( TAny* /*aAny*/ )
    {
    CActiveScheduler::Stop();
    return 0;
    }
}

// ============================ MEMBER FUNCTIONS ===============================

// --------------------------------------------------------------------------
// CVPbkSimServer::CVPbkSimServer
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CVPbkSimServer::CVPbkSimServer() :
        CPolicyServer( CActive::EPriorityStandard, KSecurityPolicy )
    {
    }

// --------------------------------------------------------------------------
// CVPbkSimServer::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CVPbkSimServer::ConstructL()
    {
    FeatureManager::InitializeLibL();
    iObjectContainerIndex = CObjectConIx::NewL();
    StartL( KVPbkSimServerName );
    iModificationCmdStore = CVPbkSimCommandStore::NewL( ETrue );
    iAsyncShutDown = CPeriodic::NewL( EPriorityIdle );
    iStoreManager = CVPbkSimStoreManager::NewL();
    }

// --------------------------------------------------------------------------
// CVPbkSimServer::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CVPbkSimServer* CVPbkSimServer::NewLC()
    {
    CVPbkSimServer* self = new( ELeave ) CVPbkSimServer;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CVPbkSimServer::~CVPbkSimServer
// Destructor.
// --------------------------------------------------------------------------
//
CVPbkSimServer::~CVPbkSimServer()
    {
    delete iAsyncShutDown;
    delete iModificationCmdStore;
    delete iStoreManager;
    delete iObjectContainerIndex;
    FeatureManager::UnInitializeLib();
    REComSession::FinalClose();
    }

// --------------------------------------------------------------------------
// CVPbkSimServer::NewSessionL
// --------------------------------------------------------------------------
//
CSession2* CVPbkSimServer::NewSessionL( const TVersion& aVersion,
    const RMessage2& /*aMessage*/ ) const
    {
    TVersion serverVersion(
        KVPbkSimServerMajorVersion,
        KVPbkSimServerMinorVersion,
        KVPbkSimServerBuildVersion );

    if ( !User::QueryVersionSupported( serverVersion, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    CVPbkStoreSession* session = CVPbkStoreSession::NewL( *this );
    CleanupStack::PushL( session );
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimServer: session %x, NEW SESSION"), session );
    CleanupStack::Pop( session );
    return session;
    }

// --------------------------------------------------------------------------
// CVPbkSimServer::CustomSecurityCheckL
// --------------------------------------------------------------------------
//
CPolicyServer::TCustomResult CVPbkSimServer::CustomSecurityCheckL( const RMessage2& aMsg, 
                                                                   TInt& /*aAction*/, 
                                                                   TSecurityInfo& aMissing )
    {
    CVPbkStoreSession* session = static_cast< CVPbkStoreSession* > ( aMsg.Session() );
    TSecurityInfo& info = session->SecurityInfo();
    
	aMissing.iCaps.AddCapability( ECapabilityReadUserData );
    switch( aMsg.Function() )
        {
        case EVPbkSimSrvSaveContact:
        case EVPbkSimSrvDeleteContact:
            {
	        aMissing.iCaps.AddCapability( ECapabilityWriteUserData );
            break;
            }
        case EVPbkSimSrvOpenStore:
            {
            TPckg<TSecurityInfo> secPckg( info );
            aMsg.ReadL( KVPbkSlot1, secPckg );            
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    
	TSecurityInfo messageInfo( aMsg );
	// Intersection of real client and RMessage2 caps.
	// Both has to pass.
	messageInfo.iCaps.Intersection( info.iCaps );

	TCustomResult result = EFail;
    if( messageInfo.iCaps.HasCapabilities( aMissing.iCaps ) ) // check caps.
        {
        result = EPass;
        }
	aMissing.iCaps.Remove( messageInfo.iCaps ); // update missing
    return result;
    }

// --------------------------------------------------------------------------
// CVPbkSimServer::NewContainerL
// --------------------------------------------------------------------------
//
CObjectCon* CVPbkSimServer::NewContainerL()
    {
    return iObjectContainerIndex->CreateL();
    }

// --------------------------------------------------------------------------
// CVPbkSimServer::RemoveContainer
// --------------------------------------------------------------------------
//
void CVPbkSimServer::RemoveContainer( CObjectCon& aContainer )
    {
    iObjectContainerIndex->Remove( &aContainer );
    }

// --------------------------------------------------------------------------
// CVPbkSimServer::AppendModificationCmdL
// --------------------------------------------------------------------------
//
void CVPbkSimServer::AppendModificationCmdL( MVPbkSimCommand* aCommand )
    {
    iModificationCmdStore->AddAndExecuteCommandL( aCommand );
    }

// --------------------------------------------------------------------------
// CVPbkSimServer::CancelCommand
// --------------------------------------------------------------------------
//
void CVPbkSimServer::CancelCommand( MVPbkSimCommand& aCommand )
    {
    iModificationCmdStore->CancelCommand( aCommand );
    }

// --------------------------------------------------------------------------
// CVPbkSimServer::IncreaseNumberOfSessions
// --------------------------------------------------------------------------
//
void CVPbkSimServer::IncreaseNumberOfSessions()
    {
    iAsyncShutDown->Cancel();
    ++iNumOfSessions;
    }

// --------------------------------------------------------------------------
// CVPbkSimServer::DecreaseNumberOfSessions
// --------------------------------------------------------------------------
//
void CVPbkSimServer::DecreaseNumberOfSessions()
    {
    --iNumOfSessions;
    if ( iNumOfSessions <= 0 )
        {
        if ( !iAsyncShutDown->IsActive() )
            {
            iAsyncShutDown->Start( KVPbkSimServerShutDownDelay,
                KVPbkSimServerShutDownDelay, TCallBack( StopActiveScheduler ) );
            }
        }
    }

//  End of File
