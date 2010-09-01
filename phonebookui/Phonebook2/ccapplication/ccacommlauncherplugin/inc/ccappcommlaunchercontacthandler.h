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
* Description:  Contact handling wrapper
*
*/


#ifndef C_CCAPPCOMMLAUNCHERCONTACTHANDLER_H
#define C_CCAPPCOMMLAUNCHERCONTACTHANDLER_H

// INCLUDES
#include "ccappcommlauncherheaders.h"
#include <mccacontactobserver.h>

class CCCAppCommLauncherPlugin;
class CCCAParameter;
class CCmsContactFieldInfo;
class CCCAppCommLauncherView;
class MCCAppContactFieldDataObserver;

/*
 * Small macro to ease up counting of array sizes below
 */
#define CCACOMMLAUNCHER_ROMARRAY_SIZE( aRomArray ) \
    ( sizeof( aRomArray ) / sizeof(( aRomArray )[0] ))

/*
 * Defines the addresses used for voice calls.
 * (todo; should be dynamically loaded from pbk selector resources to CMS
 * and handled by CMS CCmsContactField::GroupType() -functionality )
 * */
const TInt KCCAppCommLauncherVoiceCallGroup[] = {
    CCmsContactFieldItem::ECmsMobilePhoneGeneric,
    CCmsContactFieldItem::ECmsMobilePhoneHome,
    CCmsContactFieldItem::ECmsMobilePhoneWork,
    CCmsContactFieldItem::ECmsLandPhoneGeneric,
    CCmsContactFieldItem::ECmsLandPhoneHome,
    CCmsContactFieldItem::ECmsLandPhoneWork,
    CCmsContactFieldItem::ECmsPagerNumber,
    CCmsContactFieldItem::ECmsAssistantNumber,
    CCmsContactFieldItem::ECmsCarPhone };

/*
 * Defines the addresses used for messaging.
 * (todo; should be dynamically loaded from pbk selector resources to CMS
 * and handled by CMS CCmsContactField::GroupType() -functionality )
 * */
const TInt KCCAppCommLauncherMessageEmailGroup[] = {
    CCmsContactFieldItem::ECmsMobilePhoneGeneric,
    CCmsContactFieldItem::ECmsMobilePhoneHome,
    CCmsContactFieldItem::ECmsMobilePhoneWork,
    CCmsContactFieldItem::ECmsLandPhoneGeneric,
    CCmsContactFieldItem::ECmsLandPhoneHome,
    CCmsContactFieldItem::ECmsLandPhoneWork,
    //CCmsContactFieldItem::ECmsAssistantNumber,
    //CCmsContactFieldItem::ECmsCarPhone,
    CCmsContactFieldItem::ECmsEmailGeneric,
    CCmsContactFieldItem::ECmsEmailHome,
    CCmsContactFieldItem::ECmsEmailWork };

const TInt KCCAppCommLauncherMessageGroup[] = {
	    CCmsContactFieldItem::ECmsMobilePhoneGeneric,
	    CCmsContactFieldItem::ECmsMobilePhoneHome,
	    CCmsContactFieldItem::ECmsMobilePhoneWork,
	    CCmsContactFieldItem::ECmsLandPhoneGeneric,
	    CCmsContactFieldItem::ECmsLandPhoneHome,
	    CCmsContactFieldItem::ECmsLandPhoneWork};

/*
 * Defines the addresses used for emailing.
 * (todo; should be dynamically loaded from pbk selector resources to CMS
 * and handled by CMS CCmsContactField::GroupType() -functionality )
 * */
const TInt KCCAppCommLauncherEmailGroup[] = {
    CCmsContactFieldItem::ECmsEmailGeneric,
    CCmsContactFieldItem::ECmsEmailHome,
    CCmsContactFieldItem::ECmsEmailWork };

