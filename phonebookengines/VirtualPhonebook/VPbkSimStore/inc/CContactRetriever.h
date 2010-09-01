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
* Description:  A virtual phonebook operation class that reads a contact
*
*/



#ifndef VPBKSIMSTORE_CCONTACTRETRIEVER_H
#define VPBKSIMSTORE_CCONTACTRETRIEVER_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactOperation.h>
#include <MVPbkContactObserver.h>

// FORWARD DECLARATIONS
class MVPbkSingleContactOperationObserver;

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CContactStore;

// CLASS DECLARATION

/**
*  A virtual phonebook operation class that reads a contact
*
*/
NONSHARABLE_CLASS( CContactRetriever ): 
        public CActive,
        public MVPbkContactOperation
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aStore     The store to read from.
        * @param aSimIndex  The sim index of the contact to read.
        * @return A new instance of this class.
        */
        static CContactRetriever* NewL( CContactStore& aStore, 
            TInt aSimIndex, MVPbkSingleContactOperationObserver& aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CContactRetriever();

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
        TInt RunError(TInt aError);
        
        /**
        * From MVPbkContactOperation
        */
        void StartL();

        /**
        * From MVPbkContactOperation
        */
        void Cancel();
    
    private: // Construction

        /**
        * C++ constructor.
        */
        CContactRetriever(
            TInt aSimIndex,
            CContactStore& aStore,
            MVPbkSingleContactOperationObserver& aObserver );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        /// Own: The SIM index from where to retrieve the contact
        TInt iSimIndex;
        /// Ref: The store to read from
        CContactStore& iStore;
        /// Ref: The observer for the operation
        MVPbkSingleContactOperationObserver& iObserver;
    };

} // namespace VPbkSimStore

#endif      // VPBKSIMSTORE_CCONTACTRETRIEVER_H
            
// End of File
