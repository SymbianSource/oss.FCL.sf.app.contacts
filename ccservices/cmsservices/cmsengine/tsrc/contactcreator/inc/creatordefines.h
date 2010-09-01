/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CREATORDEFINES_H__
#define __CREATORDEFINES_H__

#include <e32def.h>
    
// server name
_LIT( KCreatorServerName,                      "!creatorserver" );
_LIT( KCreatorServerLib,                       "creatorserver" );
_LIT( KCreatorServerExe,                       "creatorserver.exe");

// A version must be specifyed when creating a session with the server
const TUint KMajorVersionNumber                = 0;
const TUint KMinorVersionNumber                = 1;
const TUint KBuildVersionNumber                = 1;
    
//Opcodes used in message passing between client and server
enum TCreatorServerRequest
	{
	ECreateContacts = 0
	};	

//Number of asynchronous requests
const TUint KTTMaxAsyncRequests                     = 4;

//Number of data transfer requests
const TUint KTTDataTransferRequests                 = 3;

//Default number of message slots per session
const TUint KTTDefaultMessageSlots	                = KTTMaxAsyncRequests + 2;

#endif // __CREATORDEFINES_H__

// End of File
