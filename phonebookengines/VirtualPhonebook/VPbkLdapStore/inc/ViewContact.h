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
* Description:  CViewContact - LDAP store view contact.
*
*/


#ifndef __VIEWCONTACT_H__
#define __VIEWCONTACT_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <mvpbkviewcontact.h>
#include <mvpbkviewcontactfieldcollection.h>
#include <mvpbkviewcontactfield.h>
#include <mvpbkcontactfieldtextdata.h>
#include <mvpbkcontactobserver.h>
#include <mvpbkfieldtype.h>
#include <mvpbkexpandable.h>

// FORWARD DECLARATIONS
class CVPbkContactViewDefinition;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// FORWARD DECLARATIONS
class CViewContact;
class CContactView;
class CContact;

// CLASS DECLARATIONS

/**
* TViewContactFieldData - contact field data class
*
*/
class TViewContactFieldData : public MVPbkContactFieldTextData
    {
    public:     // TViewContactFieldData constructor

        /**
        @function   TViewContactFieldData
        @discussion TViewContactFieldData constructor
        @param      aData Field data
        */
        TViewContactFieldData(const TDesC& aData);

    public:     // Methods from MVPbkContactFieldData

        /**
        @function   IsEmpty
        @discussion Returns true if the storage is empty.
        @return     ETrue, data is empty
        */
        TBool IsEmpty() const;

        /**
        @function   CopyL
        @discussion Returns true if the storage is empty.
        @param      aFieldData Data to copy
        */
        void CopyL(const MVPbkContactFieldData& aFieldData);

    public:     // Methods from MVPbkContactFieldTextData

        /**
        @function   Text
        @discussion Returns the string data.
        @return     String data
        */
        TPtrC Text() const;

        /**
        @function   SetTextL
        @discussion Sets the string data.
        */
        void SetTextL(const TDesC& aText);

        /**
        @function   MaxLength
        @discussion Returns maximum length of the field.
        @return     Maximum length of the field.
        */
        TInt MaxLength() const;

    private:    // TViewContactFieldData private members

        /// The field data
        TPtrC iData;
    };

/**
* TViewContactField -  view contact field class.
*
*/
class TViewContactField : public MVPbkViewContactField
    {
    public:     // TViewContactField constructors

        /**
        @function   TViewContactField
        @discussion TViewContactField constructor
        @param      aParentContact Parent contact
        @param      aFieldType Field type
        @param      aData      Field data
        */
        TViewContactField(CViewContact& aParentContact,
            const MVPbkFieldType& aFieldType,const TDesC& aData);

    public:     // TViewContactField public method

        /**
        @function   MatchFieldType
        @discussion Returns the field data.
        @return     Field data
        */
        TViewContactFieldData& FieldData();

    public:     // Methods from MVPbkBaseContactField

        /**
        @function   ParentContact
        @discussion Returns the parent contact where this field is from.
        @return     Parent contact.
        */
        MVPbkBaseContact& ParentContact() const;

        /**
        @function   MatchFieldType
        @discussion Returns this field's type or null.
        @param      aMatchPriority Matching priority to use.
        @return     Field type or null.
        */
        const MVPbkFieldType* MatchFieldType(TInt aMatchPriority) const;

        /**
        @function   BestMatchingFieldType
        @discussion Returns this field's type or null.
        @return     Field type or null.
        */
        const MVPbkFieldType* BestMatchingFieldType() const;

        /**
        @function   MatchFieldType
        @discussion Returns the field data.
        @return     Field data
        */
        const MVPbkContactFieldData& FieldData() const;

        /**
        @function   IsSame
        @discussion Returns the field data.
        @param      aOther Contact field to compare
        @return     ETrue, same field
        */
        TBool IsSame(const MVPbkBaseContactField& aOther) const;

    private:    // TViewContactField private members

        /// The parent contact
        CViewContact& iParentContact;

        /// The field type of the field
        const MVPbkFieldType& iFieldType;

        /// The data of the field
        TViewContactFieldData iFieldData;
    };

/**
*  CViewContactFieldCollection - view contact field collection class.
*
*/
class CViewContactFieldCollection : public CBase,
                                    public MVPbkViewContactFieldCollection
    {
    public:     // CViewContactFieldCollection constructor and destructor

        /**
        @function   CViewContactFieldCollection
        @discussion CViewContactFieldCollection constructor
        @param      aParentContact Parent contact that owns the collection.
        */
        CViewContactFieldCollection(CViewContact& aParentContact);

        /**
        @function  ~CViewContactFieldCollection
        @discussion CViewContactFieldCollection destructor
        */
        virtual ~CViewContactFieldCollection();
        
    public:     // CViewContactFieldCollection public methods

        /**
        @function   ResetFields
        @discussion Resets field collection
        */
        void ResetFields();

        /**
        @function   AppendFieldL
        @discussion Appends a new field to the field collection
        @param      aNewField the new view contact field
        */
        void AppendFieldL(TViewContactField& aNewField);

    public:     // Functions from MVPbkBaseContactFieldCollection

        /**
        @function   ParentContact
        @discussion Returns the parent contact of this field set.
        @return     Parent contact of this field set
        */
        MVPbkBaseContact& ParentContact() const;

        /**
        @function   FieldCount
        @discussion Returns the number of fields in this collection.
        @return     This collection field count
        */
        TInt FieldCount() const;

        /**
        @function   FieldAt
        @discussion Returns a read-only field in this collection.
        @parama     aIndex Index of field
        @return     Contact field at index
        */
        const MVPbkBaseContactField& FieldAt(TInt aIndex) const;

    private:    // CViewContactFieldCollection private members

        /// Parent contact of the collection
        CViewContact& iParentContact;

        /// An array of fields
        RArray<TViewContactField> iFields;
    };

