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
* Description:  A command to save a contact to sim store
*
*/



// INCLUDE FILES
#include "CSaveCommand.h"

#include "SimServerErrors.h"
#include "CVPbkStoreSession.h"
#include "SimServerInternal.h"
#include <MVPbkSimCntStore.h>
#include <MVPbkSimCommandObserver.h>
#include <MVPbkSimStoreOperation.h>
#include <VPbkDebug.h>

namespace VPbkSimServer {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSaveCommand::CSaveCommand
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSaveCommand::CSaveCommand( CVPbkStoreSession& aParentSession,
    const TDesC8& aETelContact, TInt aSimIndex, const RMessage2& aMessage )
    :   iParentSession( aParentSession ),
        iETelContact( aETelContact ),
        iSimIndex( aSimIndex ),
        iMessage( aMessage )
    {
    }

// Destructor
CSaveCommand::~CSaveCommand()
    {
    delete iSaveOperation;
    }

// -----------------------------------------------------------------------------
// CSaveCommand::PanicClient
// -----------------------------------------------------------------------------
//
void CSaveCommand::PanicClient( TClientPanicCode aPanicCode )
    {
    delete iSaveOperation;
    iSaveOperation = NULL;
    iObserver->CommandDone( *this );
    iParentSession.ResetLatestSaveCommand();
    VPbkSimServer::PanicClient( iMessage, aPanicCode );
    }
    
// -----------------------------------------------------------------------------
// CSaveCommand::Execute
// -----------------------------------------------------------------------------
//
void CSaveCommand::Execute()
    {
    TRAPD( result,
        iSaveOperation =
            iParentSession.Store().SaveL( iETelContact, iSimIndex , *this ) );
    if ( result != KErrNone )
        {
        CompleteSaveRequest( result );
        iObserver->CommandDone( *this );
        }
    }

// -----------------------------------------------------------------------------
// CSaveCommand::AddObserverL
// -----------------------------------------------------------------------------
//
void CSaveCommand::AddObserverL( MVPbkSimCommandObserver& aObserver )
    {
    __ASSERT_DEBUG( !iObserver,
        VPbkSimServer::Panic( ECommandObserverAlreadySetInSaveCmd ) );
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CSaveCommand::CancelCmd
// -----------------------------------------------------------------------------
//
void CSaveCommand::CancelCmd()
    {
    delete iSaveOperation;
    iSaveOperation = NULL;
    CompleteSaveRequest( KErrCancel );
    }

// -----------------------------------------------------------------------------
// CSaveCommand::ContactEventComplete
// -----------------------------------------------------------------------------
//
void CSaveCommand::ContactEventComplete( TEvent VPBK_DEBUG_ONLY( aEvent ),
        CVPbkSimContact* /*aContact*/ )
    {
    __ASSERT_DEBUG( aEvent == MVPbkSimContactObserver::ESave,
        VPbkSimServer::Panic( EIncorrectContactEventInSaveCmd ) );
    __ASSERT_DEBUG( iMessage.Handle(),
        VPbkSimServer::Panic( EInvalidHandleInSaveCmd ) );

    // Write the sim index to client memory
    TPckg<TInt> index( iSimIndex );
    TInt result = iMessage.Write( KVPbkSlot1, index );
    CompleteSaveRequest( result );
    iObserver->CommandDone( *this );
    }

// -----------------------------------------------------------------------------
// CSaveCommand::ContactEventError
// -----------------------------------------------------------------------------
//
void CSaveCommand::ContactEventError( TEvent /*aEvent*/,
    CVPbkSimContact* /*aContact*/, TInt aError )
    {
    // Write the sim index to client memory, contact may be partially saved
    TPckg<TInt> index( iSimIndex );
    TInt result = iMessage.Write( KVPbkSlot1, index );
    CompleteSaveRequest( aError );
    iObserver->CommandDone( *this );
    }

// -----------------------------------------------------------------------------
// CSaveCommand::CompleteSaveRequest
// -----------------------------------------------------------------------------
//
void CSaveCommand::CompleteSaveRequest( TInt aResult )
    {
    __ASSERT_DEBUG( iMessage.Handle(),
        VPbkSimServer::Panic( VPbkSimServer::EInvalidHandleInSaveCmd ) );
    VPbkSimSrvUtility::CompleteRequest( iMessage, aResult );
    iParentSession.ResetLatestSaveCommand();
    }
} // namespace VPbkSimServer
//  End of File
