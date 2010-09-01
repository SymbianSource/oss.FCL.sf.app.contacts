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
* Description:  Virtual Phonebook folding view contact
*
*/


#include "CVPbkFoldingViewContact.h"

#include <MVPbkContactStore.h>
#include <TVPbkFieldTypeMapping.h>
#include <MVPbkContactStoreProperties.h>

#include "CVPbkFoldingContactView.h"
#include "TVPbkFoldingContactBookmark.h"

//////////////////////////////////////////////////////////////////////////
// TFoldingViewContactTextFieldData implementation
//////////////////////////////////////////////////////////////////////////
TFoldingViewContactTextFieldData::TFoldingViewContactTextFieldData(
    CVPbkFoldingViewContact& aParentContact) :
    iParentContact(aParentContact)
    {
    }

TBool TFoldingViewContactTextFieldData::IsEmpty() const
    {
    return Text().Length() == 0;
    }

TPtrC TFoldingViewContactTextFieldData::Text() const
    {
    return iParentContact.NativeParentView().Name();
    }

void TFoldingViewContactTextFieldData::CopyL(const MVPbkContactFieldData& /*aFieldData*/)
    {
    }

void TFoldingViewContactTextFieldData::SetTextL(const TDesC& /* aText */)
    {
    }

TInt TFoldingViewContactTextFieldData::MaxLength() const
    {
    return KVPbkUnlimitedFieldLength;
    }

// TFoldingViewContactField
TFoldingViewContactField::TFoldingViewContactField(CVPbkFoldingViewContact& aParentContact) :
    iParentContact(aParentContact),
    iFieldData(aParentContact)
    {
    }

MVPbkBaseContact& TFoldingViewContactField::ParentContact() const
    {
    return iParentContact;
    }

const MVPbkFieldType* TFoldingViewContactField::MatchFieldType(TInt /*aMatchPriority*/) const
    {
    if (!iFieldType)
        {
        TVPbkFieldTypeMapping typeMapping;
        typeMapping.SetNonVersitType(EVPbkNonVersitTypeGenericLabel);

        iFieldType = typeMapping.FindMatch(iParentContact.ParentView().SortOrder());
        }
    return iFieldType;
    }

const MVPbkFieldType* TFoldingViewContactField::BestMatchingFieldType() const
    {
    TInt dummy = 0;
    /// MatchFieldType above doesn't use the priority
    return MatchFieldType(dummy);
    }

const MVPbkContactFieldData& TFoldingViewContactField::FieldData() const
    {
    return iFieldData;
    }

TBool TFoldingViewContactField::IsSame(const MVPbkBaseContactField& /*aOther*/) const
    {
    return EFalse;
    }

// TFoldingViewFieldCollection
TFoldingViewFieldCollection::TFoldingViewFieldCollection(
    CVPbkFoldingViewContact& aParentContact) :
    iParentContact(aParentContact),
    iContactField(aParentContact)
    {
    }

MVPbkBaseContact& TFoldingViewFieldCollection::ParentContact() const
    {
    return iParentContact;
    }

TInt TFoldingViewFieldCollection::FieldCount() const
    {
    // The only field is the last name field that contains name of the contact view
    return 1;
    }

const MVPbkBaseContactField& TFoldingViewFieldCollection::FieldAt(TInt /*aIndex*/) const
    {
    return iContactField;
    }


// CVPbkFoldingViewContact
CVPbkFoldingViewContact::CVPbkFoldingViewContact(CVPbkFoldingContactView& aParentView) :
    iParentView(aParentView),
    iFields(*this)
    {
    }

CVPbkFoldingViewContact* CVPbkFoldingViewContact::NewL(CVPbkFoldingContactView& aParentView)
    {
    CVPbkFoldingViewContact* self = new(ELeave) CVPbkFoldingViewContact(aParentView);
    return self;
    }

const MVPbkBaseContactFieldCollection& CVPbkFoldingViewContact::Fields() const
    {
    return iFields;
    }

TBool CVPbkFoldingViewContact::IsSame(
        const MVPbkStoreContact& /*aOtherContact*/) const
    {
    // A folding contact can not be a store contact
    return EFalse;
    }

TBool CVPbkFoldingViewContact::IsSame(const MVPbkViewContact& aOtherContact) const
    {
    const MVPbkViewContact* thisCnt = this;
    return thisCnt == &aOtherContact;
    }

TBool CVPbkFoldingViewContact::IsSame(
        const MVPbkViewContact& /*aOtherContact*/, 
        const MVPbkContactStore* /*aContactStore*/) const
    {
    return EFalse;
    }

TBool CVPbkFoldingViewContact::IsSame(
        const MVPbkStoreContact& /*aOtherContact*/, 
        const MVPbkContactStore* /*aContactStore*/) const
    {
    return EFalse;
    }

TBool CVPbkFoldingViewContact::MatchContactStore(
        const TDesC& aContactStoreUri) const
    {
    return iParentView.MatchContactStore(aContactStoreUri);
    }

TBool CVPbkFoldingViewContact::MatchContactStoreDomain(
        const TDesC& aContactStoreDomain) const
    {
    return iParentView.MatchContactStoreDomain(aContactStoreDomain);
    }

MVPbkContactBookmark* CVPbkFoldingViewContact::CreateBookmarkLC() const
    {
    TVPbkFoldingContactBookmark* bookMark = 
        new(ELeave) TVPbkFoldingContactBookmark(*this);
    CleanupStack::PushL(bookMark);
    return bookMark;
    }
        
MVPbkContactLink* CVPbkFoldingViewContact::CreateLinkLC() const
    {
    return NULL;
    }

void CVPbkFoldingViewContact::DeleteL(MVPbkContactObserver& /*aObserver*/) const
    {
    User::Leave(KErrNotSupported);
    }

MVPbkContactViewBase& CVPbkFoldingViewContact::ParentView() const
    {
    return iParentView;
    }

void CVPbkFoldingViewContact::ReadL(MVPbkContactObserver& /*aObserver*/) const
    {
    User::Leave(KErrNotSupported);
    }

void CVPbkFoldingViewContact::ReadAndLockL(MVPbkContactObserver& /*aObserver*/) const
    {
    User::Leave(KErrNotSupported);
    }

MVPbkExpandable* CVPbkFoldingViewContact::Expandable() const
    {
    return &iParentView;
    }
//End of file
