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


#ifndef VPBKCNTMODEL_CVIEWCONTACT_H
#define VPBKCNTMODEL_CVIEWCONTACT_H

// INCLUDES
#include <e32base.h>
#include <MVPbkViewContactFieldCollection.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkViewContactField.h>
#include <MVPbkViewContact.h>
#include <MVPbkExpandable.h>
#include <cntdef.h>

// FORWARD DECLARATIONS
class CViewContact;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CViewBase;
class CViewContact; // Declared below

/**
 * View contact text field data.
 */
NONSHARABLE_CLASS( TViewContactTextFieldData ): 
        public MVPbkContactFieldTextData
    {
    public: // Constructor

        /**
         * Constructor.
         *
         * @param aTextData     The field data.
         */
        TViewContactTextFieldData(
                const TDesC& aTextData );
        
    public: // Interface

        /**
         * Sets field data.
         *
         * @param aTextData     The data to set.
         */
        void SetTextData(
                const TDesC& aTextData );

    private: // From MVPbkContactFieldTextData
        TBool IsEmpty() const;
        void CopyL(
                const MVPbkContactFieldData& aFieldData );
        TPtrC Text() const;
        void SetTextL(
                const TDesC& aText );
        TInt MaxLength() const;

    private: // Data
        /// Own: Pointer to the data            
        TPtrC iTextData;
    };

/**
 * View contact field.
 */
NONSHARABLE_CLASS(TViewContactField) : public MVPbkViewContactField
    {
    public: // Constructor

        /**
         * Constructor.
         *
         * @param aParentContact    Parent view contact.
         * @param aFieldData        Data of the field.
         * @param aFieldType        Field type.
         * @param aFieldIndex       The field index of contact model field
         */
        TViewContactField(
                CViewContact& aParentContact, 
                const TDesC& aFieldData, 
                const MVPbkFieldType& aFieldType,
                TInt aFieldIndex );

    public: // Interface

        /**
         * Sets field data.
         *
         * @param aFieldData    The field data to set.
         */
        void SetFieldData(
                const TDesC& aFieldData );

    public: // From MVPbkViewContactField
        MVPbkBaseContact& ParentContact() const;
        const MVPbkFieldType* MatchFieldType(
                TInt aMatchPriority ) const;
        const MVPbkFieldType* BestMatchingFieldType() const;
        const MVPbkContactFieldData& FieldData() const;
        TBool IsSame(
                const MVPbkBaseContactField& aOther ) const;

    private: // Data
        /// Ref: Parent contact
        CViewContact& iParentContact;
        /// Own: Field data
        TViewContactTextFieldData iFieldData;
        /// Ref: Field type
        const MVPbkFieldType* iFieldType;
        /// Own: a field index of contact model field
        TInt iFieldIndex;
    };

/**
 * View contact field collection.
 */
NONSHARABLE_CLASS(CViewContactFieldCollection) :
            public CBase,
            public MVPbkViewContactFieldCollection
    {
    public: // Constructor and destructor

        /**
         * Constructor.
         *
         * @param aParentContact    Parent contact.
         */
        CViewContactFieldCollection(
                CViewContact& aParentContact );
        /**
         * Destructor.
         */
        ~CViewContactFieldCollection();

    public: // Interface for CViewContact
        void SetFieldsData(
                const ::CViewContact& aViewContact );
        void SetTypeListL(
                const MVPbkFieldTypeList& aFieldTypes );

    public: // From MVPbkViewContactFieldCollection
        MVPbkBaseContact& ParentContact() const;
        TInt FieldCount() const;
        const MVPbkBaseContactField& FieldAt(
                TInt aIndex ) const;
        
    private: // Data
        /// Ref: Parent contact
        CViewContact& iParentContact;
        /// Own: Current field
        RArray<TViewContactField> iFields;
    };

/**
 * Thin wrapper for native CViewContact.
 */
NONSHARABLE_CLASS(CViewContact) : public CBase,
                                  public MVPbkViewContact,
                                  public MVPbkExpandable,
                                  public MVPbkBaseContact2
    {
    public: // Constructor

        /**
         * Creates a new instance of this class.
         *
         * @param aParentView   The parent view.
         * @param aFieldTypes   Field types.
         * @return  A new instance of this class.
         */
        static CViewContact* NewL(
                CViewBase& aParentView,
                const MVPbkFieldTypeList& aFieldTypes);

    public: // Interface

        /**
         * Sets the field type list that will be used as types
         * of the fields in contact field collection.
         *
         * @param aFieldTypes   List of types for contact field collection.
         */
        void SetTypeListL(
                const MVPbkFieldTypeList& aFieldTypes );

        /**
         * Sets the native view contact that this vpbk view contact maps to.
         *
         * @param aViewContact  Native view contact that this Virtual
         *                      Phonebook  view contact maps to.
         */
        void SetViewContact(const ::CViewContact& aViewContact);

        /**
         * Returns the parent view of this contact.
         *
         * @return  Parent view.
         */
        CViewBase& View() const;

        /**
         * Returns the contact item id of the underlying native view contact.
         *
         * @return  Native contact item id.
         */
        TContactItemId Id() const;

        /**
         * Returns the native contact.
         *
         * @return  Native contact item id.
         */
        const ::CViewContact* NativeContact() const;
        
    public: // From MVPbkBaseContact
        const MVPbkBaseContactFieldCollection& Fields() const;
        TBool IsSame(
                const MVPbkStoreContact& aOtherContact ) const;
        TBool IsSame(
                const MVPbkViewContact& aOtherContact ) const;
        MVPbkContactLink* CreateLinkLC() const;
        void ReadL(
                MVPbkContactObserver& aObserver ) const;
        void ReadAndLockL(
                MVPbkContactObserver& aObserver ) const;
        MVPbkExpandable* Expandable() const;
        void DeleteL(
                MVPbkContactObserver& aObserver ) const;
        TBool MatchContactStore(
                const TDesC& aContactStoreUri ) const;
        TBool MatchContactStoreDomain(
                const TDesC& aContactStoreDomain ) const;
        MVPbkContactBookmark* CreateBookmarkLC() const;
        TAny* BaseContactExtension( TUid aExtensionUid );
        
    public:  // from MVPbkBaseContact2
        TBool IsOwnContact( TInt& aError ) const;

    public:  // From MVPbkViewContact
        MVPbkContactViewBase& ParentView() const;
        TBool IsSame(
                const MVPbkViewContact& aOtherContact, 
                const MVPbkContactStore* aContactStore ) const;
        TBool IsSame(
                const MVPbkStoreContact& aOtherContact, 
                const MVPbkContactStore* aContactStore ) const;

    public: // From MVPbkExpandable
        MVPbkContactViewBase* ExpandLC(
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder ) const;

    private: // Implementation
        CViewContact(
                CViewBase& aParentView );

    private: // Data
        /// Ref: Parent view
        CViewBase& iParentView;
        /// Ref: Native view contact
        const ::CViewContact* iViewContact;
        /// Ref: Field collection
        CViewContactFieldCollection iFieldCollection;
    };

} // namespace VPbkCntModel

#endif // VPBKCNTMODEL_CVIEWCONTACT_H

// End of File
