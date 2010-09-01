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
* Description:  Handles binding and unbinding to service, fetches presense of identities.
*
*/


#ifndef C_CLOGSEXTSERVICEHANDLER_H
#define C_CLOGSEXTSERVICEHANDLER_H

#include <e32base.h>
#include "mlogsextensioncchhandlerobserver.h"


#include <presencecachereadhandler2.h>
#include <mpresencebuddyinfo2.h>

class CLogsExtPresentityData;
class CLogsExtensionCchHandler;
class MLogsExtServiceHandlerObserver;
class TXIMPRequestId;


class MPresenceCacheReader2;
class MPresenceBuddyInfo2;

/**
 * Service and presence handler.
 *
 * @since S60 v3.2
 */ 
NONSHARABLE_CLASS(CLogsExtServiceHandler) : 
        public CBase,
        public MPresenceCacheReadHandler2
    {
   
public:     

    /**
     * Creates a new CLogsExtServiceHandler.
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aObserver service handler observer
     * @return a new CLogsExtServiceHandler instance
     */
    static CLogsExtServiceHandler* NewL( 
        const TUint32 aServiceId,
        MLogsExtServiceHandlerObserver& aObserver );
        
    /**
     * Creates a new CLogsExtServiceHandler.
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aObserver service handler observer
     * @return a new CLogsExtServiceHandler instance
     */    
    static CLogsExtServiceHandler* NewLC( 
        const TUint32 aServiceId,
        MLogsExtServiceHandlerObserver& aObserver );
        
    /**
     * Destructor.
     */
    ~CLogsExtServiceHandler();
    
    /**
     * Returns the service id.
     *
     * @since S60 v3.2
     * @return the service id
     */ 
    TUint32 ServiceId();   

    /**
     * Adds the specified presentity to this service handler
     * and starts to handle subscription/event handling etc.
     * for this presentity.
     *
     * @since S60 v3.2
     * @param aPresentityId a presentity
     */
    void AddPresentityL( const TDesC& aPresentityId );

public:     // From MPresCacheReadHandler2
        
    void HandlePresenceReadL(TInt aErrorCode,
         RPointerArray<MPresenceBuddyInfo2>& aPresenceBuddyInfoList);
                           
    void HandlePresenceNotificationL(TInt aErrorCode,
        MPresenceBuddyInfo2* aPresenceBuddyInfo);    

    
private:

    /**
     * Symbian second-phase constructor.
     */                    
    void ConstructL();

    /**
     * Constructor.
     */
    CLogsExtServiceHandler( const TUint32 aServiceId,
                            MLogsExtServiceHandlerObserver& aObserver );

    /**
     * Submits a request to get the content of a certain group.
     *
     * @since S60 v3.2
     * @param aGroupName the name of group to retrieve content from.
     */    
    void GetGroupContentL( const TDesC& aGroupName );
    
    /**
     * Create presentity data object for a presentity.
     *
     * @since S60 v3.2
     * @param aPresentityId a presentity to create data object for.
     */
    void DoPresentityDataCreationL(const TDesC& aPresentityId );
    
    /**
     * Tests whether presentity data object exists or not.
     *
     * @since S60 v3.2     
     * @param aPresentityId a presentity
     * @return True, if exists; False otherwise
     */
    TBool PresentityDataExists( const TDesC16& aPresentityId );
    
    /**
     * Gets presentity data object of a certain presentity.
     *
     * @since S60 v3.2
     * @param aPresentityId a presentity
     * @param aPresentityData presentity data object pointer. 
     * @return KErrNone in case data has been retrieved successfully,
     *         system-wide errorcode otherwise 
     */ 
    TInt GetPresentityData(
            const TDesC16& aPresentityId,
            CLogsExtPresentityData*& aPresentityData );
    
    /**
     * Subscribe presentity of the specified PresentityData object.
     * Subscribing presence is an asynchronous operation and it
     * might evolve network operations.
     *
     * @since S60 v3.2
     * @param aPresentityData a presentity data object 
     */       
    void DoPresentitySubscriptionL( CLogsExtPresentityData& aPresentityData );
    
    /**
     * UnSubscribe all contact for presence changes. The Presence Change
     * notification for these contacts will no longer be delivered. 
     *
     * 
     */                
    void UnSubscribePresenceInfosL();
 
    /**
     * Determines whether the service handler has to make subscriptions
     * and also initiates those. It also informs the service handler 
     * oberver in case the status of a presentity should not be displayed
     * (e.g. because the presentity is not found from the subscribed buddy
     * list anymore)
     *
     * @since S60 v3.2
     */     
    void CheckServiceHandler();

    /**
     * Accomplishes the same as CheckServiceHandler().
     *
     * @since S60 v3.2
     */ 
    void DoCheckServiceHandlerL();    

    
private: // data
        
    /**
     * Service id
     */
    TUint32 iServiceId;   
    
    /**
     * The array that contains the presence fetchers. 
     * Own. 
     */  
    RPointerArray<CLogsExtPresentityData> iPresentityDataArray;
    
    /**
     * The observer of this service handler.
     * Not own.
     */
    MLogsExtServiceHandlerObserver* iObserver;    
    
    
    /**
     * Presence cache reader
     * 
     */
    MPresenceCacheReader2*   iCacheReader;
    
    };

#endif  //  C_CLOGSEXTSERVICEHANDLER_H
