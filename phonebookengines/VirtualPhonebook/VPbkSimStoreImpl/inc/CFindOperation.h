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
* Description:  A find operation that loops the contact data
*
*/



#ifndef VPBKSIMSTOREIMPL_CFINDOPERATION_H
#define VPBKSIMSTOREIMPL_CFINDOPERATION_H

//  INCLUDES
#include <e32base.h>
#include "MVPbkSimStoreOperation.h"
#include "RVPbkStreamedIntArray.h"

// FORWARD DECLARATIONS
class MVPbkSimFindObserver;
class MVPbkSimContact;
class CVPbkContactFindPolicy;

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CStoreBase;

// CLASS DECLARATION

/**
*  A command that finds the contacts that matches to given number
*
*/
NONSHARABLE_CLASS(CFindOperation) : public CActive,
                        public MVPbkSimStoreOperation
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aObserver the observer to notify results
        * @param aStore the sim store
        * @return a new instance of this class
        */
        static CFindOperation* NewL( MVPbkSimFindObserver& aObserver,
            CStoreBase& aStore, const TDesC& aStringToFind, 
            RVPbkSimFieldTypeArray& aFieldTypes );
        
        /**
        * Destructor.
        */
        ~CFindOperation();

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
        CFindOperation( MVPbkSimFindObserver& aObserver,
            CStoreBase& aStore );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aStringToFind, 
            RVPbkSimFieldTypeArray& aFieldTypes );
            
    private:    // New functions
        /// Activate the object
        void NextCycle();
        /// Returns ETrue if contact matches to the iStringToFind
        TBool IsMatch( MVPbkSimContact& aContact );
        /// Returns ETrue if the string matches to the iStringToFind
        TBool IsMatch( const TDesC& aString );
        
    private:    // Data
        ///Ref: The observer to notify when ready
        MVPbkSimFindObserver& iObserver;
        ///Ref: The store that is used to find
        CStoreBase& iStore;
        ///Own: the string to search for
        HBufC* iStringToFind;
        ///Own: field types of the fields that are checked
        RVPbkSimFieldTypeArray iFieldTypes;
        ///Own: The current contact index to search for
        TInt iNextSimIndex;
        ///An array of sim indexes that matched
        RVPbkStreamedIntArray iResults;
        ///Own: virtual phonebook find policy
        CVPbkContactFindPolicy* iFindPolicy;
    };

} // namespace VPbkSimStoreImpl

#endif      // VPBKSIMSTOREIMPL_CFINDOPERATION_H
            
// End of File
