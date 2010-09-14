/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 command store.
*
*/


// INCLUDE FILES
#include "CPbk2CommandStore.h"
#include "MPbk2CommandResourceRelease.h"

// Phonebook 2
#include <MPbk2Command.h>
#include <MPbk2MenuCommandObserver.h>

// --------------------------------------------------------------------------
// CPbk2CommandStore::CPbk2CommandStore
// --------------------------------------------------------------------------
//
inline CPbk2CommandStore::CPbk2CommandStore() :
iInputBlocker( NULL )
    {
    }

// --------------------------------------------------------------------------
// CPbk2CommandStore::~CPbk2CommandStore
// --------------------------------------------------------------------------
//
CPbk2CommandStore::~CPbk2CommandStore()
    {
    iIdleDestructableCommands.Reset();
    iIdleDestructableCommands.Close();
    iCommandObservers.Reset();
    iCommandObservers.Close();
    iCommandArray.ResetAndDestroy();
    iCommandArray.Close();
    delete iIdleDestroyer;
    delete iInputBlocker;
    }

// --------------------------------------------------------------------------
// CPbk2CommandStore::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2CommandStore* CPbk2CommandStore::NewL()
    {
    CPbk2CommandStore* self = new ( ELeave ) CPbk2CommandStore;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CommandStore::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2CommandStore::ConstructL()
    {
    iIdleDestroyer = CIdle::NewL( CActive::EPriorityIdle );
    }

// --------------------------------------------------------------------------
// CPbk2CommandStore::AddMenuCommandObserver
// --------------------------------------------------------------------------
//
void CPbk2CommandStore::AddMenuCommandObserver
        ( MPbk2MenuCommandObserver& aObserver )
    {
    TRAP_IGNORE( iCommandObservers.AppendL( &aObserver ) );
    }


// --------------------------------------------------------------------------
// CPbk2CommandStore::RemoveMenuCommandObserver
// --------------------------------------------------------------------------
//
void CPbk2CommandStore::RemoveMenuCommandObserver
        ( MPbk2MenuCommandObserver& aObserver )
    {
    TInt index = iCommandObservers.Find( &aObserver );
    if ( index > KErrNotFound )
        {
        iCommandObservers.Remove( index );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommandStore::AddAndExecuteCommandL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2CommandStore::AddAndExecuteCommandL
        ( MPbk2Command* aCommand )
    {
    const TInt KEmpty( 0 );
    if ( aCommand )
        {
        // If command array is empty, execute command.
        // Otherwise delete command to avoid overlapping commands.
        if ( iCommandArray.Count() == KEmpty )
            {
            // Avoid user input during command execution
            if( iInputBlocker )
                {
                iInputBlocker->Cancel();
                }
            iInputBlocker = CAknInputBlock::NewCancelHandlerLC( this );
            CleanupStack::Pop( iInputBlocker );   
            iInputBlocker->SetCancelDelete( iInputBlocker );
                        
            // notify all command observers
            for ( TInt i = 0; i < iCommandObservers.Count(); ++i )
                {
                iCommandObservers[i]->PreCommandExecutionL( *aCommand );
                }
            aCommand->AddObserver( *this );
                        
            // Some command observers needs always postcommand call,
            // if ExecuteLD leaves
            TRAPD( err, aCommand->ExecuteLD() );
            if ( err != KErrNone )
                {
                for (TInt i = 0; i < iCommandObservers.Count(); ++i)
                    {
                    // ignore leave, we are leaving anyway after this
                    TRAP_IGNORE
                        ( iCommandObservers[i]->PostCommandExecutionL( *aCommand ) );
                    }
                
                // stop user input blocker
                if( iInputBlocker )
                    {
                    iInputBlocker->Cancel();
                    }
                
                User::Leave( err );
                }
            User::LeaveIfError( iCommandArray.Append( aCommand ) );
            }
         else
            {
            delete aCommand;
            aCommand = NULL;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommandStore::CommandFinished
// --------------------------------------------------------------------------
//
void CPbk2CommandStore::CommandFinished( const MPbk2Command& aCommand )
    {
    // notify all command observers
    for (TInt i = 0; i < iCommandObservers.Count(); ++i)
        {
        TRAP_IGNORE
            ( iCommandObservers[i]->PostCommandExecutionL( aCommand ) );
        }
    
    // stop user input blocker
    if( iInputBlocker )
        {
        iInputBlocker->Cancel();
        }

    // Add command to the list of idle destroyable objects
    iIdleDestructableCommands.Append( &aCommand );
    // Cancel any outstanding before starting new one.
    iIdleDestroyer->Cancel();
    // Request idle destruction. If the append above failed,
    // it doesn't matter. The command gets destructed then
    // at the Phonebook exit, when this objects destructor
    // deletes also commands in iCommandArray.
    iIdleDestroyer->Start( TCallBack(
        ( &CPbk2CommandStore::IdleDestructorCallback ),this ) );
    }

// --------------------------------------------------------------------------
// CPbk2CommandStore::AknInputBlockCancel
// --------------------------------------------------------------------------
//
void CPbk2CommandStore::AknInputBlockCancel()
	{
	iInputBlocker = NULL;
	}


// --------------------------------------------------------------------------
// CPbk2CommandStore::IdleDestructorCallback
// --------------------------------------------------------------------------
//
TInt CPbk2CommandStore::IdleDestructorCallback( TAny* aSelf )
    {
    CPbk2CommandStore* self = static_cast<CPbk2CommandStore*>( aSelf );
    self->IdleDestructor();
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2CommandStore::IdleDestructor
// --------------------------------------------------------------------------
//
void CPbk2CommandStore::IdleDestructor()
    {
    // Delete objects in idle destroyable array
    for (TInt i = 0; i < iIdleDestructableCommands.Count(); ++i)
        {
        MPbk2Command* idleDestCmd = iIdleDestructableCommands[i];
        for (TInt j = iCommandArray.Count()-1; j >= 0; --j)
            {
            MPbk2Command* arrayCmd = iCommandArray[j];
            if (idleDestCmd == arrayCmd)
                {
                iIdleDestructableCommands.Remove(i);
                                
                // Try to get MPbk2ResourceRelease interface.
                TUid uid;
                uid.iUid = MPbk2ResourceReleaseUID;
                MPbk2ResourceRelease* release = static_cast<MPbk2ResourceRelease *>(idleDestCmd->CommandExtension(uid));
                if (release)
                  {
                    // Call ReleaseResource before delete.
                    release->ReleaseResource();
                  }
              
                // Remove from command array
                iCommandArray.Remove(j);                
                
                delete idleDestCmd;
                idleDestCmd = NULL;
                
                break;
                }
            }
        }
    }


// --------------------------------------------------------------------------
// CPbk2CommandStore::DestroyAllCommands
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2CommandStore::DestroyAllCommands()
    {
    // Delete objects in idle commands array using CommandFinished
    for (TInt i = 0; i < iCommandArray.Count(); ++i)
        {
        MPbk2Command* destCmd = iCommandArray[i];
        CommandFinished ( *destCmd );
        }
    }
// End of File
