/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class handling the use of Cch
*
*/


#include <e32def.h>
#include <cchserver.h>
#include <cchclient.h>

#include "clogsextensioncchhandler.h"
#include "simpledebug.h"



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CLogsExtensionCchHandler::CLogsExtensionCchHandler
// ---------------------------------------------------------------------------
// 
CLogsExtensionCchHandler::CLogsExtensionCchHandler()
: CActive( EPriorityStandard )
    {
    }

// ---------------------------------------------------------------------------
// CLogsExtensionCchHandler::ConstructL
// ---------------------------------------------------------------------------
// 
void CLogsExtensionCchHandler::ConstructL( 
    MLogsExtensionCchHandlerObserver* aObserver,
    TUint aServiceId )
    {    
    _LOG("CLogsExtensionCchHandler::ConstructL...")
    _LOGP("...ConstructL - aServiceId: %d", aServiceId );
    
    iObserver = aObserver;
        
    // Create service selection here
    iCchClient = new ( ELeave ) RCCHClient();
    iCchServer = new ( ELeave ) RCCHServer();
    User::LeaveIfError( iCchServer->Connect() );
    User::LeaveIfError( iCchClient->Open( *iCchServer ) );
    
    // Add to active scheduler
    CActiveScheduler::Add( this );
    
    iServiceSelection.iServiceId = aServiceId;
    iServiceSelection.iType = ECCHVoIPSub;
    
    iCchClient->SubscribeToEvents( 
        iServiceSelection, iServiceStatus, iStatus );

    SetActive();
    }

// ---------------------------------------------------------------------------
// CLogsExtensionCchHandler::NewL
// ---------------------------------------------------------------------------
// 
CLogsExtensionCchHandler* 
    CLogsExtensionCchHandler::NewL( 
        MLogsExtensionCchHandlerObserver* aObserver,
        TUint aServiceId )
    {
    CLogsExtensionCchHandler* self = NewLC( 
        aObserver, aServiceId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CLogsExtensionCchHandler::NewLC
// ---------------------------------------------------------------------------
// 
CLogsExtensionCchHandler* 
    CLogsExtensionCchHandler::NewLC( 
        MLogsExtensionCchHandlerObserver* aObserver,
        TUint aServiceId )
    {
    CLogsExtensionCchHandler* self =
        new ( ELeave ) CLogsExtensionCchHandler();
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aServiceId );
    return self;
    }

// ---------------------------------------------------------------------------
// CLogsExtensionCchHandler::~CLogsExtensionCchHandler
// ---------------------------------------------------------------------------
// 
CLogsExtensionCchHandler::~CLogsExtensionCchHandler()
    {
    _LOG("CLogsExtensionCchHandler::...")
    _LOG("...~CLogsExtensionCchHandler - begin")
    
    Cancel();
    
    if ( iCchClient )
        {
        iCchClient->Close();
        }
        
    delete iCchClient;
    iCchClient = NULL;
    
    if ( iCchServer )
        {
        iCchServer->Close();
        }
        
    delete iCchServer;
    iCchServer = NULL;
    _LOG("...~CLogsExtensionCchHandler - end")
    }

// ---------------------------------------------------------------------------
// CLogsExtensionCchHandler::RunL
// ---------------------------------------------------------------------------
// 
void CLogsExtensionCchHandler::RunL()
    {
    _LOG("CLogsExtensionCchHandler::RunL ...");
    _LOGP("...RunL - stat: %d", iStatus.Int() );

    if ( iStatus == KErrNone )  
        {
        TServiceStatus serviceStatus = iServiceStatus();
        _LOG("...RunL - Notify client")
        
        if ( iObserver )
            {
            iObserver->CchEventOccuredL( 
                serviceStatus.iConnectionInfo.iServiceSelection.iServiceId, 
                serviceStatus.iState,
                serviceStatus.iError );
            }

        _LOGP("...start listening again: %d", 
            serviceStatus.iConnectionInfo.iServiceSelection.iServiceId );
        }
    else 
        {
        // To be sure that event is cancelled
        DoCancel(); 
        }
    // Reissue notification request from CCH
    iCchClient->SubscribeToEvents( 
            iServiceSelection, iServiceStatus, iStatus );
    SetActive(); 
    _LOG("...RunL - out ");
    }

// ---------------------------------------------------------------------------
// CLogsExtensionCchHandler::DoCancel
// ---------------------------------------------------------------------------
// 
void CLogsExtensionCchHandler::DoCancel()
    {
    _LOG("CLogsExtensionCchHandler::DoCancel -begin")
    if ( IsActive() )
        {
        _LOG("CLogsExtensionCchHandler:: cancel events")
        iCchClient->SubscribeToEventsCancel();
        }
    _LOG("CLogsExtensionCchHandler::DoCancel -end")
    }

// ---------------------------------------------------------------------------
// CLogsExtensionCchHandler::RunError
// ---------------------------------------------------------------------------
// 
TInt CLogsExtensionCchHandler::RunError( TInt aError )
    {
    _LOG("CLogsExtensionCchHandler::RunError...");
    _LOGP("... RunError: %d", aError );
    
    if ( KErrServerTerminated != aError )
        {
        if ( !IsActive() )
            {
            iCchClient->SubscribeToEvents( 
                iServiceSelection, iServiceStatus, iStatus );
            SetActive();    
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CLogsExtensionCchHandler::GetServiceState
// ---------------------------------------------------------------------------
// 
TInt CLogsExtensionCchHandler::GetServiceState( 
    TServiceSelection aServiceSelection, 
    TCCHSubserviceState& aServiceState )
    {
    return iCchClient->GetServiceState( aServiceSelection, 
            aServiceState );
    }


// ---------------------------------------------------------------------------
// CLogsExtensionCchHandler::ResolveSubServiceState
// ---------------------------------------------------------------------------
// 
void CLogsExtensionCchHandler::ResolveSubServiceState( 
        TUint aServiceId, TInt& aCchErr )
    {
    _LOG("CLogsExtensionCchHandler::ResolveSubServiceState - begin");
        
    TServiceSelection serviceSelection;
    serviceSelection.iServiceId = aServiceId;
    serviceSelection.iType = ECCHPresenceSub;
    
    TCCHSubserviceState serviceState( 
            ECCHUninitialized );
    _LOG("...  Get Service state");
    
    TInt error = GetServiceState( 
        serviceSelection, serviceState );
    _LOGP("...ResolveServiceState - error: %d", error );
    _LOGP("...ResolveServiceState - service state: %d", 
        serviceState );
    
    aCchErr = error;
    
    if ( KErrNone == error &&
        serviceState != ECCHEnabled )
        {
        aCchErr = KErrNotReady;
        }
        
    _LOGP("...ResolveServiceState - aCchErr: %d", aCchErr );

    _LOG("CLogsExtensionCchHandler::ResolveSubServiceState - end");
    }

    

