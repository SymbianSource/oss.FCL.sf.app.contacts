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


#ifndef PRESENCEICONINFOLISTENER_H
#define PRESENCEICONINFOLISTENER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>

class MContactPresenceObs;
class MPresenceTrafficLights;
class CContactStoreAccessor;
class CPresenceIconInfo;
class CVPbkContactManager;

class MVPbkContactLink;

// CLASS DECLARATION

/**
*  CPresenceIconInfoListener
*
*/
NONSHARABLE_CLASS( CPresenceIconInfoListener ) : public CActive
{
public: // Constructors and destructor

    /**
     * Destructor.
     */
    ~CPresenceIconInfoListener();

    /**
     * Two-phased constructor.
     */
    static CPresenceIconInfoListener* NewL( CVPbkContactManager* aManager, MContactPresenceObs& aObs );


public: // New methods

    /**
     * Start info subscriotion
     * @param aLinkArray packed contact links
     */
    void SubscribePresenceInfoL( const MDesC8Array& aLinkArray  );
          
    /*
     * Resubscribe
     * @param aLink contact link 
     */
    void ResubscribePresenceInfoL( MVPbkContactLink* aLink );
    
    /**
     * Get presence for all services at once
     * @param aPackedLink packed contact link
     * @param aOpId operation id used in response callback
     */
    void GetPresenceInfoL( const TDesC8& aPackedLink, TInt aOpId  );      

    /**
     * Contact store accessor
     * @return contact store
     */
    CContactStoreAccessor*  ContactStoreAccessor();

    /**
     * Cancel info subscriptions
     * @param aLinkArray packed contact links
     */
    void CancelSubscribePresenceInfo( const MDesC8Array& aLinkArray );
    
    /**
     * Cancel info subscriptions
     * @param aLink contact link 
     */
    void CancelSubscribePresenceInfo( MVPbkContactLink* aLink );    

    /**
     * Delete all subscription requests
     */
    void DeleteAllRequests();
    
    void CancelGet( TInt aOpId );    


private:

    /**
    * Constructor for performing 1st stage construction
    */
    CPresenceIconInfoListener( MContactPresenceObs& aObs );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL( CVPbkContactManager* aContactManager );

    /**
     * Search icon info handler
     * @return handler or NULL
     */
    CPresenceIconInfo* SearchIconInfo( const TDesC8& aContactLink );

    /**
     * Create icon info handler
     * @return handler
     */
    CPresenceIconInfo* CreateIconInfoL(
        const TDesC8& aContactLink, TBool aIsSubscription, TInt aOpId );

    /**
     * Create or access existing CPresenceIconInfo
     */
    CPresenceIconInfo*  GetSinglePresenceIconInfoL(
        const TDesC8& aContactLink, TBool aIsSubscription, TInt aOpId );

    /**
     * Open contact store
     */
    void OpenStoreL();
    
    /**
     * Open contact store for given link and existing Contact Manager
     * @param aPackedLink packed contact link
     */
    void OpenStoreL( const TDesC8& aPackedLink  );        

    /**
     * Start pending subscriptions
     */
    void StartPendingsL( );

    /**
     * Send callbacks to all requests
     */
    void SendErrorCallbacks( TInt aMyStatus );
    
    /*
     * Cancel current subscritpion
     * @param aLink contact link 
     * @param aResubscribe if resubscritpion is done 
     */
    void DoResubscribePresenceInfoL( MVPbkContactLink* aLink, TBool aResubscribe );    

private:  // from CActive

    void RunL();

    void DoCancel();

// data

    MContactPresenceObs& iObs;

    /**
     * Queue for info requests
     */
    TDblQue<CPresenceIconInfo> iInfoReqList;

    /**
     * OWN
     */
    MPresenceTrafficLights* iBrandedIcons;

    /**
     * Whether contact store is open
     */
    TBool iStoreOpen;

    /**
     * contact db access
     * OWN
     */
    CContactStoreAccessor* iStoreAccess;

    /**
     * Pointer to stack variable to detect the deletion of the heap
     * instance. This case takes place if a client
     * calls destructor in callback method.
     * Own.
     */
     TBool* iDestroyedPtr;

};

#endif // PRESENCEICONINFOLISTENER_H

