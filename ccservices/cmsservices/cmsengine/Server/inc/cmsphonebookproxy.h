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
* Description:  Phonebook proxy.
*       
*
*/


#ifndef __CMSPHONEBOOKPROXY__
#define __CMSPHONEBOOKPROXY__

// INCLUDES
#include <MVPbkContactFindObserver.h>
#include <MVPbkContactStoreObserver.h>
#include "cmsnotificationhandlerapi.h"
#include <MVPbkContactStoreListObserver.h>
#include <MPbk2StoreConfigurationObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include "MCmsPhonebookOperationsObserver.h"
#include <badesca.h>

//FORWARD DECLARATIONS
class RFs;
class CVPbkSortOrder;
class TResourceReader;
class MVPbkContactLink;
class MVPbkContactStore;
class MVPbkStoreContact;
class CCmsXspStoreSearch;
class CVPbkContactManager;
class TVPbkWordParserParam;
class MCmsContactInterface;
class MVPbkContactViewBase;
class MVPbkContactLinkArray;
class MVPbkContactStoreList;
class CVPbkFieldTypeRefsList;
class CPbk2StoreConfiguration;
class MVPbkContactOperationBase;
class CVPbkContactStoreUriArray;
class CVPbkContactViewDefinition;
class MVPbkContactFindFromStoresObserver;
class CmsSetVoiceCallDefault;

/**
 * CCmsPhonebookProxy keeps all stores opened to speed up contact
 * operations. CCmsPhonebookProxy is created and owned by CmsServer
 * instance and is shared among all client sessions.
 */
