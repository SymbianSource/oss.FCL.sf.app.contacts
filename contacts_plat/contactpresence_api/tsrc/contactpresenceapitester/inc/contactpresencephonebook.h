/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef __CONTACTPRESENCEPHONEBOOK__
#define __CONTACTPRESENCEPHONEBOOK__

// INCLUDES
#include <barsc.h>
#include <e32base.h>
#include <f32file.h>
#include <MVPbkContactFindObserver.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkContactStoreObserver.h>
#include <MVPbkContactStoreListObserver.h>
#include <MVPbkSingleContactOperationObserver.h>

#include <MVPbkContactObserver.h>

//Konsts
_LIT( KCmsCntDbUri,                    "cntdb://c:contacts.cdb" );

_LIT( KResourcePath, "\\resource\\cms\\cmstester.rsc" );

const TInt KNumberOfContacts = 1;

//Forwar declarations
class RFs;
class CVPbkSortOrder;
class TResourceReader;
class MVPbkContactLink;
class MVPbkContactStore;
class MVPbkStoreContact;     
class CVPbkContactManager;
class MVPbkContactViewBase;
class MVPbkContactLinkArray;
class MVPbkContactStoreList;
class MVPbkContactOperationBase;
class CVPbkContactViewDefinition;

// CLASS DECLARATION
NONSHARABLE_CLASS( CContactPresencePhonebook ) : public CBase,
                               public MVPbkContactFindObserver,
                               public MVPbkContactStoreObserver,
                               public MVPbkContactStoreListObserver,
                               public MVPbkContactObserver,                
                               public MVPbkSingleContactOperationObserver                                                                                             
                               
                               // public MVPbkContactViewObserver,
                               // public MVPbkSetAttributeObserver                               
    {
    public:  // Constructors and destructor
        
        /**
        * Symbian OS Constructor
        * @return CContactMethodSelector* 
        */
        static CContactPresencePhonebook* NewL();

        /**
        * Initialise contact stores.
        * Must be called before doing anything else
        *
        * @param TRequestStatus& Client's request status
        * @return void 
        */
        void InitStoresL( TRequestStatus& aStatus );
        
        /**
         * Create test contact
         */
        void CreateContactL( TRequestStatus& aStatus );   
        
        /**
        * Initialise contact stores.
        * Must be called before doing anything else
        *
        * @param TRequestStatus& Client's request status
        * @return void 
        */
        HBufC8* FetchLinkLC( TInt32 aContactId );
        
        /**
        * Initialise contact stores.
        * Must be called before doing anything else
        *
        * @param TRequestStatus& Client's request status
        * @return void 
        */
        TInt32 FetchContactIDL();

        /**
        * Fetch contact information
        *
        * @param TInt32 Contact ID
        * @param TRequestStatus& Client's request status
        * @return void  
        */
        void FetchContactL( TInt32 aContactId, TRequestStatus& aStatus );

        /**
        * Fetch contact information
        *
        * @param TPtrC MSISDN of the contact
        * @param TRequestStatus& Client's request status
        * @return void  
        */
        void FetchContactL( const TDesC& aMsisdn, TRequestStatus& aStatus );

        /**
        * Fetch contact information
        *
        * @param MVPbkContactLink Link to the contact
        * @param TRequestStatus& Client's request status
        * @return void  
        */
        void FetchContactL( const TDesC8& aPackedlink, TRequestStatus& aStatus );

        /**
        * Cancel an ongoing asynchronous operation
        *
        * @return void  
        */
        void Cancel();
                          
        /**
        * Destructor
        */
        virtual ~CContactPresencePhonebook();

        CVPbkContactManager* ContactManager();
    
    private:

         /**
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
        void VPbkSingleContactOperationComplete( MVPbkContactOperationBase& aOperation,
                                                 MVPbkStoreContact* aContact );

         /**
         * Called if the operation fails.
         *
         * @param aOperation    the failed operation.
         * @param aError        error code of the failure.
         */
        void VPbkSingleContactOperationFailed( MVPbkContactOperationBase& aOperation, TInt aError );

    private:  //From MVPbkContactStoreListObserver

        /**
        * Called when the opening process is complete, ie. all stores have been reported
        * either failed or successful open.
        */
        void OpenComplete();
    
    private:  //From MVPbkContactFindObserver
        
        /**
        * Called when the opening process is complete, ie. all stores have been reported
        * either failed or successful open.
        */
        void FindFailed( TInt aError );

        /**
        * Called when the opening process is complete, ie. all stores have been reported
        * either failed or successful open.
        */
        void FindCompleteL( MVPbkContactLinkArray* aResults );
        
    private:  //From MVPbkContactStoreObserver

        /**
        * Called when a contact store is ready to use.
        */
        void StoreReady( MVPbkContactStore& aContactStore );

        /**
        * Called when a contact store becomes unavailable.
        * Client may inspect the reason of the unavailability and decide whether or not
        * it will keep the store opened (ie. listen to the store events).
        * @param aContactStore The store that became unavailable.
        * @param aReason The reason why the store is unavailable.
        *                This is one of the system wide error codes.
        */
        void StoreUnavailable( MVPbkContactStore& aContactStore, TInt aReason );

        /**
         * Called when changes occur in the contact store.
         * @see TVPbkContactStoreEvent
         * @param aContactStore The store the event occurred in.
         * @param aStoreEvent   Event that has occured.
         */
        void HandleStoreEventL( MVPbkContactStore& aContactStore, TVPbkContactStoreEvent aStoreEvent );
        
    private: // from MVPbkContactObserver  
        
        void ContactOperationCompleted(TContactOpResult aResult);


        void ContactOperationFailed(TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified);

     

    private:
        
        /**
        * C++ constructor
        * @return CXmlFormatter
        */
        CContactPresencePhonebook();
        
        /**
        * Symbian OS second-phase constructor
        * @return void
        */
        void ConstructL();
        
        void ResetData();

        void ResetData( MVPbkContactOperationBase& aOperation );
        
        void DoCreateContactL();        
        
    private: //Data
        
        enum TSelectorState
            {
            ESelectorIdle = 0,
            ESelectorInit,
            ESelectorFetchContact,
            ESelectorCreateContact
            };

        RFs                                 iFileSession;
        TBool                               iStoreReady;
        TSelectorState                      iSelectorState;
        TRequestStatus*                     iClientStatus;
        MVPbkContactLink*                   iContactLink;
        MVPbkContactStoreList*              iStoreList;
        MVPbkContactStore*                  iContactStore;
        MVPbkStoreContact*                  iContact;
        CVPbkContactManager*                iContactManager; 
        MVPbkContactLinkArray*              iLinkArray;
        MVPbkContactOperationBase*          iOperation;
        
        TInt                                iContactCount;        
       
    };

#endif  //__CMSTESTERPHONEBOOK__


// End of File
