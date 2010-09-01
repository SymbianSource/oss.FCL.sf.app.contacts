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
* Description:  The virtual phonebook contact
*
*/



#ifndef VPBKCNTMODEL_CCONTACT_H
#define VPBKCNTMODEL_CCONTACT_H


// INCLUDES
#include <e32base.h>
#include <MVPbkContactGroup.h>
#include <cntdef.h>
#include "TContactFieldCollection.h"

// FORWARD DECLARATIONS
class CContactItem;
class CContactIdArray;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;
class CFieldTypeMap;

// CLASS DECLARATIONS

/**
 * Maps a Contact Model CContactItem to a virtual Phonebook contact.
 */
NONSHARABLE_CLASS( CContact ): public CBase, 
                               public MVPbkContactGroup,
                               public MVPbkBaseContact2
    {
    public:  // Constructor and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aParentStore  the store where this contact is from.
         * @param aContactItem  the Contact Model contact to map. This object
         *                      takes ownership of the passed object. Ownership
         *                      is not taken if this function leaves.
         * @param aIsNewContact is this a new contact
         *
         * @return a new instance of this class
         */
        static CContact* NewL(
                CContactStore& aParentStore, 
                CContactItem* aContactItem,
                TBool aIsNewContact = EFalse );

        ~CContact();

    public:  // New functions
        /**
         * Returns the parent store of this contact.
         */
        CContactStore& Store() const;

        /**
         * Returns the Contact Model contact.
         */
        CContactItem* NativeContact() const;

        /**
         * Sets a new Contact Model contact.
         *
         * @param aContactItem  the Contact Model contact to map. This object
         *                      takes ownership of the passed object.
         */
        void SetContact( CContactItem* aContactItem );

        /**
         * Returns the field type map to use for mapping Contact Model field
         * types to Virtual Phonebook field types.
         */
        const CFieldTypeMap& FieldTypeMap() const;
        
        /**
         * Returns ETrue if this contact is a new contact.
         */
        TBool IsNewContact() const;
        
        /**
         * Set modifed flag.
         */
        void SetModified();
        
    public:  // from MVPbkObjectHierarchy
        MVPbkObjectHierarchy& ParentObject() const;

    public:  // from MVPbkBaseContact
        TAny* BaseContactExtension( TUid aExtensionUid );
        
    public:  // from MVPbkBaseContact2
        TBool IsOwnContact( TInt& aError ) const;
        
    public:  // from MVPbkStoreContact
        const MVPbkStoreContactFieldCollection& Fields() const;
        TBool IsSame( const MVPbkStoreContact& aOtherContact ) const;
        TBool IsSame( const MVPbkViewContact& aOtherContact ) const;
        MVPbkContactLink* CreateLinkLC() const;
        void DeleteL( MVPbkContactObserver& aObserver ) const;
        TBool MatchContactStore( const TDesC& aContactStoreUri ) const;
        TBool MatchContactStoreDomain( const TDesC& aContactStoreDomain ) const;
        MVPbkContactBookmark* CreateBookmarkLC() const;
            
        MVPbkContactStore& ParentStore() const;
        MVPbkStoreContactFieldCollection& Fields();
        MVPbkStoreContactField* CreateFieldLC( const MVPbkFieldType& aFieldType ) const;
        TInt AddFieldL( MVPbkStoreContactField* aField );
        void RemoveField( TInt aIndex );
        void RemoveAllFields();
        void LockL( MVPbkContactObserver& aObserver ) const;
        void CommitL( MVPbkContactObserver& aObserver ) const;
        MVPbkContactLinkArray* GroupsJoinedLC() const;
        MVPbkContactGroup* Group();
        TInt MaxNumberOfFieldL( const MVPbkFieldType& aType ) const;
        TAny* StoreContactExtension(TUid aExtensionUid) ;
        
    public:  // from MVPbkStoreContact2
        MVPbkStoreContactProperties* PropertiesL() const;
        void SetAsOwnL(MVPbkContactObserver& aObserver) const;
        
    public: // from MVPbkContactGroup
        void SetGroupLabelL( const TDesC& aLabel );
        TPtrC GroupLabel() const;
        void AddContactL( const MVPbkContactLink& aContactLink );
        void RemoveContactL( const MVPbkContactLink& aContactLink );
        MVPbkContactLinkArray* ItemsContainedLC() const;
        
    private:  // Implementation
        CContact( CContactStore& aParentStore, CContactItem& aContactItem, 
                    TBool aIsNewContact );
        void ConstructL();
        void UpdateTimeStampOfContactInGroupL( const MVPbkContactLink& aContactLink );
        
    public:
        void UpdateTimeStampOfAllContactsInGroupL();

    private:  // Data
        /// Own: Is this new contact
        TBool iIsNewContact;
        /// Own:The Contact Model contact to map
        CContactItem* iContactItem;
        /// Own: Contact fields
        TContactFieldCollection iFields;
        /// Not Own: The store where this contact is from.
        CContactStore& iParentStore;
        /// Own: If this CContact is group, this contains all added contacts 
        // to this group after this group is read from DB
        CContactIdArray* iAddedContacts;
        TBool iModified;
        
        TContactItemId iLastUpdatedGroupContactId;
    };


// INLINE FUNCTIONS

inline CContactStore& CContact::Store() const
    {
    return iParentStore;
    }

inline CContactItem* CContact::NativeContact() const
    {
    return iContactItem;
    }

inline TBool CContact::IsNewContact() const
    {
    return iIsNewContact;
    }

inline void CContact::SetModified()
    {
    iModified = ETrue;
    }

} // namespace VPbkCntModel

#endif  // VPBKCNTMODEL_CCONTACT_H
//End of file


