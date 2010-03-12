/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wrapper for CMS contact fetching
*
*/


#ifndef C_CCACMSCONTACTFETCHERWRAPPER_H
#define C_CCACMSCONTACTFETCHERWRAPPER_H

#include <VPbkFieldTypeSelectorFactory.h>
#include "cmsnotificationhandlerapi.h"

class CCCAParameter;
class RCmsContact;
class RCmsSession;
class CCmsContactFieldInfo;

/**
 * Interface-class to notify observers about contact data fields
 *
 * Inherit from this class and start getting notifications.
 *
 *  @code
 *   See CCCAppCmsContactFetcherWrapper
 *  @endcode
 *
 *  @lib ccappcommlauncherplugin.dll
 *  @since S60 v5.0
 */
class MCCAppContactFieldDataObserver
    {
public:

    /**
     * Simple class to transfer parameters through observer
     * interface. 
     *
     * @since S60 v5.0
     */  
    class TParameter
        {
    public:

        enum TNotifyType
            {
            EUninitialised,
            EContactInfoAvailable,
            EContactDataFieldAvailable,
            EContactsChanged,
            EContactDeleted,
            EContactPresenceChanged
            };

        inline TParameter():
            iType( EUninitialised ),
            iContactInfo( NULL ),
            iContactField( NULL ),
            iStatusFlag( 0 ){};
    
        /*  
         * With iType the observer knows what data is available.
         *
         * @since S60 v5.0
         */ 
        TNotifyType iType;
        /*  
         * With iStatusFlag the observer knows are fetching operations
         * completed. In the last fetched field notify this flag is set
         * as EOperationsCompleted (from CCCAppCmsContactFetcherWrapper::TOperationState)
         * 
         * @since S60 v5.0
         */ 
         TInt iStatusFlag;
         /*  
         * Only the one member indicated by TNotifyType above
         * will have valid data. Others will be NULL.
         *
         * Note: Ownership of the objects stays in 
         * CCCAppCmsContactFetcherWrapper-class
         *
         * @since S60 v5.0
         */  
        CCmsContactFieldInfo* iContactInfo;
        CCmsContactField* iContactField;
        };
   
    /**
     * Observer for CCCAppCmsContactFetcherWrapper -class.
     * This callback is triggered when there is information
     * available about the enabled fields of contact or 
     * contact field is fetched.
     *
     * Note: Ownership of the object stays in 
     * CCCAppCmsContactFetcherWrapper-class
     *
     * @since S60 v5.0
     * @param aParameter containing the needed data
     */   
    virtual void ContactFieldDataObserverNotifyL( 
        MCCAppContactFieldDataObserver::TParameter& aParameter ) = 0;

    /**
     * Observer for CCCAppCmsContactFetcherWrapper -class.
     * This callback is triggered when there has been or
     * are errors during contact data fetching. If there has
     * been errors before observer registers itself, this
     * callback will be called immediately.
     *
     * @since S60 v5.0
     * @param aState state of CCCAppCmsContactFetcherWrapper-class
     * @param aError error occured
     */  
    virtual void ContactFieldDataObserverHandleErrorL( 
        TInt aState, TInt aError ) = 0;

    };

/**
 * This class encapsulates the contact fetching functionality from CMS.
 *
 *  @code
 *
 *  // Basic usage
 *  // create the instance first
 *  iCmsWrapper = CCCAppCmsContactFetcherWrapper::InstanceL();//not owned
 *
 *  // 1st get whatever fetched so far
 *  const CCmsContactFieldInfo* info = iCmsWrapper->ContactInfo();//not owned
 *  if ( info )// if NULL, info fetching not completed
 *      {
 *      HandleTheInfoAsBestForYouL( *info );
 *      // if info is fetched, there can be actual fields fetched also
 *      RPointerArray<CCmsContactField> fieldArray = iCmsWrapper->ContactFieldDataArray();//not owned
 *      const TInt count = fieldArray.Count();
 *      for ( TInt i = 0; i < count; i++ )
 *          {
 *          HandleTheContactDataFieldAsYouLikeL( *fieldArray[i] );
 *          }
 *      }
 *
 *  // 2nd register as observer for following events
 *  iCmsWrapper->AddObserverL( *this );
 *
 *  // remember to remove observer when not needed anymore
 *
 *  iCmsWrapper->RemoveObserver( *this );
 *  iCmsWrapper->Release();
 *
 *  @endcode
 *
 *  @lib ccapputil.dll
 *  @since S60 v5.0
 */ 
