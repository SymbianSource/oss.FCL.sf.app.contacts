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
* Description:  A call back for contact operation e.g lock
*
*/



// INCLUDE FILES
#include "CContactOperationCallback.h"

#include <MVPbkStoreContact.h>

namespace VPbkSimStore {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContactOperationCallback::CContactOperationCallback
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContactOperationCallback::CContactOperationCallback(
    MVPbkContactObserver::TContactOpResult& aOpResult,
    MVPbkContactObserver& aObserver, TInt aResult )
    :   iObserver( aObserver ),
        iResult( aResult )
    {
    iOpResult = aOpResult;
    }
    
// Destructor
CContactOperationCallback::~CContactOperationCallback()
    {
    delete iOpResult.iStoreContact;
    }

// -----------------------------------------------------------------------------
// CContactOperationCallback::operator()
// -----------------------------------------------------------------------------
//
void CContactOperationCallback::operator()()
    {
    if ( iResult == KErrNone )
        {
        // Observer takes the ownership of the possible contact
        iObserver.ContactOperationCompleted( iOpResult );
        iOpResult.iStoreContact = NULL;
        }
    else
        {
        iObserver.ContactOperationFailed( iOpResult.iOpCode, iResult, EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CContactOperationCallback::Error
// -----------------------------------------------------------------------------
//
void CContactOperationCallback::Error( TInt /*aError*/ )
    {
    // There is nothing leaving in operator() so there is no error to handle
    }
} // namespace VPbkSimStore
//  End of File  
