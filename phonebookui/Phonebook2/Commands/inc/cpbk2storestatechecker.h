/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Checks store state, shows also unavailable note
*
*/


#ifndef CPBK2STORESTATECHECKER_H
#define CPBK2STORESTATECHECKER_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactStoreObserver.h>


// FORWARD DECLARATIONS
class MPbk2StoreStateCheckerObserver;

// CLASS DECLARATIONS
/**
 * Phonebook 2 checks store state
 */
NONSHARABLE_CLASS( CPbk2StoreStateChecker ):  
        public CBase,
        private MVPbkContactStoreObserver
    {
    public: // Constructor and destructor
        static CPbk2StoreStateChecker* NewL( 
                MVPbkContactStore& aContactStore,
                MPbk2StoreStateCheckerObserver& aObserver );
        /**
         * Destructor.
         */
        ~CPbk2StoreStateChecker();
     
    public: // Interface
        void ShowUnavailableNoteL();

    private: // MVPbkContactStoreObserver
        void StoreReady(MVPbkContactStore& aContactStore);
        void StoreUnavailable(MVPbkContactStore& aContactStore, 
                TInt aReason);
        void HandleStoreEventL(
                        MVPbkContactStore& aContactStore, 
                        TVPbkContactStoreEvent aStoreEvent); 
                
    private: // Implementation
        CPbk2StoreStateChecker( 
                MVPbkContactStore& aContactStore,
                MPbk2StoreStateCheckerObserver& aObserver );
        void ConstructL();
                               
    private: // Data
        /// Not own: Contact store which state is checked
        MVPbkContactStore& iContactStore;
        /// Not own: Observer of this state
        MPbk2StoreStateCheckerObserver& iObserver;
        
    };

#endif // CPBK2STORESTATECHECKER_H

// End of File
