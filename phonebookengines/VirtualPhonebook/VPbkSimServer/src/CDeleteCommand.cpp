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
* Description:  A command to delete a contact from the sim store
*
*/



// INCLUDE FILES
#include "CDeleteCommand.h"

#include "CVPbkStoreSession.h"
#include "SimServerInternal.h"
#include <MVPbkSimCntStore.h>
#include <MVPbkSimCommandObserver.h>
#include <MVPbkSimStoreOperation.h>

#include <VPbkDebug.h>

namespace VPbkSimServer {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDeleteCommand::CDeleteCommand
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDeleteCommand::CDeleteCommand( CVPbkStoreSession& aParentSession,
    RVPbkStreamedIntArray& aSimIndexes, const RMessage2& aMessage )
    :   iParentSession( aParentSession ),
        iSimIndexes( aSimIndexes ),
        iMessage( aMessage )
    {
    }

// Destructor
CDeleteCommand::~CDeleteCommand()
    {
    delete iDeleteOperation;
    }

// -----------------------------------------------------------------------------
// CDeleteCommand::PanicClient
// -----------------------------------------------------------------------------
//
void CDeleteCommand::PanicClient( TClientPanicCode aPanicCode )
    {
    delete iDeleteOperation;
    iDeleteOperation = NULL;
    iParentSession.ResetLatestDeleteCommand();
    iObserver->CommandDone( *this );
    VPbkSimServer::PanicClient( iMessage, aPanicCode );
    }

// -----------------------------------------------------------------------------
// CDeleteCommand::Execute
// -----------------------------------------------------------------------------
//
void CDeleteCommand::Execute()
    {
    TRAPD( result,
        iDeleteOperation = 
            iParentSession.Store().DeleteL( iSimIndexes, *this ) );
    if ( result != KErrNone )
        {
        CompleteDeleteRequest( result );
        iObserver->CommandDone( *this );
        }
    }

// -----------------------------------------------------------------------------
// CDeleteCommand::AddObserverL
// -----------------------------------------------------------------------------
//
void CDeleteCommand::AddObserverL( MVPbkSimCommandObserver& aObserver )
    {
    __ASSERT_DEBUG( !iObserver,
        VPbkSimServer::Panic( ECommandObserverAlreadySetInDeleteCmd ) );
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CDeleteCommand::CancelCmd
// -----------------------------------------------------------------------------
//
void CDeleteCommand::CancelCmd()
    {
    delete iDeleteOperation;
    iDeleteOperation = NULL;
    CompleteDeleteRequest( KErrCancel );
    }

// -----------------------------------------------------------------------------
// CDeleteCommand::ContactEventComplete
// -----------------------------------------------------------------------------
//
void CDeleteCommand::ContactEventComplete( TEvent VPBK_DEBUG_ONLY( aEvent ),
        CVPbkSimContact* /*aContact*/ )
    {
    __ASSERT_DEBUG( aEvent == MVPbkSimContactObserver::EDelete,
        VPbkSimServer::Panic( EIncorrectContactEventInDeleteCmd ) );
    __ASSERT_DEBUG( iMessage.Handle(),
        VPbkSimServer::Panic( EInvalidHandleInDeleteCmd ) );

    CompleteDeleteRequest( KErrNone );
    iObserver->CommandDone( *this );
    }

// -----------------------------------------------------------------------------
// CDeleteCommand::ContactEventError
// -----------------------------------------------------------------------------
//
void CDeleteCommand::ContactEventError( TEvent /*aEvent*/,
    CVPbkSimContact* /*aContact*/, TInt aError )
    {
    CompleteDeleteRequest( aError );
    iObserver->CommandDone( *this );
    }

// -----------------------------------------------------------------------------
// CDeleteCommand::CompleteDeleteRequest
// -----------------------------------------------------------------------------
//
void CDeleteCommand::CompleteDeleteRequest( TInt aResult )
    {
    __ASSERT_DEBUG( iMessage.Handle(),
        VPbkSimServer::Panic( VPbkSimServer::EInvalidHandleInDeleteCmd ) );
    VPbkSimSrvUtility::CompleteRequest( iMessage, aResult );
    iParentSession.ResetLatestDeleteCommand();
    }
} // namespace VPbkSimServer
//  End of File
