/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class for store list implementation
*
*/


#ifndef CVPBKCONTACTSTORELIST_H
#define CVPBKCONTACTSTORELIST_H

#include <e32base.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreObserver.h>

class TVPbkContactStoreUriPtr;
class CVPbkContactStoreUriArray;


/**
 * A class for store list implementation
 *
 * @lib VPbkEng.lib
 */
class CVPbkContactStoreList : 
        public CBase,
        public MVPbkContactStoreList,
        private MVPbkContactStoreObserver
    {
    public:
        /**
         * Constructor.
         * @return A new instance of this class
         */
        IMPORT_C static CVPbkContactStoreList* NewL();
    
        /**
         * Destructor. Destroys all stores.
         */
        virtual ~CVPbkContactStoreList();
    
        /**
         * Appends a new store to the list. Ownership is taken.
         *
         * @param aStore a new store to the list. Ownership is taken.
         */
        IMPORT_C void AppendL( MVPbkContactStore* aStore );
    
    public: // from MVPbkContactStoreList
        TInt Count() const;
        MVPbkContactStore& At( TInt aIndex ) const;
        MVPbkContactStore* Find( const TVPbkContactStoreUriPtr& aUri ) const;
        void OpenAllL( MVPbkContactStoreListObserver& aObserver );
        void CloseAll( MVPbkContactStoreListObserver& aObserver );
    
    private: // from MVPbkContactStoreObserver
        void StoreReady( MVPbkContactStore& aContactStore );
        void StoreUnavailable( MVPbkContactStore& aContactStore, TInt aReason );
        void HandleStoreEventL(
            MVPbkContactStore& aContactStore, 
            TVPbkContactStoreEvent aStoreEvent);
        
    private: // implementation
        void SignalOpenComplete();  
        
    private: // implementation
        CVPbkContactStoreList();
        
    private: // data
        /// Own: an array that owns the store instances
        RPointerArray<MVPbkContactStore> iStores;
        /// Ref: a references to the stores that are opened
        RPointerArray<MVPbkContactStore> iStoresToOpen;
        /// Ref: the observer of this list.
        MVPbkContactStoreListObserver* iObserver;
    };


#endif // CVPBKCONTACTSTORELIST_H

// End of File
