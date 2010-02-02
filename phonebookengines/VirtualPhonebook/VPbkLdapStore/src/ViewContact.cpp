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
* Description:  CViewContact implementation
*
*/


// INCLUDE FILES
#include "viewcontact.h"
#include "contactstore.h"
#include "contactview.h"
#include "contactlink.h"
#include "fieldtypemappings.h"

// Contact
#include "contact.h"
#include "ldapcontact.h"
#include "ldapcontactfield.h"

#include <mvpbkstorecontact.h>
#include <tvpbkfieldversitproperty.h>

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
//                          TViewContactFieldData class
// -----------------------------------------------------------------------------
// TViewContactFieldData::TViewContactFieldData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TViewContactFieldData::TViewContactFieldData(const TDesC& aData)
:   iData(aData)
    {
    }

// -----------------------------------------------------------------------------
// TViewContactFieldData::IsEmpty
// -----------------------------------------------------------------------------
//
TBool TViewContactFieldData::IsEmpty() const
    {
    return iData.Length() == 0;
    }

// -----------------------------------------------------------------------------
// TViewContactFieldData::CopyL
// -----------------------------------------------------------------------------
//
void TViewContactFieldData::CopyL(const MVPbkContactFieldData& /*aFieldData*/)
    {
    }

// -----------------------------------------------------------------------------
// TViewContactFieldData::Text
// -----------------------------------------------------------------------------
//
TPtrC TViewContactFieldData::Text() const
    {
    return iData;
    }

// -----------------------------------------------------------------------------
// TViewContactFieldData::SetTextL
// -----------------------------------------------------------------------------
//
void TViewContactFieldData::SetTextL(const TDesC& aText)
    {
    iData.Set(aText);
    }

// -----------------------------------------------------------------------------
// TViewContactFieldData::MaxLength
// -----------------------------------------------------------------------------
//
TInt TViewContactFieldData::MaxLength() const
    {
    return iData.Length();
    }

// -----------------------------------------------------------------------------
//                          TViewContactField class
// -----------------------------------------------------------------------------
// TViewContactField::TViewContactField
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TViewContactField::TViewContactField(CViewContact& aParentContact,
    const MVPbkFieldType& aFieldType,const TDesC& aData)
:   iParentContact(aParentContact),
    iFieldType(aFieldType),
    iFieldData(aData)
    {
    }

// -----------------------------------------------------------------------------
// TViewContactField::FieldData
// -----------------------------------------------------------------------------
//
TViewContactFieldData& TViewContactField::FieldData()
    {
    return iFieldData;
    }

// -----------------------------------------------------------------------------
// TViewContactField::ParentContact
// -----------------------------------------------------------------------------
//
MVPbkBaseContact& TViewContactField::ParentContact() const
    {
    return iParentContact;
    }

// -----------------------------------------------------------------------------
// TViewContactField::MatchFieldType
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* TViewContactField::MatchFieldType( 
    TInt /*aMatchPriority*/ ) const
    {
    return &iFieldType;
    }
// -----------------------------------------------------------------------------
// TViewContactField::MatchFieldType
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* TViewContactField::BestMatchingFieldType() const
    {
    return &iFieldType;
    }

// -----------------------------------------------------------------------------
// TViewContactField::FieldData
// -----------------------------------------------------------------------------
//
const MVPbkContactFieldData& TViewContactField::FieldData() const
    {
    return iFieldData;
    }

