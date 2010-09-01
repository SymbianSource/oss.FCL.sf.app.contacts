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
* Description:  A command that finds the contacts that matches to given number
*
*/



#ifndef VPBKSIMSTOREIMPL_CNUMBERMATCHOPERATION_H
#define VPBKSIMSTOREIMPL_CNUMBERMATCHOPERATION_H

//  INCLUDES
#include <e32base.h>
#include "MVPbkSimStoreOperation.h"

// FORWARD DECLARATIONS
class MVPbkSimFindObserver;
class MVPbkSimCntStore;

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CSimPhoneNumberIndex;

// CLASS DECLARATION

/**
*  A command that finds the contacts that matches to given number
*
*/
NONSHARABLE_CLASS(CNumberMatchOperation) : public CActive,
                        public MVPbkSimStoreOperation
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aObserver the observer to notify results
        * @param aSimPhoneNumberIndex the index of phone numbers
        * @param aPhoneNumber the number to search for
        * @param aNumDigits the number of digits from the end of the number
        *        to match
        * @param aStore the sim store
        * @return a new instance of this class
        */
        static CNumberMatchOperation* NewL( MVPbkSimFindObserver& aObserver,
            CSimPhoneNumberIndex& aSimPhoneNumberIndex,
            const TDesC& aPhoneNumber, TInt aNumDigits,
            MVPbkSimCntStore& aStore );
        
        /**
        * Destructor.
        */
        ~CNumberMatchOperation();

    public: // New functions
        
        /**
        * Executes the operation.
        */
        void Execute();
        
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

    private:

        /**
        * C++ default constructor.
        */
        CNumberMatchOperation( MVPbkSimFindObserver& aObserver,
            CSimPhoneNumberIndex& aSimPhoneNumberIndex, TInt aNumDigits,
            MVPbkSimCntStore& aStore );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aPhoneNumber );

    private:    // Data
        /// The observer to notify when ready
        MVPbkSimFindObserver& iObserver;
        /// The phone number index
        CSimPhoneNumberIndex& iNumberIndex;
        /// The number to search for
        HBufC* iPhoneNumber; 
        /// The number of digits from the end of the number to match
        TInt iNumDigits;
        /// The store that is used to find
        MVPbkSimCntStore& iStore;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CNUMBERMATCHOPERATION_H
            
// End of File
