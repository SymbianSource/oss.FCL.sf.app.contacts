/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object that reads all the contacts from the database
*
*/


#ifndef C_PCS_CONTACT_FETCH_H
#define C_PCS_CONTACT_FETCH_H

// SYSTEM INCLUDES
#include <f32file.h>
#include <badesca.h>
#include <e32std.h>
#include <e32cmn.h>
#include <MVPbkContactStoreListObserver.h>
#include <MVPbkContactFindFromStoresObserver.h>
#include <MVPbkSingleContactOperationObserver.h>

// USER INCLUDES
#include "mdatastoreobserver.h"
#include "cpcscontactstore.h"
#include "PSContactsAdapterInternalCRKeys.h"

// FORWARD DECLARATIONS
class CVPbkContactManager;


/**
 *  This class is the contact fetch class.
 *  This class manages all the data stores defined.
 *  @lib pscontactsadapter.lib
 */
class CPcsContactFetch: public CActive,
                        public MVPbkContactStoreListObserver


{
    public:

            /**
             * 1st phase constructor
             * @return returns pointer to the constructed object of type CPcsContactFetch
             */
            static CPcsContactFetch* NewL();

            /**
             * Destructor
             */
            virtual ~CPcsContactFetch();

    public:

            // From base class MVPbkContactStoreListObserver

            /**
             * From MVPbkContactStoreListObserver
             * Called when the opening process is complete, ie. all stores have been reported
             * either failed or successfully opened.
             */
            void OpenComplete();

            /**
             * From MVPbkContactStoreListObserver
             * Called when a contact store is ready to use.
             * @param aContactStore - The store that became available.
             */
            void StoreReady(MVPbkContactStore& aContactStore);

            /**
             * From MVPbkContactStoreListObserver
             * Called when a contact store becomes unavailable.
             * Client may inspect the reason of the unavailability and decide whether or not
             * it will keep the store opened (ie. listen to the store events).
             * @param aContactStore - The store that became unavailable.
             * @param aReason - The reason why the store is unavailable.
             *                  This is one of the system wide error codes.
             */
            void StoreUnavailable(MVPbkContactStore& aContactStore, TInt aReason);

            /**
             * From MVPbkContactStoreListObserver
             * Called when changes occur in the contact store i.e contact
             * added/deleted, group added/deleted etc.
             * @see TVPbkContactStoreEvent
             * @param aContactStore - The store the event occurred in.
             * @param aStoreEvent - Event that has occured.
             */
            void HandleStoreEventL(
                    MVPbkContactStore& aContactStore,
                    TVPbkContactStoreEvent aStoreEvent);

    public:
            /**
             * Gets the supported data stores URIs
             * @param aDataStoresURIs supported data stores URIs
             */
            void GetSupportedDataStoresL( RPointerArray<TDesC> &aDataStoresURIs );

            /**
             * Checks if the data store is supported by this adapter
             * @param aDataStoreURI - The datastore to be checked
             * @return ETrue if this store is supported
             */
            TBool IsDataStoresSupportedL( TDesC& aDataStoreURI );

            /**
             * Initiate data fetch from the contacts adapter
             * @param aDataStoreURI - The store from which data is requested
             */
            void RequestForDataL(TDesC& aDataStoreURI);

            /**
             * Set the observer to receive the fetched results
             * @param aObserver - Observer to receive the contacts data
             */
            void SetObserver(MDataStoreObserver& aObserver);

    public:
            /**
             * Gets the supported data fields
             * @param aDataFields - supported data fields
             */
            void GetSupportedDataFieldsL(RArray<TInt> &aDataFields );

    protected:

            // From base class CActive

            /**
             * From CActive
             * Implements cancellation of an outstanding request.
             * This function is called as part of the active object's Cancel().
             */
            void DoCancel();

            /**
             * From CActive
             * Handles an active object's request completion event.
             *
             * The function is called by the active scheduler when a request
             * completion event occurs, i.e. after the active scheduler's
             * WaitForAnyRequest() function completes.
             *
             * Before calling this active object's RunL() function, the active scheduler
             * has:
             *
             * 1. decided that this is the highest priority active object with
             *   a completed request
             *
             * 2. marked this active object's request as complete (i.e. the request is no
             *   longer outstanding)
             *
             * RunL() runs under a trap harness in the active scheduler. If it leaves,
             * then the active scheduler calls RunError() to handle the leave.
             *
             * Note that once the active scheduler's Start() function has been called,
             * all user code is run under one of the program's active object's RunL() or
             * RunError() functions.
             */
            void RunL();


            /**
             * From CActive
             * If the RunL function leaves,
             * then the active scheduler calls RunError() to handle the leave.
             * @param aError - The error code
             */
            TInt RunError( TInt aError );

    private:

            /**
             * Constructor
             */
            CPcsContactFetch();

            /**
             * 2nd phase constructor
             */
            void ConstructL();

            /**
             * Creates the substore
             * @param aDataStoreURI - The store for which substore is to be created
             */
             void CreateSubStoresL(TDesC& aDataStoreURI);

            /**
             * Reads the configured URIs from the central repository
             */
            void ReadUrisFromCenrepL();

    private:

            /**
             * Holds the observer object to communicate add/modify/delete of contacts
             * Not owned.
             */
            MDataStoreObserver* iObserver;

            /**
             * Owns an instance of active scheduler wait
             */
            CActiveSchedulerWait *iWait;

            /**
             * Flags for store operations
             */
            TBool iAtLeastOneStoreReady;
            TBool iNoContactStoreAvailable;
            TBool iRequestForData;
            TBool iSubStoreCreated;

            /**
             * The contact manager for accessing the phone contacts
             * Own.
             */
            CVPbkContactManager*  iContactManager;

            /**
             * Uris(data stores) read from the central repository
             */
            RPointerArray<HBufC> iUriFromCenrep;

            /**
             * Data stores instances
             */
            RPointerArray<CPcsContactStore> iAllDataStores;
};

#endif // C_PCS_CONTACT_FETCH_H
