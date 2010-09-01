/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A composite command that runs subcommands parallel.
*
*/



// INCLUDE FILES
#include "CSequentialCompositeCmd.h"

#include <VPbkDebug.h>
#include <MVPbkSimCommandObserver.h>
#include "VPbkSimStoreImplError.h"

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSequentialCompositeCmd::CSequentialCompositeCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSequentialCompositeCmd::CSequentialCompositeCmd()
    {
    }

// -----------------------------------------------------------------------------
// CSequentialCompositeCmd::NewL
// -----------------------------------------------------------------------------
//    
CSequentialCompositeCmd* CSequentialCompositeCmd::NewL()
    {
    return new( ELeave ) CSequentialCompositeCmd;
    }

// -----------------------------------------------------------------------------
// CSequentialCompositeCmd::NewLC
// -----------------------------------------------------------------------------
//    
CSequentialCompositeCmd* CSequentialCompositeCmd::NewLC()
    {
    CSequentialCompositeCmd* self = 
        new( ELeave ) CSequentialCompositeCmd;
    CleanupStack::PushL( self );
    return self;
    }
    
// Destructor
CSequentialCompositeCmd::~CSequentialCompositeCmd()
    {
    }

// -----------------------------------------------------------------------------
// CSequentialCompositeCmd::Execute
// -----------------------------------------------------------------------------
//
void CSequentialCompositeCmd::Execute()
    {
    __ASSERT_DEBUG( SubCommands().Count() > 0,
        VPbkSimStoreImpl::Panic( EPreCondCSequentialCompositeCmdExecute ) );
        
    iCommandCursor = 0;
    // Execute first command
    TArray<MVPbkSimCommand*> subCommands = SubCommands();
    if ( subCommands.Count() > 0 )
        {
        subCommands[iCommandCursor]->Execute();
        }
    else
        {
        SendCommandDone();
        }
    }
    
// -----------------------------------------------------------------------------
// CSequentialCompositeCmd::CommandDone
// -----------------------------------------------------------------------------
//
void CSequentialCompositeCmd::CommandDone( MVPbkSimCommand& /*aCommand*/ )
    {
    ++iCommandCursor;
    TArray<MVPbkSimCommand*> subCommands = SubCommands();
    if ( iCommandCursor < subCommands.Count() )
        {
        subCommands[iCommandCursor]->Execute();
        }
    else
        {
        SendCommandDone();
        }
    }    
} // namespace VPbkSimStoreImpl
//  End of File  
