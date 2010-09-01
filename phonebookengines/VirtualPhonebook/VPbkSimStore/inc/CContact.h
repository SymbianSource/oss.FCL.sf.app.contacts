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
* Description:  A contact adapter between VPbk framework and VPbkSimStoreImpl
*
*/


#ifndef VPBKSIMSTORE_CCONTACT_H
#define VPBKSIMSTORE_CCONTACT_H


// INCLUDES
#include <e32base.h>
#include <babitflags.h>
#include "MVPbkStoreContact.h"
#include <MVPbkStoreContact2.h>
#include "TStoreContactFieldCollection.h"
#include "MVPbkSimContactObserver.h"

// FORWARD DECLARATIONS
class CVPbkSimContact;
class MVPbkSimStoreOperation;
class MVPbkStoreContactProperties;

// FORWARD DECLARATIONS
namespace VPbkEngUtils { 
class CVPbkAsyncOperation; 
}

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CContactStore;

// CLASS DECLARATIONS

/**
 * Maps a Sim entry to a virtual Phonebook contact.
 */
NONSHARABLE_CLASS( CContact ): 
        public CBase, 
        public MVPbkStoreContact,
        public MVPbkStoreContact2,
        private MVPbkSimContactObserver
    {
    public:  // Constructor and destructor
        /**
        * Creates a new instance of this class.
        *
        * @param aParentStore the store in which this contact belongs
        * @param aSimContact a sim contact, ownership is taken 
        *        after construction.
        * @return a new instance of this class
        */
        static CContact* NewL( CContactStore& aParentStore,
            CVPbkSimContact* aSimContact, TBool aIsNewContact = EFalse );

        /**
        * Destructor.
        */
        ~CContact();

    public:  // New functions

        /**
        * Returns the parent store of this contact.
        * @return the parent store of this contact.
        */
        inline CContactStore& Store() const;

        /**
        * Returns the sim contact
        * @return the sim contact
        */
        inline CVPbkSimContact& SimContact() const;

        /**
        * Sets the lock status of the contact
        * @param aLocked if ETrue the contact is set to locked
        */
        inline void SetLock( TBool aLocked );

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

        /**
        * From MVPbkStoreContact
        */
		TAny* StoreContactExtension(TUid aExtensionUid) ;
        
        /**
        * From MVPbkStoreContact2
        */        
        MVPbkStoreContactProperties* PropertiesL() const;
        
        /**
        * From MVPbkStoreContact2
        */        
        void SetAsOwnL(MVPbkContactObserver& aObserver) const;
        
    private:  // Construction

        /**
        * C++ constructor.
        */
        CContact( CContactStore& aParentStore );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CVPbkSimContact& aSimContact, TBool aIsNewContact );
    
    private:    // Functions from base classes
        
        /**
        * From MVPbkSimContactObserver
        */
        void ContactEventComplete( TEvent aEvent, CVPbkSimContact* aContact );

        /**
        * From MVPbkSimContactObserver
        */
        void ContactEventError( TEvent aEvent, CVPbkSimContact* aContact, 
            TInt aError );
            
    private:    // New functions
        /// Resets the contact state after contact operation
        void ResetContactOperationState();
        
        /**
        * Remove empty field from aSimContact, these empty field was added 
        * in the sim contact before save it to keep the order of additional 
        * numbers in the USIM card. 
        */
        void RemoveAllEmptyFields( CVPbkSimContact& aSimContact );
        
        /**
        * Fill some empty field in the sim contact. hese empty field was added 
        * in the sim contact before save it to keep the order of additional 
        * numbers in the USIM card. 
        */
        void FillWithEmptyFieldsL() const;
               
    private:  // Data

        TStoreContactFieldCollection iFields;
        /// The parent store of the contact
        CContactStore& iParentStore;
        /// Own: the sim contact
        CVPbkSimContact* iSimContact;
        /// ETrue if this contact has been locked by the client.
        /// This is a requirement of vpbk framework -> sim contacts
        /// can't be actually locked
        mutable TBool iLocked;
        /// Flags for contact state
        mutable TBitFlagsT<TUint32> iFlags;
        /// Own: an asynchronous call back that is needed if observer
        /// must be notified e.g from ReadL
        VPbkEngUtils::CVPbkAsyncOperation* iAsyncOp;
        /// Ref: the observer for the CommitL/DeleteL
        mutable MVPbkContactObserver* iObserver;
        /// Own: an async operaton handle.
        mutable MVPbkSimStoreOperation* iStoreOperation;
    };


// INLINE FUNCTIONS

inline CContactStore& CContact::Store() const
    {
    return iParentStore;
    }

inline CVPbkSimContact& CContact::SimContact() const
    {
    return *iSimContact;
    }

inline void CContact::SetLock( TBool aLocked )
    {
    iLocked = aLocked;
    }
} // namespace VPbkSimStore

#endif  // VPBKSIMSTORE_CCONTACT_H

