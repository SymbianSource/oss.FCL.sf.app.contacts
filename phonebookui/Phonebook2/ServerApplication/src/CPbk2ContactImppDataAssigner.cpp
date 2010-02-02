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


#include "CPbk2ContactImppDataAssigner.h"

// Phonebook 2
#include "MPbk2ContactAssignerObserver.h"

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <TVPbkFieldVersitProperty.h>

// --------------------------------------------------------------------------
// CPbk2ContactImppDataAssigner::CPbk2ContactImppDataAssigner
// --------------------------------------------------------------------------
//
CPbk2ContactImppDataAssigner::CPbk2ContactImppDataAssigner
        ( MPbk2ContactAssignerObserver& aObserver ):
            CActive( EPriorityIdle ), iObserver( aObserver ),
            iIndex( KErrNotSupported )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactImppDataAssigner::~CPbk2ContactImppDataAssigner
// --------------------------------------------------------------------------
//
CPbk2ContactImppDataAssigner::~CPbk2ContactImppDataAssigner()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2ContactImppDataAssigner::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactImppDataAssigner* CPbk2ContactImppDataAssigner::NewL
        ( MPbk2ContactAssignerObserver& aObserver )
    {
    CPbk2ContactImppDataAssigner* self =
        new ( ELeave ) CPbk2ContactImppDataAssigner( aObserver );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactImppDataAssigner::AssignDataL
// --------------------------------------------------------------------------
//
void CPbk2ContactImppDataAssigner::AssignDataL(
        MVPbkStoreContact& aStoreContact,
        MVPbkStoreContactField* aContactField,
        const MVPbkFieldType* aFieldType, const HBufC* aDataBuffer )
    {
    TPtrC xSP;
    TPtrC firstName;
    TPtrC lastName;
    TPtrC nickname;

    ParseDataBuffer(&xSP, &firstName, &lastName, &nickname,
            TPtrC(*aDataBuffer));

    if ( !aContactField )
        {
        MVPbkStoreContactField* field =
            aStoreContact.CreateFieldLC( *aFieldType );
        MVPbkContactFieldUriData::Cast(field->FieldData()).
                SetUriL(xSP);
        iIndex = aStoreContact.AddFieldL( field ); // takes ownership
        CleanupStack::Pop(); // field
        }
    else
        {
        MVPbkContactFieldUriData::Cast(aContactField->FieldData()).
                SetUriL(xSP);

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

    if (firstName.Length())
        {
        TVPbkFieldVersitProperty prop;
        prop.SetName(EVPbkVersitNameN);
        prop.SetSubField(EVPbkVersitSubFieldGivenName);
        UpdateField(prop, firstName, &aStoreContact);
        }

    if (lastName.Length())
        {
        TVPbkFieldVersitProperty prop;
        prop.SetName(EVPbkVersitNameN);
        prop.SetSubField(EVPbkVersitSubFieldFamilyName);
        UpdateField(prop, lastName, &aStoreContact);
        }

    if (nickname.Length())
        {
        _LIT8(KXNickname, "X-NICKNAME");
        TVPbkFieldVersitProperty prop;
        prop.SetName(EVPbkVersitNameX);
        prop.SetExtensionName(KXNickname);
        UpdateField(prop, nickname, &aStoreContact);
        }

    // Notify observer asynchronously
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2ContactImppDataAssigner::AssignAttributeL
// --------------------------------------------------------------------------
//
void CPbk2ContactImppDataAssigner::AssignAttributeL
        ( MVPbkStoreContact& /*aStoreContact*/,
          MVPbkStoreContactField* /*aContactField*/,
          TPbk2AttributeAssignData /*aAttributeAssignData*/ )
    {
    // Not supported
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CPbk2ContactImppDataAssigner::RunL
// --------------------------------------------------------------------------
//
void CPbk2ContactImppDataAssigner::RunL()
    {
    // Just notify the observer
    iObserver.AssignComplete( *this, iIndex );
    }

// --------------------------------------------------------------------------
// CPbk2ContactImppDataAssigner::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ContactImppDataAssigner::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2ContactImppDataAssigner::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ContactImppDataAssigner::RunError( TInt /*aError*/ )
    {
    // No leaving code in RunL
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContactImppDataAssigner::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2ContactImppDataAssigner::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2ContactImppDataAssigner::ParseDataBuffer
// --------------------------------------------------------------------------
//
void CPbk2ContactImppDataAssigner::ParseDataBuffer(TPtrC* axSP,
        TPtrC* aFirstName, TPtrC* aLastName, TPtrC* aNickname,
        TPtrC aDataBuffer)
    {
    GetDataBufferPart(&aDataBuffer, axSP) &&
    GetDataBufferPart(&aDataBuffer, aFirstName) &&
    GetDataBufferPart(&aDataBuffer, aLastName) &&
    GetDataBufferPart(&aDataBuffer, aNickname);
    }

// --------------------------------------------------------------------------
// CPbk2ContactImppDataAssigner::UpdateField
// --------------------------------------------------------------------------
//
void CPbk2ContactImppDataAssigner::UpdateField(
        const TVPbkFieldVersitProperty& aVersitProp, const TDesC& aValue,
        MVPbkStoreContact* aStoreContact)
    {
    MVPbkStoreContactFieldCollection& fields = aStoreContact->Fields();
    TInt count = fields.FieldCount();
    TInt i = 0;
    for (; i < count; i++)
        {
        MVPbkStoreContactField& field = fields.FieldAt(i);
        if (field.BestMatchingFieldType()->Matches(aVersitProp, 0))
            {
            MVPbkContactFieldTextData& data = MVPbkContactFieldTextData::Cast(
                    field.FieldData());
            if (!data.Text().Length())
                {
                data.SetTextL(aValue);
                }
            // contact contains field of this type so finish loop
            break;
            }
        }
    
    // if field was not found then create it
    if (i == count)
        {
        const MVPbkFieldTypeList& suportedTypes = aStoreContact->ParentStore().
                StoreProperties().SupportedFields();
        count = suportedTypes.FieldTypeCount();
        for (i = 0; i < count; i++)
            {
            const MVPbkFieldType& fieldType = suportedTypes.FieldTypeAt(i);
            if (fieldType.Matches(aVersitProp, 0))
                {
                MVPbkStoreContactField* field =
                    aStoreContact->CreateFieldLC(fieldType);
                MVPbkContactFieldTextData::Cast(field->FieldData()).
                        SetTextL(aValue);
                aStoreContact->AddFieldL(field); // takes ownership
                CleanupStack::Pop(); // field
                break;
                }
            }
        }
    }

// End of File
