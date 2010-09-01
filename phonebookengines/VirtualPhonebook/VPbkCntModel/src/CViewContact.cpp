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
* Description:  Contacts Model store view contact implementation.
*
*/


// INCLUDES
#include "CViewContact.h"
#include <cntviewbase.h>
#include <cntitem.h>

#include <VPbkError.h>
#include <MVPbkContactObserver.h>
#include <MVPbkFieldType.h>

#include "CContact.h"
#include "CContactView.h"
#include "CGroupMembersView.h"
#include "CContactStore.h"
#include "CContactLink.h"
#include "CFieldTypeMap.h"

namespace VPbkCntModel {

// --------------------------------------------------------------------------
// TViewContactTextFieldData::TViewContactTextFieldData
// --------------------------------------------------------------------------
//
TViewContactTextFieldData::TViewContactTextFieldData(const TDesC& aTextData) :
    iTextData(aTextData)
    {
    }

// --------------------------------------------------------------------------
// TViewContactTextFieldData::SetTextData
// --------------------------------------------------------------------------
//
inline void TViewContactTextFieldData::SetTextData(const TDesC& aTextData)
    {
    iTextData.Set(aTextData);
    }

// --------------------------------------------------------------------------
// TViewContactTextFieldData::IsEmpty
// --------------------------------------------------------------------------
//
TBool TViewContactTextFieldData::IsEmpty() const
    {
    return iTextData.Length() == 0;
    }

// --------------------------------------------------------------------------
// TViewContactTextFieldData::CopyL
// --------------------------------------------------------------------------
//
void TViewContactTextFieldData::CopyL(const MVPbkContactFieldData& /*aFieldData*/)
    {
    /// Do nothing
    }

// --------------------------------------------------------------------------
// TViewContactTextFieldData::Text
// --------------------------------------------------------------------------
//
TPtrC TViewContactTextFieldData::Text() const
    {
    return iTextData;
    }

// --------------------------------------------------------------------------
// TViewContactTextFieldData::SetTextL
// --------------------------------------------------------------------------
//
void TViewContactTextFieldData::SetTextL(const TDesC& /* aText */)
    {
    /// Do nothing
    }

// --------------------------------------------------------------------------
// TViewContactTextFieldData::MaxLength
// --------------------------------------------------------------------------
//
TInt TViewContactTextFieldData::MaxLength() const
    {
    return KVPbkUnlimitedFieldLength;
    }

// --------------------------------------------------------------------------
// TViewContactField::TViewContactField
// --------------------------------------------------------------------------
//
TViewContactField::TViewContactField( CViewContact& aParentContact, 
    const TDesC& aFieldData, const MVPbkFieldType& aFieldType, 
    TInt aFieldIndex ) 
    :   iParentContact(aParentContact),
        iFieldData(aFieldData),
        iFieldType(&aFieldType),
        iFieldIndex(aFieldIndex)
    {
    }
    
// --------------------------------------------------------------------------
// TViewContactField::TViewContactField
// --------------------------------------------------------------------------
//
inline void TViewContactField::SetFieldData(const TDesC& aFieldData)
    {
    iFieldData.SetTextData(aFieldData);
    }

// --------------------------------------------------------------------------
// TViewContactField::ParentContact
// --------------------------------------------------------------------------
//
MVPbkBaseContact& TViewContactField::ParentContact() const
    {
    return iParentContact;
    }

// --------------------------------------------------------------------------
// TViewContactField::MatchFieldType
// --------------------------------------------------------------------------
//
const MVPbkFieldType* TViewContactField::MatchFieldType(TInt /* aMatchPriority */) const
    {
    return iFieldType;
    }

// --------------------------------------------------------------------------
// TViewContactField::BestMatchingFieldType
// --------------------------------------------------------------------------
//
const MVPbkFieldType* TViewContactField::BestMatchingFieldType() const
    {
    return iFieldType;
    }

// --------------------------------------------------------------------------
// TViewContactField::FieldData
// --------------------------------------------------------------------------
//
const MVPbkContactFieldData& TViewContactField::FieldData() const
    {
    return iFieldData;
    }

// --------------------------------------------------------------------------
// TViewContactField::IsSame
// --------------------------------------------------------------------------
//
TBool TViewContactField::IsSame( const MVPbkBaseContactField& aOther ) const
    {
    // View contact fields don't have unique
    // id. Fields are same if the parent view contact is same instance
    // and field indexes are the same
    if (&aOther.ParentContact() == &iParentContact)
        {
        return iFieldIndex == 
            static_cast<const TViewContactField&>( aOther ).iFieldIndex;
        }

    return EFalse;
    }

// --------------------------------------------------------------------------
// CViewContactFieldCollection::CViewContactFieldCollection
// --------------------------------------------------------------------------
//
CViewContactFieldCollection::CViewContactFieldCollection
        (CViewContact& aParentContact) :
    iParentContact(aParentContact)
    {
    }

// --------------------------------------------------------------------------
// CViewContactFieldCollection::CViewContactFieldCollection
// --------------------------------------------------------------------------
//
CViewContactFieldCollection::~CViewContactFieldCollection()
    {
    iFields.Close();
    }

// --------------------------------------------------------------------------
// CViewContactFieldCollection::SetFieldsData
// --------------------------------------------------------------------------
//
void CViewContactFieldCollection::SetFieldsData(
        const ::CViewContact& aViewContact )
    {
    const TInt count = aViewContact.FieldCount()<iFields.Count()?
        aViewContact.FieldCount():iFields.Count();
    for (TInt i = 0; i < count; ++i)
        {
        iFields[i].SetFieldData(aViewContact.Field(i));
        }
    }

// --------------------------------------------------------------------------
// CViewContactFieldCollection::SetTypeListL
// --------------------------------------------------------------------------
//
void CViewContactFieldCollection::SetTypeListL(
        const MVPbkFieldTypeList& aFieldTypes )
    {
    iFields.Reset();
    const TInt count = aFieldTypes.FieldTypeCount();
    for (TInt i = 0; i < count; ++i)
        {
        User::LeaveIfError(iFields.Append(TViewContactField(iParentContact, 
                KNullDesC, aFieldTypes.FieldTypeAt(i), iFields.Count() )));
        }
    }

// --------------------------------------------------------------------------
// CViewContactFieldCollection::ParentContact
// --------------------------------------------------------------------------
//
MVPbkBaseContact& CViewContactFieldCollection::ParentContact() const
    {
    return iParentContact;
    }

// --------------------------------------------------------------------------
// CViewContactFieldCollection::FieldCount
// --------------------------------------------------------------------------
//
TInt CViewContactFieldCollection::FieldCount() const
    {
    return iFields.Count();
    }

// --------------------------------------------------------------------------
// CViewContactFieldCollection::FieldAt
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField& CViewContactFieldCollection::FieldAt(TInt aIndex) const
    {
    return iFields[aIndex];
    }

// --------------------------------------------------------------------------
// CViewContact::CViewContact
// --------------------------------------------------------------------------
//
#pragma warning(push)
#pragma warning(disable : 4355) // this used in base member initializer list
CViewContact::CViewContact(CViewBase& aParentView) :
    iParentView(aParentView),
    iViewContact(NULL),
    iFieldCollection(*this)
    {
    }
#pragma warning(pop)

// --------------------------------------------------------------------------
// CViewContact::NewL
// --------------------------------------------------------------------------
//
CViewContact* CViewContact::NewL
        (CViewBase& aParentView, const MVPbkFieldTypeList& aFieldTypes)
    {
    CViewContact* self = new(ELeave) CViewContact(aParentView);
    CleanupStack::PushL(self);
    self->SetTypeListL(aFieldTypes);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CViewContact::SetTypeListL
// --------------------------------------------------------------------------
//
void CViewContact::SetTypeListL(const MVPbkFieldTypeList& aFieldTypes)
    {
    iFieldCollection.SetTypeListL(aFieldTypes);
    }

// --------------------------------------------------------------------------
// CViewContact::SetViewContact
// --------------------------------------------------------------------------
//
void CViewContact::SetViewContact(const ::CViewContact& aViewContact)
    {
    iViewContact = &aViewContact;
    iFieldCollection.SetFieldsData(aViewContact);
    }

// --------------------------------------------------------------------------
// CViewContact::View
// --------------------------------------------------------------------------
//
CViewBase& CViewContact::View() const
    {
    return iParentView;
    }

// --------------------------------------------------------------------------
// CViewContact::Id
// --------------------------------------------------------------------------
//
TContactItemId CViewContact::Id() const
    {
    return iViewContact->Id();
    }

// --------------------------------------------------------------------------
// CViewContact::NativeContact
// --------------------------------------------------------------------------
//
const ::CViewContact* CViewContact::NativeContact() const
    {
    return iViewContact;
    }

// --------------------------------------------------------------------------
// CViewContact::Fields
// --------------------------------------------------------------------------
//
const MVPbkBaseContactFieldCollection& CViewContact::Fields() const
    {
    return iFieldCollection;
    }

// --------------------------------------------------------------------------
// CViewContact::IsSame
// --------------------------------------------------------------------------
//
TBool CViewContact::IsSame(const MVPbkStoreContact& aOtherContact) const
    {
    if (iViewContact && &iParentView.ContactStore()
            == &aOtherContact.ContactStore())
        {
        return (iViewContact->Id() == 
            static_cast<const CContact&>(aOtherContact).
                NativeContact()->Id());
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CViewContact::IsSame
// --------------------------------------------------------------------------
//
TBool CViewContact::IsSame(const MVPbkViewContact& aOtherContact) const
    {
    return aOtherContact.IsSame(*this, &iParentView.ContactStore());
    }

// --------------------------------------------------------------------------
// CViewContact::CreateLinkLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CViewContact::CreateLinkLC() const
    {
    return iParentView.Store().CreateLinkLC(iViewContact->Id());
    }

// --------------------------------------------------------------------------
// CViewContact::ReadL
// --------------------------------------------------------------------------
//
void CViewContact::ReadL(MVPbkContactObserver& aObserver) const
    {
    iParentView.Store().ReadContactL(iViewContact->Id(), aObserver);
    }

// --------------------------------------------------------------------------
// CViewContact::ReadAndLockL
// --------------------------------------------------------------------------
//
void CViewContact::ReadAndLockL(MVPbkContactObserver& aObserver) const
    {
    iParentView.Store().ReadAndLockContactL(iViewContact->Id(), aObserver);
    }

// --------------------------------------------------------------------------
// CViewContact::Expandable
// --------------------------------------------------------------------------
//
MVPbkExpandable* CViewContact::Expandable() const
    {
    MVPbkExpandable* result = NULL;
    
    if (iViewContact->ContactType() == ::CViewContact::EGroup)
        {
        result = const_cast<CViewContact*>(this);
        }
        
    return result;
    }

// --------------------------------------------------------------------------
// CViewContact::DeleteL
// --------------------------------------------------------------------------
//
void CViewContact::DeleteL(MVPbkContactObserver& aObserver) const
    {
    iParentView.Store().DeleteContactL(iViewContact->Id(), aObserver);
    }

// --------------------------------------------------------------------------
// CViewContact::MatchContactStore
// --------------------------------------------------------------------------
//
TBool CViewContact::MatchContactStore(const TDesC& aContactStoreUri) const
    {
    return iParentView.MatchContactStore(aContactStoreUri);
    }
    
// --------------------------------------------------------------------------
// CViewContact::MatchContactStoreDomain
// --------------------------------------------------------------------------
//
TBool CViewContact::MatchContactStoreDomain
        (const TDesC& aContactStoreDomain) const
    {
    return iParentView.MatchContactStoreDomain(aContactStoreDomain);
    }

// --------------------------------------------------------------------------
// CViewContact::CreateBookmarkLC
// --------------------------------------------------------------------------
//
MVPbkContactBookmark* CViewContact::CreateBookmarkLC() const
    {
    return iParentView.Store().CreateBookmarkLC(iViewContact->Id());
    }

// --------------------------------------------------------------------------
// CViewContact::ParentView
// --------------------------------------------------------------------------
//
MVPbkContactViewBase& CViewContact::ParentView() const
    {
    return iParentView;
    }

// --------------------------------------------------------------------------
// CViewContact::IsSame
// --------------------------------------------------------------------------
//
TBool CViewContact::IsSame(
        const MVPbkViewContact& aOtherContact, 
        const MVPbkContactStore* aContactStore) const
    {
    if (iViewContact && &iParentView.ContactStore() == aContactStore)
        {
        return (iViewContact->Id() ==
            static_cast<const CViewContact&>(aOtherContact).Id());
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CViewContact::IsSame
// --------------------------------------------------------------------------
//
TBool CViewContact::IsSame(
        const MVPbkStoreContact& aOtherContact, 
        const MVPbkContactStore* aContactStore) const
    {
    if (iViewContact && &iParentView.ContactStore() == aContactStore)
        {
        return (iViewContact->Id() == 
            static_cast<const CContact&>(aOtherContact).
                NativeContact()->Id());
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CViewContact::ExpandLC
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CViewContact::ExpandLC(
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder) const
    {
    return CGroupMembersView::NewLC(iParentView.Store(),
        iViewContact->Id(), aObserver, aSortOrder);
    }

// --------------------------------------------------------------------------
// CViewContact::BaseContactExtension
// --------------------------------------------------------------------------
//
TAny* CViewContact::BaseContactExtension( TUid aExtensionUid )
    {
    if( aExtensionUid == KVPbkBaseContactExtension2Uid )
        return static_cast<MVPbkBaseContact2*>( this );
    return NULL;
    }

// --------------------------------------------------------------------------
// CViewContact::IsOwnContact
// --------------------------------------------------------------------------
//
TBool CViewContact::IsOwnContact( TInt& aError ) const
    {
    aError = KErrNone;
    return ( iViewContact->ContactTypeUid() == KUidContactOwnCard );
    }

} // namespace VPbkCntModel

// End of File