NONSHARABLE_CLASS( CCmsPhonebookProxy ) : public CBase,
                   public MVPbkContactFindObserver,
                   public MVPbkContactStoreObserver,
                   public MVPbkContactStoreListObserver,
                   public MPbk2StoreConfigurationObserver,
                   public MVPbkSingleContactOperationObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Public constructor
        *
        * @param aCmsStoreOpencompleteObserver Observer to be notified about
        *               store opening completion
        * @param aFileSessionHandle Opened handle to file server
        * @return Pointer to CCmsPhonebookProxy instance
        */
        static CCmsPhonebookProxy* NewL(
            MCmsPhonebookOperationsObserver& aCmsStoreOpencompleteObserver,
            RFs& aFileSessionHandle );

        /**
        * Start contact stores initialization.
        * Must be called before doing anything else.
        */
        void InitStoresL();
        
        /**
        * Fetch contact information
        *
        * @param aContactId Contact ID
        * @param aContactInterface Observer to be notified about operation 
        *                          completion
        * @return Error code        
        */
        TInt FetchContactL( TInt32 aContactId,
            MCmsContactInterface* aContactInterface );

        /**
        * Fetch contact information
        *
        * @param aMsisdn MSISDN of the contact
        * @param aContactInterface Observer to be notified about operation 
        *                          completion
        * @return Error code        
        */
        TInt FetchContactL( const TDesC& aMsisdn,
            MCmsContactInterface* aContactInterface );

        /**
        * Fetch contact information
        *
        * @param aPackedlink Link to the contact
        * @param aContactInterface Observer to be notified about operation 
        *                          completion
        * @return Error code
        */
        TInt FetchContactL( const TDesC8& aPackedlink,
            MCmsContactInterface* aContactInterface );
        
        /**
        * Starts xSP contacts find operation. Operation result is returned via 
        * MVPbkContactFindFromStoresObserver. Function caller should release 
        * the operation handle
        * 
        * @param aXspId Field content to be searched in the xSP contacts
        * @param aContacthandler Observer to be notife about operation result
        * @return handle to the find operation or NULL if it could not be created.
        */
        MVPbkContactOperationBase*  FindXSPContactL( const TDesC& aXspId,
                              MVPbkContactFindFromStoresObserver& aContacthandler );
        
        /**
        * @return ETrue, if there is at least one xSP store is installed
        */
        TBool ExternalStoresIntalled() const;

        /**
        * Subscribe for contact operation status notifications
        *
        * @param aObserver Observer
        */
        void AddObserverL( MCmsContactInterface* aObserver );
        
        /**
        * Unsubscribe for contact operation status notifications
        *
        * @param aObserver Observer
        */
        void RemoveObserver( MCmsContactInterface* aObserver );
        
        /**
        * Maps VPbk's store event to CMS's store event
        *
        * @param aPhonebookEvent VPbk's store event
        * @return CMS's store event 
        */
        TCmsPhonebookEvent SelectEventType(
            TVPbkContactStoreEvent::TVPbkContactStoreEventType aPhonebookEvent );

        /**
        * @return Contact manager, ownership is not transfered.
        */
        CVPbkContactManager* ContactManager() const;

        /**
        * Cancel an ongoing asynchronous operation
        */
        void Cancel();
        
        /**
        * @param aExcludedStoreUri Store URI which is not used in search.
        * @return Returns number of stores used in the find operation
        */
        TInt NumberOfStoresInFind( const TDesC& aExcludedStoreUri );
        
        /**
        * Updates the list of xSP stores.
        */
        void UpdateXSPStoresListL();
                          
        /**
        * Destructor
        */
        ~CCmsPhonebookProxy();
        
        /**
        * Set default attribute for voice call
        */
        void SetVoiceCallDefaultL( MVPbkStoreContact* aContact );
        
        /**
         * Set contact
         */
        void SetContact( MVPbkStoreContact* aContact );

    public:
        
        /**
        * Get the store opening status 
        * 
        * @return ETrue : If all stores are opend and atleast one store available 
        * @return EFalse: If not all stores are opened or no stores available   
        */ 
        TBool StoreOpenStatus();
   
    public:  //static

        /**
        * Implementation of word parser function that separates the field data 
        * into words. 
        *
        * @param aWordParserParam TVPbkWordParserCallbackParam
        * @return Parsing status code  
        */
        static TInt WordParser( TAny* aWordParserParam );
    
    private:  //From MVPbkSingleContactOperationObserver

        void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation,
            TInt aError );

    private:  //From MVPbkContactStoreListObserver

        void OpenComplete();
    
    private:  //From MVPbkContactFindObserver
        
        void FindFailed( TInt aError );
        void FindCompleteL( MVPbkContactLinkArray* aResults );
        
    private:  //From MVPbkContactStoreObserver

        void StoreReady( MVPbkContactStore& aContactStore );
        void StoreUnavailable( MVPbkContactStore& aContactStore, TInt aReason );
        void HandleStoreEventL(
            MVPbkContactStore& aContactStore,
            TVPbkContactStoreEvent aStoreEvent );

    private:  //From MPbk2StoreConfigurationObserver
        
        void ConfigurationChanged();
        void ConfigurationChangedComplete();
        
    private:
        
        /**
        * C++ constructor
        *
        * @param aCmsStoreOpencompleteObserver Observer to be notified about
        *               store opening completion
        * @param aFileSessionHandle Opened handle to file server
        * @return CCmsPhonebookProxy
        */
        CCmsPhonebookProxy(
             MCmsPhonebookOperationsObserver& aCmsStoreOpencompleteObserver,
             RFs& aFileSessionHandle );
        
        /**
        * Symbian OS second-phase constructor
        */
        void ConstructL();
        
        /**
        * Called on contact operation completion. Notifies contact object.
        */
        void CompleteContactRequestL( TInt aError, MVPbkStoreContact* aContact );
        
        /**
        * Creates a list of stores in current configuration
        */
        void CreateConfigurationL();
        
        /**
        * Finds all installed xSP stores
        *
        * @param aArray Array to keep a list of xSP stores
        */
        void FindXSPStoresL( CDesCArrayFlat& aArray );
        
        /**
        * Cleans data after contact operation is done.
        */
        void ResetData();

    private:
        /**
        * Finds whether the contact belongs to XSP Store or not
        * @param aContactLink  - Contact which needs to be checked
        * @return - ETrue if XSP Contacts else EFalse.
        */
        TBool IsXspContact( const MVPbkContactLink& aContactLink ) const;
        
    private: //Data
        
        /// ETrue, if at least one store was opened successfully
        TBool                                   iAtLeastOneStoreReady;
        /// ETrue, if stores opening is done
        TBool                                   iOpenComplete;
        /// ETrue, if there is at least one xSP store installed in the phone
        TBool                                   iXspStoresInstalled;
        /// Default contact database, NULL if not opened. Not owned.
        MVPbkContactStore*                      iContactStore;
        /// Contact manager. Owned.
        CVPbkContactManager*                    iContactManager;
        /// CmsServerContact. Not owned.
        MCmsContactInterface*                   iContactInterface;
        /// List of stores in current configuration. Not owned.
        MVPbkContactStoreList*                  iStoreList;
        /// List of fields to make a search in xSP stores. Owned.
        CVPbkFieldTypeRefsList*                 iFieldTypeRefList;
        /// Store configuration. Owned.
        CPbk2StoreConfiguration*                iStoreConfiguration;
        /// List of stores URIs in current configuration. Owned.
        CVPbkContactStoreUriArray*              iUriList;
        /// Contact operation handle. Owned.
        MVPbkContactOperationBase*              iOperation;
        /// Array of observers which are notified about 
        RPointerArray<MCmsContactInterface>     iObserverArray;
        /// Observer which is notified about contact operations completion.
        /// Not owned.
        MCmsPhonebookOperationsObserver&        iCmsPhonebookOperationsObserver;
        /// Reference to file session handle. Not owned.
        RFs&                                    iFileSessionHandle;
        /// Array to keep a list of installed xSP stores. Owned.
        CDesCArrayFlat                          ixSPStoresArray;
        /// Default setting handle
        CmsSetVoiceCallDefault*                 iSetDefault;        
        // Current contact, not Owned
        MVPbkStoreContact* iContact; 
        // ETrue, if the current activated contact store ready
        TBool iCurrentContactStoreReady;
        // Completed stores
        RPointerArray<HBufC> iReadyStores;
        
    };

#endif  //__CMSPHONEBOOKPROXY__


// End of File
