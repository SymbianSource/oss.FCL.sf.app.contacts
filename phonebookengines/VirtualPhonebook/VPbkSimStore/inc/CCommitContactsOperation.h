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
* Description:  A contact operation for commiting multiple contacts
*
*/



#ifndef VPBKSIMSTORE_CCOMMITCONTACTSOPERATION_H
#define VPBKSIMSTORE_CCOMMITCONTACTSOPERATION_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactOperation.h>
#include <MVPbkContactObserver.h>

// FORWARD DECLARATIONS
class MVPbkBatchOperationObserver;
class MVPbkStoreContact;

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CContactStore;

// CLASS DECLARATION

/**
*  A contact operation for commtting multiple contacts
*
*/
NONSHARABLE_CLASS( CCommitContactsOperation ): 
        public CBase,
        public MVPbkContactOperation,
        private MVPbkContactObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aStore the store that is used by the operation
        * @param aObserver an observer to notify while commiting
        * @param aContact virtual phonebook contacts
        * @return a new instance of this class
        */
        static CCommitContactsOperation* NewL(
            CContactStore& aStore,
            MVPbkBatchOperationObserver& aObserver,
            const TArray<MVPbkStoreContact*>& aContacts );
        
        /**
        * Destructor.
        */
        virtual ~CCommitContactsOperation();

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
        * From MVPbkContactObserver
        */
        void ContactOperationCompleted( TContactOpResult aResult );

        /**
        * From MVPbkContactObserver
        */
        void ContactOperationFailed( TContactOp aOpCode, TInt aErrorCode,
            TBool aErrorNotified );

    private:    // Construction

        /**
        * C++ constructor.
        */
        CCommitContactsOperation( CContactStore& aStore,
            MVPbkBatchOperationObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TArray<MVPbkStoreContact*>& aContacts );
    
    private:    // New functions
        /// Commits the next contact or quits the operation
        TInt NextCycle();
        /// Tries to commit next contact
        void TryCommitNext();

    private:    // Data
        /// The store that is used by the operation
        CContactStore& iStore;
        /// An observer to notify while executing the operation
        MVPbkBatchOperationObserver& iObserver;
        /// An array for the contacts, not owned.
        RArray<MVPbkStoreContact*> iStoreContacts;
        /// A counter for commited contacts
        TInt iCommitCounter;
    };
} // namespace VPbkSimStore
#endif      // VPBKSIMSTORE_CCOMMITCONTACTSOPERATION_H
            
// End of File
