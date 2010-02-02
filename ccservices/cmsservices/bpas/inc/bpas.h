/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class handles presence related tasks for
*                branding
*
*/



#ifndef C_BPAS_H
#define C_BPAS_H

#include <e32base.h>
#include <badesca.h>
#include <presencecachereadhandler2.h>
#include <spdefinitions.h>
#include <mpresencebuddyinfo2.h>

class MBPASObserver;
class CBPASInfo;
class CBPASPresenceInfo;
class CBPASServiceInfo;
class CBPASIconInfo;
class CSPSettings;
class MPresenceCacheReader2;
class MPresenceBuddyInfo2;


    /** 
     * BPAS presence data type
     */
    enum EBPASPresenceType
        {
        EBPASPresenceTypeNotDefined,
        EBPASPersonPresence,
        EBPASDevicePresence,
        EBPASServicePresence,
        };
        
    /** 
     * BPAS element type
     */
    enum EBPASElementType
        {
        EBPASElementTypeNotDefined,
        EBPASElementImage,
        EBPASElementText,
        };

/**
 *  This is a middleware dll which is used to get branded presence.
 *  It uses presence cache and spsettings to do the job
 *  ?more_complete_description
 *
 *  @lib bpas.lib
 *  @since S60 v3.2
 */
class CBPAS : public CActive,
              public MPresenceCacheReadHandler2
    {

public:

    /**
     * Public constructor. 
     */
    IMPORT_C static CBPAS* NewL();
    
    /**
     * Public constructor. 
     *
     *@param aObserver      [in] Reference to the MBPASObserver
     *                      interface implementation class.
     */
    IMPORT_C static CBPAS* NewL( MBPASObserver* aObserver );
    
    
    /**
     * Public destructor. 
     */
    virtual ~CBPAS();
    
    
    /**
     * Set observer. 
     *
     *@param aObserver      [in] Reference to the MBPASObserver
     *                      interface implementation class.
     */
    IMPORT_C void SetObserver( MBPASObserver* aObserver );
                
    /**
     * Synchronous
     *
     * @since S60 ?S60_version
     * @param aContacts             [in] contact string array
     * @param aInfos                [out] result array
     *                              Ownership is not transferred.
     * @return ?description
     */
    IMPORT_C void GiveInfosL( MDesCArray& aContacts, 
                              RPointerArray<CBPASInfo>& aInfos );
                                    

     
    /**
     * Asynchronous, user of this method needs to implement
     * MBPASObserver interface and give reference to it
     *
     * Leaves with KErrNotSupported if the observer doesn't exist
     * Leaves with KErrInUse if prosessing of the previous request 
     * is not ready.
     * @since S60 ?S60_version
     * @param aContacts            [in] contact string array
     * @return ?description
     */
    IMPORT_C void GiveInfosL( MDesCArray& aContacts );
    
    
    /**
     * Subscribe given contacts for presence changes. User of this method needs
     * to implement MBPASObserver interface and give reference to it using
     * SetObserver method. All notifications are delivered to the observer's 
     * HandleSubscribedInfoL method. If observer is changed later, all presence
     * changes previously subscribed or subscribed in future, will be delivered
     * to the new observer. The contacts should be in xsp id format.
     *
     * This method varifies either that given xsp id's service is found from the
     * device's sp table. Otherwise
     * ids are ignored.
     * 
     * @param aContacts contact string array, ids in xsp id format
     */                                   
     IMPORT_C void SubscribePresenceInfosL( MDesCArray& aContacts);
     
    
    /**
     * UnSubscribe given contact for presence changes. The Presence Change
     * notification for these contacts will no longer be delivered. Contacts
     * which are not subscribed are ignored. The contacts should be in xsp id 
     * format. The contacts should be in xsp id format.
     *
     * This method varifies either that given xsp id's service is found from the
     * device's sp table. Otherwise
     * ids are ignored.
     * 
     * @param aContacts contact string array, ids in xsp id format
     */                                   
     IMPORT_C void UnSubscribePresenceInfosL( MDesCArray& aContacts);
             
    
    /**
     * Synchronous
     *
     * @since S60 ?S60_version
     * @param aContacts             [in] 
     * @param aInfos                [out] 
     *                              Owner
     * @return ?description
     */
    void CreateInfosL( RPointerArray<CBPASInfo>& aInfos );
    
    
    /**
     * Synchronous
     *
     * @since S60 ?S60_version
     * @param aContacts             [in] contact string array
     * @param aInfos                [out] result array
     *                              Ownership is not transferred.
     * @return ?description
     */
    void GivePresenceInfosL( MDesCArray& aContacts, 
                             RPointerArray<CBPASPresenceInfo>& aInfos );
                                    

     /**
     * Synchronous
     *
     * @param aBrandId                [in] brand id
     * @param aServiceId              [in] element id
     * @param aIconHandle             [out] icon info
     * @return 
     */
    CBPASIconInfo* GiveIconInfoL( const TDesC8& aImageId, 
                                  const TDesC8& aTextId );                                        

    /**
     * Synchronous
     *
     * @since S60 ?S60_version
     * @param aPresenceType             [in] presence type
     * @param aElementType              [in] element type
     * @param aAttribute                [in] presence attribute
     * @param aAttributeValue           [in] presence attribute value
     * @return ?description
     */                                   
     HBufC8* CreateElementIdLC( EBPASPresenceType aPresenceType, 
                                EBPASElementType aElementType, 
                                const TDesC& aAttributeValue );
     
     //NULL if not found and in that case not pushed to cleanup
     CBPASServiceInfo* ReadServiceDataLC(TInt aServiceId);
                                
    
public:     // From CActive

    /**
     * Handles an active object’s request completion event.
     * Gets data and notifies observers.
     */
    void RunL();
    

    /**
     * Handles a leave occurring in the RunL().
     *
     * @param aError The leave code from RunL().
     * @return aError
     */
    TInt RunError( TInt aError );


    /**
     * Implements cancellation of an outstanding request.
     * This function is called as part of the active object’s Cancel().
     */
    void DoCancel();
    
    
public:     // From MPresCacheReadHandler2
        
    void HandlePresenceReadL(TInt aErrorCode,
         RPointerArray<MPresenceBuddyInfo2>& aPresenceBuddyInfoList);
                           
    void HandlePresenceNotificationL(TInt aErrorCode,
        MPresenceBuddyInfo2* aPresenceBuddyInfo);    

    
private:

    CBPAS( MBPASObserver* aObserver );

    void ConstructL( );
    
    
    void ReadPresenceInfosL( MDesCArray& aContacts, 
                             RPointerArray<CBPASPresenceInfo>& aInfos );
                             
    void SetPresenceInfosL( RPointerArray<CBPASInfo>& aInfos );
                                                              
    void SetServiceInfosL( RPointerArray<CBPASInfo>& aInfos );
    
    void SetIconInfosL( RPointerArray<CBPASInfo>& aInfos );
     
    void ReadAndCreatePresenceInfoL( const TDesC& aContact,
                                     RPointerArray<CBPASPresenceInfo>& aInfos );
            
    
    /**
     * Get service id from the given Uri. the Uri has possibly xsp id prefix
     * using which it tries to find service id. If service id is found then
     * ETrue is returned
     *
     * @param aServiceId, found service id returned here or KErrNotFound [out]
     * @param aUri, complete uri as read from contact database [in]
     * @return ETrue if service id is found.
     */
    TBool GetServiceId(TInt& aServiceId, TPtrC aUri);
    
    /**
     * Get serviceId from xsp id. e.g. (msn)
     *
     * @param aXsp XSP id
     * @return service id or KErrNotFound.
     */
    TInt XspToServiceId(TPtrC aXsp);
    
    /**
     * Read service table data and store locally to iSPIds and iSPNames
     *
     * @param none
     */
    void ReadSPSettingsToLocalStoresL();
    
    /**
     * Verify whether given service id is available in sptable or not, return its index
     * @param aServiceId
     * @return index of service id in iSPIds, or KErrNotFound
     */
    TInt VerifyServiceIdL(TInt aServiceId);                
   
private:


    /** 
     * BPAS States
     */
    enum EBPASState
        {
        EBPASIdle,
        EBPASProcessing
        };
        

    MBPASObserver*          iObserver;
    TInt                    iState;
    CPtrCArray*             iContacts;
    CActiveSchedulerWait*   iSchedulerWait;
    CSPSettings*            iSettings;    
    MPresenceCacheReader2*   iCacheReader;
    
    RIdArray iSPIds;
    CDesCArray* iSPNames;
    };




#endif // C_BPAS_H
