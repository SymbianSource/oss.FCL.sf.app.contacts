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
* Description:  CContact - Contact item class
*
*/


#ifndef __CONTACT_H__
#define __CONTACT_H__

// INCLUDES
#include <e32base.h>
#include "mvpbkstorecontact.h"
#include "tstorecontactfieldcollection.h"

// FORWARD DECLARATIONS
class CLDAPContact;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// FORWARD DECLARATIONS
class CContactStore;

/**
*  CContact - contact item class.
*
*/
class CContact : public CBase, public MVPbkStoreContact
    {
    public:     // CContact public constructors and destructor

        /**
        @function   NewL
        @discussion Create CContact object
        @param      aStore   Store
        @param      aContact Contact
        @param      aOwns    Owns ldap contact
        @return     Pointer to instance of CContactStore
        */
        static CContact* NewL(CContactStore& aStore,
                CLDAPContact* aContact,TBool aOwns = ETrue);

        /**
        @function   NewLC
        @discussion Create CContact object
        @param      aStore   Store
        @param      aContact Contact
        @param      aOwns    Owns ldap contact
        @return     Pointer to instance of CContactStore
        */
        static CContact* NewLC(CContactStore& aStore,
                CLDAPContact* aContact,TBool aOwns = ETrue);

        /**
        @function  ~CContact
        @discussion CContact destructor
        */
        ~CContact();

    public:     // CContact public methods

        /**
        @function   Index
        @discussion Returns contact index.
        @return     Contact index.
        */
        TInt Index() const;
        /**
        @function   SetIndex
        @discussion Sets contact index.
        @param      Contact index.
        */
        void SetIndex(TInt aIndex);

        /**
        @function   Store
        @discussion Returns the store of this contact.
        @param      Store.
        */
        CContactStore& Store() const;

        /**
        @function   LDAPContact
        @discussion Returns the contact inside this contact.
        @param      LDAP Contact.
        */
        CLDAPContact& LDAPContact() const;

        /**
        @function   Contact
        @discussion Returns the contact inside this contact.
        @param      Contact.
        */
        CLDAPContact* Contact() const;

    public:  // From base classes

        /**
        * From MVPbkObjectHierarchy
        */
        MVPbkObjectHierarchy& ParentObject() const;

        /**
        * From MVPbkBaseContact
        */
        const MVPbkStoreContactFieldCollection& Fields() const;
        
        /**
        * From MVPbkBaseContact
        */
        TBool IsSame( const MVPbkStoreContact& aOtherContact ) const;

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
        * From MVPbkStoreContact
        */
        MVPbkContactStore& ParentStore() const;

        /**
        * From MVPbkStoreContact
        */
        MVPbkStoreContactFieldCollection& Fields();
        
        /**
        * From MVPbkStoreContact
        */
        MVPbkStoreContactField* CreateFieldLC( 
            const MVPbkFieldType& aFieldType ) const;
        
        /**
        * From MVPbkStoreContact
        */
        TInt AddFieldL( MVPbkStoreContactField* aField );

        /**
        * From MVPbkStoreContact
        */
        void RemoveField( TInt aIndex );

        /**
        * From MVPbkStoreContact
        */
        void RemoveAllFields();
        
        /**
        * From MVPbkStoreContact
        */
        void LockL(MVPbkContactObserver& aObserver) const;

        /**
        * From MVPbkStoreContact
        */
        void CommitL( MVPbkContactObserver& aObserver ) const;
        
        /**
        * From MVPbkStoreContact
        */
        MVPbkContactLinkArray* GroupsJoinedLC() const;
        
        /**
        * From MVPbkStoreContact
        */
        MVPbkContactGroup* Group();
        
        /**
        * From MVPbkStoreContact
        */
        TInt MaxNumberOfFieldL(const MVPbkFieldType& aType) const;
        
    private:    // CContactStore private constructors
        /**
        @function   CContact
        @discussion CContact default contructor
        @param      aStore   Store
        @param      aOwns    Owns ldap contact
        */
        CContact(CContactStore& aStore,TBool aOwns = ETrue);

        /**
        @function   ConstructL
        @discussion Perform the second stage construction of CContact 
        @param      aContact Contact
        */
        void ConstructL(CLDAPContact* aContact);
                   
    private:    // CContact private member variables

        /// Owns ldap contact
        TBool iOwnsContact;

        /// Contact index
        TInt iIndex;

        /// Contact field collection
        TStoreContactFieldCollection iFields;

        /// Contact store
        CContactStore& iStore;

        /// LDAP contact, ownership depends on flag above
        CLDAPContact* iContact;
    };

} // End of namespace LDAPStore
#endif  // __CONTACT_H__

