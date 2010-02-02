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
* Description:  Phonebook 2 application server service class.
*
*/


#include "CPbk2AppService.h"

// Phonebook 2
#include "CPbk2AssignService.h"
#include "CPbk2FetchService.h"
#include <Pbk2ServerAppIPC.h>

// Debug
#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPbk2AppService::CPbk2AppService
// --------------------------------------------------------------------------
//
CPbk2AppService::CPbk2AppService()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2AppService::CPbk2AppService()") );
    }

// --------------------------------------------------------------------------
// CPbk2AppService::~CPbk2AppService
// --------------------------------------------------------------------------
//
CPbk2AppService::~CPbk2AppService()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2AppService::~CPbk2AppService()") );
    if ( !iAcceptMsg.IsNull() )
        {
        iAcceptMsg.Complete( KErrServerTerminated );
        }

    if ( !iExitMsg.IsNull() )
        {
        iExitMsg.Complete( KErrServerTerminated );
        }

    delete iFetchService;
    delete iAssignService;
    }

// --------------------------------------------------------------------------
// CPbk2AppService::NewL
// --------------------------------------------------------------------------
//
CPbk2AppService* CPbk2AppService::NewL()
    {
    CPbk2AppService* self = new ( ELeave ) CPbk2AppService;
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AppService::ServiceL
// --------------------------------------------------------------------------
//
void CPbk2AppService::ServiceL( const RMessage2& aMessage )
    {
    TInt functionId = aMessage.Function();
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2AppService::ServiceL(%d)"), functionId );
    switch ( functionId )
        {
        case EPbk2LaunchFetch:
            {
            // Launch fetch service
            delete iFetchService;
            iFetchService = NULL;
            iFetchService = CPbk2FetchService::NewL( iExitMsg, iAcceptMsg );
            iFetchService->LaunchServiceL( aMessage );
            break;
            }

        case EPbk2CancelFetch:
            {
            // Synchronous service
            // Cancel fetch service
            if ( iFetchService )
                {
                iFetchService->CancelService( aMessage );
                }
            // Complete message if it is not completed by service
            if ( !aMessage.IsNull() )
                {                
                aMessage.Complete( KErrNone );
                }
            break;
            }

        case EPbk2LaunchAssign:             // FALLTHROUGH
        case EPbk2LaunchAttributeAssign:    // FALLTHROUGH
        case EPbk2LaunchAttributeUnassign:
            {
            // Launch assign contact service
            delete iAssignService;
            iAssignService = NULL;
            iAssignService = CPbk2AssignService::NewL( iExitMsg );
            iAssignService->LaunchServiceL( aMessage );
            break;
            }

        case EPbk2CancelAssign:
            {
            // Synchronous service
            // Cancel assign service
            if ( iAssignService )
                {
                iAssignService->CancelService( aMessage );
                }
            // Complete message if it is not completed by service
            if ( !aMessage.IsNull() )
                {                
                aMessage.Complete( KErrNone );
                }
            break;
            }

        case EPbk2GetResults:
            {
            // Synchronous service
            if ( iFetchService )
                {
                iFetchService->GetResultsL( aMessage );
                }
            if ( iAssignService )
                {
                iAssignService->GetResultsL( aMessage );
                }                
            // Complete message if it is not completed by service
            if ( !aMessage.IsNull() )
                {                
                aMessage.Complete( KErrNone );
                }
            break;
            }

        case EPbk2GetResultSize:
            {
            // Synchronous service
            if ( iFetchService )
                {
                iFetchService->GetResultSizeL( aMessage );
                }
            if ( iAssignService )
                {
                iAssignService->GetResultSizeL( aMessage );
                }
            // Complete message if it is not completed by service
            if ( !aMessage.IsNull() )
                {                
                aMessage.Complete( KErrNone );
                }
            break;
            }

        case EPbk2ExitRequest:
            {
            StoreExitRequest( aMessage );
            break;
            }

        case EPbk2CancelExitRequest:
            {
            CancelExitRequest( aMessage );
            break;
            }

        case EPbk2AcceptRequest:
            {
            StoreAcceptRequest( aMessage );
            break;
            }

        case EPbk2CancelAcceptRequest:
            {
            CancelAcceptRequest( aMessage );
            break;
            }

        case EPbk2ExitService:
            {
            // Synchronous service
            // After the client has received a completion
            // to EPbk2ExitRequest, it must send this
            // message to fulfill the protocol
            if ( iFetchService )
                {
                iFetchService->TryExitServiceL( aMessage );
                }
            if ( iAssignService )
                {
                iAssignService->TryExitServiceL( aMessage );
                }
            // Complete message if it is not completed by service
            if ( !aMessage.IsNull() )
                {                
                aMessage.Complete( KErrNone );
                }                
            break;
            }

        case EPbk2AcceptService:
            {
            // Synchronous service
            // After the client has received a completion
            // to EPbk2AcceptRequest, it must send this
            // message to fulfill the protocol
            if ( iFetchService )
                {
                iFetchService->TryAcceptServiceL( aMessage );
                }
            if ( iAssignService )
                {
                iAssignService->TryAcceptServiceL( aMessage );
                }
            // Complete message if it is not completed by service
            if ( !aMessage.IsNull() )
                {                
                aMessage.Complete( KErrNone );
                }                
            break;
            }

        default:
            {
            // Message not recognized, pass it to base class
            CAknAppServiceBase::ServiceL( aMessage );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AppService::SecurityCheckL
// Security check performed when client sends a message requesting a service.
// --------------------------------------------------------------------------
//
CPolicyServer::TCustomResult CPbk2AppService::SecurityCheckL
        ( const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing )
    {
    CPolicyServer::TCustomResult res = CPolicyServer::EFail;
    aAction = CPolicyServer::EPanicClient;

    TInt functionId = aMsg.Function();

    switch ( functionId )
        {
        case EPbk2LaunchFetch:
        case EPbk2CancelFetch:              // FALLTHROUGH
            {
            // Fetch needs ReadUserData capability
            if ( aMsg.HasCapability( ECapabilityReadUserData ) )
                {
                res = CPolicyServer::EPass;
                }
            break;
            }

        case EPbk2LaunchAssign:             // FALLTHROUGH
        case EPbk2CancelAssign:             // FALLTHROUGH
        case EPbk2LaunchAttributeAssign:    // FALLTHROUGH
        case EPbk2LaunchAttributeUnassign:
            {
            // Assign needs ReadUserData and WriteUserData capabilities
            if ( aMsg.HasCapability( ECapabilityReadUserData ) &&
                 aMsg.HasCapability( ECapabilityWriteUserData ) )
                {
                res = CPolicyServer::EPass;
                }
            break;
            }

        case EPbk2GetResults:               // FALLTHROUGH
        case EPbk2GetResultSize:
            {
            // Result fetching needs ReadUserData capability
            if ( aMsg.HasCapability( ECapabilityReadUserData ) )
                {
                res = CPolicyServer::EPass;
                }
            break;
            }

        case EPbk2AcceptService:            // FALLTHROUGH
        case EPbk2AcceptRequest:
            {
            // Accepting needs ReadUserData capability
            if ( aMsg.HasCapability( ECapabilityReadUserData ) )
                {
                res = CPolicyServer::EPass;
                }
            break;
            }

        case EPbk2ExitService:              // FALLTHROUGH
        case EPbk2ExitRequest:              // FALLTHROUGH
        case EPbk2CancelExitRequest:        // FALLTHROUGH
        case EPbk2CancelAcceptRequest:
            {
            // No capabilities required
            res = CPolicyServer::EPass;
            break;
            }

        default:
            {
            // Call base class
            res = CAknAppServiceBase::SecurityCheckL
                ( aMsg, aAction, aMissing );
            break;
            }
        }

    return res;
    }

// --------------------------------------------------------------------------
// CPbk2AppService::StoreExitRequest
// --------------------------------------------------------------------------
//
void CPbk2AppService::StoreExitRequest( const RMessage2& aMessage )
    {
    if ( iExitMsg.Handle() != KNullHandle )
        {
        aMessage.Complete( KErrAlreadyExists );
        }
    else
        {
        iExitMsg = aMessage;
        }
    }

// --------------------------------------------------------------------------
// CPbk2AppService::CancelExitRequest
// --------------------------------------------------------------------------
//
void CPbk2AppService::CancelExitRequest( const RMessage2& aMessage )
    {
    if ( iExitMsg.Handle() != KNullHandle )
        {
        iExitMsg.Complete( KErrCancel );
        }
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CPbk2AppService::StoreAcceptRequest
// --------------------------------------------------------------------------
//
void CPbk2AppService::StoreAcceptRequest( const RMessage2& aMessage )
    {
    if ( iAcceptMsg.Handle() != KNullHandle )
        {
        aMessage.Complete( KErrAlreadyExists );
        }
    else
        {
        iAcceptMsg = aMessage;
        }
    }

// --------------------------------------------------------------------------
// CPbk2AppService::CancelAcceptRequest
// --------------------------------------------------------------------------
//
void CPbk2AppService::CancelAcceptRequest( const RMessage2& aMessage )
    {
    if ( iAcceptMsg.Handle() != KNullHandle )
        {
        iAcceptMsg.Complete( KErrCancel );
        }
    aMessage.Complete( KErrNone );
    }

// End of File
