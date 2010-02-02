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
* Description:  Virtual Phonebook contact field collection.
*
*/


#include "CVPbkContactFieldCollection.h"

#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactField.h>

EXPORT_C CVPbkContactFieldCollection* 
CVPbkContactFieldCollection::NewLC(MVPbkStoreContact& aParentContact) 
    {
    CVPbkContactFieldCollection* self = new(ELeave)CVPbkContactFieldCollection(aParentContact);
    CleanupStack::PushL(self);
    return self;
    }

CVPbkContactFieldCollection::CVPbkContactFieldCollection(MVPbkStoreContact& aParentContact) :
        iParentContact(aParentContact)
    {
    }

CVPbkContactFieldCollection::~CVPbkContactFieldCollection()
    {
    iFields.ResetAndDestroy();
    }

EXPORT_C void CVPbkContactFieldCollection::AppendL(MVPbkStoreContactField* aField)
    {
    iFields.AppendL(aField);
    }

MVPbkBaseContact& CVPbkContactFieldCollection::ParentContact() const
    {
    return iParentContact;
    }

MVPbkStoreContactField& CVPbkContactFieldCollection::FieldAt(TInt aIndex)
    {
    return *iFields[aIndex];
    }

TInt CVPbkContactFieldCollection::FieldCount() const
    {
    return iFields.Count();
    }

const MVPbkStoreContactField& CVPbkContactFieldCollection::FieldAt(TInt aIndex) const
    {
    return *iFields[aIndex];
    }

MVPbkStoreContactField* CVPbkContactFieldCollection::FieldAtLC(TInt aIndex) const
    {
    MVPbkStoreContactField* field = iFields[aIndex];
    return field->CloneLC();
    }

MVPbkStoreContact& CVPbkContactFieldCollection::ParentStoreContact() const
    {
    return iParentContact;
    }
    
MVPbkStoreContactField* CVPbkContactFieldCollection::RetrieveField(
            const MVPbkContactLink& aContactLink) const
    {
    return iParentContact.Fields().RetrieveField(aContactLink);
    }

// End of file
