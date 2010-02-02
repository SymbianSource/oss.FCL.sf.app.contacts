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


#ifndef CVPBKFOLDINGVIEWCONTACT_H
#define CVPBKFOLDINGVIEWCONTACT_H

#include <e32base.h>
#include <MVPbkViewContact.h>
#include <MVPbkViewContactFieldCollection.h>
#include <MVPbkViewContactField.h>
#include <MVPbkContactFieldTextData.h>
#include <bamdesca.h>

class CVPbkFoldingContactView;
class CVPbkFoldingViewContact;
class MVPbkContactViewObserver;

NONSHARABLE_CLASS(TFoldingViewContactTextFieldData) 
        : public MVPbkContactFieldTextData
    {
    public:
        TFoldingViewContactTextFieldData(CVPbkFoldingViewContact& aParentContact);

    private: // From MVPbkContactFieldTextData
        TBool IsEmpty() const;
        void CopyL(const MVPbkContactFieldData& aFieldData);
        TPtrC Text() const;
        void SetTextL(const TDesC& aText);
        TInt MaxLength() const;

    private: // Data
        CVPbkFoldingViewContact& iParentContact;
    };

NONSHARABLE_CLASS(TFoldingViewContactField) : public MVPbkViewContactField
    {
    public: // Constructor
        TFoldingViewContactField(CVPbkFoldingViewContact& aParentContact);

    public:  // From MVPbkObjectHierarchy
        MVPbkObjectHierarchy& ParentObject() const;

    public:  // From MVPbkViewContactField
        MVPbkBaseContact& ParentContact() const;
        const MVPbkFieldType* MatchFieldType(TInt aMatchPriority) const;
        const MVPbkFieldType* BestMatchingFieldType() const;
        const MVPbkContactFieldData& FieldData() const;
        TBool IsSame(const MVPbkBaseContactField& aOther) const;

    private: // Data
        /// Ref: Parent contact
        CVPbkFoldingViewContact& iParentContact;
        TFoldingViewContactTextFieldData iFieldData;
        /// Field type
        mutable const MVPbkFieldType* iFieldType;
    };

NONSHARABLE_CLASS(TFoldingViewFieldCollection) 
        : public MVPbkViewContactFieldCollection
    {
    public: // Construction
        TFoldingViewFieldCollection(CVPbkFoldingViewContact& aParentContact);

    public: // From MVPbkViewContactFieldCollection
        MVPbkBaseContact& ParentContact() const;
        TInt FieldCount() const;
        const MVPbkBaseContactField& FieldAt(TInt aIndex) const;
        
    private: // Data
        /// Ref: Parent contact
        CVPbkFoldingViewContact& iParentContact;

        TFoldingViewContactField iContactField;
    };

NONSHARABLE_CLASS(CVPbkFoldingViewContact) : public CBase,
                                public MVPbkViewContact
    {
    public:
        static CVPbkFoldingViewContact* NewL(CVPbkFoldingContactView& aParentView);

    public: // Interface
        CVPbkFoldingContactView& NativeParentView() const;

    public: // From MVPbkViewContact
        const MVPbkBaseContactFieldCollection& Fields() const;
        TBool IsSame(const MVPbkStoreContact& aOtherContact) const;
        TBool IsSame(const MVPbkViewContact& aOtherContact) const;
        MVPbkContactLink* CreateLinkLC() const;
        void DeleteL(MVPbkContactObserver& aObserver) const;
        MVPbkContactViewBase& ParentView() const;
        void ReadL(MVPbkContactObserver& aObserver) const;
        void ReadAndLockL(MVPbkContactObserver& aObserver) const;
        MVPbkExpandable* Expandable() const;
        TBool IsSame(const MVPbkViewContact& aOtherContact, 
                     const MVPbkContactStore* aContactStore) const;
        TBool IsSame(const MVPbkStoreContact& aOtherContact, 
                     const MVPbkContactStore* aContactStore) const;
        TBool MatchContactStore(const TDesC& aContactStoreUri) const;
        TBool MatchContactStoreDomain(const TDesC& aContactStoreDomain) const;
        MVPbkContactBookmark* CreateBookmarkLC() const;        
        
    private: // Implementation
        CVPbkFoldingViewContact(CVPbkFoldingContactView& aParentView);

    private: // Data
        /// Ref: Parent view of this contact
        CVPbkFoldingContactView& iParentView;
        TFoldingViewFieldCollection iFields;
    };

inline CVPbkFoldingContactView& CVPbkFoldingViewContact::NativeParentView() const
    {
    return iParentView;
    }

#endif // CVPBKFOLDINGVIEWCONTACT_H
//End of file


