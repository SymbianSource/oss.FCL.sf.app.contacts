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
* Description:  Contact field class implementation
*
*/


// INCLUDES
#include "tstorecontactfield.h"
#include "contact.h"
#include "ldapcontact.h"
#include "ldapcontactfield.h"
#include "contactstore.h"
#include "contactlink.h"
#include "fieldtypemappings.h"
#include <mvpbkbasecontact.h>
#include <mvpbkcontactstoreproperties.h>

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// TStoreContactField::TStoreContactField
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
TStoreContactField::TStoreContactField()
    {
    }

// -----------------------------------------------------------------------------
//                      TStoreContactField public methods
// -----------------------------------------------------------------------------
// TStoreContactField::SetContact
// -----------------------------------------------------------------------------
//
void TStoreContactField::SetContact(CContact& aContact)
    {
    iContact = &aContact;
    }

// -----------------------------------------------------------------------------
// TStoreContactField::SetField
// -----------------------------------------------------------------------------
//
void TStoreContactField::SetField(CLDAPContactField& aField)
    {
    iField = &aField;
    iFieldData.SetField(aField);
    }

// -----------------------------------------------------------------------------
// TStoreContactField::Field
// -----------------------------------------------------------------------------
//
CLDAPContactField* TStoreContactField::Field()
    {
    return iField;
    }

// -----------------------------------------------------------------------------
//                      MVPbkObjectHierarchy implementation
// -----------------------------------------------------------------------------
// TStoreContactField::ParentObject
// -----------------------------------------------------------------------------
//
MVPbkObjectHierarchy& TStoreContactField::ParentObject() const
    {
    return *iContact;
    }

// -----------------------------------------------------------------------------
//                      MVPbkBaseContactField implementation
// -----------------------------------------------------------------------------
// TStoreContactField::ParentContact
// -----------------------------------------------------------------------------
//
MVPbkBaseContact& TStoreContactField::ParentContact() const
    {
    return *iContact;
    }

// -----------------------------------------------------------------------------
// TStoreContactField::MatchFieldType
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* TStoreContactField::MatchFieldType(
    TInt /*aMatchPriority*/ ) const
    {
    return iContact->Store().FieldTypeMappings().Match(iField->Type());
    }

// -----------------------------------------------------------------------------
// TStoreContactField::BestMatchingFieldType
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* TStoreContactField::BestMatchingFieldType() const
    {
    return MatchFieldType(0);
    }

// -----------------------------------------------------------------------------
// TStoreContactField::FieldData
// -----------------------------------------------------------------------------
//
const MVPbkContactFieldData& TStoreContactField::FieldData() const
    {
    return iFieldData;
    }

// -----------------------------------------------------------------------------
// TStoreContactField::IsSame
// -----------------------------------------------------------------------------
//
TBool TStoreContactField::IsSame(const MVPbkBaseContactField& aOther) const
    {
    if (&ParentContact() == &aOther.ParentContact())
        {
        return iField == static_cast<const TStoreContactField&>(aOther).iField; 
        }
    return EFalse;
    }
   
// -----------------------------------------------------------------------------
//                      MVPbkStoreContactField implementation
// -----------------------------------------------------------------------------
// TStoreContactField::SupportsLabel
// -----------------------------------------------------------------------------
//
TBool TStoreContactField::SupportsLabel() const
    {
    // Store supports label - nope
    return iContact->ParentStore().StoreProperties().SupportsFieldLabels();
    }

// -----------------------------------------------------------------------------
// TStoreContactField::FieldLabel
// -----------------------------------------------------------------------------
//
TPtrC TStoreContactField::FieldLabel() const
    {
    return KNullDesC();
    }

// -----------------------------------------------------------------------------
// TStoreContactField::SetFieldLabelL
// -----------------------------------------------------------------------------
//
void TStoreContactField::SetFieldLabelL(const TDesC& /*aText*/)
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// TStoreContactField::MaxLabelLength
// -----------------------------------------------------------------------------
//
TInt TStoreContactField::MaxLabelLength() const
    {
    return 0;
    }
    
// -----------------------------------------------------------------------------
// TStoreContactField::FieldData
// -----------------------------------------------------------------------------
//
MVPbkContactFieldData& TStoreContactField::FieldData()
    {
    return iFieldData;
    }

// -----------------------------------------------------------------------------
// TStoreContactField::CloneLC
// -----------------------------------------------------------------------------
//
MVPbkStoreContactField* TStoreContactField::CloneLC() const
    {
    TStoreContactField* clone = new (ELeave) TStoreContactField;
    clone->SetContact(*this->iContact);
    clone->SetField(*this->iField);
    CleanupDeletePushL(clone);
    return clone;
    }

// -----------------------------------------------------------------------------
// TStoreContactField::CreateLinkLC
// -----------------------------------------------------------------------------
//    
MVPbkContactLink* TStoreContactField::CreateLinkLC() const
    {
    return CContactLink::NewLC(iContact->Store(),iContact->Index(),iField->Type());
    }

}  // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
