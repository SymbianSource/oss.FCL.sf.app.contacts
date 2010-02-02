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
* Description:  Presence in pbonebook
*
*/


#ifndef CONTACTSTOREACCESSOR_H
#define CONTACTSTOREACCESSOR_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <MVPbkContactStoreListObserver.h>
#include <MVPbkContactStoreObserver.h>
#include <MPbk2StoreConfigurationObserver.h>

class TVPbkContactStoreEvent;
class MVPbkContactStore;
class CVPbkContactManager;
class MVPbkContactStoreList;
class CPbk2StoreConfiguration;

class CPresenceIconInfoListener;

// CLASS DECLARATION

/**
*  CContactStoreAccessor
*
*/
NONSHARABLE_CLASS( CContactStoreAccessor ): public CBase,
public MVPbkContactStoreListObserver
{
public: // Constructors and destructor

    /**
    * Destructor.
    */
    ~CContactStoreAccessor();

    /**
    * Two-phased constructor.
    */
    static CContactStoreAccessor* NewL( CVPbkContactManager* aManager, CPresenceIconInfoListener& aListener );

public: // new methods

    void OpenStoresL( TRequestStatus& aStatus );
    
    void OpenStoresL( TRequestStatus& aStatus, const TDesC8& aPackedLink  );    

    MVPbkContactStoreList* AccessStoreList();

    CVPbkContactManager* AccessContactManager();

    void CreateConfigurationL();

private:  //From MVPbkContactStoreListObserver

    /**
    * Called when the opening process is complete, ie. all stores have been reported
    * either failed or successful open.
    */
    void OpenComplete();

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
         
private:

    /**
    * Constructor for performing 1st stage construction
    */
    CContactStoreAccessor(CPresenceIconInfoListener& aListener);

    /**
    * EPOC default constructor for performing 2nd stage construction
    */
    void ConstructL( CVPbkContactManager* aManager );
    
    void DoCloseStore();    

private: // data

    TRequestStatus* iClientStatus;
    TBool iStoreReady;
    MVPbkContactStore* iContactStore;
    CVPbkContactManager* iContactManager;
    MVPbkContactStoreList* iStoreList;
    CPbk2StoreConfiguration* iStoreConfiguration;

    /**
     * Whether open request is pending
     */
    TBool iOpening;
    TBool iGivenManager;

    /**
     * Pointer to stack variable to detect the deletion of the heap
     * instance. This case takes place if a client
     * calls destructor in callback method.
     * Own.
     */
     TBool* iDestroyedPtr;
     
     /**
      * Listener for contact modifications
      */
     CPresenceIconInfoListener& iListener;            

};

#endif // CONTACTSTOREACCESSOR_H

