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
* Description:  A virtual phonebook operation for find feature
*
*/



#ifndef VPBKSIMSTORE_CFINDOPERATION_H
#define VPBKSIMSTORE_CFINDOPERATION_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactOperation.h>
#include <MVPbkSimFindObserver.h>
#include <RVPbkStreamedIntArray.h>
#include <MVPbkContactFindObserver.h>

// FORWARD DECLARATIONS
class MVPbkFieldTypeList;
class MVPbkSimStoreOperation;
class CVPbkContactLinkArray;

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CContactStore;

// CLASS DECLARATION

/**
*  A virtual phonebook operation for find feature
*
*/
NONSHARABLE_CLASS( CFindOperation ): 
        public CActive,
        public MVPbkContactOperation,
        private MVPbkSimFindObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aStore the target store for the find
        * @param aObserver the observer for the operation
        * @param aStringToFind the find text string
        * @param aFieldTypes types to check in find operation
        * @return a new instance of this class
        */
        static CFindOperation* NewL( CContactStore& aStore,
            MVPbkContactFindObserver& aObserver,
            const TDesC& aStringToFind,
            const MVPbkFieldTypeList& aFieldTypes );
        
        /**
        * Destructor.
        */
        ~CFindOperation();

    public: // Functions from base classes
        
        /**
        * From CActive
        */
        void RunL();
        
        /**
        * From CActive
        */
        void DoCancel();
        
        /**
        * From CActive
        */
        TInt RunError( TInt aError );
        
        /**
        * From MVPbkContactOperation
        */
        void StartL();

        /**
        * From MVPbkContactOperation
        */
        void Cancel();
    
    private:

        /**
        * C++ constructor.
        */
        CFindOperation( CContactStore& aStore, 
            MVPbkContactFindObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aStringToFind,
            const MVPbkFieldTypeList& aFieldTypes );
    
    private:  // Functions from base classes
        
        /**
        * From MVPbkSimFindObserver
        */
        void FindCompleted( MVPbkSimCntStore& aStore,
            const RVPbkStreamedIntArray& aSimIndexArray );

        /**
        * From MVPbkSimFindObserver
        */
        void FindError( MVPbkSimCntStore& aStore, TInt aError );
        
    private:    // New functions
        void HandleFindCompletedL( 
            const RVPbkStreamedIntArray& aSimIndexArray );
        void CompleteL();
        
    private:    // Data
        /// The target store for the find operation
        CContactStore& iStore;
        /// The observer for the operation
        MVPbkContactFindObserver& iObserver;
        /// Own the text string to use for find
        HBufC* iStringToFind;
        /// An array of sim field types for the find operation
        RVPbkSimFieldTypeArray iFieldTypes;
        /// Own: the sim store number match operation
        MVPbkSimStoreOperation* iSimOperation;
        /// Own: result array, owned until client gets the ownership
        CVPbkContactLinkArray* iLinkArray;
    };
} // namespace VPbkSimStore
#endif      // VPBKSIMSTORE_CFINDOPERATION_H
            
// End of File
