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
* Description:  CContact implementation
*
*/


// INCLUDE FILES
#include "contact.h"
#include "ldapcontact.h"
#include "contactlink.h"
#include "contactstore.h"
#include "storebookmark.h"
// Meaning full errors for ldap has not yet been defined, published
// #include <vpbkerror.h>      // Virtual phonebook errors

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// CContact::CContact
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CContact::CContact(CContactStore& aStore,TBool aOwns)
:   iOwnsContact(aOwns), iStore(aStore)
    {
    // No implementation required
    }
// -----------------------------------------------------------------------------
// CContact::ConstructL
// CContactStore constructor for performing 2nd stage construction
// -----------------------------------------------------------------------------
//
void CContact::ConstructL(CLDAPContact* aContact)
    {
    iFields.SetContact(*this,*aContact);
    }

// -----------------------------------------------------------------------------
// CContact::NewLC
// CContact two-phased constructor.
// -----------------------------------------------------------------------------
//
CContact* CContact::NewLC(CContactStore& aStore,CLDAPContact* aContact,TBool aOwns)
    {
    CContact* self = new (ELeave) CContact(aStore,aOwns);
    CleanupStack::PushL(self);
    self->ConstructL(aContact);
    self->iContact = aContact;
    return self;
    }
// -----------------------------------------------------------------------------
// CContact::NewL
// CContact two-phased constructor.
// -----------------------------------------------------------------------------
//
CContact* CContact::NewL(CContactStore& aStore,CLDAPContact* aContact,TBool aOwns)
    {
    CContact* self = CContact::NewLC(aStore,aContact,aOwns);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CContact::~CContact
// CContact Destructor
// -----------------------------------------------------------------------------
//
CContact::~CContact()
    {
    // LDAP Contact
    if (iContact && iOwnsContact)
        {
        delete iContact;
        iContact = NULL;
        }
    }

// -----------------------------------------------------------------------------
//                          CContact public methods
// -----------------------------------------------------------------------------
// CContact::Index
// Returns contact index.
// -----------------------------------------------------------------------------
//
TInt CContact::Index() const
    {
    return iIndex;
    }
// -----------------------------------------------------------------------------
// CContact::SetIndex
// Sets contact index.
// -----------------------------------------------------------------------------
//
void CContact::SetIndex(TInt aIndex)
    {
    iIndex = aIndex;
    }

// -----------------------------------------------------------------------------
// CContact::Store
// Returns the store of this contact.
// -----------------------------------------------------------------------------
//
CContactStore& CContact::Store() const
    {
    return iStore;
    }

// -----------------------------------------------------------------------------
// CContact::LDAPContact
// Returns the contact inside this contact.
// -----------------------------------------------------------------------------
//
CLDAPContact& CContact::LDAPContact() const
    {
    return *iContact;
    }

// -----------------------------------------------------------------------------
// CContact::Contact
// Returns the contact inside this contact.
// -----------------------------------------------------------------------------
//
CLDAPContact* CContact::Contact() const
    {
    return iContact;
    }

// -----------------------------------------------------------------------------
//                      MVPbkObjectHierarchy implementation
// -----------------------------------------------------------------------------
// CContact::ParentObject
// Returns parent of this contact - store
// -----------------------------------------------------------------------------
//
MVPbkObjectHierarchy& CContact::ParentObject() const
    {
    return iStore;
    }

// -----------------------------------------------------------------------------
//                      MVPbkBaseContact implementation
// -----------------------------------------------------------------------------
// CContact::Fields
// Returns this contact's fields (read only).
// -----------------------------------------------------------------------------
//
const MVPbkStoreContactFieldCollection& CContact::Fields() const
    {
    return iFields;
    }

// -----------------------------------------------------------------------------
// CContact::IsSame
// Returns true if this is same contact.
// -----------------------------------------------------------------------------
//
TBool CContact::IsSame(const MVPbkStoreContact& aOtherContact) const
    {
    // From same store
    if (&ParentStore() == &aOtherContact.ParentStore())
        {
        // Cast to contact
        const CContact& other = static_cast<const CContact&>(aOtherContact);
        // Same index
        return (other.Index() == Index());
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CContact::CreateLinkLC
// Creates link representing this contact.
// -----------------------------------------------------------------------------
//
MVPbkContactLink* CContact::CreateLinkLC() const
    {
    return CContactLink::NewLC(iStore,Index());
    }

// -----------------------------------------------------------------------------
// CContact::DeleteL
// -----------------------------------------------------------------------------
//
void CContact::DeleteL(MVPbkContactObserver& /* aObserver */) const
    {
    // Cannot delete, contact is from read only store
    User::Leave(KErrAccessDenied);
    }

// -----------------------------------------------------------------------------
// CContact::MatchContactStore
// -----------------------------------------------------------------------------
//
TBool CContact::MatchContactStore(const TDesC& aContactStoreUri) const
    {
    return iStore.MatchContactStore(aContactStoreUri);
    }

// -----------------------------------------------------------------------------
// CContact::MatchContactStoreDomain
// -----------------------------------------------------------------------------
//
TBool CContact::MatchContactStoreDomain(const TDesC& aContactStoreDomain) const
    {
    return iStore.MatchContactStoreDomain(aContactStoreDomain);
    }

// -----------------------------------------------------------------------------
// CContact::CreateBookmarkLC
// Bookmark this contact.
// -----------------------------------------------------------------------------
//
MVPbkContactBookmark* CContact::CreateBookmarkLC() const
    {
    return CStoreBookmark::NewLC(iStore,Index());
    }

// -----------------------------------------------------------------------------
//                      MVPbkStoreContact implementation
// -----------------------------------------------------------------------------
// CContact::ParentStore
// Returns this contact's parent store.
// -----------------------------------------------------------------------------
//
MVPbkContactStore& CContact::ParentStore() const
    {
    return iStore;
    }

// -----------------------------------------------------------------------------
// CContact::Fields
// Returns this contact's fields.
// -----------------------------------------------------------------------------
//
MVPbkStoreContactFieldCollection& CContact::Fields()
    {
    return iFields;
    }

// -----------------------------------------------------------------------------
// CContact::CreateFieldLC
// Create field
// -----------------------------------------------------------------------------
//
MVPbkStoreContactField* CContact::CreateFieldLC(
    const MVPbkFieldType& /* aFieldType */) const
    {
    // Cannot create ldap fields like this
    User::Leave(KErrAccessDenied);
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContact::AddFieldL
// Add field
// -----------------------------------------------------------------------------
//
TInt CContact::AddFieldL(MVPbkStoreContactField* /* aField */)
    {
    // Cannot add fields
    User::Leave(KErrAccessDenied);
    return KErrAccessDenied;
    }

// -----------------------------------------------------------------------------
// CContact::RemoveField
// Remove field
// -----------------------------------------------------------------------------
//
void CContact::RemoveField(TInt /* aIndex */)
    {
    // Cannot remove - this is not yet defined, should be coming
    // VPbkError::Panic(VPbkError::EInvalidAccessToReadOnlyContact);
    }

// -----------------------------------------------------------------------------
// CContact::RemoveAllFields
// Remove all field
// -----------------------------------------------------------------------------
//
void CContact::RemoveAllFields()
    {
    // Cannot remove - this is not yet defined, should be coming
    // VPbkError::Panic(VPbkError::EInvalidAccessToReadOnlyContact);
    }

// -----------------------------------------------------------------------------
// CContact::LockL
// -----------------------------------------------------------------------------
//
void CContact::LockL(MVPbkContactObserver& /* aObserver */) const
    {
    // Read only
    User::Leave(KErrAccessDenied);
    }

// -----------------------------------------------------------------------------
// CContact::CommitL
// -----------------------------------------------------------------------------
//
void CContact::CommitL(MVPbkContactObserver& /* aObserver */) const
    {
    // Read only
    User::Leave(KErrAccessDenied);
    }

// -----------------------------------------------------------------------------
// CContact::GroupsJoinedLC
// -----------------------------------------------------------------------------
//
MVPbkContactLinkArray* CContact::GroupsJoinedLC() const
    {
    // No groups
    return NULL;
    }
   
// -----------------------------------------------------------------------------
// CContact::Group
// -----------------------------------------------------------------------------
//
MVPbkContactGroup* CContact::Group()
    {
    // No groups
    return NULL;
    }    

// -----------------------------------------------------------------------------
// CContact::MaxNumberOfFieldL
// -----------------------------------------------------------------------------
//
TInt CContact::MaxNumberOfFieldL(const MVPbkFieldType& /* aType */) const
    {
    return KVPbkStoreContactUnlimitedNumber;
    }

} // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
