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
* Description:  A virtual phonebook view contact implementation
*
*/



#ifndef VPBKSIMSTORE_CVIEWCONTACT_H
#define VPBKSIMSTORE_CVIEWCONTACT_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkViewContact.h>
#include <MVPbkViewContactFieldCollection.h>
#include <MVPbkViewContactField.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactObserver.h>
#include <MVPbkSimContactObserver.h>

// FORWARD DECLARATIONS
class MVPbkSimContact;
class MVPbkSimStoreOperation;

namespace VPbkEngUtils {
class CVPbkAsyncOperation;    
}

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CViewContact;
class CContactView;

// CLASS DECLARATION

/**
* A simple field data class for the view contact
*
*/
NONSHARABLE_CLASS( TViewContactFieldData ): 
        public MVPbkContactFieldTextData
    {
    public:  // Constructors and destructor

        /**
        * C++ constructor
        * @param aData the field data
        */
        TViewContactFieldData( const TDesC& aData );

    public: // Functions from base classes

        /**
        * From MVPbkContactFieldData
        */
        TBool IsEmpty() const;

        /**
        * From MVPbkContactFieldData
        */
        void CopyL( const MVPbkContactFieldData& aFieldData );

        /**
        * From MVPbkContactFieldTextData
        */
        TPtrC Text() const;

        /**
        * From MVPbkContactFieldTextData
        */
        void SetTextL( const TDesC& aText );

        /**
        * From MVPbkContactFieldTextData
        */
        TInt MaxLength() const;

    private:    // Data
        /// The field data
        TPtrC iData;
    };

/**
* A simple view contact field class for the view contact
*
*/
NONSHARABLE_CLASS( TViewContactField ): 
        public MVPbkViewContactField
    {
    public:  // Constructors and destructor

        /**
        * C++ constructor
        * @param aParentContact the parent view contact of this  field
        * @param aFieldType the field type of this field
        */
        TViewContactField( CViewContact& aParentContact,
            const MVPbkFieldType& aFieldType,
            const TDesC& aData );

    public: // New functions

        /**
        * Returns the field data
        */
        TViewContactFieldData& FieldData() { return iFieldData; }

    public: // Functions from base classes

        /**
        * From MVPbkObjectHierarchy
        */
        MVPbkObjectHierarchy& ParentObject() const;

        /**
        * From MVPbkBaseContactField
        */
        MVPbkBaseContact& ParentContact() const;

        /**
        * From MVPbkBaseContactField
        */
        const MVPbkFieldType* MatchFieldType( TInt aMatchPriority ) const;
        
        /**
        * From MVPbkBaseContactField
        */
        const MVPbkFieldType* BestMatchingFieldType() const;

        /**
        * From MVPbkBaseContactField
        */
        const MVPbkContactFieldData& FieldData() const;

        /**
        * From MVPbkBaseContactField
        */
        TBool IsSame(const MVPbkBaseContactField& aOther) const;

    private:    // Data
        /// The parent contact
        CViewContact& iParentContact;
        /// The field type of the field
        const MVPbkFieldType& iFieldType;
        /// The data of the field
        TViewContactFieldData iFieldData;
    };

/**
* A simple view contact field collection class for the view contact
*
*/
NONSHARABLE_CLASS( CViewContactFieldCollection ): 
        public CBase,
        public MVPbkViewContactFieldCollection
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CViewContactFieldCollection();
        
    public: // New functions

        /**
        * Sets the parent contact
        * @param aParentContact the contact that owns field collection
        */
        void SetParentContact( CViewContact& aParentContact );

        /**
        * Resets field collection
        */
        void ResetFields();

        /**
        * Appends a new field to the field collection
        * @param aNewField the new view contact field
        */
        void AppendFieldL( TViewContactField& aNewField );

    public: // Functions from base classes

        /**
        * From MVPbkViewContactFieldCollection
        */
        MVPbkBaseContact& ParentContact() const;

        /**
        * From MVPbkViewContactFieldCollection
        */
        TInt FieldCount() const;

        /**
        * From MVPbkViewContactFieldCollection
        */
        const MVPbkBaseContactField& FieldAt( TInt aIndex ) const;
    private:    // Data
        /// Ref: the parent contact of the collection
        CViewContact* iParentContact;
        /// An array of fields
        RArray<TViewContactField> iFields;
    };

