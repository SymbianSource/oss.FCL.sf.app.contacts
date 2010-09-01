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
* Description:  Phonebook 2 speed dial attribute assigner.
*
*/


#include "CPbk2ContactSpeedDialAttributeAssigner.h"

// Phonebook 2
#include "MPbk2ContactAssignerObserver.h"

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactField.h>
#include <CVPbkSpeedDialAttribute.h>
#include <MVPbkContactOperationBase.h>


// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeAssigner::CPbk2ContactSpeedDialAttributeAs.
// --------------------------------------------------------------------------
//
CPbk2ContactSpeedDialAttributeAssigner::CPbk2ContactSpeedDialAttributeAssigner
        ( MPbk2ContactAssignerObserver& aObserver,
          CVPbkContactManager& aContactManager ):
            CActive( EPriorityIdle ),
            iObserver( aObserver ),
            iContactManager( aContactManager )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeAssigner::~CPbk2ContactSpeedDialAttributeAs.
// --------------------------------------------------------------------------
//
CPbk2ContactSpeedDialAttributeAssigner::
        ~CPbk2ContactSpeedDialAttributeAssigner()
    {
    Cancel();

    delete iAssignOperation;
    delete iAttribute;
    delete iStoreContactField;
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeAssigner::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactSpeedDialAttributeAssigner*
    CPbk2ContactSpeedDialAttributeAssigner::NewL
        ( MPbk2ContactAssignerObserver& aObserver,
          CVPbkContactManager& aContactManager )
    {
    CPbk2ContactSpeedDialAttributeAssigner* self =
        new ( ELeave ) CPbk2ContactSpeedDialAttributeAssigner
            ( aObserver, aContactManager );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeAssigner::AssignDataL
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeAssigner::AssignDataL
        ( MVPbkStoreContact& /*aStoreContact*/,
          MVPbkStoreContactField* /*aContactField*/,
          const MVPbkFieldType* /*aFieldType*/,
          const HBufC* /*aDataBuffer*/ )
    {
    // Not supported
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeAssigner::AssignAttributeL
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeAssigner::AssignAttributeL
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
// CPbk2ContactSpeedDialAttributeAssigner::AttributeOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeAssigner::AttributeOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/ )
    {
    iObserver.AssignComplete( *this, KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeAssigner::AttributeOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeAssigner::AttributeOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iObserver.AssignFailed( *this, aError );
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeAssigner::RunL
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeAssigner::RunL()
    {
    AssignAttributeL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeAssigner::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeAssigner::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeAssigner::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ContactSpeedDialAttributeAssigner::RunError( TInt aError )
    {
    iObserver.AssignFailed( *this, aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeAssigner::AssignAttributeL
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeAssigner::AssignAttributeL()
    {
    delete iAttribute;
    iAttribute = NULL;
    iAttribute = CVPbkSpeedDialAttribute::NewL
        ( iAttributeAssignData.iAttributeValue );

    // Check does the attribute already exist
    TBool hasAttribute = iContactManager.ContactAttributeManagerL().
        HasFieldAttributeL( *iAttribute, *iStoreContactField );

    if ( !hasAttribute )
        {
        delete iAssignOperation;
        iAssignOperation = NULL;
        iAssignOperation = iContactManager.ContactAttributeManagerL().
            SetFieldAttributeL( *iStoreContactField, *iAttribute, *this );
        }
    else
        {
        iObserver.AssignComplete( *this, KErrNotSupported );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeAssigner::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2ContactSpeedDialAttributeAssigner::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// End of File
