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
* Description:  Helper class for asynchronous operations
*
*/


// INCLUDES
#include "CVPbkAsyncOperation.h"

#include "Error.h"

namespace VPbkEngUtils {

// anonymous namespace for local definitions
namespace {

} // anonymous namespace

// -----------------------------------------------------------------------------
// CVPbkAsyncOperation::CVPbkAsyncOperation
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkAsyncOperation::CVPbkAsyncOperation()
    :   iPriority( CActive::EPriorityIdle )
    {
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncOperation::CVPbkAsyncOperation
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkAsyncOperation::CVPbkAsyncOperation( 
        CActive::TPriority aPriority )
        :   iPriority( aPriority )
    {
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncOperation::~CVPbkAsyncOperation
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkAsyncOperation::~CVPbkAsyncOperation()
    {
    delete iIdle;
    iCallbacks.ResetAndDestroy();
    
    if (iDestroyed)
        {
        *iDestroyed = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncOperation::CallbackL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkAsyncOperation::CallbackL(MAsyncCallback* aCallback)
    {
    __ASSERT_DEBUG(aCallback, Error::Panic(Error::EPanicInvalidOperation));
    
    if ( !iIdle )
        {
        iIdle = CIdle::NewL( iPriority );
        }
        
    iCallbacks.AppendL( aCallback );

    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncOperation::Purge
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkAsyncOperation::Purge()
    {
    // Cancel and destroy all callbacks
    if ( iIdle )
        {
        iIdle->Cancel();
        }
    iCallbacks.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncOperation::CancelCallback
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkAsyncOperation::CancelCallback( 
        const MAsyncCallback& aCallback )
    {
    TInt index = iCallbacks.Find( &aCallback );
    if ( index != KErrNotFound )
        {
        delete iCallbacks[index];
        iCallbacks.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncOperation::CallbackCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVPbkAsyncOperation::CallbackCount() const
    {
    return iCallbacks.Count();
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncOperation::CallbackAt
// -----------------------------------------------------------------------------
//
EXPORT_C const MAsyncCallback& CVPbkAsyncOperation::CallbackAt( 
        TInt aIndex ) const
    {
    return *iCallbacks[aIndex];
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncOperation::IssueRequest
// -----------------------------------------------------------------------------
//
void CVPbkAsyncOperation::IssueRequest()
    {
    if ( !iIdle->IsActive() )
        {
        iIdle->Start( TCallBack( CVPbkAsyncOperation::IdleCallback, this) );
        }    
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncOperation::HandleIdleCallback
// -----------------------------------------------------------------------------
//
TInt CVPbkAsyncOperation::HandleIdleCallback()
    {
    // Default is not to continue idle.
    TInt result = EFalse;
    volatile TBool destroyed = EFalse;
    iDestroyed = &destroyed;
    
    if (iCallbacks.Count() > 0)
        {
        MAsyncCallback* callback = iCallbacks[0];
        iCallbacks.Remove(0);
        TRAPD(err, (*callback)());
        if (err != KErrNone)
            {
            callback->Error(err);
            }
        delete callback;
        }

    if (!destroyed)
        {
        // Change to NULL because local variable goes out of scope
        // in the end of this function
        iDestroyed = NULL;
        if (iCallbacks.Count() > 0 && !iIdle->IsActive() )
            {
            // There are callbacks left and client hasn't called CallbackL in its
            // callback function -> continue CIdle.
            result = ETrue;
            }
        }
    // DO NOT TOUCH TO MEMBER VARIABLES AFTER THIS POINT BECAUSE THIS
    // OBJECT MAY ALREADY BE DESTROYED.
    return result;
    }

// -----------------------------------------------------------------------------
// CVPbkAsyncOperation::IdleCallback
// -----------------------------------------------------------------------------
//
TInt CVPbkAsyncOperation::IdleCallback( TAny* aThis )
    {
    return static_cast<CVPbkAsyncOperation*>( aThis )->HandleIdleCallback();
    }
} // namespace VPbkEngUtils
