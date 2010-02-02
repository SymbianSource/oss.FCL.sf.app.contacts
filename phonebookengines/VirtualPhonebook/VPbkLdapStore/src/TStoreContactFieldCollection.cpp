/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contact field collection class implementation.
*
*/


// INCLUDES
#include "tstorecontactfieldcollection.h"
#include "contact.h"
#include "contactlink.h"
#include "ldapcontact.h"
#include "ldapcontactfield.h"
#include <mvpbkcontactlink.h>
// #include <vpbkerror.h>        // Virtual phonebook error handling

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::TStoreContactFieldCollection
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
TStoreContactFieldCollection::TStoreContactFieldCollection()
    {
    }

// -----------------------------------------------------------------------------
//                  TStoreContactFieldCollection public methods
// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::SetContact
// -----------------------------------------------------------------------------
//
void TStoreContactFieldCollection::SetContact(CContact& aContact,
                                              CLDAPContact& aLDAPContact)
    {
    iCurrentField.SetContact(aContact);
    iContact = &aContact;
    iFields  = &aLDAPContact.Fields();
    }

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::FieldPointer
// -----------------------------------------------------------------------------
//
const MVPbkStoreContactField* TStoreContactFieldCollection::FieldPointer() const
    {
    return &iCurrentField;
    }
// -----------------------------------------------------------------------------
//              MVPbkBaseContactFieldCollection implemetation
// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::ParentContact
// -----------------------------------------------------------------------------
//
MVPbkBaseContact& TStoreContactFieldCollection::ParentContact() const
    {
    return *iContact;
    }
// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::FieldCount
// -----------------------------------------------------------------------------
//
TInt TStoreContactFieldCollection::FieldCount() const
    {
    return iFields->Count();
    }
// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::FieldAt
// -----------------------------------------------------------------------------
//
const MVPbkStoreContactField& TStoreContactFieldCollection::FieldAt(TInt aIndex) const
    {
    iCurrentField.SetField(*(*iFields)[aIndex]);
    return iCurrentField;
    }

// -----------------------------------------------------------------------------
//              MVPbkBaseContactFieldCollection implementation
// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::FieldAt
// -----------------------------------------------------------------------------
//
MVPbkStoreContactField& TStoreContactFieldCollection::FieldAt(TInt aIndex)
    {
    iCurrentField.SetField(*(*iFields)[aIndex]);
    return iCurrentField;
    }

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::FieldAtLC
// -----------------------------------------------------------------------------
//
MVPbkStoreContactField* TStoreContactFieldCollection::FieldAtLC(TInt aIndex) const
    {
    TStoreContactField* field = new (ELeave) TStoreContactField;
    field->SetContact(*iContact);
    field->SetField(*(*iFields)[aIndex]);
    CleanupDeletePushL(field);
    return field;
    }

// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::ParentStoreContact
// -----------------------------------------------------------------------------
//
MVPbkStoreContact& TStoreContactFieldCollection::ParentStoreContact() const
    {
    return *iContact;
    }
    
// -----------------------------------------------------------------------------
// TStoreContactFieldCollection::RetrieveField
// -----------------------------------------------------------------------------
//
MVPbkStoreContactField* TStoreContactFieldCollection::RetrieveField(
                        const MVPbkContactLink& aContactLink) const
    {
    /* Virtual phonebook error handling is not made public yet
    __ASSERT_ALWAYS(aContactLink.RefersTo(*iContact), 
                    VPbkError::Panic(VPbkError::EInvalidContactField));    
    */

    MVPbkStoreContactField* result = NULL;
    // From same store
    if (&aContactLink.ContactStore() == &iContact->ContactStore())
        {
        // Cast to our link
        const CContactLink& link = static_cast<const CContactLink&>(aContactLink);
        if (link.Type() >= 0)
            {
            const TInt count = FieldCount();
            for (TInt loop = 0;loop < count; loop++)
                {
                // Same type
                if ((*iFields)[loop]->Type() == link.Type())
                    {
                    // Set current field
                    iCurrentField.SetField(*(*iFields)[loop]);
                    result = &iCurrentField;
                    break;
                    }
                }
            }
        }
    return result;
    }

} // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
