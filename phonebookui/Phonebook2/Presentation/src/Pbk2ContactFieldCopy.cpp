/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class for copying fields to between contacts
*
*/


#include <Pbk2ContactFieldCopy.h>

// From Phonebook2
#include <MPbk2ContactNameFormatter.h>

// From Virtual Phonebook
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldData.h>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkStoreContact.h>

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Pbk2ContactFieldCopy::CopyTitleFieldsL
// ---------------------------------------------------------------------------
//
EXPORT_C void Pbk2ContactFieldCopy::CopyTitleFieldsL(
        const MVPbkStoreContact& aSourceContact,
        MVPbkStoreContact& aTargetContact,
        MPbk2ContactNameFormatter& aNameFormatter )
    {
    const MVPbkStoreContactFieldCollection& sourceFields = 
        aSourceContact.Fields();
    const TInt sourceFieldCount = sourceFields.FieldCount();
    for ( TInt i = 0; i < sourceFieldCount; ++i )
        {
        const MVPbkFieldType* type = 
            sourceFields.FieldAt(i).BestMatchingFieldType();
        if ( type && aNameFormatter.IsTitleFieldType( *type ) )
            {
            MVPbkStoreContactFieldCollection& targetFields = 
                aTargetContact.Fields();
            CVPbkContactFieldTypeIterator* itr = 
                CVPbkContactFieldTypeIterator::NewLC( *type, targetFields );
            if ( !itr->HasNext() )
                {
                // target has no this type of title field -> copy the field
                CopyFieldL( sourceFields.FieldAt(i), *type, aTargetContact );
                }
            CleanupStack::PopAndDestroy( itr );
            }
        }
    }

// ---------------------------------------------------------------------------
// Pbk2ContactFieldCopy::CopyFieldL
// ---------------------------------------------------------------------------
//
EXPORT_C void Pbk2ContactFieldCopy::CopyFieldL( 
        const MVPbkStoreContactField& aSourceField,
        const MVPbkFieldType& aType,
        MVPbkStoreContact& aTargetContact )
    {
    MVPbkStoreContactField* newField = aTargetContact.CreateFieldLC( aType );
    if ( newField->SupportsLabel() )
        {
        TPtrC fieldLabel = aSourceField.FieldLabel();
        if (fieldLabel.Length() > 0)
            {
            newField->SetFieldLabelL(fieldLabel);
            }
        }
    newField->FieldData().CopyL(aSourceField.FieldData());
    aTargetContact.AddFieldL(newField);
    CleanupStack::Pop(); // newField
    }