// -----------------------------------------------------------------------------
// TViewContactField::IsSame
// -----------------------------------------------------------------------------
//
TBool TViewContactField::IsSame(const MVPbkBaseContactField& aOther) const
    {
    if (&aOther.ParentContact() == &ParentContact() &&
        &aOther.FieldData()     == &FieldData())
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
//                      CViewContactFieldCollection class
// -----------------------------------------------------------------------------
// CViewContactFieldCollection::CViewContactFieldCollection
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CViewContactFieldCollection::CViewContactFieldCollection(
    CViewContact&  aParentContact) :
    iParentContact(aParentContact)
    {
    }
// -----------------------------------------------------------------------------
// CViewContactFieldCollection::~CViewContactFieldCollection
// CViewContactFieldCollection Destructor
// -----------------------------------------------------------------------------
//
CViewContactFieldCollection::~CViewContactFieldCollection()
    {
    iFields.Close();
    }

// -----------------------------------------------------------------------------
// CViewContactFieldCollection::ResetFields
// -----------------------------------------------------------------------------
//
void CViewContactFieldCollection::ResetFields()
    {
    iFields.Reset();
    }

// -----------------------------------------------------------------------------
// CViewContactFieldCollection::AppendFieldL
// -----------------------------------------------------------------------------
//
void CViewContactFieldCollection::AppendFieldL(TViewContactField& aNewField)
    {
    iFields.AppendL(aNewField);
    }

// -----------------------------------------------------------------------------
// CViewContactFieldCollection::ParentContact
// -----------------------------------------------------------------------------
//
MVPbkBaseContact& CViewContactFieldCollection::ParentContact() const
    {
    return iParentContact;
    }

// -----------------------------------------------------------------------------
// CViewContactFieldCollection::FieldCount
// -----------------------------------------------------------------------------
//
TInt CViewContactFieldCollection::FieldCount() const
    {
    return iFields.Count();
    }

// -----------------------------------------------------------------------------
// CViewContactFieldCollection::FieldAt
// -----------------------------------------------------------------------------
//
const MVPbkBaseContactField& CViewContactFieldCollection::FieldAt(TInt aIndex) const
    {
    return iFields[aIndex];
    }

// -----------------------------------------------------------------------------
//                          CViewContact class
// -----------------------------------------------------------------------------
// CViewContact::CViewContact
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CViewContact::CViewContact(CContactView& aView,
                           const MVPbkFieldTypeList& aSortOrder)
:   iView(aView),iFieldCollection(*this), iSortOrder(aSortOrder)
    {
    }

// -----------------------------------------------------------------------------
// CViewContact::ConstructL
// CViewContact constructor for performing 2nd stage construction
// -----------------------------------------------------------------------------
//
void CViewContact::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CViewContact::NewLC
// CViewContact two-phased constructor.
// -----------------------------------------------------------------------------
//
CViewContact* CViewContact::NewLC(CContactView& aView,
                                  const MVPbkFieldTypeList& aSortOrder)
    {
    CViewContact* self = new (ELeave)CViewContact(aView,aSortOrder);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
// -----------------------------------------------------------------------------
// CViewContact::NewL
// CViewContact two-phased constructor.
// -----------------------------------------------------------------------------
//
CViewContact* CViewContact::NewL(CContactView& aView,
                                 const MVPbkFieldTypeList& aSortOrder)
    {
    CViewContact* self = CViewContact::NewLC(aView,aSortOrder);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CViewContact::~CViewContact
// CViewContact Destructor
// -----------------------------------------------------------------------------
//
CViewContact::~CViewContact()
    {
    }

// -----------------------------------------------------------------------------
//                          CViewContact public methods
// -----------------------------------------------------------------------------
// CViewContact::ContactIndex
// Return contact index
// -----------------------------------------------------------------------------
//
TInt CViewContact::ContactIndex() const
    {
    return iContactIndex;
    }

// -----------------------------------------------------------------------------
// CViewContact::ContactView
// Return contact view of this view contact, got it
// -----------------------------------------------------------------------------
//
CContactView& CViewContact::ContactView() const
    {
    return iView;
    }

// -----------------------------------------------------------------------------
// CViewContact::SetContactViewContact
// Set contact view contact
// -----------------------------------------------------------------------------
//
void CViewContact::SetContactViewContact(CContact& aContact)
    {
    // Clear all fields
    iFieldCollection.ResetFields();

    // Field mappings
    CFieldTypeMappings& map = iView.Store().FieldTypeMappings();
    // Contact inside contact
    CLDAPContact& contact = aContact.LDAPContact();

    // Sort order field count
    TInt count = iSortOrder.FieldTypeCount();
    // Contact field count
    TInt field_count = contact.FieldCount();

    // Find name field
    for (TInt loop = 0;loop < count;loop++)
        {
        // Field type
        const MVPbkFieldType& type = iSortOrder.FieldTypeAt(loop);
        // Contact field
        CLDAPContactField* fp = NULL;

        // Find match
        for (TInt next = 0; next < field_count;next++)
            {
            // LDAP Field
            fp = contact.FieldAt(next);
            // Virtual phonebook type
            const MVPbkFieldType* vpbk_type = map.Match(fp->Type());
            if (vpbk_type && vpbk_type == &type)
                break;
            // Clear field
            fp = NULL;
            }

        // Found one
        if (fp)
            {
            // With data
            TViewContactField field(*this,type,fp->Data());
            iFieldCollection.AppendFieldL(field);
            }
        else
            {
            // Empty field
            TViewContactField field(*this,type,KNullDesC);
            iFieldCollection.AppendFieldL(field);
            }
        } // End of loop over sort order fields
    iContactIndex = aContact.Index();
    }

// -----------------------------------------------------------------------------
// CViewContact::SetFoldingViewContact
// Set folding view contact
// -----------------------------------------------------------------------------
//
void CViewContact::SetFoldingViewContact(TPtrC aName)
    {
    // Clear all fields
    iFieldCollection.ResetFields();

    // Field count
    TInt count = iSortOrder.FieldTypeCount();
    // Found name
    TBool found = EFalse;
    // Find name field
    for (TInt loop = 0;loop < count;loop++)
        {
        // Field type
        const MVPbkFieldType& type = iSortOrder.FieldTypeAt(loop);
        // Properties
        TArray<TVPbkFieldVersitProperty> props = type.VersitProperties();
        for (TInt next = 0; next < props.Count();next++)
            {
            // This property name
            if (props[next].Name() == EVPbkVersitNameN)
                {
                // Any name can hold the name
                TViewContactField field(*this,type,aName);
                iFieldCollection.AppendFieldL(field);
                found = ETrue;
                break;
                }
            } // End of loop over properties

        // Found one
        if (found)
            {
            break;
            }
        } // End of loop over fields
    }

// -----------------------------------------------------------------------------
//                      MVPbkBaseContact implementation
// -----------------------------------------------------------------------------
// CViewContact::Fields
// -----------------------------------------------------------------------------
//
const MVPbkBaseContactFieldCollection& CViewContact::Fields() const
    {
    return iFieldCollection;
    }

// -----------------------------------------------------------------------------
// CViewContact::IsSame
// -----------------------------------------------------------------------------
//
TBool CViewContact::IsSame(const MVPbkStoreContact& aContact) const
    {
    // From same store
    if (&iView.ContactStore() == &aContact.ContactStore())
        {
        // Store folding view
        if (iView.Type() == EVPbkFoldingView)
            {
            return ETrue;
            }
        else
            {
            // Cast to contact
            const CContact& contact = static_cast<const CContact&>(aContact);
            return (iContactIndex == contact.Index());
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CViewContact::IsSame
// -----------------------------------------------------------------------------
//
TBool CViewContact::IsSame(const MVPbkViewContact& aOtherContact) const
    {
    return aOtherContact.IsSame(*this,&iView.ContactStore());
    }

// -----------------------------------------------------------------------------
// CViewContact::CreateLinkLC
// -----------------------------------------------------------------------------
//
MVPbkContactLink* CViewContact::CreateLinkLC() const
    {
    return CContactLink::NewLC(iView.Store(),iContactIndex);
    }

// -----------------------------------------------------------------------------
// CViewContact::DeleteL
// -----------------------------------------------------------------------------
//
void CViewContact::DeleteL(MVPbkContactObserver& /* aObserver */) const
    {
    // Cannot delete, read only
    User::Leave(KErrAccessDenied);
    }

// -----------------------------------------------------------------------------
// CViewContact::MatchContactStore
// -----------------------------------------------------------------------------
//
TBool CViewContact::MatchContactStore(const TDesC& aContactStoreUri) const
    {
    return iView.MatchContactStore(aContactStoreUri);
    }

// -----------------------------------------------------------------------------
// CViewContact::MatchContactStoreDomain
// -----------------------------------------------------------------------------
//
TBool CViewContact::MatchContactStoreDomain(const TDesC& aContactStoreDomain) const
    {
    return iView.MatchContactStoreDomain(aContactStoreDomain);
    }

// -----------------------------------------------------------------------------
// CViewContact::CreateBookmarkLC
// -----------------------------------------------------------------------------
//
MVPbkContactBookmark* CViewContact::CreateBookmarkLC() const
    {
    return iView.CreateBookmarkLC(iContactIndex);
    }

// -----------------------------------------------------------------------------
//                      MVPbkViewContact implementation
// -----------------------------------------------------------------------------
// CViewContact::ParentView
// -----------------------------------------------------------------------------
//
MVPbkContactViewBase& CViewContact::ParentView() const
    {
    return iView;
    }

// -----------------------------------------------------------------------------
// CViewContact::Read
// -----------------------------------------------------------------------------
//
void CViewContact::ReadL(MVPbkContactObserver& /* aObserver */) const
    {
    // No implementation required - never called
    }

// -----------------------------------------------------------------------------
// CViewContact::ReadAndLockL
// -----------------------------------------------------------------------------
//
void CViewContact::ReadAndLockL(MVPbkContactObserver& /* aObserver */) const
    {
    // Cannot lock, read only
    User::Leave(KErrAccessDenied);
    }

// -----------------------------------------------------------------------------
// CViewContact::Expandable
// -----------------------------------------------------------------------------
//
MVPbkExpandable* CViewContact::Expandable() const
    {
    // Folding view is expandale
    if (iView.Type() == EVPbkFoldingView)
        {
        return const_cast<CViewContact*>(this);
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CViewContact::IsSame
// -----------------------------------------------------------------------------
//
TBool CViewContact::IsSame(
    const MVPbkViewContact&  aOtherContact,
    const MVPbkContactStore* aContactStore
)   const
    {
    // From same store
    if (&iView.ContactStore() == aContactStore)
        {
        // Store folding view
        if (iView.Type() == EVPbkFoldingView)
            {
            return ETrue;
            }
        else
            {
            // Check contact
            return IsSame(aOtherContact);
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CViewContact::IsSame
// -----------------------------------------------------------------------------
//
TBool CViewContact::IsSame(
    const MVPbkStoreContact& aOtherContact,
    const MVPbkContactStore* aContactStore
)   const
    {
    // From same store
    if (&iView.ContactStore() == aContactStore)
        {
        // Store folding view
        if (iView.Type() == EVPbkFoldingView)
            {
            return ETrue;
            }
        else
            {
            // Check contact
            return IsSame(aOtherContact);
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CViewContact::ExpandLC
// -----------------------------------------------------------------------------
//
MVPbkContactViewBase* CViewContact::ExpandLC(
    MVPbkContactViewObserver& aObserver,
    const MVPbkFieldTypeList& aSortOrder
)   const
    {
    return CContactView::NewLC(aObserver,iView.Store(),aSortOrder,EVPbkContactsView);
    }

}  // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
