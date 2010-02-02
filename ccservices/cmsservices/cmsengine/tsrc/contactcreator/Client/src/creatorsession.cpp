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
* Description: 
*       
*
*/

#include <flogger.h>
#include "creatordefines.h"
#include "cmscontactcreator.h"

//CLASS DECLARATION
EXPORT_C RCmsCreatorSession::RCmsCreatorSession() 
    {
    }

// ----------------------------------------------------
// RCmsClient::~RCmsClient
// 
// ----------------------------------------------------
//
EXPORT_C RCmsCreatorSession::~RCmsCreatorSession()
    {
    }

// ----------------------------------------------------
// RCmsCreatorSession::CreateContacts
//
// ----------------------------------------------------
//
EXPORT_C TInt RCmsCreatorSession::CreateContacts()
    {
    TInt ret = KErrNone;
    TRequestStatus status = KRequestPending; 
    RProcess server;
    ret = server.Create( KCreatorServerExe, KNullDesC );    
    if( ret != KErrNone )
        return ret;     
    server.Rendezvous( status );    	
    status != KRequestPending ? server.Kill( 0 ) : server.Resume();
    //Wait for start or death 
    User::WaitForRequest( status );	
    ret = server.ExitType() == EExitPanic ? KErrGeneral : status.Int();
    server.Close();	
    return ret;
    }
 