/*
 * Defines the addresses used for chatting.
 * (todo; should be dynamically loaded from pbk selector resources to CMS
 * and handled by CMS CCmsContactField::GroupType() -functionality )
 * */
const TInt KCCAppCommLauncherChatGroup[] = {
    CCmsContactFieldItem::ECmsImpp };

/*
 * Defines the addresses used for browsing.
 * (todo; should be dynamically loaded from pbk selector resources to CMS
 * and handled by CMS CCmsContactField::GroupType() -functionality )
 * */
const TInt KCCAppCommLauncherUrlGroup[] = {
    CCmsContactFieldItem::ECmsUrlGeneric,
    CCmsContactFieldItem::ECmsUrlHome,
    CCmsContactFieldItem::ECmsUrlWork };

/*
 * Defines the addresses used for video calls.
 * (todo; should be dynamically loaded from pbk selector resources to CMS
 * and handled by CMS CCmsContactField::GroupType() -functionality )
 * */
const TInt KCCAppCommLauncherVideoCallGroup[] = {
    CCmsContactFieldItem::ECmsMobilePhoneGeneric,
    CCmsContactFieldItem::ECmsMobilePhoneHome,
    CCmsContactFieldItem::ECmsMobilePhoneWork,
    CCmsContactFieldItem::ECmsVideoNumberGeneric,
    CCmsContactFieldItem::ECmsVideoNumberHome,
    CCmsContactFieldItem::ECmsVideoNumberWork };

/*
 * Defines the addresses used for find/show on map.
 * (todo; should be dynamically loaded from pbk selector resources to CMS
 * and handled by CMS CCmsContactField::GroupType() -functionality )
 * */
const TInt KCCAppCommLauncherAddressGroup[] = {
    CCmsContactFieldItem::ECmsAddrStreetGeneric,
    CCmsContactFieldItem::ECmsAddrLocalGeneric,
    CCmsContactFieldItem::ECmsAddrCountryGeneric,
    CCmsContactFieldItem::ECmsAddrPostcodeGeneric,
    CCmsContactFieldItem::ECmsAddrPOGeneric,
    CCmsContactFieldItem::ECmsAddrExtGeneric,
    CCmsContactFieldItem::ECmsAddrRegionGeneric,
    CCmsContactFieldItem::ECmsAddrStreetHome,
    CCmsContactFieldItem::ECmsAddrLocalHome,
    CCmsContactFieldItem::ECmsAddrCountryHome,
    CCmsContactFieldItem::ECmsAddrPostcodeHome,
    CCmsContactFieldItem::ECmsAddrPOHome,
    CCmsContactFieldItem::ECmsAddrExtHome,
    CCmsContactFieldItem::ECmsAddrRegionHome,
    CCmsContactFieldItem::ECmsAddrStreetWork,
    CCmsContactFieldItem::ECmsAddrLocalWork,
    CCmsContactFieldItem::ECmsAddrCountryWork,
    CCmsContactFieldItem::ECmsAddrPostcodeWork,
    CCmsContactFieldItem::ECmsAddrPOWork,
    CCmsContactFieldItem::ECmsAddrExtWork,
    CCmsContactFieldItem::ECmsAddrRegionWork };


/**
 * Interface-class for contact data notifications
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappcommlauncherplugin.dll
 *  @since S60 v5.0
 */ 
class MCCAppContactHandlerNotifier
    {
public:

    /**
     * Notifies observer when CCmsContactFieldInfo is available.
     * Ownership not transferred.
     *
     * @param CCmsContactFieldInfo
     */
    virtual void ContactInfoFetchedNotifyL( 
        const CCmsContactFieldInfo& aContactFieldInfo ) = 0;    

    /**
     * Notifies observer when new CCmsContactField is available.
     * Ownership not transferred.
     *
     * @param CCmsContactField
     */
    virtual void ContactFieldFetchedNotifyL( 
        const CCmsContactField& aContactField ) = 0;

    /**
     * Notifies observer when all the contact fields have been fetched
     *
     * @param CCmsContactField
     */
    virtual void ContactFieldFetchingCompletedL() = 0;
    
    /**
     * Notifies observer that contact data has been changed and will be reloaded
     */
    virtual void ContactsChangedL() = 0;
    
    /**
     * Notifies observer that contact presence has changed
     * 
     * @param CCmsContactField
     */
    virtual void ContactPresenceChangedL( const CCmsContactField& aContactField) = 0;
    };

