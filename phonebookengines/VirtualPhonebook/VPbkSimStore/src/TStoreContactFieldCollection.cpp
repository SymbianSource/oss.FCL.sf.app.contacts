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
* Description:  The virtual phonebook store field collection implementation
*
*/


// INCLUDES
#include "TStoreContactFieldCollection.h"

#include <MVPbkContactLink.h>
#include <VPbkError.h>
#include <e32base.h>
#include <CVPbkSimContact.h>
#include "TStoreContactField.h"
#include "CContact.h"
#include "CContactLink.h"

namespace VPbkSimStore {

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::TStoreContactFieldCollection
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TStoreContactFieldCollection::TStoreContactFieldCollection()
    {
    }

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::SetContact
// -----------------------------------------------------------------------------
//
void TStoreContactFieldCollection::SetContact( CContact& aParentContact,
    CVPbkSimContact& aSimContact )
    {
    iCurrentField.SetParentContact( aParentContact );
    iParentContact = &aParentContact;
    iSimFields = &aSimContact.FieldArray();
    }

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::ParentContact
// -----------------------------------------------------------------------------
//
MVPbkBaseContact& TStoreContactFieldCollection::ParentContact() const
    {
    return *iParentContact;
    }

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::FieldCount
// -----------------------------------------------------------------------------
//
TInt TStoreContactFieldCollection::FieldCount() const
    {
    return iSimFields->Count();
    }

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::FieldAt
// -----------------------------------------------------------------------------
//
const MVPbkStoreContactField& TStoreContactFieldCollection::FieldAt(
    TInt aIndex) const
    {
    iCurrentField.SetSimField( *(*iSimFields)[aIndex] );
    return iCurrentField;
    }

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::FieldAt
// -----------------------------------------------------------------------------
//
MVPbkStoreContactField& TStoreContactFieldCollection::FieldAt(TInt aIndex)
    {
    iCurrentField.SetSimField( *(*iSimFields)[aIndex] );
    return iCurrentField;
    }

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::FieldAtLC
// -----------------------------------------------------------------------------
//
MVPbkStoreContactField* TStoreContactFieldCollection::FieldAtLC(
        TInt aIndex ) const
    {
    TStoreContactField* field = new( ELeave ) TStoreContactField;
    field->SetParentContact( *iParentContact );
    field->SetSimField( *(*iSimFields)[aIndex] );
    CleanupDeletePushL( field );
    return field;
    }

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::ParentStoreContact
// -----------------------------------------------------------------------------
//
MVPbkStoreContact& TStoreContactFieldCollection::ParentStoreContact() const
    {
    return *iParentContact;
    }

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::RetrieveField
// -----------------------------------------------------------------------------
//    
MVPbkStoreContactField* TStoreContactFieldCollection::RetrieveField(
        const MVPbkContactLink& aContactLink) const
    {
    __ASSERT_ALWAYS(aContactLink.RefersTo(*iParentContact), 
                    VPbkError::Panic(VPbkError::EInvalidContactField));    
    
    MVPbkStoreContactField* result = NULL;
    
    if (&aContactLink.ContactStore() == &iParentContact->ContactStore())
        {
        const CContactLink& link = static_cast<const CContactLink&>(aContactLink);
        TInt fieldIndex = link.FieldId();
        if ( fieldIndex >= 0 && fieldIndex < iSimFields->Count())
            {
            iCurrentField.SetSimField(*(*iSimFields)[fieldIndex]);
            result = &iCurrentField;
            }
        }
    
    return result;
    }
} // namespace VPbkSimStore

