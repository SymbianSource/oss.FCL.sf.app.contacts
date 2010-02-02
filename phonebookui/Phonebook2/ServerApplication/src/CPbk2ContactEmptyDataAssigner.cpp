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
* Description:  Phonebook 2 empty contact data assigner.
*
*/


#include "CPbk2ContactEmptyDataAssigner.h"

// Phonebook 2
#include "MPbk2ContactAssignerObserver.h"

// --------------------------------------------------------------------------
// CPbk2ContactEmptyDataAssigner::CPbk2ContactEmptyDataAssigner
// --------------------------------------------------------------------------
//
CPbk2ContactEmptyDataAssigner::CPbk2ContactEmptyDataAssigner
        ( MPbk2ContactAssignerObserver& aObserver ):
            CActive( EPriorityIdle ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEmptyDataAssigner::~CPbk2ContactEmptyDataAssigner
// --------------------------------------------------------------------------
//
CPbk2ContactEmptyDataAssigner::~CPbk2ContactEmptyDataAssigner()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEmptyDataAssigner::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactEmptyDataAssigner* CPbk2ContactEmptyDataAssigner::NewL
        ( MPbk2ContactAssignerObserver& aObserver )
    {
    CPbk2ContactEmptyDataAssigner* self =
        new ( ELeave ) CPbk2ContactEmptyDataAssigner( aObserver );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEmptyDataAssigner::AssignDataL
// --------------------------------------------------------------------------
//
void CPbk2ContactEmptyDataAssigner::AssignDataL(
        MVPbkStoreContact& /*aStoreContact*/,
        MVPbkStoreContactField* /*aContactField*/,
        const MVPbkFieldType* /*aFieldType*/, const HBufC* /*aDataBuffer*/ )
    {
    // Notify observer asynchronously
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEmptyDataAssigner::AssignAttributeL
// --------------------------------------------------------------------------
//
void CPbk2ContactEmptyDataAssigner::AssignAttributeL
        ( MVPbkStoreContact& /*aStoreContact*/,
          MVPbkStoreContactField* /*aContactField*/,
          TPbk2AttributeAssignData /*aAttributeAssignData*/ )
    {
    // Not supported
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEmptyDataAssigner::RunL
// --------------------------------------------------------------------------
//
void CPbk2ContactEmptyDataAssigner::RunL()
    {
    // Just notify the observer
    iObserver.AssignComplete( *this, KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEmptyDataAssigner::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ContactEmptyDataAssigner::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2ContactEmptyDataAssigner::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEmptyDataAssigner::RunError( TInt /*aError*/ )
    {
    // No leaving code in RunL
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEmptyDataAssigner::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2ContactEmptyDataAssigner::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// End of File
