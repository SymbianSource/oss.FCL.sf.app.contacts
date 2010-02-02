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
* Description:  Fetches the presence id of a certain contact link
*
*/


#ifndef C_CLOGSEXTPRESENTITYIDFETCHER_H
#define C_CLOGSEXTPRESENTITYIDFETCHER_H

#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactStoreObserver.h>

#include "logwrap.h"

class MLogsExtPresentityIdFetcherObserver;
class MVPbkContactOperationBase;
class MVPbkContactLinkArray;
class CVPbkContactManager;
class MVPbkContactStore;
class CSPSettings;

/**
 * Presentity Id fetcher class.
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS(CLogsExtPresentityIdFetcher) : 
        public CBase,
        public MVPbkSingleContactOperationObserver,        
        public MVPbkContactStoreObserver
    {
    
    friend class ut_clogsextfetchdonelistener;
    
public:     
        
    /**
     * Creates a new CLogsExtPresentityIdFetcher.
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aCntLink a contact link
     * @param aLogId the log id of the log event that this presentity id is 
     *        fetched for
     * @param aObserver the observer of this fetching process
     * @return a new CLogsExtPresentityIdFetcher instance
     */       
    static CLogsExtPresentityIdFetcher* NewL( 
        const TUint32 aServiceId, 
        const TDesC8& aCntLink,
        const TLogId aLogId,
        MLogsExtPresentityIdFetcherObserver& aObserver );

    /**
     * Creates a new CLogsExtPresentityIdFetcher.
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aCntLink a contact link 
     * @param aLogId the log id of the log event that this presentity id is 
     *        fetched for
     * @param aObserver the observer of this fetching process
     * @return a new CLogsExtPresentityIdFetcher instance
     */        
    static CLogsExtPresentityIdFetcher* NewLC( 
        const TUint32 aServiceId,
        const TDesC8& aCntLink,
        const TLogId aLogId,
        MLogsExtPresentityIdFetcherObserver& aObserver );
        
    /**
     * Destructor.
     */
    ~CLogsExtPresentityIdFetcher();        

public: 
    
    /**
     * Starts the fetching of the presentity id.
     *
     * @since S60 v3.2
     * @return system-wide error code
     */           
    TInt Fetch();

    /**
     * Returns the service id.
     *
     * @since S60 v3.2
     * @return the service id
     */ 
    TUint32 ServiceId();
    
    /**
     * Returns the unique log event id the presentity id is fetched for.
     *
     * @since S60 v3.2
     * @return the unique log event id
     */
    TLogId LogId();
    
    /**
     * Returns the presentity id.
     *
     * @since S60 v3.2
     * @return the presentity id
     */
    const TDesC& PresentityId();    

private:

// from base class MVPbkSingleContactOperationObserver

    /**
     * From MVPbkSingleContactOperationObserver
     * Called when the operation is completed.
     *
     * @since S60 v3.2
     * @param aOperation The completed operation.
     * @param aContact The contact returned by the operation.
     *                 A client must take the ownership immediately.
     *
     */
    void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );

    /** 
     * From MVPbkSingleContactOperationObserver
     * Called if the operation fails.
     *
     * @since S60 v3.2
     * @param aOperation The failed operation.
     * @param aError An error code of the failure.
     */
    void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation, 
            TInt aError );

private:

// from base class MVPbkContactStoreObserver

    /** 
     * From MVPbkContactStoreObserver
     * Called when a contact store is ready to use.
     *
     * @since S60 v3.2
     * @param aContactStore The store that is ready.
     */
    void StoreReady( MVPbkContactStore& aContactStore );

    /** 
     * From MVPbkContactStoreObserver
     * Called when a contact store becomes unavailable.
     *
     * Client may inspect the reason of the unavailability and decide
     * whether or not it will keep the store opened (ie. listen to 
     * the store events).
     *
     * @since S60 v3.2
     * @param aContactStore The store that became unavailable.
     * @param aReason The reason why the store is unavailable.
     *                This is one of the system wide error codes.
     */
    void StoreUnavailable( MVPbkContactStore& aContactStore, TInt aReason );

    /** 
     * From MVPbkContactStoreObserver
     * Called when changes occur in the contact store.
     *
     * @since S60 v3.2
     * @see TVPbkContactStoreEvent
     * @param aContactStore A store whose event it is.
     * @param aStoreEvent The event that has occurred.
     */
    void HandleStoreEventL(
            MVPbkContactStore& aContactStore, 
            TVPbkContactStoreEvent aStoreEvent );
    
private:

    /**
     * Symbian second-phase constructor
     *
     * @since S60 v3.2
     * @param aCntLink a contact link
     */                    
    void ConstructL( const TDesC8& aCntLink );
    
    /**
     * Constructor.
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aLogId a unique log event id
     * @param aObserver observer of this fetcher
     */
    CLogsExtPresentityIdFetcher( 
        const TUint32 aServiceId,
        const TLogId aLogId,                               
        MLogsExtPresentityIdFetcherObserver& aObserver );
        
    /**
     * Starts the fetching. 
     *
     * @since S60 v3.2
     */           
    void DoPresenceIdFetchL();        
      
    /**
     * Retrieves the contact store id of a certain service from the service
     * provider settings table. 
     *
     * @since S60 v3.2
     * @param aSPSettings Service provider settings API      
     */      
    void GetContactStoreIdL( CSPSettings& aSPSettings );
    
    /**
     * Retrieves the FieldType of a certain service from the service
     * provider settings table.
     *
     * @since S60 v3.2
     * @param aSPSettings Service provider settings API 
     */     
    void GetPresentityIDFieldTypeL( CSPSettings& aSPSettings );
    
    /**
     * Handles the completion of an operation.
     *
     * @since S60 v3.2
     * @param aContact The contact returned by the operation.
     *                 A client must take the ownership immediately.
     */
    void HandleRetrievedContactL( MVPbkStoreContact* aContact );

    
private: // data
        
    /**
     * Service id
     */
    TUint32 iServiceId;
    
    /**
     * Field type resource id.
     */
    TInt iFieldTypeResId;
    
    /**
     * Unique event ID associated with a log event.
     */
    TLogId iLogId;
    
    /**
     * The observer of this fetcher. 
     * Not own.
     */
    MLogsExtPresentityIdFetcherObserver* iObserver;
    
    /**
     * Contact store.
     * Not own.
     */
    MVPbkContactStore* iCntStore;
    
    /**
     * Contact link. Note: this contact link descriptor should have been 
     * obtained using the MVPbkStreamable interface of the virtual phonebook
     * Own.
     */
    HBufC8* iCntLink;
    
    /**
     * Presentity Id.
     * Own.
     */
    HBufC* iPresentityId;
    
    /**
     * Contact store id.
     * Own.
     */
    HBufC* iContactStoreId;    
            
    /**
     * Contact Manager.
     * Own.
     */
    CVPbkContactManager* iContactManager;
    
    /**
     * Contact link array.
     * Own.
     */
    MVPbkContactLinkArray* iContactLinkArray;
    
    /**
     * Contact operation base.
     * Own.
     */
    MVPbkContactOperationBase* iContactOperationBase;
    
    };

#endif // C_CLOGSEXTPRESENTITYIDFETCHER_H
