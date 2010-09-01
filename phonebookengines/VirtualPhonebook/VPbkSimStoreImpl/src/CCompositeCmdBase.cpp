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
* Description:  A composite command base class
*
*/



// INCLUDE FILES
#include "CCompositeCmdBase.h"

#include <VPbkDebug.h>
#include "VPbkSimStoreImplError.h"
#include "MVPbkSimCommandObserver.h"

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCompositeCmdBase::CCompositeCmdBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCompositeCmdBase::CCompositeCmdBase()
    {
    }
    
// Destructor
CCompositeCmdBase::~CCompositeCmdBase()
    {
    iObservers.Close();
    iSubCommands.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCompositeCmdBase::AddSubCommandL
// -----------------------------------------------------------------------------
//
void CCompositeCmdBase::AddSubCommandL( MVPbkSimCommand* aSubCommand )
    {
    __ASSERT_DEBUG( aSubCommand,
        VPbkSimStoreImpl::Panic( EPreCondCCompositeCmdBaseAddSubCommandL ) );
    iSubCommands.AppendL( aSubCommand );
    aSubCommand->AddObserverL( *this );
    }
    
// -----------------------------------------------------------------------------
// CCompositeCmdBase::AddObserverL
// -----------------------------------------------------------------------------
//
void CCompositeCmdBase::AddObserverL( MVPbkSimCommandObserver& aObserver )
    {
    iObservers.AppendL( &aObserver );
    }

// -----------------------------------------------------------------------------
// CCompositeCmdBase::CancelCmd
// -----------------------------------------------------------------------------
//
void CCompositeCmdBase::CancelCmd()
    {
    const TInt count = iSubCommands.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iSubCommands[i]->CancelCmd();
        }
    }

// -----------------------------------------------------------------------------
// CCompositeCmdBase::CommandError
// -----------------------------------------------------------------------------
//
void CCompositeCmdBase::CommandError( MVPbkSimCommand& /*aCommand*/, 
        TInt aError )
    {
    // Cancel all other subcommands
    CancelCmd();
    SendCommandError( aError );
    }

// -----------------------------------------------------------------------------
// CCompositeCmdBase::SendCommandDone
// -----------------------------------------------------------------------------
//
void CCompositeCmdBase::SendCommandDone()
    {
    const TInt count = iObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        iObservers[i]->CommandDone( *this );
        }
    }
        
// -----------------------------------------------------------------------------
// CCompositeCmdBase::SendCommandError
// -----------------------------------------------------------------------------
//
void CCompositeCmdBase::SendCommandError( TInt aError )
    {
    const TInt count = iObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        iObservers[i]->CommandError( *this, aError );
        }
    }
} // namespace VPbkSimStoreImpl
//  End of File  
