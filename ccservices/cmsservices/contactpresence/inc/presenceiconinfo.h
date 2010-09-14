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
* Description:  Presence in pbonebook
*
*/


#ifndef PRESENCEICONINFO_H
#define PRESENCEICONINFO_H

#include <e32std.h>
#include <badesca.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <mcontactpresence.h>
#include <vpbkeng.rsg>

#include "mpresencetrafficlightsobs.h"
#include "presenceiconinfolistener.h"


class MContactPresenceObs;
class MPresenceTrafficLights;
class CVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MContactPresenceInfo;
class CPresenceIconInfoRes;
class MPresenceServiceIconInfo;

//The types for chat
const TInt KPresenceFieldGroup[] =  { R_VPBK_FIELD_TYPE_IMPP };

/**
 * CPresenceInfoInfo
 *
 * Presence in phonebook
 *
 * @lib contactpresence
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CPresenceIconInfo ) : public CBase,
    public MVPbkSingleContactOperationObserver, public MPresenceIconNotifier
    {
public:

    /**
     * Constructor.
     * @param aObs callback observer
     * @param aPackedContactId contact id
     */
    static CPresenceIconInfo* NewL(
        CPresenceIconInfoListener& aParent,
        MContactPresenceObs& aObs,
        const TDesC8& aPackedContactId,
        MPresenceTrafficLights& aIcons,
        TBool aIsSubscription,
        TInt aOpId );

    virtual ~CPresenceIconInfo();

    /**
     * Destructor plus remove from queue
     */
    void Destroy();

    /**
     * start to get icon info
     * @param aPpId operation id used in response
     */
    void StartGetIconInfoL( );
    
    /**
     * start to get icon info once
     */
    // void StartGetIconInfoOnceL(  TInt aOpId );    

    /**
     * contact link accessor
     * @return contact link
     */
    TPtrC8 ContactLink();

    /**
     * Is this request running or waiting
     * @Return ETrue if waiting
     */
    TBool IsWaiting();

    /**
     * Send empty Notification to tell that there is no valid presence status anymore
     * This is called when contact link is modified or deleted
     */
    void SendEmptyNotification();  
    
    /**
     * Is this subscritpion or get method 
     */
    TBool IsSubscription(); 

    /**
     * Operation id accessor
     * @return operation id
     */    
    TInt OpId();
    
    /**
     * Send error callback method and delete this instance
     * @param aError error code
     */
    void SendErrorCallback( TInt aError );    

    inline static TInt LinkOffset();

public: // from base class MPresenceIconNotifier

    void NewIconForContact(
        MVPbkContactLink* aLink,
        const TDesC8& aBrandId,
        const TDesC8& aElementId,
        TInt aId,
        TInt aBrandLanguage );

    void NewIconsForContact( 
            MVPbkContactLink* aLink, 
            RPointerArray <MPresenceServiceIconInfo>& aInfoArray,
            TInt aId );


private:


// from MVPbkSingleContactOperationObserver

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

private:

    /**
     *  constructor
     */
    CPresenceIconInfo(
        CPresenceIconInfoListener& aParent,
        MContactPresenceObs& aObs,
        MPresenceTrafficLights& aIcons,
        TBool aIsSubscription,
        TInt aOpId );

    /**
     *  constructor
     */
    void ConstructL( const TDesC8& aPackedContactId );

private:

    // void OpenCmsConnectionsL();

    /**
     * Fetch contact data
     */
    void FetchContactL( );

    /**
     * Reset members
     */
    void ResetData();

    /**
     * Reset members
     */
    void ResetData( MVPbkContactOperationBase& aOperation );

    void DoVPbkSingleContactOperationCompleteL( MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );


    /**
     * Fetch relevant fields
     * @param aContact open store contact
     * @return array of field values
     */
    CDesCArrayFlat* FetchContactFieldsLC( MVPbkStoreContact* aContact );

    /**
     * Fetch a single field
     * @param aResourceId resource id
     * @param aArray array where to put findings
     * @param aContact open store contact
     */
    void FetchContactFieldL( TInt aResourceId, CDesCArrayFlat& aArray, MVPbkStoreContact* aContact  );

    /**
     * Temporary save of first notification data
     */
    void AllocatePendingNotifyL(
        const TDesC8& aBrandId,
        const TDesC8& aElementId );
    
    /**
     * Send pending callback method call
     */
    void SendPendingCallback();
       
    void DoCopyInfosL( 
            MVPbkContactLink* aLink, 
            RPointerArray <MPresenceServiceIconInfo>& aInfoArray,
            TInt aId );    
    
  

private: // Data

    /**
     * Parent
     */
    CPresenceIconInfoListener& iParent;

    TDblQueLink iLink;

    /**
     * callback observer
     */
    MContactPresenceObs& iObs;


    /**
     * Packed Contact link
     * Own.
     */
    HBufC8* iContactLink;

    /**
    * Pointer to stack variable to detect the deletion of the heap
    * instance. This case takes place if a client
    * calls destructor in callback method.
    * Own.
    */
    TBool*                                         iDestroyedPtr;

    /**
     * traffic light icon provider
     */
    MPresenceTrafficLights& iIcons;

    /**
     * Waiting store open
     */
    TBool iWaiting;

    /**
     * links
     * OWN
     */
    CVPbkContactLinkArray* iLinkArray;

    MVPbkContactOperationBase* iOperation;

    /**
     * Contact link
     * OWN
     */
    MVPbkContactLink* iPbkContactLink;
    
    /**
     * Are we in intialization, then we have to wait before sending data to callback method
     */
    TBool iInitialization;
        
    /**
     * Temporary save for pending notification, M-classes
     */
    RPointerArray <MContactPresenceInfo> iPendings;
    
    /**
     * Temporary save for pending notification, C-classes to enable deletion
     */    
    RPointerArray <CPresenceIconInfoRes> iPendings2;     
    
    TBool iNotifyPending;
    
    /** 
     * Subscription or get method
     */
    TBool iSubscription;       
      
    /**
     * Operation id used in get once method
     */
    TInt iOpId;
                 
    };

    TInt CPresenceIconInfo::LinkOffset()
        {
        return _FOFF(CPresenceIconInfo, iLink);
        }


#endif // PRESENCEICONINFO_H
