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
* Description:  Phonebook 2 USIM UI Extension Check available sim stores
*
*/


#ifndef CPSU2CHECKAVAILABLESTORE_H
#define CPSU2CHECKAVAILABLESTORE_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATIONS
class CVPbkContactStoreUriArray;
class MVPbkContactStoreList;

// CLASS DECLARATION

/**
 * Phonebook 2 USIM Thin UI Extension, 
 * Check the sim store availability
 */
class CPsu2CheckAvailabeSimStore : public CBase,
                         	       private MVPbkContactStoreObserver
                        
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPsu2CheckAvailabeSimStore* NewL();

        /**
         * Destructor.
         */
        virtual ~CPsu2CheckAvailabeSimStore();

    public:
        /**
         * check wether store available for the given uri
         * @param aUri  uri of store 
         * @return ETrue if store is available, else EFalse
         */
        TBool IsContactsAvailabe( const TDesC& aUri  );

    private: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );      
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );        
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent );

    private: // Implementation  	
        CPsu2CheckAvailabeSimStore();
        void ConstructL();
        void OpenSimStoresL();
        void CloseStores();

    private: // Data
        /// Own: URI array that contains the SIM source stores
        CVPbkContactStoreUriArray* iValidSourceStoreUris;
        /// Own: Ready stores
        CVPbkContactStoreUriArray* iReadyStores;
        /// Ref: Contact store list
        MVPbkContactStoreList* iStoreList;
    };

#endif // CPSU2CHECKAVAILABLESTORE_H

// End of File
