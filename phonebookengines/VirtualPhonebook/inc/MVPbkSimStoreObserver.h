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
* Description:  An observer API for store events
*
*/


#ifndef MVPBKSIMSTOREOBSERVER_H
#define MVPBKSIMSTOREOBSERVER_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkSimCntStore;

// CLASS DECLARATION

/**
*  An observer API for store events
*
*/
class MVPbkSimStoreObserver
    {
    public: // Types
        /**
        * SIM contact events
        */
        enum TEvent
            {
            /// Unknown operation
            EUnknown = 0,
            /// A contact was added to the store
            EContactAdded,
            /// A contact was deleted from the store
            EContactDeleted,
            /// A contact has changed in store
            EContactChanged
            };

    public: // New functions
        
        /**
        * Called after a store is ready
        * @param aStore the store that became ready
        */
        virtual void StoreReady( MVPbkSimCntStore& aStore ) = 0;

        /**
        * Called if opening of the store failed.
        * @param aStore the store that couldn't be opened.
        * @param aError the error code.
        */
        virtual void StoreError( MVPbkSimCntStore& aStore, TInt aError ) = 0;

        /**
        * Called if the sim store was not supported by the (U)SIM.
        */
        virtual void StoreNotAvailable( MVPbkSimCntStore& aStore ) = 0;
        
        /**
        * Called when there is changes in the store
        * @param aEvent the store event
        * @param aSimIndex the changed contact sim index
        */
        virtual void StoreContactEvent( TEvent aEvent, TInt aSimIndex ) = 0;        

    protected:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~MVPbkSimStoreObserver() {}
    };

#endif      // MVPBKSIMSTOREOBSERVER_H
            
// End of File
