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
* Description:    Entry point for executable, creates and then runs the
*                predefined contacts engine.
*
*/

// System includes
#include <e32base.h>        // CActiveScheduler, CTrapCleanup

// User includes
#include "PdcEngine.h"      // CPdcEngine

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Leaving main functions, creates the predefined contacts engine
// and starts the active scheduler.
// ---------------------------------------------------------------------------
//
void StartPdcEngineL()
	{
    // Create and install the active scheduler		
	CActiveScheduler* activeScheduler	= new (ELeave) CActiveScheduler();
	CleanupStack::PushL( activeScheduler );
	CActiveScheduler::Install( activeScheduler );
	
	// Create the engine
	CPdcEngine* engine = CPdcEngine::NewLC();
	
	RProcess::Rendezvous(KErrNone);
	
	// Check if the predefined contacts need to be added to contacts
	if ( engine->PredefinedContactsNeedAddingL() )
	    {
	    // Begin the process of adding the predefined contacts
	    engine->AddPredefinedContactsL();
	    
	    // Start the active scheduler, the active scheduler is stopped
	    // by the engine, when the adding of the predefined contacts
	    // has finished.
	    CActiveScheduler::Start();
	    }
	
	// Cleanup.
	CleanupStack::PopAndDestroy( engine );	
	CleanupStack::PopAndDestroy( activeScheduler );
	}

// ---------------------------------------------------------------------------
// Main function of the predefined contacts executable.
// ---------------------------------------------------------------------------
//
TInt E32Main()
    {
    __UHEAP_MARK;
    
    // Create the  cleanup stack   
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if( !cleanup )
    	{
    	return KErrNoMemory;
    	}
    
    // Start the engine
    TRAPD( err, StartPdcEngineL() );

    // Cleanup.
    delete cleanup;
    
    __UHEAP_MARKEND;
    
    return err;
    }
    
    
