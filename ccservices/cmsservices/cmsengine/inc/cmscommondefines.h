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


#ifndef __CMSCOMMON_H__
#define __CMSCOMMON_H__

#include <e32def.h>
    
// server name
_LIT( KCmsServerName,                          "!CmsServer" );
_LIT( KCmsServerLib,                           "CmsServer" );
_LIT( KCmsServerExe,                           "cmsserver.exe");

// A version must be specifyed when creating a session with the server
const TUint KCmsSrvMajorVersionNumber          = 0;
const TUint KCmsSrvMinorVersionNumber          = 1;
const TUint KCmsSrvBuildVersionNumber          = 1;

// Server panic codes
enum TCmsServerPanic
    {
    ECmsCreateServer = 0,
    ECmsStartServer,
    ECmsFileServerConnect,
    EMainSchedulerError,
    EBadSubSessionRemove,
    EBadSubsessionHandle,
    ENullInterfaceHandle,
    ENullMessageHandle,
    ENullContactHandle,
    ENullBPASHandle,
    EBufferOverflow,
    EBadRequest
    };
    
//Opcodes used in message passing between client and server
enum TCmsServerRequest
	{
	ECmsOpenID = 0,
    ECmsOpenLink,
	ECmsOpenPhoneNbr,
    ECmsCancelOperation,
    ECmsCloseSubsession,
    ECmsFetchContactField,
    ECmsFetchEnabledFields,
    ECmsFetchContactIdentifier,
    ECmsOrderNotification,
    ECmsOrderNextNotification,
    ECmsCancelNotification,
    ECmsCancelExternalContactFind,
    ECmsFindParentStore,
    ECmsFindExternalContact,
    ECmsFetchServiceAvailability,
    ECmsGetContactActionFieldCount,
    ECmsIsTopContact,
	ECmsSetVoiceCallDefault
	};	

//Presence data stream delimiters
_LIT8( KDelimiterNewLine,                           "\r\n\r\n" );
_LIT8( KDelimiterSpaceLit,                          " " );
_LIT8( KDelimiterQuoteLit,                          "\"" );
const TInt KDelimiterSpace                          = 32;
const TInt KDelimiterQuote                          = 34;

//Presence data stream identifiers
const TInt KPresenceDataBrandId                     = 0;
const TInt KPresenceDataElementTextId               = 1;
const TInt KPresenceDataElementImageId              = 2;
const TInt KPresenceDataLanguageId                  = 3;
const TInt KPresenceDataUserIdentity                = 4;
const TInt KPresenceDataServiceType                 = 5;

//Number of asynchronous requests
const TUint KTTMaxAsyncRequests                     = 4;

//Number of data transfer requests
const TUint KTTDataTransferRequests                 = 3;

//Default number of message slots per session
const TUint KTTDefaultMessageSlots	                = 52; // KTTMaxAsyncRequests + 2; // TODO: temporary for demo

const TInt KServerLogBufferMaxSize                  = 2000;

#endif // #ifndef __CMSCOMMON_H__

// End of File
