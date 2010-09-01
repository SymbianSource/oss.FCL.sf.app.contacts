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
* Description:  A class for managing find operations
*
*/



// INCLUDE FILES
#include "CFindManager.h"

#include <s32mem.h>
#include <RVPbkStreamedIntArray.h>
#include <VPbkSimStoreTemplateFunctions.h>
#include <MVPbkSimStoreOperation.h>
#include "VPbkSimServerCommon.h"
#include "SimServerInternal.h"
#include "VPbkDebug.h"

namespace VPbkSimServer {

// CONSTANTS
const TInt KInitSimIndexBufSize = 10;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFindManager::CFindManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFindManager::CFindManager() 
    {
    }

// -----------------------------------------------------------------------------
// CFindManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFindManager::ConstructL()
    {
    iIndexBuffer = HBufC8::NewL( KInitSimIndexBufSize );
    }

// -----------------------------------------------------------------------------
// CFindManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
// 
CFindManager* CFindManager::NewL()
    {
    CFindManager* self = new( ELeave ) CFindManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CFindManager::~CFindManager()
    {
    delete iStoreOperation;
    delete iIndexBuffer;
    }

// -----------------------------------------------------------------------------
// CFindManager::SetL
// -----------------------------------------------------------------------------
//
void CFindManager::SetL( const RMessage2& aMessage, 
    MVPbkSimStoreOperation* aOperation )
    {
    CleanupDeletePushL( aOperation );
    if ( iMessage.Handle() )
        {
        User::Leave( KErrInUse );
        }
    iMessage = aMessage;
    delete iStoreOperation;
    iStoreOperation = aOperation;
    CleanupStack::Pop(aOperation);
    }

// -----------------------------------------------------------------------------
// CFindManager::CancelMessage
// -----------------------------------------------------------------------------
//
void CFindManager::CancelMessage()
    {
    CompleteRequest( KErrCancel );
    }

// -----------------------------------------------------------------------------
// CFindManager::IsActive
// -----------------------------------------------------------------------------
//
TBool CFindManager::IsActive() const
    {
    // If the handle is non zero then the message is valid.
    return iMessage.Handle() != 0;
    }

// -----------------------------------------------------------------------------
// CFindManager::PanicClient
// -----------------------------------------------------------------------------
//
void CFindManager::PanicClient( TClientPanicCode aPanicCode )
    {
    delete iStoreOperation;
    iStoreOperation = NULL;
    VPbkSimServer::PanicClient( iMessage, aPanicCode );
    }
    
// -----------------------------------------------------------------------------
// CFindManager::FindCompleted
// -----------------------------------------------------------------------------
//
void CFindManager::FindCompleted( MVPbkSimCntStore& /*aStore*/,
    const RVPbkStreamedIntArray&  aSimIndexArray )
    {
    TRAPD( res, HandleFindCompleteL( aSimIndexArray ) );
    if ( res != KErrNone )
        {
        CompleteRequest( res );
        }
    }
  
// -----------------------------------------------------------------------------
// CFindManager::FindError
// -----------------------------------------------------------------------------
// 
void CFindManager::FindError( MVPbkSimCntStore& /*aStore*/, TInt aError )
    {
    CompleteRequest( aError );
    }

// -----------------------------------------------------------------------------
// CFindManager::HandleFindCompleteL
// -----------------------------------------------------------------------------
//
void CFindManager::HandleFindCompleteL( 
    const RVPbkStreamedIntArray& aSimIndexArray )
    {
    TInt result = KErrNone;
    if ( aSimIndexArray.Count() == 0 )
        {
        result = KErrNotFound;
        }
    else
        {
        // Read client buffer size
        TInt clientBufLength = iMessage.GetDesMaxLengthL( KVPbkSlot2 );
        // Get the needed length
        TInt neededBufLength = aSimIndexArray.ExternalizedSize();
        if ( neededBufLength <= clientBufLength )
            {
            TPtr8 ptr( iIndexBuffer->Des() );
            VPbkSimStoreImpl::CheckAndUpdateBufferSizeL( iIndexBuffer, 
                ptr, neededBufLength );
            ptr.Zero();
            RDesWriteStream stream( ptr );
            CleanupClosePushL( stream );
            stream << aSimIndexArray;
            CleanupStack::PopAndDestroy(); // stream
            ptr.SetLength( neededBufLength );
            iMessage.WriteL( KVPbkSlot2, ptr );
            result = KErrNone;
            }
        else
            {
            result = KErrOverflow;
            }
        }
    CompleteRequest( result );
    }

// -----------------------------------------------------------------------------
// CFindManager::CompleteRequest
// -----------------------------------------------------------------------------
//    
void CFindManager::CompleteRequest( TInt result )
    {
    VPbkSimSrvUtility::CompleteRequest( iMessage, result );
    delete iStoreOperation;
    iStoreOperation = NULL;
    }
} // namespace VPbkSimServer 
//  End of File  
