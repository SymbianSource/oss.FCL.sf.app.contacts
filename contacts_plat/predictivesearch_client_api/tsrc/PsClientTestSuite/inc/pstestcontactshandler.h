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
* Description:  Contacts handler for client API test suite
*
*/


#ifndef C_PHONE_CONTACTS_FETCHER_H
#define C_PHONE_CONTACTS_FETCHER_H

//SYSTEM INCLUDES
#include <f32file.h>
#include <badesca.h>
#include <e32std.h>
#include <e32cmn.h>
#include <mvpbkcontactstorelistobserver.h>
#include <MVPbkContactObserver.h>
#include <MVPbkBatchOperationObserver.h>
#include <CPSRequestHandler.h>
#include<eikenv.h>
#include<coemain.h>

//USER INCLUDES
#include "psclienttestsuitedefs.h"

// FORWARD DECLARATIONS
class CVPbkContactManager;
class CVPbkContactStoreUriArray;
class CVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MVPbkContactLinkArray;
class MVPbkBaseContact;
class CVPbkFieldTypeRefsList;

/**
 *  This class performs all the operation related to the virtual phonebook
 *
 *  @since S60 v3.2
 */
class CPcsTestContactsHandler: public CActive,
                        public MVPbkContactStoreListObserver ,
                        public MVPbkContactObserver,
                        public MVPbkBatchOperationObserver
{	
public:

   /**
    * 1st phase constructor 
    *
  	* @return returns pointer to the constructed object of type CPcsContactFetch
	*/
	static CPcsTestContactsHandler* NewL(TBool SimStoreOnly=EFalse);
	
   /**
    * Destructor
    * 
    */
    virtual ~CPcsTestContactsHandler();
   
public:

//From CActive
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
	void RunL() ;
	
	
	/**
	* From CActive 
	* If the RunL function leaves,
	* then the active scheduler calls RunError() to handle the leave.
	* @param aError - The error code
	*/
	TInt RunError(TInt aError) ;


// From base class MVPbkContactStoreListObserver

	/**
	* From MVPbkContactStoreListObserver  
	* Called when the opening process is complete, ie. all stores have been reported
	* either failed or successfully opened.
	*
	*/
	void OpenComplete();  

	/**
	* From MVPbkContactStoreListObserver  
	* Called when a contact store is ready to use.
	*/
	void StoreReady(MVPbkContactStore& aContactStore);

	/**
	* From MVPbkContactStoreListObserver  
	* Called when a contact store becomes unavailable.
	* Client may inspect the reason of the unavailability and decide whether or not
	* it will keep the store opened (ie. listen to the store events).
	*
	* @param aContactStore The store that became unavailable.
	* @param aReason - The reason why the store is unavailable.
	*                  This is one of the system wide error codes.
	*/
	void StoreUnavailable(MVPbkContactStore& aContactStore, TInt aReason);

	/**
	* From MVPbkContactStoreListObserver  
	* Called when changes occur in the contact store.
	* @see TVPbkContactStoreEvent
	*
	* @param aContactStore The store the event occurred in.
	* @param aStoreEvent   Event that has occured.
	*/
	void HandleStoreEventL(MVPbkContactStore& aContactStore, TVPbkContactStoreEvent aStoreEvent);

	/**
	* From MVPbkContactObserver
	* Called when a contact operation has succesfully completed.
	*
	* @param aResult   The result of the operation. 
	*/
	void ContactOperationCompleted(TContactOpResult aResult);

	/**
	* From MVPbkContactObserver
	* Called when a contact operation has failed.
	*
	* @param aOpCode           The operation that failed.
	* @param aErrorCode        System error code of the failure.
	*							KErrAccessDenied when EContactCommit 
	*							means that contact hasn't been locked.
	* @param aErrorNotified    ETrue if the implementation has already
	*                          notified user about the error, 
	*                          EFalse otherwise. 
	*/
	void ContactOperationFailed(TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified);

	/**
	* From MVPbkBatchOperationObserver
	* Called when one step of the operation is completed.
	*
	* @param aOperation Operation whose step has completed
	* @param aStepSize Size of the performed step
	*/
	void StepComplete(MVPbkContactOperationBase& aOperation, TInt aStepSize );


// From MVPbkBatchOperationObserver
	/**
	* From MVPbkBatchOperationObserver
	* Called when one step of the operation fails.
	*
	* @param aOperation Operation whose step has failed
	* @param aStepSize Size of the performed step
	* @param aError Error that occured
	* @return ETrue if the batch operation should continue, 
	*               EFalse otherwise
	*         NOTE! If returning ETrue the operation can NOT be deleted.
	*               Operation should be deleted in OperationCompleted() 
	*               function.
	*               If returning EFalse the operation can be deleted
	*               safely.
	*/
	TBool StepFailed(MVPbkContactOperationBase& aOperation, TInt aStepSize, TInt aError );

	/**
	* From MVPbkBatchOperationObserver
	* Called when operation is completed.
	* This is called when all steps are executed. If EFalse is returned
	* in StepFailed() call this function is NOT called.
	*
	* @param aOperation    the completed operation
	*/
	void OperationComplete(MVPbkContactOperationBase& aOperation );
	    
public:                

	/**
	* Creates one contact in the phonebook
	*
	* @param aInputCacheData - The contacts data
	*/
	void CreateOneContactL(cacheData& aInputCacheData);    
	
	/**
	* Deletes one contact in the phonebook
	*
	* @param aContactId - The contact id to be deleted
	*/
	void DeleteOneContactL(TInt aContactId);
	
	/**
	* Deletes all the contacts from the phonebook
	*
	*/
	void DeleteAllCreatedContactsL();
	
	
	/**
	* Adds a group to the phonebook
	*
	*/  
	void AddGroupL(const TDesC& aGroupName);


	/**
	* Deletes contacts from the phonebook 
	*
	* @param aContactIdArray - The array of contact ids to be deleted
	*/
	void DeleteContactsWithIdsL(RArray<TInt>& aContactIdArray);
	
	/**
    *  Tests if Id returns correct vpbklink
    * 
    * @param aPsClientHandler -  PsClientHandler instance
    * @param srchId -  Id to be tested\
    * @return - ETrue if Id returns correct link, else EFalse
    */
    TBool TestVpbkLinkForIdL(CPSRequestHandler& aPsClientHandler,
    												const CPsClientData& aSearchResult);
	/**
    *  Adds contacts to the group
    * 
    */

    void AddCreatedContactsToGroup();
private:	
   /**
    * Constructor
    *
    */
    CPcsTestContactsHandler();
    
   /** 
    * 2nd phase constructor
    *
    * @param SimStoreOnly
    */
    void ConstructL(TBool SimStoreOnly=EFalse);
  
    /**
	* Stop mail scheduler if all stop conditions are fulfilled
	*/
    void StopSchedulerIfNothingToWaitFor();
    
private: // Data
    
    // Flags for  store operations
    TBool iAtLeastOneStoreReady;
    TBool iOpenComplete;
    TBool iOperationComplete;
    
    /*
    * The contact manager
    */
    CVPbkContactManager*  iContactManager;
    
    /*
    * The store contact
    */
    MVPbkStoreContact* iStoreContact;
    
    /*
    * The contact store
    */
    MVPbkContactStore* iStore;
    
    /*
    * The field type reference list
    */
    CVPbkFieldTypeRefsList* fieldTypeRefList;
    
    
    /*
	* The array of contactlinks to be deleted
	*/
    CVPbkContactLinkArray* iContactLinksToDelete;
    
    /*
	* The contact operation base
	*/
    MVPbkContactOperationBase* iOp;
    
    /*
	* The array of contact links of the created contacts
	*/
    CVPbkContactLinkArray *iContactsCreatedLinks;
    CVPbkContactLinkArray * iContactLinksToAddToGroup;
	
	/*
	* The contact group being added
	*/
    MVPbkContactGroup* iGroupAdded;

	/*
	* The Active scheduler wait
	*/
    CActiveSchedulerWait*	iWait;
    
    /*
	* Test should be ended if store event in this variable didn't come
    */
	TInt iStoreEventToWaitFor;//will be compared to TVPbkContactStoreEvent::TVPbkContactStoreEventType
    TBool iExpectedStoreEventReceived;
    
};

#endif //C_PHONE_CONTACTS_FETCHER_H
