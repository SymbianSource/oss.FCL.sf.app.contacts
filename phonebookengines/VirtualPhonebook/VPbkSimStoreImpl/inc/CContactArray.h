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
* Description:  An array of sim contacts
*
*/



#ifndef VPBKSIMSTOREIMPL_CCONTACTARRAY_H
#define VPBKSIMSTOREIMPL_CCONTACTARRAY_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkSimContact;

namespace VPbkSimStoreImpl {

// CLASS DECLARATION

/**
* An observer API for changes in the contact array
*/
NONSHARABLE_CLASS( MContactArrayObserver )
    {
    public: // New functions

        /**
        * A new contact was added to the contact array
        * @param aSimContact a new sim contact
        */ 
        virtual void ContactAdded( MVPbkSimContact& aSimContact ) = 0;

        /**
        * A contact in given sim index has been removed from the array
        * @param aRemovedContact the contact that was removed from the
        *                        array and will be deleted
        */
        virtual void ContactRemoved( MVPbkSimContact& aRemovedContact ) = 0;

        /**
        * A contact has been changed.
        * @param aOldContact the contact that changed and is deleted
        * @param aNewContact the contact that replaces the deleted one.
        */
        virtual void ContactChanged( MVPbkSimContact& aOldContact,
            MVPbkSimContact& aNewContact ) = 0;
            
        /**
        * A contact will be removed from the contact array
        * @param aSimContact a contact to be removed
        */
        virtual void PrepareForContactRemoval( 
            MVPbkSimContact& /*aSimContact*/ ) {}
        
    protected: // Destruction
        virtual ~MContactArrayObserver() {}
    };

/**
*  An array of sim contacts. The array uses SIM indexes that start
*  from 1, not zero.
*
*/
NONSHARABLE_CLASS(CContactArray) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CContactArray();
        
        /**
        * Destructor.
        */
        virtual ~CContactArray();

    public: // New functions
        
        /**
        * Adds contact to the array. Deletes the old one if there is
        * a contact with same sim index
        * @param aSimContact a new contacts to the array. Ownership is taken.
        */
        void AddOrReplaceL( MVPbkSimContact* aSimContact );

        /**
        * Deletes contact(s) from the array.
        * @param aFromSimIndex the sim index of the first contact to delete.
        * @param aToSimIndex  the sim index of the last contact to delete
        */
        void Delete( TInt aFromSimIndex, TInt aToSimIndex );

        /**
        * Returns a contact in given sim index or NULL if not exists
        * NOTE: SIM indexes start from 1 not zero.
        *
        * @return a contact in given sim index or NULL if not exists
        */
        MVPbkSimContact* At( TInt aSimIndex ) const;

        /**
        * Deletes all contact instances from the array but 
        * doesn't free the array memory.
        */
        void DeleteAll();

        /**
        * Adds a new observer to the contact array
        * @param aObserver a new observer
        */
        void AddObserverL( MContactArrayObserver& aObserver );

        /**
        * Removes an observer from the contact array
        * @param aObserver the observer to be removed
        */
        void RemoveObserver( MContactArrayObserver& aObserver );
        
        /**
        * Allocates room for contact pointers
        * @param aSize the size for the array
        */
        void ReserveL( TInt aSize );
        
        /**
        * Returns the size of the array.
        */
        inline TInt Size() const;
        
        /**
        * Returns first empty slot index
        */
        TInt FirstEmptySlot() const;

    private:    // Data
        /// An array of sim contacts. Owns contacts.
        RPointerArray<MVPbkSimContact> iCntArray;
        /// An array of observers
        RPointerArray<MContactArrayObserver> iObservers;
    };

// INLINE FUNCTIONS
inline TInt CContactArray::Size() const
    {
    return iCntArray.Count();
    }

} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CCONTACTARRAY_H
            
// End of File