/**
*  CViewContact - ldap store view contact.
*
*/
class CViewContact : public CBase,
                     public MVPbkViewContact,
                     public MVPbkExpandable
    {
    public:     // CViewContact public constructors and destructor

        /**
        @function   NewL
        @discussion Create CViewContact object
        @return     Pointer to instance of CViewContact
        */
        static CViewContact* NewL(CContactView& aView,
                const MVPbkFieldTypeList& aSortOrder);

        /**
        @function   NewLC
        @discussion Create CViewContact object
        @return     Pointer to instance of CViewContact
        */
        static CViewContact* NewLC(CContactView& aView,
                const MVPbkFieldTypeList& aSortOrder);

        /**
        @function  ~CViewContact
        @discussion CViewContact destructor
        */
        ~CViewContact();

    public:     // CViewContact public methods

        /**
        @function   ContactIndex
        @discussion Return this view contact index.
        @return     Contact index.
        */
        TInt ContactIndex() const;

        /**
        @function   ContactView
        @discussion Return contact view of this view contact.
        @return     Contact view.
        */
        CContactView& ContactView() const;

        /**
        @function   SetContactViewContact
        @discussion Sets view contact.
        @param      aContact Contact to set
        */
        void SetContactViewContact(CContact& aContact);

        /**
        @function   SetFoldingViewContact
        @discussion Sets folding view contact.
        @param      aName Folding view name, store name
        */
        void SetFoldingViewContact(TPtrC aName);

    public:     // Methods from MVPbkBaseContact

        /**
        @function   Fields
        @discussion Returns this contact's fields (read only).
        */
        const MVPbkBaseContactFieldCollection& Fields() const;

        /**
        @function   IsSame
        @discussion Checks whether passed contact is same as this.
        @return     ETrue is the same contact.
        */
        TBool IsSame(const MVPbkStoreContact& aOtherContact) const;

        /**
        @function   IsSame
        @discussion Checks whether passed contact is same as this.
        @return     ETrue is the same contact.
        */
        TBool IsSame(const MVPbkViewContact& aOtherContact) const;

        /**
        @function   CreateLinkLC
        @discussion Creates link representing this contact.
        */
        MVPbkContactLink* CreateLinkLC() const;

        /**
        @function   MatchContactStore
        @discussion Deletes this contact from store asynchronously.
        @param      aObserver Observer to notify when operation completes.
        */
        void DeleteL(MVPbkContactObserver& aObserver) const;

        /**
        @function   MatchContactStore
        @discussion Checks whether this view is from given store.
        @param      aContactStoreUri the URI of the store to compare
        @return     ETrue if the view was from the given store
        */
        TBool MatchContactStore(const TDesC& aContactStoreUri) const;

        /**
        @function   MatchContactStoreDomain
        @discussion Checks whether this view is from given store domain.
        @param      aContactStoreDomain the the store domain
        @return     ETrue if the view was from the same store domain.
        */
        TBool MatchContactStoreDomain(const TDesC& aContactStoreDomain) const;

        /**
        @function   CreateBookmarkLC
        @discussion Creates and returns bookmark to the item at given index.
        @return     New bookmark to the view item.
        */
        MVPbkContactBookmark* CreateBookmarkLC() const;

    public:     // Methods from MVPbkViewContact

        /**
        @function   ParentView
        @discussion Returns this contact's parent view.
        @return     Parent view.
        */
        MVPbkContactViewBase& ParentView() const;

        /**
        @function   ReadL
        @discussion Reads this contact from store asynchronously.
        @param      aObserver Observer to notify when operation completes.
        */
        void ReadL(MVPbkContactObserver& aObserver) const;

        /**
        @function   ReadL
        @discussion Reads and locks this contact from store asynchronously.
        @param      aObserver Observer to notify when operation completes.
        */
        void ReadAndLockL(MVPbkContactObserver& aObserver) const;

        /**
        @function   Expandable
        @discussion Returns expandable interface for this object
        @return     Expandable or null.
        */
        MVPbkExpandable* Expandable() const;

        /**
        @function   IsSame
        @discussion Checks whether passed contact is the same as this.
        @return     ETrue is the same contact.
        */
        TBool IsSame(const MVPbkViewContact&  aOtherContact, 
                     const MVPbkContactStore* aContactStore) const;
        TBool IsSame(const MVPbkStoreContact& aOtherContact, 
                     const MVPbkContactStore* aContactStore) const;

    public:     // Methods from MVPbkExpandable

        /**
        @function   ExpandLC
        @discussion Expands this view
        @param      aObserver  View observer
        @param      aSortOrder View sort order
        @return     Expanded contact view
        */
        MVPbkContactViewBase* ExpandLC(MVPbkContactViewObserver& aObserver,
                                const MVPbkFieldTypeList& aSortOrder) const;

    private:    // CViewContact private constructors

        /**
        @function   CViewContact
        @discussion CViewContact default contructor
        */
        CViewContact(CContactView& aView,
                const MVPbkFieldTypeList& aSortOrder);

        /**
        @function   ConstructL
        @discussion Perform the second stage construction of CViewContact 
        */
        void ConstructL();

    private:    // CViewContact private member variables

        /// Contact index
        TInt iContactIndex;

        /// The parent view of the contact
        CContactView& iView;

        /// Field collection
        CViewContactFieldCollection iFieldCollection;

        /// The view sort order
        const MVPbkFieldTypeList& iSortOrder;

    };

} // End of namespace LDAPStore
#endif // __VIEWCONTACT_H__
