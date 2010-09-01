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


#ifndef CVPBKCONTACTFIELDCOLLECTION_H
#define CVPBKCONTACTFIELDCOLLECTION_H

#include <e32base.h>
#include <MVPbkStoreContactFieldCollection.h>

class MVPbkStoreContactField;
class MVPbkStoreContact;
class TVPbkContactFieldIterator;

/**
 * Virtual Phonebook contact field collection.
 */
class CVPbkContactFieldCollection : public CBase,
        public MVPbkStoreContactFieldCollection
    {
    public: // Construction
        IMPORT_C static CVPbkContactFieldCollection* NewLC(MVPbkStoreContact& aParentContact);
        ~CVPbkContactFieldCollection();

    public: // Interface
        IMPORT_C void AppendL(MVPbkStoreContactField* aField);

    public: // From MVPbkStoreContactFieldCollection
        MVPbkBaseContact& ParentContact() const;
        MVPbkStoreContactField& FieldAt(TInt aIndex);
        TInt FieldCount() const;
        const MVPbkStoreContactField& FieldAt(TInt aIndex) const;
        MVPbkStoreContactField* FieldAtLC(TInt aIndex) const;
        MVPbkStoreContact& ParentStoreContact() const;
        MVPbkStoreContactField* RetrieveField(
            const MVPbkContactLink& aContactLink) const;

    private:
        CVPbkContactFieldCollection(MVPbkStoreContact& aParentContact);
        
    private: // Data
        // Not owned: Contact which fields are accessed through this interface
        MVPbkStoreContact& iParentContact;
        // Owned: Array of fields
        RPointerArray<MVPbkStoreContactField> iFields;
    };

#endif // CVPBKCONTACTFIELDCOLLECTION_H
// End of file