/**
 * This class encapsulates the contact handling functionality through CCAPP/CMS.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappcommlauncherplugin.dll
 *  @since S60 v5.0
 */ 
class CCCAppCommLauncherContactHandler : 
    public CBase,
    public MCCAppContactFieldDataObserver
    {
#ifdef __COMMLAUNCHERPLUGINUNITTESTMODE
    friend class T_CCCAppCommLauncherContactHandler;
#endif// __COMMLAUNCHERPLUGINUNITTESTMODE   

public: // Construction and destruction

    /**
     * Creates a new instance of this class.
     *
     * @return pointer to a new instance of this clas.
     * @param pointer to target the contact data
     * @param aPlugin reference
     */
    static CCCAppCommLauncherContactHandler* NewL(
        MCCAppContactHandlerNotifier& aObserver,
        CCCAppCommLauncherPlugin& aPlugin );

    /**
     * Destructor.
     */
    ~CCCAppCommLauncherContactHandler();

public: //new

    /**
     * Request current and coming data from CMS wrapper
     *
     * @since S60 v5.0
     */ 
    void RequestContactDataL();

    /**
     * Returns amount of address fields in a contact
     *
     * @since S60 v5.0
     * @param aContactAction defining the address type
     * @return amount of suitable addresses 
     */    
    TInt AddressAmount( 
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction );
    
    /**
     * Checks if there is a service available for the contact method
     *
     * @since S60 v5.0
     * @param aContactAction defining the address type
     * @return TInt 1 if there is a service available, 0 otherwise. Voip is 
     * a special case having more levels than just on/off.
     */    
    TInt IsServiceAvailable( 
         VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction );

    /**
     * Check from which store this contact is from
     *
     * @return store type containing this contact
     */
    TCmsContactStore ContactStore() const;
    
    /**
     * Returns contact field array.
     *
     * @since S60 v5.0
     */    
    RPointerArray<CCmsContactField>& ContactFieldDataArray();

    /**
     * Returns packed contact identifiers
     *
     * @since S60 v5.0
     */    
    HBufC8* ContactIdentifierLC();

    /**
     * Find out is there default attributes set to this communication method.
     * Maps communication methods with default already fetched default
     * attributes.
     *
     * @since S60 v5.0
     * @param aContactAction communication method
     * @return TBool 
     */
    TBool HasDefaultAttribute( 
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction );

    /**
     * Maps between different communication methods and CMS default fields
     *
     * @since S60 v5.0
     * @param aContactAction communication method
     * @return CMS default type 
     */
    CCmsContactFieldItem::TCmsDefaultAttributeTypes MapContactorTypeToCMSDefaultType(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction );    
    
    /**
     * Returns data of 1st index as descriptor if found, otherwise KNullDesC.
     *
     * @since S60 v5.0
     * @param aFieldType type of the field
     * @param aData of field as descriptor
     */    
    void ContactFieldItemDataL( 
        const CCmsContactFieldItem::TCmsContactField aFieldType,
        TPtrC& aData );

    /**
     * Checks if the contact field is among the fields of certain selector
     *
     * @since S60 v5.0
     * @param aContactField contact field to be matched
     * @param aContactAction defines set of selector fields
     * @return ETrue if contact field is among the fields of certain selector
     */ 
    TBool ContactFieldTypeAndContactActionMatch(
        CCmsContactFieldItem::TCmsContactField aContactField,
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction );
    
    /**
     * Checks if the address is thought as number or address
     *
     * @since S60 v5.0
     * @param aContactAction to be checked
     * @return ETrue if address is thought to be number address
     */     
    TBool IsItNumberAddress( 
            VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction );   
    
    /**
     * Refetches a new contact from a store
     *
     * @since S60 v5.0
     */     
    void RefetchContactL();
    
	/**
     * Checks whether the Current contact is top contact.
     *
     * @return ETrue if Contact is a top contact, otherwise EFalse 
     */
    TBool IsTopContact();
    
private: // from MCCAppContactFieldDataObserver

	/**
	 * from MCCAppContactFieldDataObserver
	 */
    void ContactFieldDataObserverNotifyL( 
        MCCAppContactFieldDataObserver::TParameter& aParameter );        

	/**
	 * from MCCAppContactFieldDataObserver
	 */
    void ContactFieldDataObserverHandleErrorL( 
        TInt aState, TInt aError );           

private:// new

    /**
     * ContactInfo -notifies are handled through this.
     * See ContactFieldDataObserverNotifyL.
     *
     * @since S60 v5.0
     */
    void ContactInfoFetchedNotifyL( 
        const CCmsContactFieldInfo& aContactFieldInfo );

    /**
     * ContactField -notifies are handled through this.
     * See ContactFieldDataObserverNotifyL.
     *
     * @since S60 v5.0
     */
    void ContactFieldFetchedNotifyL( 
        CCmsContactField& aContactField );

    /**
     * Helper function to fetch attribute of given contact field.
     *
     * @since S60 v5.0
     * @param aContactField: contact field. 
     */  
    void UpdateDefaultAttributes( const CCmsContactField& aContactField );
    
    /**
     * Checks if the contact field is among the fields of certain selector
     *
     * @since S60 v5.0
     * @param aContactField contact field to be matched
     * @param aFieldArray array of fields
     * @param aCount amount of the array fields
     * @return ETrue if contact field is among the fields of certain selector
     */     
    TBool FieldArrayAndContactActionMatch(
        CCmsContactFieldItem::TCmsContactField aContactField,
        const TInt aFieldArray[], const TInt aCount );

    /**
     * Checks if dynamic field array is needed and creates one if it is
     *
     * @since S60 v5.0
     * @param aContactAction to be a array
     */     
    void ConstructDynamicFieldArrayIfNeeded(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction );    

    /**
     * Creates VoIP address group
     *
     * @since S60 v5.0
     * @param aVoipSupportFlag defines the details of voip support
     */     
    void CreateDynamicVoipAddressGroup( const TInt aVoipSupportFlag );    
    
private: // construction

    CCCAppCommLauncherContactHandler( 
        MCCAppContactHandlerNotifier& aObserver,
        CCCAppCommLauncherPlugin& aPlugin );

    void ConstructL();

private: // data
    
    /**
     * Observer to be updated of contact fields
     * Own.
     */
    MCCAppContactHandlerNotifier& iObserver;
    /**
     * Notify if dynamic address fields are set or not
     * Own.
     */
    TBool iDynamicAddressGroupSet;
    /**
     * Dynamic address fields
     * Own.
     * */
    RArray<TInt> iDynamicVoipAddressGroup;
    /**
     * Default attributes as bitmasks
     * Own.
     */
    TInt iDefaultAttributesBitMask; 
    /**
     * CMS is used through CmsHandler to fetch contact data fields
     * Not own.
     */
    CCCAppCmsContactFetcherWrapper* iCmsWrapper;
    
    /**
     * Ref to plugin.     
     * Not own.
     */
    CCCAppCommLauncherPlugin& iPlugin;
    
    /**
     * Address amount cache
     * Own
     */
    RHashMap<TInt, TInt> iAddressCache;
    };

#endif // C_CCAPPCOMMLAUNCHERCONTACTHANDLER_H

// End of File