class CCCAppCmsContactFetcherWrapper : public CActive, public MCmsNotificationHandlerAPI
    {
#ifdef __CCAPPUNITTESTMODE
    friend class T_CCCACmsContactFetcherWrapper;
#endif// __COMMLAUNCHERPLUGINUNITTESTMODE   

public: // Construction and destruction

    enum TWrapperParam
        {
        EDefaultFunctionality,
        /**
         * When this is used the contact information is searched also
         * from contact stores not defined in contact link
         */
        EFindContactFromOtherStores = 1
        };
    
    /**
     * Creates a new instance of this class. This should be used only
     * during the lifetime of CCApplication.
     *
     * Note: Deletion of this object is done by calling Release()
     *
     * @param aParameter contains the contact related information     
     * @param aWrapperParam contains wrapper specific parameters 
     * @return pointer to a new instance of this class
     */
    IMPORT_C static CCCAppCmsContactFetcherWrapper* CreateInstanceL( 
        CCCAParameter* aParameter, TWrapperParam aWrapperParam = EDefaultFunctionality );


public: //new

    /**
     * States of the class
     *
     * @since S60 v5.0
     */ 
    enum TOperationState
        {
        EInitial,
        EOpeningContact,
        EFindingFromOtherStores,
        ELoadingEnabledFields,
        ELoadingFields,
        EOperationsCompleted,
        ESettingVoiceCallDefault
        };

    /**
     * Returns with pointer to already created instance. 
     *
     * Note: Deletion of this object is done by calling Release()
     *
     * @return pointer to a instance of this clas.
     */
    IMPORT_C static CCCAppCmsContactFetcherWrapper* InstanceL();
    
     /**
     * Adds observer to start receiving MCCAppContactFieldDataObserver-events
     *
     * @since S60 v5.0
     */
    IMPORT_C void Release();

    /**
     * Getter function for the CCmsContactFieldInfo. Returns NULL if
     * not fetched yet.
     * 
     * Note: Ownership of the object stays in 
     * CCCAppCmsContactFetcherWrapper-class
     *
     * @since S60 v5.0
     * @return CCmsContactFieldInfo contains info all the available fields
     */ 
    IMPORT_C const CCmsContactFieldInfo* ContactInfo();

    /**
     * Returns contact field array.
     *
     * Note: Ownership of the object stays in 
     * CCCAppCmsContactFetcherWrapper-class
     *
     * @since S60 v5.0
     */    
    IMPORT_C RPointerArray<CCmsContactField>& ContactFieldDataArray();

    /**
     * Returns packed contact identifiers
     *
     * @since S60 v5.0
     */    
    IMPORT_C HBufC8* ContactIdentifierLC();

    /**
     * Adds observer to start receiving MCCAppContactFieldDataObserver-events.
     * If there has been errors already, ContactFieldDataObserverHandleErrorL-
     * callback will be called immediately.
     *
     * @since S60 v5.0
     */    
    IMPORT_C void AddObserverL( MCCAppContactFieldDataObserver& aObserver );

    /**
     * Removes observer from observer queue
     *
     * @since S60 v5.0
     */    
    IMPORT_C void RemoveObserver( MCCAppContactFieldDataObserver& aObserver );
    
    /**
     * Checks if the service is available
     *
     * @since S60 v5.0
     * @param aContactAction Type of the service
     * @return TInt 1 if there is a service available, 0 otherwise. Voip is 
     * a special case having more levels than just on/off.
     */
    IMPORT_C TInt IsServiceAvailable( 
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction );
    
    /**
     * Check from which store this contact is from
     *
     * @since S60 v5.0
     * @return store type containing this contact
     */
    IMPORT_C TCmsContactStore ContactStore() const;
    
    /**
     * Gets contact action field count for a current contact.
     * 
     * @param aContactAction contact action
     */
    IMPORT_C TInt GetContactActionFieldCount(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction);
    
    /**
    * From MCmsNotificationHandlerAPI
    * (see details from baseclass )
    */
    virtual void HandlePresenceNotificationL( CCmsContactField* aField );
    
    /**
     * From MCmsNotificationHandlerAPI
     * (see details from baseclass )
     */
    virtual void HandlePhonebookNotificationL( TCmsPhonebookEvent aNotificationType );
    
    /**
     * From MCmsNotificationHandlerAPI
     * (see details from baseclass )    
     */    
    virtual void CmsNotificationTerminatedL( TInt aStatus, 
        CCmsContactFieldItem::TCmsContactNotification aNotificationType );
    
    /**
     * Reset the associated contact from phonebook
     */
    virtual void RefetchContactL();

	/**
     * Checks whether the Current contact is top contact.
     *
     * @return ETrue if aContact it top contact, otherwise EFalse 
     */
    IMPORT_C TBool IsTopContact();

private:// new

    /**
     * Starts the fetching.
     *
     * @since S60 v5.0
     */ 
    void StartFetcherL( TBool aSetDefault = EFalse );        
    
    /**
     * Opens the contact from contact db
     *
     * @since S60 v5.0
     */ 
    void OpenContactL();

    /**
     * Starts the asynchronic part of the fetching.
     *
     * @since S60 v5.0
     */    
    void StartAsyncFetchingL();     
    
    /**
     * Finds the possible matches from other contact stores than
     * the one defined in contact link
     *
     * @since S60 v5.0
     */ 
    void FindFromOtherStoresL();    
    
    /**
     * Starts the fetching of enabled fields -operation.
     *
     * @since S60 v5.0
     */ 
    void FetchContactInfoL();
    
    /**
     * Informs observers about the change. Then start fetching
     * concrete data fields.
     *
     * @since S60 v5.0
     */       
    void HandleContactInfoUpdateL();

    /**
     * Depending on the iNeededContactData-array constructed
     * by MapPreferredCommMethodsToAddressFieldsL will start
     * the contact data field fetching.
     *
     * @since S60 v5.0
     */        
    void StartContactDataFieldFetchingL();

    /**
     * Fetches one contact data field set using CMS.
     * A MCCAppCLContactChangeObserver::ContactChangedNotifyL 
     * is used to notify when information has been fetched.
     *
     * @since S60 v5.0
     */        
    void FetchContactDataFieldL();

    /**
     * Updates the contact data storage and informs
     * observers about the change. Then start fetching
     * next field or completes the operation.
     *
     * @since S60 v5.0
     */        
    void HandleContactDataFieldUpdateL();
 
     /**
     * Notifies the observers.
     *
     * @since S60 v5.0
     */ 
    void NotifyErrorL();
    
    /**
    * Set default for voice call.
    *
    * @since S60 v5.0
    */
    void SetDefaultForVoiceCallL();
private: // from base class CActive
 
    /**
     * From CActive
     */
    void RunL();

    /**
     * From CActive
     */
    TInt RunError( TInt aError );

    /**
     * From CActive
     */
    void DoCancel();

private: // construction

    CCCAppCmsContactFetcherWrapper( CCCAParameter* aParameter, TWrapperParam aWrapperParam );
    void ConstructL();
    /**
     * Destructor.
     */
    ~CCCAppCmsContactFetcherWrapper();

private: // data
    
    /**
     * CCA parameter.
     * Not own.
     */    
    CCCAParameter* iParameter;
    /**
     * CMS session
     * Own.
     */
    RCmsSession iCmsSession;
    /**
     * Data field fetcher
     * Own.
     */    
    RCmsContact iCmsContactDataFetcher;
    /**
     * The internal state of this class
     * Own.
     */
    TOperationState iHandlerState;
    /**
     * Contact data field used when fetched new field
     * Not own.
     */
    CCmsContactField* iContactField;
    /**
     * Index for the iNeededContactData-array
     * Own.
     */    
    TInt iContactDataFetchingIndex;
    /**
     * Pointer to enabled fields.
     * Own.
     */
    CCmsContactFieldInfo* iContactInfo;
    /**
     * Pointer to already fetched contact data fields.
     * Own.
     */
    RPointerArray<CCmsContactField> iContactFieldsArray;
    /**
     * Pointer to observers.
     * Not owed.
     */
    RPointerArray<MCCAppContactFieldDataObserver> iObservers;
    /**
     * Member to store error code
     * Own.
     */
    TInt iErrorsOccured;
    /**
     * Singleton reference counter.
     * Own.
     */
    TInt iRefCount;
    /**
     * Info related to wrapper
     * Own.
     */
    TInt iWrapperParam;
    };


#endif // C_CCACMSCONTACTFETCHERWRAPPER_H

// End of File
