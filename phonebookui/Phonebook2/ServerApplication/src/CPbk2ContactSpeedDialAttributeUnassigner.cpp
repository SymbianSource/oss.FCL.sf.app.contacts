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
* Description:  Phonebook 2 speed dial attribute unassigner.
*
*/


#include "CPbk2ContactSpeedDialAttributeUnassigner.h"

// Phonebook 2
#include "MPbk2ContactAssignerObserver.h"

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkStoreContactField.h>
#include <CVPbkSpeedDialAttribute.h>
#include <MVPbkContactOperationBase.h>


// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeUnassigner::CPbk2ContactSpeedDialAttributeU.
// --------------------------------------------------------------------------
//
CPbk2ContactSpeedDialAttributeUnassigner::
    CPbk2ContactSpeedDialAttributeUnassigner
        ( MPbk2ContactAssignerObserver& aObserver,
          CVPbkContactManager& aContactManager ) :
            CActive( EPriorityIdle ),
            iObserver( aObserver ),
            iContactManager( aContactManager )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeUnassigner::~CPbk2ContactSpeedDialAttribute.
// --------------------------------------------------------------------------
//
CPbk2ContactSpeedDialAttributeUnassigner::
        ~CPbk2ContactSpeedDialAttributeUnassigner()
    {
    Cancel();

    delete iUnassignOperation;
    delete iAttribute;
    delete iStoreContactField;
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeUnassigner::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactSpeedDialAttributeUnassigner*
    CPbk2ContactSpeedDialAttributeUnassigner::NewL
        ( MPbk2ContactAssignerObserver& aObserver,
          CVPbkContactManager& aContactManager )
    {
    CPbk2ContactSpeedDialAttributeUnassigner* self =
        new ( ELeave ) CPbk2ContactSpeedDialAttributeUnassigner
            ( aObserver, aContactManager );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeUnassigner::AssignDataL
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeUnassigner::AssignDataL
        ( MVPbkStoreContact& /*aStoreContact*/,
          MVPbkStoreContactField* /*aContactField*/,
          const MVPbkFieldType* /*aFieldType*/,
          const HBufC* /*aDataBuffer*/ )
    {
    // Not supported
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeUnassigner::AssignAttributeL
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeUnassigner::AssignAttributeL
        ( MVPbkStoreContact& /*aStoreContact*/,
          MVPbkStoreContactField* aContactField,
          TPbk2AttributeAssignData aAttributeAssignData )
    {
    iAttributeAssignData = aAttributeAssignData;

    if ( aContactField )
        {
        iStoreContactField = aContactField->CloneLC();
        CleanupStack::Pop(); // iStoreContactField
        }
    else
        {
        // Speed dial attribute works on contact field level
        User::Leave( KErrArgument );
        }

    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeUnassigner::AttributeOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeUnassigner::AttributeOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/ )
    {
    iObserver.AssignComplete( *this, KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeUnassigner::AttributeOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeUnassigner::AttributeOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iObserver.AssignFailed( *this, aError );
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeUnassigner::RunL
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeUnassigner::RunL()
    {
    UnassignAttributeL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeUnassigner::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeUnassigner::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeUnassigner::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ContactSpeedDialAttributeUnassigner::RunError( TInt aError )
    {
    iObserver.AssignFailed( *this, aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeUnassigner::UnassignAttributeL
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeUnassigner::UnassignAttributeL()
    {
    delete iAttribute;
    iAttribute = NULL;
    iAttribute = CVPbkSpeedDialAttribute::NewL
        ( iAttributeAssignData.iAttributeValue );

    delete iUnassignOperation;
    iUnassignOperation = NULL;
    iUnassignOperation = iContactManager.ContactAttributeManagerL().
        RemoveFieldAttributeL( *iStoreContactField, *iAttribute, *this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeUnassigner::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeUnassigner::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// End of File
