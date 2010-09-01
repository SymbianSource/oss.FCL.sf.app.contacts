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
* Description:  Active object that reads all the contacts/groups from vpbk
*
*/


#ifndef C_PCS_CONTACT_STORE_H
#define C_PCS_CONTACT_STORE_H

// SYSTEM INCLUDES
#include <f32file.h>
#include <badesca.h>
#include <e32std.h>
#include <e32cmn.h>
#include <MVPbkContactStoreListObserver.h>
#include <MVPbkContactFindFromStoresObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactViewObserver.h>
#include <CVPbkContactManager.h>

// USER INCLUDES
#include "mdatastoreobserver.h"
#include "PSContactsAdapterInternalCRKeys.h"

// FORWARD DECLARATIONS
class CPsData;
class CVPbkContactManager;
class CVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MVPbkBaseContact;
class CVPbkFieldTypeRefsList;
class MVPbkContactViewObserver;
class CPbk2SortOrderManager;

/**
 * States involved in fetching the contacts
 */
enum
{
    ECreateView,
    EFetchAllLinks,
    EFetchContactBlock,
    EComplete
};

/**
 *  This class represents the data store for the contacts
 *  Each data store (phonecontacts, simcontact, group contacts) is an
 *  instance of this class
 *  @lib pscontactsadapter.lib
 */
class CPcsContactStore: public CActive,
                        public MVPbkSingleContactOperationObserver,
                        public MVPbkContactViewObserver

