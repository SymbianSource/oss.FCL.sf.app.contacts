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
* Description:  Phonebook 2 application server.
*
*/


#include "CPbk2ServerApp.h"

// Phonebook 2
#include "CPbk2AppService.h"
#include <Pbk2InternalUID.h>

// --------------------------------------------------------------------------
// CPbk2ServerApp::NewL
// --------------------------------------------------------------------------
//
CPbk2ServerApp* CPbk2ServerApp::NewL()
    {
    return new ( ELeave ) CPbk2ServerApp;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApp::CreateServiceL
// --------------------------------------------------------------------------
//
CApaAppServiceBase* CPbk2ServerApp::CreateServiceL( TUid aServiceType ) const
    {
    CApaAppServiceBase* result = NULL;
    
    if ( aServiceType == TUid::Uid( KPbk2UIService ) )
        {
        result = CPbk2AppService::NewL();
        }
    else
        {
        result = CAknAppServer::CreateServiceL( aServiceType );
        }
        
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApp::CreateServiceSecurityCheckL
// Security check performed when client connects to the service.
// --------------------------------------------------------------------------
//
CPolicyServer::TCustomResult CPbk2ServerApp::CreateServiceSecurityCheckL
        ( TUid aServiceType, const RMessage2& aMsg, TInt& aAction,
        TSecurityInfo& aMissing )
    {
    // No checks at connection time, forward to base class
    return CAknAppServer::CreateServiceSecurityCheckL
        ( aServiceType, aMsg, aAction, aMissing );
    }

// --------------------------------------------------------------------------
// CPbk2ServerApp::HandleAllClientsClosed
// --------------------------------------------------------------------------
//
void CPbk2ServerApp::HandleAllClientsClosed()
    {
    // For now
    CAknAppServer::HandleAllClientsClosed();
    }
       
// End of File
