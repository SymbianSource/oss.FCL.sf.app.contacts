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
* Description:  Phonebook 2 textual contact data assigner.
*
*/


#include "CPbk2ContactTextDataAssigner.h"

// Phonebook 2
#include "MPbk2ContactAssignerObserver.h"

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldTextData.h>


// --------------------------------------------------------------------------
// CPbk2ContactTextDataAssigner::CPbk2ContactTextDataAssigner
// --------------------------------------------------------------------------
//
CPbk2ContactTextDataAssigner::CPbk2ContactTextDataAssigner
        ( MPbk2ContactAssignerObserver& aObserver ):
            CActive( EPriorityIdle ), iObserver( aObserver ),
            iIndex( KErrNotSupported )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactTextDataAssigner::~CPbk2ContactTextDataAssigner
// --------------------------------------------------------------------------
//
CPbk2ContactTextDataAssigner::~CPbk2ContactTextDataAssigner()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2ContactTextDataAssigner::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactTextDataAssigner* CPbk2ContactTextDataAssigner::NewL
        ( MPbk2ContactAssignerObserver& aObserver )
    {
    CPbk2ContactTextDataAssigner* self =
        new ( ELeave ) CPbk2ContactTextDataAssigner( aObserver );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactTextDataAssigner::AssignDataL
// --------------------------------------------------------------------------
//
void CPbk2ContactTextDataAssigner::AssignDataL(
        MVPbkStoreContact& aStoreContact,
        MVPbkStoreContactField* aContactField,
        const MVPbkFieldType* aFieldType, const HBufC* aDataBuffer )
    {
    if ( !aContactField )
        {
        MVPbkStoreContactField* field =
            aStoreContact.CreateFieldLC( *aFieldType );
        InsertDataL( *field, *aDataBuffer );
        iIndex = aStoreContact.AddFieldL( field ); // takes ownership
        CleanupStack::Pop(); // field
        }
    else
        {
        InsertDataL( *aContactField, *aDataBuffer );

        // Find out the field index
        MVPbkStoreContactFieldCollection& fields = aStoreContact.Fields();
        const TInt fieldCount = fields.FieldCount();
        for (TInt i = 0; i < fieldCount; ++i )
            {
            MVPbkStoreContactField* compareField = fields.FieldAtLC( i );
            if ( compareField && compareField->IsSame( *aContactField ) )
                {
                iIndex = i;
                CleanupStack::PopAndDestroy(); // compareField
                break;
                }
            CleanupStack::PopAndDestroy(); // compareField
            }
        }

    // Notify observer asynchronously
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2ContactTextDataAssigner::AssignAttributeL
// --------------------------------------------------------------------------
//
void CPbk2ContactTextDataAssigner::AssignAttributeL
        ( MVPbkStoreContact& /*aStoreContact*/,
          MVPbkStoreContactField* /*aContactField*/,
          TPbk2AttributeAssignData /*aAttributeAssignData*/ )
    {
    // Not supported
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CPbk2ContactTextDataAssigner::RunL
// --------------------------------------------------------------------------
//
void CPbk2ContactTextDataAssigner::RunL()
    {
    // Just notify the observer
    iObserver.AssignComplete( *this, iIndex );
    }

// --------------------------------------------------------------------------
// CPbk2ContactTextDataAssigner::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ContactTextDataAssigner::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2ContactTextDataAssigner::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ContactTextDataAssigner::RunError( TInt /*aError*/ )
    {
    // No leaving code in RunL
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContactTextDataAssigner::InsertDataL
// --------------------------------------------------------------------------
//
void CPbk2ContactTextDataAssigner::InsertDataL
        ( MVPbkStoreContactField& aField, const HBufC& aDataBuffer )
    {
    MVPbkContactFieldTextData::Cast( aField.FieldData() ).
        SetTextL( aDataBuffer );
    }

// --------------------------------------------------------------------------
// CPbk2ContactTextDataAssigner::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2ContactTextDataAssigner::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// End of File
