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


#ifndef CMSONECONTACTSTATUS_H_
#define CMSONECONTACTSTATUS_H_

#include <e32base.h>
#include <badesca.h>

#include "bpasobserver.h"

class MPresenceIconNotifier;
class MVPbkContactLink;
class CBPAS;
class CBPASInfo;
class CCmsOneServiceStatus;


/**
 *  Branded presence icons handling class.
 *
 *  @lib cmsserver.exe
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCmsOneContactStatus):  public CBase, public MBPASObserver
    {

public: 

    /**
     * Symbian constructors.
     *
     * @param aCallback callback method for notifications
     * @return CCmsBrandedIcons
     */
    static CCmsOneContactStatus* NewL( MPresenceIconNotifier& aCallback,
            MVPbkContactLink* aContact,            
            HBufC8* aDefaultTrafficLightBrandId, 
            HBufC8* aDefaultOnlineElementId, 
            HBufC8* aDefaultOfflineElementId,
            TInt aId );

    /**
     * Public destructor. 
     */
    virtual ~CCmsOneContactStatus();
    
// new methods

    /*
     * Subscribe common branded icon
     */
    void SubscribeL( MDesC16Array* aIdentities );
    
    /**
     * Get all services brandings at once
     */
    void GetAllL( MDesC16Array* aIdentities );    
              
    TBool IsSameContact( MVPbkContactLink* aLink, MPresenceIconNotifier* aCallback);
    
// from base class MBPASObserver 

    /**
     * Defined in a base class
     */
    void HandleInfosL( TInt aErrorCode, RPointerArray<CBPASInfo>& aInfos );
    
    /**
     * Defined in a base class
     */
    void HandleSubscribedInfoL(CBPASInfo* aInfo);
        
            
private:
 
    CCmsOneContactStatus( MPresenceIconNotifier& aCallback,            
            HBufC8* aDefaultTrafficLightBrandId, 
            HBufC8* aDefaultOnlineElementId, 
            HBufC8* aDefaultOfflineElementId,
            TInt aId );            
    
    void ConstructL( MVPbkContactLink* aContact );
    
// new methods    
    
    /**
     * Update presence data and send notification if needed.
     */
    void UpdateStatusL( TBool aGetAllOnce );
    
    /*
     * Update one common branded icon status
     */
    void UpdateCommonStatusL( ); 
    
    /*
     * Update all services status separately
     */
    void UpdateAllAtOnceStatus( );      
    
    void DoHandleInfosL( RPointerArray<CBPASInfo>& aInfos, TBool aDelete, TBool aGetAllOnce );
    
    void DoHandleInfoL( CBPASInfo* aInfo );    
    
    TPtrC ExtractServiceL( const TDesC& aXspId );
    
    CCmsOneServiceStatus* FindService( const TDesC& aService );
    
    void SendNotificationWhenNeededL( CCmsOneServiceStatus* aOnlyStatus );  
    
    void DoSendNotificationL( const TDesC8& aService, const TDesC8& aElement, TInt aBrandLanguage );    
        
    void RemoveService( CCmsOneServiceStatus* aService );
    
    TPtrC8 TrafficLightService();
    
    TPtrC8 TrafficLightOnline();
    
    TPtrC8 TrafficLightOffline();
    
    TPtrC8 LastBrand( );
    
    TPtrC8 LastElement( );    
    
private:

    /**
     * Callback implementation
     */
    MPresenceIconNotifier& iCallback;
    
    /* NOT OWN */
    HBufC8* iDefaultTrafficLightBrandId;
    /* NOT OWN */    
    HBufC8* iDefaultOnlineElementId;
    /* NOT OWN */    
    HBufC8* iDefaultOfflineElementId; 
    
    /**
     * Contact link
     * OWN
     */
    MVPbkContactLink* iContactLink;  
    
    /**
     * BPAS entity.
     * OWN
     */
    CBPAS* iBrandedPresence;    
        
    /**
     * Number of services available
     */
    TInt iAvailableServices;
    
    /**
     * Number of services currently online.
     */
    TInt iOnlineServices;    
    
    /**
     * services for the contact
     */
    RPointerArray<CCmsOneServiceStatus> iServices;
    
    /**
     * Last sent service
     * OWN.
     */
    HBufC8* iLastService;
    
    /**
     * Last sent element
     * OWN.
     */
    HBufC8* iLastElement;
    
    /**
     * Id to identify the request responses
     */
    TInt iId;
    };


#endif /*CCMSONECONTACTSTATUS_H_*/
