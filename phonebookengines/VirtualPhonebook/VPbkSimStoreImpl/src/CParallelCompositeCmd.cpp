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
#include "CParallelCompositeCmd.h"

#include <VPbkDebug.h>
#include "VPbkSimStoreImplError.h"
#include "MVPbkSimCommandObserver.h"

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CParallelCompositeCmd::CParallelCompositeCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CParallelCompositeCmd::CParallelCompositeCmd( 
        TTerminationPolicy aTerminationPolicy )
        :   iTerminationPolicy( aTerminationPolicy )
    {
    }

// -----------------------------------------------------------------------------
// CParallelCompositeCmd::NewL
// -----------------------------------------------------------------------------
//    
CParallelCompositeCmd* CParallelCompositeCmd::NewL( 
        TTerminationPolicy aTerminationPolicy )
    {
    return new( ELeave ) CParallelCompositeCmd( aTerminationPolicy );
    }

// -----------------------------------------------------------------------------
// CParallelCompositeCmd::NewLC
// -----------------------------------------------------------------------------
//    
CParallelCompositeCmd* CParallelCompositeCmd::NewLC( 
        TTerminationPolicy aTerminationPolicy )
    {
    CParallelCompositeCmd* self = 
        new( ELeave ) CParallelCompositeCmd( aTerminationPolicy );
    CleanupStack::PushL( self );
    return self;
    }
    
// Destructor
CParallelCompositeCmd::~CParallelCompositeCmd()
    {
    }

// -----------------------------------------------------------------------------
// CParallelCompositeCmd::Execute
// -----------------------------------------------------------------------------
//
void CParallelCompositeCmd::Execute()
    {
    __ASSERT_DEBUG( SubCommands().Count() > 0,
        Panic( EPreCondCParallelCompositeCmdExecute ) );
        
    // Execute all subcommands parallel
    TArray<MVPbkSimCommand*> subCommands = SubCommands();
    const TInt count = subCommands.Count();
    if ( count > 0 )
        {
        for ( TInt i = 0; i < count; ++i )
            {
            subCommands[i]->Execute();
            }
        }
    else
        {
        SendCommandDone();
        }        
    }
    
// -----------------------------------------------------------------------------
// CParallelCompositeCmd::CommandDone
// -----------------------------------------------------------------------------
//
void CParallelCompositeCmd::CommandDone( MVPbkSimCommand& /*aCommand*/ )
    {
    ++iTerminatedCommands;
    if ( iTerminationPolicy == EFirstSubCommandDone )
        {
        // Cancel all other subcommands
        CancelCmd();
        SendCommandDone();
        }
    else if ( iTerminatedCommands == SubCommands().Count() )
        {
        SendCommandDone();
        }
    }    
} // namespace VPbkSimStoreImpl
//  End of File  