/**
*  A virtual phonebook view contact implementation
*
*/
NONSHARABLE_CLASS( CViewContact ): 
        public CBase,
        public MVPbkViewContact,
        private MVPbkSimContactObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param the parent view of the contact
        * @param aSortOrder the sort order of the view
        * @return a new instance of this class
        */
        static CViewContact* NewL( CContactView& aView,
            const MVPbkFieldTypeList& aSortOrder );
        
        /**
        * Destructor.
        */
        virtual ~CViewContact();

    public: // New functions
        
        /**
        * Sets the sim contact for this view contact
        * @param aSimContact the sim contact for the view contact
        */
        void SetSimContactL( MVPbkSimContact& aSimContact );

        /**
        * Returns the sim index of this contact
        * @return the sim index of this contact
        */
        TInt SimIndex() const;

        /**
        * Set field sort order for the view contact
        */
        void SetSortOrder( const MVPbkFieldTypeList& aSortOrder );

        /**
         * Returns a reference to the native contact if set by
         * SetSimContactL. Otherwise returns NULL
         */
        const MVPbkSimContact* NativeContact() const;
        
        /**
         * Returns the view that this contact belongs.
         */
        CContactView& View() const;

    public: // Functions from base classes
            
        /**
        * From MVPbkBaseContact
        */ 
        const MVPbkBaseContactFieldCollection& Fields() const;

        /**
        * From MVPbkBaseContact
        */ 
        TBool IsSame( const MVPbkStoreContact& aOtherContact ) const;

        /**
        * From MVPbkBaseContact
        */ 
        TBool IsSame( const MVPbkViewContact& aOtherContact ) const;

        /**
        * From MVPbkBaseContact
        */ 
        MVPbkContactLink* CreateLinkLC() const;

        /**
        * From MVPbkBaseContact
        */ 
        void DeleteL(MVPbkContactObserver& aObserver) const;
        
        /**
        * From MVPbkBaseContact
        */
        TBool MatchContactStore(const TDesC& aContactStoreUri) const;
        
        /**
        * From MVPbkBaseContact
        */
        TBool MatchContactStoreDomain(const TDesC& aContactStoreDomain) const;
        
        /**
        * From MVPbkBaseContact
        */
        MVPbkContactBookmark* CreateBookmarkLC() const;
        
        /**
        * From MVPbkViewContact
        */
        MVPbkContactViewBase& ParentView() const;

        /**
        * From MVPbkViewContact
        */
        void ReadL(MVPbkContactObserver& aObserver) const;

        /**
        * From MVPbkViewContact
        */
        void ReadAndLockL(MVPbkContactObserver& aObserver) const;

        /**
        * From MVPbkViewContact
        */
        MVPbkExpandable* Expandable() const;

        /**
        * From MVPbkViewContact
        */
        TBool IsSame(const MVPbkViewContact& aOtherContact, 
                     const MVPbkContactStore* aContactStore) const;

        /**
        * From MVPbkViewContact
        */
        TBool IsSame(const MVPbkStoreContact& aOtherContact, 
                     const MVPbkContactStore* aContactStore) const;
        
    private: // Construction

        /**
        * C++ constructor.
        */
        CViewContact( CContactView& aView, 
            const MVPbkFieldTypeList& aSortOrder );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    private:    // Functions from base classes
        
        /**
        * From MVPbkViewContact
        */
        void ContactEventComplete( TEvent aEvent, 
            CVPbkSimContact* aContact );

        /**
        * From MVPbkViewContact
        */
        void ContactEventError( TEvent aEvent, 
            CVPbkSimContact* aContact, TInt aError );
            
    private:    // New functions
        void CreateReadCallbackL( MVPbkContactObserver& aObserver,
                MVPbkContactObserver::TContactOpResult& aOpResult ) const;
    private:    // Data
        /// The parent view of the contact
        CContactView& iView;
        /// Field collection
        CViewContactFieldCollection iFieldCollection;
        /// The view sort order
        const MVPbkFieldTypeList* iSortOrder;
        /// Not Own: the native sim contact set by SetSimContactL
        MVPbkSimContact* iSimContact;
        /// Own: an asynchronous call back that is needed if observer
        /// must be notified e.g from ReadL
        VPbkEngUtils::CVPbkAsyncOperation* iAsyncOp;
        /// Ref: an observer of DeleteL
        mutable MVPbkContactObserver* iObserver;
        /// Own: an async operation handle.
        mutable MVPbkSimStoreOperation* iStoreOperation;
    };

} // namespace VPbkSimStore
#endif      // VPBKSIMSTORE_CVIEWCONTACT_H
            
// End of File