{
    public:

        /**
         * 2 phase construction
         * @param aContactManager - the contact manager
         * @param aObserver - observer for receiving data
         * @param aUri - the data store uri
         * @return returns pointer to the constructed object of type CPcsContactStore
         */
        static CPcsContactStore* NewL(CVPbkContactManager& aContactManager,
                                      MDataStoreObserver& aObserver,
                                      const TDesC& aUri);

        /**
         * Destructor
         */
        virtual ~CPcsContactStore();

        /**
         * Handles store events (contact/group addition/modification/deletion etc.)
         * @param aContactStore - the contact store
         * @param aStoreEvent - store event
         */
        void HandleStoreEventL(MVPbkContactStore& aContactStore,
                               TVPbkContactStoreEvent aStoreEvent);

        /**
         * Gets the store Uri
         * @return - the store uri supported by this instance
         */
        TDesC& GetStoreUri();

    public:

        // From base class MVPbkSingleContactOperationObserver

        /**
         * From MVPbkSingleContactOperationObserver
         * Called when operation is completed.
         *
         * @param aOperation the completed operation.
         * @param aContact  the contact returned by the operation.
         *                  Client must take the ownership immediately.
         *
         *                  !!! NOTICE !!!
         *                  If you use Cleanupstack for MVPbkStoreContact
         *                  Use MVPbkStoreContact::PushL or
         *                  CleanupDeletePushL from e32base.h.
         *                  (Do Not Use CleanupStack::PushL(TAny*) because
         *                  then the virtual destructor of the M-class
         *                  won't be called when the object is deleted).
         */
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );

        /**
         * From MVPbkSingleContactOperationObserver
         * Called if the operation fails.
         *
         * @param aOperation    the failed operation.
         * @param aError        error code of the failure.
         */
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    public :

        // From base class MVPbkContactViewObserver

        void ContactViewReady(
                    MVPbkContactViewBase& aView );

        void ContactViewUnavailable(
                    MVPbkContactViewBase& aView );

        void ContactAddedToView(
                    MVPbkContactViewBase& aView,
                    TInt aIndex,
                    const MVPbkContactLink& aContactLink );

        void ContactRemovedFromView(
                    MVPbkContactViewBase& aView,
                    TInt aIndex,
                    const MVPbkContactLink& aContactLink );

        void ContactViewError(
                    MVPbkContactViewBase& aView,
                    TInt aError,
                    TBool aErrorNotified );

    protected:

        // From base class CActive

        /**
         * From CActive
         * Implements cancellation of an outstanding request.
         *
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
        CPcsContactStore();

        /**
         * 2nd phase constructor
         */
        void ConstructL(CVPbkContactManager& aContactManager, MDataStoreObserver& aObserver,const TDesC& aUri);

        /**
         * Check next state of state machine for contacts retrieval and perform transition 
         */
        void CheckNextState();

        /**
         * Handles the operations for a single contact after it is fetched
         * @param aContact - The contact from database
         */
        void HandleRetrievedContactL(MVPbkStoreContact* aContact);

        /**
         * Fetches the data from a particular contact
         * @param aContact - The contact from database
         *
         */
        void  GetDataForSingleContactL ( MVPbkBaseContact& aContact, CPsData* aPhoneData );

        /**
         * Add the data from contact fields
         * @param aContact - The contact from database
         * @param afieldtype - Field to be added
         * @param aPhoneData - the contact data in PS format
         */
        void AddContactFieldsL(MVPbkBaseContact& aContact, TInt afieldtype, CPsData* aPhoneData);

        /**
         * Fetches the  data from contact links from the view
         */
        void FetchAllInitialContactLinksL();
        void FetchContactsBlockL();

        /**
         * Reads the fields to cache from the central repository
         */
        void ReadFieldsToCacheFromCenrepL();

        /**
         * Creates a cacheId corresponding to sim Id Array Index
         * @param - aSimId - The sim id
         */
        TInt CreateCacheIDfromSimArrayIndex(TInt aSimId);

        /**
         * Creates the contact fetch view
         */
        void CreateContactFetchViewL();

        /**
         * Issues request to active object to call RunL method
         */
        void IssueRequest( TInt aTimeDelay );

        /**
         * Creates a sort order depending on the fields specified in the cenrep
         * @param aMasterList - aMasterList (i.e list containing all the vpbk fields)
         */
        void CreateSortOrderL(const MVPbkFieldTypeList& aMasterList);
        
        /**
         * Checks if contact is my card
         */
        TBool IsMyCard( const MVPbkBaseContact& aContact );

    private:

        RPointerArray<MVPbkContactLink> iInitialContactLinks;

        /**
         * Flags for store operations
         */
        TInt iInitialContactCount; // Initial contact count in a view
        TInt iFetchBlockLowerNumber;
        TInt iFetchBlockUpperNumber;
        TInt iVPbkCallbackCount;
        TBool iContactViewReady;
        TCachingStatus iOngoingCacheUpdate;

        /**
         * Variable to store the next state for the RunL to take appropriate action
         */
        TInt iNextState;

        /**
         * The contact manager for accessing the phone contacts
         * Not Own.
         */
        CVPbkContactManager* iContactManager;

        /**
         * Holds a view base instance
         * Own.
         */
        MVPbkContactViewBase* iContactViewBase;

        /**
         * Holds a contact operation
         * Own.
         */
        MVPbkContactOperationBase* iOp;

        /**
         * Owns an instance of active scheduler wait
         */
        CActiveSchedulerWait *iWait;

        /**
         * data fields to be cached(read from the central repository)
         */
        RArray<TInt> iFieldsToCache;

        /**
         * Array of contact links
         */
        CVPbkContactLinkArray *iSimContactItems;

        /**
         * Holds the observer object to communicate add/modify/delete of contacts
         * Not owned.
         */
        MDataStoreObserver* iObserver;

        /**
         * Contacts Database URI
         */
        HBufC* iUri;

        /**
         * File session
         */
        RFs iFs;

        /**
         * RTimer variable to set the timer before RunL calls any function.
         * This is required to allow other threads to run since contact fetch
         * is CPU intensive task.
         */
        RTimer iTimer;

        /**
         * Holds the sort order fields
         */
        CVPbkFieldTypeRefsList *iSortOrder;
        
        /**
         * Holds MyCard supported status
         */
        TBool iMyCardSupported;
		
        /**
		 * Own. Sort order for all contacts view
		 */
        CPbk2SortOrderManager* iSortOrderMan;
};

#endif // C_PCS_CONTACT_STORE_H
