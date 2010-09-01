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
* Description:  Defines the entry point and starting code to server
*
*/



// INCLUDE FILES
#include <e32base.h>
#include "CVPbkSimServer.h"
#include "VPbkSimServerCommon.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RunServerL
// Creates and installs active scheduler. Creates and starts the server.
// -----------------------------------------------------------------------------
//
void RunServerL()
    {
    // Create and install active scheduler
    CActiveScheduler* as = new( ELeave ) CActiveScheduler;
    CleanupStack::PushL( as );
    CActiveScheduler::Install( as );

    // Create server
    CVPbkSimServer::NewLC();

    // Rename the main thread
	User::LeaveIfError( RThread().RenameMe( KVPbkSimServerName ) );
    // Server process is up and running. Signal client.
    RProcess::Rendezvous( KErrNone );

    // Start the active scheduler
    CActiveScheduler::Start();

    // Server closes -> destroy server and active scheduler
    CleanupStack::PopAndDestroy( 2 ); // server, as
    }

// -----------------------------------------------------------------------------
// RunServer
// Creates cleanup stack and calls the leaving starter inside TRAP
// -----------------------------------------------------------------------------
//
TInt RunServer()
    {
    CTrapCleanup* cleanUp = CTrapCleanup::New();

    TInt res = KErrNoMemory;
    if( cleanUp )
        {
        TRAP( res, RunServerL() );
        }

    delete cleanUp;
    return res;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// E32Main
// The executable main function
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
	TInt res = RunServer();
    __UHEAP_MARKEND;
    return res;
    }

//  End of File
