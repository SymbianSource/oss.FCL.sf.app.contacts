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
* Description:  A command store for handling lifetime of the commands
*
*/



// INCLUDE FILES
#include "CVPbkSimCommandStore.h"
#include "VPbkSimStoreImplError.h"
#include "MVPbkSimCommand.h"

// DATA TYPES

namespace
{

/**
* The state of the command
*/
enum TCommandState
    {
    /// command is active
    EExecuting,
    /// command is waiting to be executed
    EWaiting,
    /// command is wating to be deleted
    EToBeDeleted
    };
}

// ============================ MEMBER FUNCTIONS ===============================

/**
* A simple helper class to know which commands should be deleted
*/
NONSHARABLE_CLASS( CVPbkSimCommandStore::CCmd ): public CBase
    {
    public:
        CCmd( MVPbkSimCommand* aCommand );
        ~CCmd();

    public: // Data
        MVPbkSimCommand* iCommand;
        TCommandState iState;
    };

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::CCmd::CCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVPbkSimCommandStore::CCmd::CCmd( MVPbkSimCommand* aCommand )
:   iCommand( aCommand )
    {
    }

CVPbkSimCommandStore::CCmd::~CCmd()
    {
    delete iCommand;
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::CVPbkSimCommandStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVPbkSimCommandStore::CVPbkSimCommandStore( TBool aExecuteOneAtTime )
:   iExecuteOneAtTime( aExecuteOneAtTime )
    {
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVPbkSimCommandStore::ConstructL()
    {
    iIdleDestroyer = CIdle::NewL( CActive::EPriorityIdle );
    iIdleCmdRunner = CIdle::NewL( CActive::EPriorityStandard );
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimCommandStore* CVPbkSimCommandStore::NewL( 
    TBool aExecuteOneAtTime )
    {
    CVPbkSimCommandStore* self = 
        new( ELeave ) CVPbkSimCommandStore( aExecuteOneAtTime );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CVPbkSimCommandStore::~CVPbkSimCommandStore()
    {
    delete iIdleCmdRunner;
    delete iIdleDestroyer;
    iCmdArray.ResetAndDestroy();
    iCmdArray.Close();
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::AddAndExecuteCommandL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkSimCommandStore::AddAndExecuteCommandL( 
    MVPbkSimCommand* aCommand )
    {
    CleanupDeletePushL( aCommand );
    CCmd* cmd = new( ELeave ) CCmd( aCommand );
    CleanupStack::Pop( aCommand ); 
    CleanupStack::PushL( cmd );
    iCmdArray.AppendL( cmd );
    CleanupStack::Pop( cmd );
    
    aCommand->AddObserverL( *this );

    if ( iExecuteOneAtTime )
        {
        cmd->iState = EWaiting;
        RunNextCommand();
        }
    else
        {
        cmd->iState = EExecuting;
        aCommand->Execute();
        }
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::CancelAll
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkSimCommandStore::CancelAll()
    {
    const TInt count = iCmdArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iCmdArray[i]->iState == EExecuting ||
             iCmdArray[i]->iState == EWaiting )
            {
            iCmdArray[i]->iCommand->CancelCmd();
            }
        iCmdArray[i]->iState = EToBeDeleted;
        }

    StartAsyncDelete();
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::CancelCommand
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkSimCommandStore::CancelCommand( MVPbkSimCommand& aCommand )
    {
    TInt pos = FindCommandPosition( aCommand );
    if ( pos >= 0 )
        {
        if ( iCmdArray[pos]->iState == EExecuting )
            {
            /// Run next waiting command if exists
            StartAsyncCommandRunner();
            }
        
        if ( iCmdArray[pos]->iState != EToBeDeleted )
            {
            // Cancel it.
            iCmdArray[pos]->iCommand->CancelCmd();
            // Set command to delete state...
            iCmdArray[pos]->iState = EToBeDeleted;
            // ...and start idle deletion
            StartAsyncDelete();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::Executing
// -----------------------------------------------------------------------------
//
TBool CVPbkSimCommandStore::Executing() const
    {
    TInt count = iCmdArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iCmdArray[i]->iState == EExecuting )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::CommandDone
// -----------------------------------------------------------------------------
//
void CVPbkSimCommandStore::CommandDone( MVPbkSimCommand& aCommand )
    {
    HandleCommandComplete( aCommand );
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::CommandError
// -----------------------------------------------------------------------------
//
void CVPbkSimCommandStore::CommandError( MVPbkSimCommand& aCommand, 
    TInt /*aError*/ )
    {
    HandleCommandComplete( aCommand );
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::RunNextCommand
// -----------------------------------------------------------------------------
//
void CVPbkSimCommandStore::RunNextCommand()
    {
    CVPbkSimCommandStore::CCmd* firstWaitingCmd = NULL;
    TInt count = iCmdArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iCmdArray[i]->iState == EExecuting )
            {
            // Active command found -> don't start next command
            return;
            }
        else if ( iCmdArray[i]->iState == EWaiting )
            {
            firstWaitingCmd = iCmdArray[i];
            /// first waiting command found -> break the loop and execute it
            break;
            }
        }

    if ( firstWaitingCmd )
        {
        firstWaitingCmd->iState = EExecuting;
        firstWaitingCmd->iCommand->Execute();
        }
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::HandleCommandComplete
// -----------------------------------------------------------------------------
//
void CVPbkSimCommandStore::HandleCommandComplete( MVPbkSimCommand& aCommand )
    {
    TInt pos = FindCommandPosition( aCommand );
    if ( pos >= 0 )
        {
        iCmdArray[pos]->iState = EToBeDeleted;
        }

    StartAsyncDelete();
    StartAsyncCommandRunner();
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::StartAsyncDelete
// -----------------------------------------------------------------------------
//
void CVPbkSimCommandStore::StartAsyncDelete()
    {
    iIdleDestroyer->Cancel();
    iIdleDestroyer->Start( 
        TCallBack( (&CVPbkSimCommandStore::IdleDestroyerCallback), this ) );
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::StartAsyncCommandRunner
// -----------------------------------------------------------------------------
//
void CVPbkSimCommandStore::StartAsyncCommandRunner()
    {
    iIdleCmdRunner->Cancel();
    iIdleCmdRunner->Start(
        TCallBack( ( &CmdRunnerCallback ), this ) );
    }
    
// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::IdleDestroyerCallback
// -----------------------------------------------------------------------------
//
TInt CVPbkSimCommandStore::IdleDestroyerCallback( TAny* aSelf )
    {
    static_cast<CVPbkSimCommandStore*>( aSelf )->DestroyCommands();
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::DestroyCommands
// -----------------------------------------------------------------------------
//
void CVPbkSimCommandStore::DestroyCommands()
    {
    TInt count = iCmdArray.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        if ( iCmdArray[i]->iState == EToBeDeleted )
            {
            delete iCmdArray[i];
            iCmdArray.Remove( i );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::CmdRunnerCallback
// -----------------------------------------------------------------------------
//
TInt CVPbkSimCommandStore::CmdRunnerCallback( TAny* aSelf )
    {
    static_cast<CVPbkSimCommandStore*>( aSelf )->RunNextCommand();
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVPbkSimCommandStore::FindCommandPosition
// -----------------------------------------------------------------------------
//
TInt CVPbkSimCommandStore::FindCommandPosition( MVPbkSimCommand& aCommand )
    {
    TInt count = iCmdArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iCmdArray[i]->iCommand == &aCommand )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

//  End of File  
