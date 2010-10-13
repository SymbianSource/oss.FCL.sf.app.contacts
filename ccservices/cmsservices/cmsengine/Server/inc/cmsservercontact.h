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
* Description: 
*       
*
*/


#ifndef __CMSSERVERCONTACT__
#define __CMSSERVERCONTACT__

// INCLUDES
#include "cmscontact.h"
#include <VPbkEng.rsg>
#include <MVPbkSingleContactOperationObserver.h>
#include "bpasobserver.h"
#include "cmscontactinterface.h"
#include "mpresencetrafficlightsobs.h"
#include "cmsserver.h"

// CONSTANTS
const TInt KDefaultBinaryBufferSize              = 1024;
const TInt KPresenceFieldGroup[] =  { 
        R_VPBK_FIELD_TYPE_IMPP };

// FORWARD DECLARATIONS
class CVPbkContactManager;
class CBPAS;
class CCmsNotifyEvent;
class CCmsServerSession;
class CCmsServerAsyncContact;
class CCmsServerXSPContactHandler;
class MVPbkContactOperationBase;

/**
 * CCmsServerContact is responsible for retrieving contact information
 */
NONSHARABLE_CLASS( CCmsServerContact ) : public CBase,
                                         public MBPASObserver,
                                         public MCmsContactInterface,
                                         public MPresenceIconNotifier,
                                         public MVPbkSingleContactOperationObserver
    {
    public:  
        
        /**
        * Creates a new instance of CCmsServerContact
        *
        * @param aSession Reference to the session instance.
        * @param aCmsServer Reference to the server instance.
        * @return Pointer to CCmsServerContact
        */
        static CCmsServerContact* NewL( CCmsServerSession& aSession,
                                        CCmsServer& aCmsServer );
        
        /**
        * Starts fetching enabled fields
        *
        * @param Kernel message with client's data
        */
        void FetchEnabledFields( const RMessage2& aMessage );
        
        /**
        * Starts fetching contact info
        *
        * @param aMessage Kernel message with client's data
        */
        void FetchContactData( const RMessage2& aMessage );
        
        /**
        * Packs iPackedContactLinkArray and sends to client
        *
        * @param aMessage Kernel message with client's data
        */
        void FetchContactLinkArrayL( const RMessage2& aMessage );
        
        /**
        * Starts searching for xSP contacts
        *
        * @param aMessage Kernel message with client's data
        */
        void FindXSPContactL( const RMessage2& aMessage );
        
        /**
        * Cancels xSP contacts search
        */
        void CancelXspContactFind() const;
        
        /**
        * Cancels active async request
        */
        void CancelOperation();
        
        /**
        * Saves data into internal cache
        *
        * @param a16BitData Data to be cached. Ownership is taken.
        */
        void CacheData( HBufC* a16BitData );
        
        /**
        * Saves data into internal cache
        *
        * @param a8BitData Data to be cached. Ownership is taken.
        */
        void CacheData( HBufC8* a8BitData );

        /**
        * Destructor
        */
        ~CCmsServerContact();

        /**
        * Activates notifications
        *
        * @param aMessage Kernel message with client's data
        * @param aAskMore If ETrue, returns current presence status imemdiately
        *   from the presence cache and notify later about any changes
        */
        void OrderNotifyL( const RMessage2& aMessage, TBool aAskMore );
         
        /**
        * Completes notification request
        *
        * @param aMessage Kernel message with client's data
        * @param aStatus Status to complete notification with
        */
        void CompleteNotify( const RMessage2& aMessage, TInt aStatus );

        /**
        * @return ETrue if contact was deleted
        */
        TBool IsDeleted() const;

        /**
        * @return Contact store type, see TCmsContactStore
        */
        TInt ContactStore() const;
        
        /**
        * @param Contact store URI.
        */
        TPtrC StoreUri();

        /**
        * @return Reference to BPAS handle. Ownership is not transfered.
        */
        CBPAS& BPASHandle();

        /**
        * @return Reference to async contact. Ownership is not transfered.
        */
        CCmsServerAsyncContact& AsyncContact(); 
        
        /**
        * @return Pointer to xSP contact handle. Ownership is not transfered
        */
        CCmsServerXSPContactHandler* XSPHandler() const;
        
        /**
        * Check voip support 
        *
        * @return Bitmask of TCmsVoIPSupport, see cmscontactfield.h
        */
        TInt ParseVoIPAvailabilityL();
        
        /**
        * Start next find operation
        *
        * @param aNextFindStringPos Position of next string to find in the iIdArray
        */
        void StartNextFindOperationL( TInt aNextFindStringPos );
        
        /**
     	* Checks whether the Current contact is top contact.
        *
        * @param aMessage Kernel message with client's data
        */
        void IsTopContactL( const RMessage2& aMessage );
        
        /**
        * Sets default number for Voice Call
        */
        void SetVoiceCallDefaultL();
        
    private:
        
        /**
        * Default contructor
        *
        * @param aSession Reference to the session instance.
        * @param aCmsServer Reference to the server instance.
        */
        CCmsServerContact( CCmsServerSession& aSession,
                           CCmsServer& aCmsServer );
        
        /**
        * Second phase constructor
        */
        void ConstructL();
        
        /**
        * Sends cached data to the client
        *
        * @param aMessageParam Slot in the message to use for sending data
        */
        void CachedDataSend8( TInt aMessageParam );
        
        /**
        * @return ETrue if some contact data was cached
        */
        TBool CachedData();

        /**
        * Sends cached data to the client
        *
        * @param aMessageParam Slot in the message to use for sending data
        */
        void CachedDataSend16( TInt aMessageParam );

        /**
        * Handles contact presence info.
        *
        * @param aInfos IM contacts info
        */
        void DeliverPresenceDataL( RPointerArray<CBPASInfo>& aInfos );
        
        /**
        * Packs icon info, language and notification type into one buffer
        *
        * @param aBrandId Brand id
        * @param aElementId Element id 
        * @param aImageId Image Id
        * @param aLangId Language Id
        * @param aServiceTypeId Notification type
        * @return Packed info
        */
        HBufC8* ConstructIconInfoL( const TDesC8& aBrandId, const TDesC8& aTextId,
                                    const TDesC8& aImageId, TInt aLangId, 
                                    CCmsContactFieldItem::TCmsContactNotification aServiceTypeId );
                                    
        /**
        * Utility method for creating contactlink array of the contact. 
        */
        void CreateContactLinkArrayL(); 
        
        /**
        * Saves info about contact store.
        *
        * @param aStoreUri Store URI
        */
        void ParseContactStore( const TDesC& aStoreUri );
        
        /**
        * Notifies client about presence status changes or
        * saves notification to the queue.
        *
        * @param aData Packed branded icon info
        */
        void SendOrQueueBrandedDataL( const TDesC8& aData );        
        
        /**
        * Clears array with presence notifications
        */
        void EmptyPresenceQueue();
        
        /**
        * Clears array with contact event notifications
        */
        void EmptyPhonebookQueue();
        
        /**
        * Packs branded icon info into one buffer
        * 
        * @param aBrandId Brand id
        * @param aElementId Element id
        * @return Packed branded icon info
        */
        HBufC8* CompileBrandInfoDataLC( const TDesC8& aBrandId, const TDesC8& aElementId );
        
        /**
        * Starts listening for contact events
        */
        void StartPhonebookNotificationL();
        
        /**
        * Starts listening to presence notifications
        *
        * @param aMessage Kernel message with client's data
        * @param aAskMore If ETrue, returns current presence status imemdiately
        *   from the presence cache and notify later about any changes
        */
        void StartPresenceNotificationL( const RMessage2& aMessage, TBool aAskMore );

        /**
        * Gets length of the client's data 
        *
        * @param aMessageSlot Slot in kernel message to be checked for data length
        * @param aMessage Kernel message with client's data
        * @return TInt Length of client's data
        */
        TInt ClientDesLength( TInt aMessageSlot, RMessage2*& aMessage );
        
        /**
        * Start VoIP notifications
        */
        void StartPresenceVoipNotificationL( );

        /**
        * Start Chat notifications
        */
        void StartPresenceChatNotificationL( );   
        
        /**
        * Add Voip enabled fields
        *
        * @param aArray Contains Voip fields on return
        * @param aResourceId Field types to be used in search
        */
        void AddVoipFieldL( CDesCArrayFlat& aArray, TInt aResourceId ); 
        
        /**
        * Add Voip enabled fields
        *
        * @param aBitmask Contains updated flag for Voip support on return,
        *   if supported
        */
        void CheckServiceProviderSupportL( TInt& aBitmask );

        /**
        * Handle NewIconForContact method call
        *
        * @param aBrandId Brand id
        * @param aElementId Element id
        * @param aId Id given in SubscribeBrandingForContactL
		* @param aBrandLanguage - Brand Language ID
        */
        void DoNewIconForContactL( 
            const TDesC8& aBrandId, 
            const TDesC8& aElementId,
            TInt aId,
            TInt aBrandLanguage );
	    /**
        * Custom TCleaupItem callback function called by the CleanupStack
        *
        * @param aItem Item to be cleaned up by the cleanup stack
        */

		static void CleanupResetAndDestroy(TAny*  aItem);		

        
    private:  //From MCmsContactInterface

        void ContactReadyL( TInt aError, MVPbkStoreContact* aContact );
        void OfferContactEventL( TCmsPhonebookEvent aEventType,
                                  const MVPbkContactLink* aContactLink );
        const MVPbkStoreContact& Contact() const;
        TBool HandleField( HBufC* aFieldData );
        TBool HandleField( HBufC8* aFieldData );
        TBool HandleEnabledFields( HBufC* aEnabledFields );   
        void HandleError( TInt aError );         
        void FetchContactL( MVPbkContactLink* aContactLinkToFetch);
        
    private:  //From MBPASObserver
        
        void HandleInfosL( TInt aErrorCode, RPointerArray<CBPASInfo>& aInfos );
        void HandleSubscribedInfoL( CBPASInfo* aInfo );
    
    private:  //From MPresenceIconNotifier

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

    private:  //From MVPbkSingleContactOperationObserver

        void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation,
            TInt aError );
        
    private:  //Data
        
        /// ETrue, if contact was deleted.
        TBool                               iContactDeleted;
        
        /// ETrue, if presence notificattion is active
        TBool                               iPresenceNotifySubscribed;
        
        /// Handle to BPAS to get presence info. Oowned.
        CBPAS*                              iBrandedPresence;
        
        /// Contact's store URI. Owned.
        HBufC*                              iStoreUri;
        
        /// Cache for field content. Owned.
        HBufC*                              iCachedField16;
        
        /// Cache for field content. Owned.
        HBufC8*                             iCachedField8;
        
        /// Packed contant link array whicvh contains iContact. Owned.
        HBufC8*                             iPackedContactLinkArray;
        
        /// Kernel message to be completed later for async requests
        RMessage2                           iMessage;
        
        /// Kernel message to be completed for contact notifications
        RMessage2                           iPbkNotifyMessage;
        
        /// Kernel message to be completed for presence notifications
        RMessage2                           iPresenceNotifyMessage;
        
        /// Type of contact store.
        TCmsContactStore                    iContactStore;
        
        /// Reference to session instance. Not owned.
        CCmsServerSession&                  iSession;
        
        /// Store contact. Owned.
        MVPbkStoreContact*                  iContact;
        
        /// Contact manager. Not owned.
        CVPbkContactManager*                iContactManager;
        
        /// Contact object for async requests. Owned.
        CCmsServerAsyncContact*             iAsyncContact;
        
        /// xSPContact handle. Owned.
        CCmsServerXSPContactHandler*        iXSPContactHandler;
        
        /// Array for keeping presence events. Owned.
        RPointerArray<CCmsNotifyEvent>      iPresenceEvents;
        
        /// Array for keeping contact events. Owned.
        RPointerArray<CCmsNotifyEvent>      iPhonebookEvents;
        
        /// Conact link of iContact. Owned.
        MVPbkContactLink*                   iContactLink; 
        
        /// Voip handler to get voip presence status. Owned.
        MPresenceTrafficLights*             iVoipHandler;
        
        /// Chat handler to get chat presence status. Owned.
        MPresenceTrafficLights*             iChatHandler;
        
        /// Array for keeping all contact's IMPP fields. Owned.
        CDesCArrayFlat* iIdArray;
        
        /// Reference to CCmsServer instance. Not owned.
        CCmsServer& iCmsServer;
        
        /// Array of find operations in xSP stores. Owned.
        RPointerArray<MVPbkContactOperationBase>          iXSPFindOperationsArray;

        /// Used to save Retrieve Contact operation. Owned.
        MVPbkContactOperationBase*              iOperation;

        /// Cached VoIP features
        TInt iVoipFeatures;
    };

#endif  //__CMSSERVERCONTACT__


// End of File
