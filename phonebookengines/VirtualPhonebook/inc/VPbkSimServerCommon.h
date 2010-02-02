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
* Description:  Common definitions for the sim server.
*
*/



#ifndef VPBKSIMSERVERCOMMON_H
#define VPBKSIMSERVERCOMMON_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS

// Name for the server thread
_LIT( KVPbkSimServerName, "VPbkSimServer" );
// Executable name of the server
_LIT( KVPbkSimServerExeName, "VPbkSimServer" );

// Version numbers
const TInt KVPbkSimServerMajorVersion = 1;
const TInt KVPbkSimServerMinorVersion = 0;
const TInt KVPbkSimServerBuildVersion = 0;

// Message slot definitions
const TInt KVPbkSlot0 = 0;
const TInt KVPbkSlot1 = 1;
const TInt KVPbkSlot2 = 2;
const TInt KVPbkSlot3 = 3;

#if defined(__WINS__) || defined(WINS)
// The delay to server shut down after last session closes
const TInt KVPbkSimServerShutDownDelay = 10000000; // 10 seconds in emulator
#else
// The delay to server shut down after last session closes
const TInt KVPbkSimServerShutDownDelay = 600000000; // 10 minutes in HW
#endif

// DATA TYPES

// Store events
enum TVPbkSimEvent
    {
    EVPbkSimUnknown,
    EVPbkSimStoreOpen           = 100,
    EVPbkSimStoreError,
    EVPbkSimStoreNotAvailable,
    EVPbkSimViewOpen            = 200,
    EVPbkSimViewError,
    EVPbkSimViewNotAvailable,
    EVPbkSimContactAdded        = 300,
    EVPbkSimContactDeleted,
    EVPbkSimContactChanged,
    EVPbkSimPhoneOpen           = 400,
    EVPbkSimPhoneError,
    EVPbkSimPhoneServiceTableUpdated,
    EVPbkSimPhoneFdnStatusChanged
    };

// View event structure
struct TVPbkSimContactEventData
    {
    TVPbkSimContactEventData()
        {
        iEvent = EVPbkSimUnknown;
        iData = KErrNotFound;
        iOpData = KErrNotFound;
        }
    TVPbkSimEvent iEvent;
    TInt iData;
    TInt16 iOpData;
    };

#endif      // VPBKSIMSERVERCOMMON_H
            
// End of File
