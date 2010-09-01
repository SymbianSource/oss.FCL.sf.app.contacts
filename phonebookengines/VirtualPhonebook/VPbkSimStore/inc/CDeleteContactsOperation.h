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
* Description:  A contact operation for deleting multiple contacts
*
*/



#ifndef VPBKSIMSTORE_CDELETECONTACTSOPERATION_H
#define VPBKSIMSTORE_CDELETECONTACTSOPERATION_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactOperation.h>
#include <MVPbkSimContactObserver.h>
#include <RVPbkStreamedIntArray.h>
#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATIONS
class MVPbkBatchOperationObserver;
class MVPbkSimStoreOperation;

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CContactStore;

// CLASS DECLARATION

/**
*  A contact operation for deleting multiple contacts
*
*/
NONSHARABLE_CLASS( CDeleteContactsOperation ): 
        public CBase,
        public MVPbkContactOperation,
        private MVPbkSimContactObserver,
        private MVPbkContactStoreObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aStore the store that is used by the operation
        * @param aObserver an observer to notify while commiting
        * @return a new instance of this class
        */
        static CDeleteContactsOperation* NewL( CContactStore& aStore,
            MVPbkBatchOperationObserver& aObserver,
            const TArray<TInt>& aSimIndexes );
        
        /**
        * Destructor.
        */
        virtual ~CDeleteContactsOperation();

    public: // Functions from base classes

        /**
        * From MVPbkContactOperation
        */
        void StartL();

        /**
        * From MVPbkContactOperation
        */
        void Cancel();
        
    private:  // Functions from base classes
        
        /**
        * From MVPbkSimContactObserver
        */
        void ContactEventComplete( TEvent aEvent, CVPbkSimContact* aContact );
        
        /**
        * From MVPbkSimContactObserver
        */
        void ContactEventError( TEvent aEvent, CVPbkSimContact* aContact, 
            TInt aError );

    private:  // Functions from base classes MVPbkContactStoreObserver

        /**
        * From MVPbkSimContactObserver
        */
        void StoreReady(MVPbkContactStore& aContactStore);

        /**
        * From MVPbkSimContactObserver
        */
        void StoreUnavailable(MVPbkContactStore& aContactStore, TInt aReason);

        /**
        * From MVPbkSimContactObserver
        */
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent);
                
    private:    // Construction

        /**
        * C++ constructor.
        */
        CDeleteContactsOperation( CContactStore& aStore, 
            MVPbkBatchOperationObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TArray<TInt>& aSimIndexes );

    private:    // New functions
        void CompleteWithError( TInt aError );
    
    private:    // Data
        /// The store that is used by the operation
        CContactStore& iStore;
        /// An observer to notify while executing the operation
        MVPbkBatchOperationObserver& iObserver;
        /// An array for the indexes of the contacts to be deleted
        RVPbkStreamedIntArray iSimIndexes;
        /// Own: a sim operation
        MVPbkSimStoreOperation* iSimOperation;
    };
} // namespace VPbkSimStore
#endif      // VPBKSIMSTORE_CDELETECONTACTSOPERATION_H
            
// End of File
