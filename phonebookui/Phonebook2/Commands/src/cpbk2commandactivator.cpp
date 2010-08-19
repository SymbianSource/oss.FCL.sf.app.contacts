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
* Description:  Takes a command and executes it if predefined
*                condition is met
*
*/


// INCLUDE FILES
#include "cpbk2commandactivator.h"

// pbk2
#include <MPbk2ContactUiControl.h>
#include "cpbk2storestatechecker.h"

// vpbk
#include <MVPbkStoreContact.h>

// system 
#include <coemain.h>


// ============================ LOCAL FUNCTIONS =============================
namespace
    {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicObserverNotSet = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2CommandActivator" );
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

    }

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CPbk2CommandActivator::CPbk2CommandActivator
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CPbk2CommandActivator::CPbk2CommandActivator(
        MPbk2Command* aCommand,
        const MPbk2ContactUiControl& aUiControl ) :  
    iCommand( aCommand ),
    iUiControl( aUiControl )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CommandActivator::CPbk2CommandActivator
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2CommandActivator::~CPbk2CommandActivator()
    {
    delete iCommand;
    delete iStoreStateChecker;
    }

// --------------------------------------------------------------------------
// CPbk2CommandActivator::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2CommandActivator* CPbk2CommandActivator::NewL( 
        MPbk2Command* aCommand,
        const MPbk2ContactUiControl& aUiControl )
    {
    return new( ELeave ) CPbk2CommandActivator
        ( aCommand, aUiControl );
    }

// --------------------------------------------------------------------------
// CPbk2CommandActivator::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2CommandActivator::ExecuteLD()
    {
    __ASSERT_DEBUG( iObserver, Panic( EPanicObserverNotSet ) );
    const MVPbkStoreContact* storeContact = iUiControl.FocusedStoreContact();
    if ( storeContact )
        {
        // Check is store available of current contact
        // and if it is not, show note and skip command execution
        MVPbkContactStore& contactStore = storeContact->ParentStore();
        delete iStoreStateChecker;
        iStoreStateChecker = NULL;
        iStoreStateChecker = 
            CPbk2StoreStateChecker::NewL( contactStore, *this );
        }
    else
        {
        iCommand->ExecuteLD();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommandActivator::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2CommandActivator::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    iCommand->ResetUiControl( aUiControl );
    }

// --------------------------------------------------------------------------
// CPbk2CommandActivator::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2CommandActivator::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iObserver = &aObserver;
    iCommand->AddObserver( *this );
    }

// --------------------------------------------------------------------------
// CPbk2CommandActivator::StoreState
// --------------------------------------------------------------------------
//
void CPbk2CommandActivator::StoreState( 
        MPbk2StoreStateCheckerObserver::TState aState )
    {
    if ( aState == MPbk2StoreStateCheckerObserver::EStoreAvailable )
        {
        TRAPD( err, iCommand->ExecuteLD() );
        if ( err != KErrNone )
            {
            CCoeEnv::Static()->HandleError( err );
            if ( iObserver )
                {
                iObserver->CommandFinished( *this );
                }
            else
                {
                // No observer set so ExecuteLD is a real one here
                iCommand = NULL;
                delete this;
                }
            }
        }
    else
        {
        // It is not fatal error if information note is not shown
        TRAP_IGNORE( iStoreStateChecker->ShowUnavailableNoteL() );
        if ( iObserver )
            {
            iObserver->CommandFinished( *this );
            }
        else
            {
            // No observer so this means that command should destroy itself
            // without help of command handler
            delete this;        
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommandActivator::CommandFinished
// --------------------------------------------------------------------------
//
void CPbk2CommandActivator::CommandFinished( const MPbk2Command& /*aCommand*/ )
    {
    // If this is called, it means that observer is set
    iObserver->CommandFinished( *this );
    }
  
// --------------------------------------------------------------------------
// CPbk2CommandActivator::CommandExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2CommandActivator::CommandExtension(TUid aExtensionUid )
    {
      if( iCommand )
        {
          return iCommand->CommandExtension(aExtensionUid);
        }
      else
        {
          return NULL;
        }
    }

//  End of File  
