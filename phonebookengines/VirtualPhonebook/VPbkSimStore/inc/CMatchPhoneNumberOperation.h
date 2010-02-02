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
* Description:  An operation for number matching
*
*/



#ifndef VPBKSIMSTORE_CMATCHPHONENUMBEROPERATION_H
#define VPBKSIMSTORE_CMATCHPHONENUMBEROPERATION_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactOperation.h>
#include <MVPbkSimFindObserver.h>

// FORWARD DECLARATIONS
class MVPbkContactFindObserver;
class MVPbkSimStoreOperation;

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CContactStore;

// CLASS DECLARATION

/**
*  An operation for number matching
*
*/
NONSHARABLE_CLASS( CMatchPhoneNumberOperation ): 
        public CBase,
        public MVPbkContactOperation,
        private MVPbkSimFindObserver
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        * @param aPhoneNumber the phone number to match
        * @param aMaxMatchDigits the max digits from the end to match
        * @param aObserver the observer to notify when done
        * @param aStore the store to use in match
        */
        static CMatchPhoneNumberOperation* NewL( const TDesC& aPhoneNumber, 
            TInt aMaxMatchDigits,
            MVPbkContactFindObserver& aObserver,
            CContactStore& aStore );
        
        /**
        * Destructor.
        */
        virtual ~CMatchPhoneNumberOperation();

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
        * From MVPbkSimFindObserver
        */
        void FindCompleted( MVPbkSimCntStore& aStore,
            const RVPbkStreamedIntArray& aSimIndexArray );

        /**
        * From MVPbkSimFindObserver
        */
        void FindError( MVPbkSimCntStore& aStore, TInt aError );

    private:
        CMatchPhoneNumberOperation( TInt aMaxMatchDigits,
            MVPbkContactFindObserver& aObserver,
            CContactStore& aStore );
        void ConstructL( const TDesC& aPhoneNumber );
        /// Handles number match results
        void HandleFindCompletedL( 
            const RVPbkStreamedIntArray& aSimIndexArray );

    private:    // Data

        /// Own: the phone number to match
        HBufC* iPhoneNumber; 
        /// Own: the max digits from the end to match
        TInt iMaxMatchDigits;
        /// the observer to notify when done
        MVPbkContactFindObserver& iObserver;
        /// Ref: The store to use in match
        CContactStore& iStore;
        /// Own: the sim store number match operation
        MVPbkSimStoreOperation* iSimOperation;
    };
} // namespace VPbkSimStore 
#endif      // VPBKSIMSTORE_CMATCHPHONENUMBEROPERATION_H
            
// End of File
