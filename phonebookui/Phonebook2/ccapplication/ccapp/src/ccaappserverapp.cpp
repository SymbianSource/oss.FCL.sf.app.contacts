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
* Description:  Implementation of server app server.
*
*/


#include "ccaappserverapp.h"
#include "ccaappservice.h"
#include "ccauids.h"

// --------------------------------------------------------------------------
// CCAAppServerApp::NewL
// --------------------------------------------------------------------------
//
CCAAppServerApp* CCAAppServerApp::NewL()
    {
    return new ( ELeave ) CCAAppServerApp;
    }

// --------------------------------------------------------------------------
// CCAAppServerApp::CreateServiceL
// --------------------------------------------------------------------------
//
CApaAppServiceBase* CCAAppServerApp::CreateServiceL( 
    TUid aServiceType ) const
    {
    CApaAppServiceBase* result = NULL;
 
    if ( aServiceType == TUid::Uid( KCCAClientServiceType ) )
        {
        result = new ( ELeave ) CCAAppService;
        }
    else
        {
        result = CAknAppServer::CreateServiceL( aServiceType );
        }
     
    return result;
    }

// end of file
