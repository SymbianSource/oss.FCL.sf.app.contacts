/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Maps a Contact Model field collection to a virtual Phonebook 
                 field collection.
*
*/


// INCLUDES
#include "TContactFieldCollection.h"
#include <cntitem.h>
#include <VPbkError.h>
#include "TContactField.h"
#include "CContact.h"
#include "CContactLink.h"

namespace VPbkCntModel {

TContactFieldCollection::TContactFieldCollection
        (CContact& aParentContact, CContactItemFieldSet& aFieldSet) :
    iFieldSet(&aFieldSet),
    iCurrentField(aParentContact),
    iParentContact(&aParentContact)
    {
    }

TContactFieldCollection::~TContactFieldCollection()
    {
    }

void TContactFieldCollection::SetContact
        (CContact& aParentContact, CContactItemFieldSet& aFieldSet)
    {
    iFieldSet = &aFieldSet;
    iCurrentField.SetParentContact(aParentContact);
    iParentContact = &aParentContact;
    }

MVPbkBaseContact& TContactFieldCollection::ParentContact() const
    {
    return *iParentContact;
    }

TInt TContactFieldCollection::FieldCount() const
    {
    return iFieldSet->Count();
    }

const MVPbkStoreContactField& TContactFieldCollection::FieldAt(TInt aIndex) const
    {
    __ASSERT_ALWAYS(aIndex >= 0 && aIndex < iFieldSet->Count(), 
        VPbkError::Panic(VPbkError::EInvalidFieldIndex));
    iCurrentField.SetField((*iFieldSet)[aIndex]);
    return iCurrentField;
    }

MVPbkStoreContactField& TContactFieldCollection::FieldAt(TInt aIndex)
    {
    __ASSERT_ALWAYS(aIndex >= 0 && aIndex < iFieldSet->Count(), 
        VPbkError::Panic(VPbkError::EInvalidFieldIndex));
    iCurrentField.SetField((*iFieldSet)[aIndex]);
    return iCurrentField;
    }

MVPbkStoreContactField* TContactFieldCollection::FieldAtLC(TInt aIndex) const
    {
    __ASSERT_ALWAYS(aIndex >= 0 && aIndex < iFieldSet->Count(), 
        VPbkError::Panic(VPbkError::EInvalidFieldIndex));
    TContactField* clone =
        new (ELeave) TContactField(*iParentContact);
    clone->SetField((*iFieldSet)[aIndex]);
    CleanupDeletePushL(clone);
    return clone;
    }

MVPbkStoreContact& TContactFieldCollection::ParentStoreContact() const
    {
    return *iParentContact;
    }
    
MVPbkStoreContactField* TContactFieldCollection::RetrieveField(
        const MVPbkContactLink& aContactLink) const
    {
    __ASSERT_ALWAYS(aContactLink.RefersTo(*iParentContact), 
                    VPbkError::Panic(VPbkError::EInvalidContactField));

    MVPbkStoreContactField* result = NULL;

    if (&aContactLink.ContactStore() == &iParentContact->ContactStore())
        {
        const CContactLink& link = static_cast<const CContactLink&>(aContactLink);
        const TInt fieldCount = FieldCount();
        for (TInt i = 0; i < fieldCount; ++i)
            {
            if ((*iFieldSet)[i].Id() == link.ContactFieldId())
                {
                iCurrentField.SetField((*iFieldSet)[i]);
                result = &iCurrentField;
                }
            }
        }

    return result;
    }
} // namespace VPbkCntModel

