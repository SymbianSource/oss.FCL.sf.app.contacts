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
* Description:  Starter for the virtual phonebook sim server
*
*/



// INCLUDE FILES
#include "VPbkSimServerStarter.h"
#include "VPbkSimServerCommon.h"

// CONSTANTS

//  The base name for the starter mutex
_LIT( KVPbkMutexNameExt, "_mutex_" );
const TUint32 KSimServerSid = 0x1028309E;

// ============================= LOCAL FUNCTIONS ===============================

namespace VPbkSimServerStarter
{

// -----------------------------------------------------------------------------
// IsServerRunning
// Returns: ETrue: if server already started
// -----------------------------------------------------------------------------
//
TBool IsServerRunning()
    {
    TFindProcess find;
    TBool result = EFalse;

    TFullName serverName( KVPbkSimServerExeName );
    find.Find( KVPbkSimServerExeName );

    if( find.Next( serverName ) == KErrNone )
        {
        result = ETrue;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// StartProcess
// Starts server as a process
// Returns: KErrNone: server started successfully
// -----------------------------------------------------------------------------
//
TInt StartProcess()
    {
    RProcess process;
    TInt res = process.Create(
        KVPbkSimServerName,
        KNullDesC );

    if ( res != KErrNone )
        {
        return res;
        }
    TSecureId secId( KSimServerSid );
    if( process.SecureId() != secId )
        {
        process.Kill( KErrPermissionDenied );
        process.Close();
        return KErrPermissionDenied;
        }
        
    TRequestStatus startingStatus;
    process.Rendezvous( startingStatus );
    if ( startingStatus != KRequestPending )
        {
        process.Kill( KErrUnknown );
        }
    else
        {
        process.Resume();
        User::WaitForRequest( startingStatus );
        }        

    process.Close();
    return startingStatus.Int();
    }

// -----------------------------------------------------------------------------
// Start
// The public starter function the virtual phonebook sim server.
// Returns: KErrNone: server started successfully
// -----------------------------------------------------------------------------
//
TInt Start()
    {
    RMutex starterMutex;

    TName mutexName( KVPbkSimServerExeName );
    mutexName.Append( KVPbkMutexNameExt );
    TInt res = starterMutex.CreateGlobal( mutexName );
    if ( res == KErrAlreadyExists )
        {
        res = starterMutex.OpenGlobal( mutexName );
        }

    if ( res == KErrNone )
        {
        starterMutex.Wait();
        if ( !IsServerRunning() )
            {
            // Go to critical section
            res = StartProcess();
            }
        starterMutex.Signal();
        }

    starterMutex.Close();

    return res;
    }
}

//  End of File
