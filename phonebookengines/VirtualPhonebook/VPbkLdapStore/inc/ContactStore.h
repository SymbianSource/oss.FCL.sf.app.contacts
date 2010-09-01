/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CContactStore - LDAP store implementation.
*
*/


#ifndef __CONTACTSTORE_H__
#define __CONTACTSTORE_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <mvpbkcontactstore.h>
#include "ldapsettings.h"           // LDAP Settings
#include "ldapdirectory.h"          // LDAP Directory

// FORWARD DECLARATIONS
class TVPbkContactStoreUriPtr;
class MVPbkContactStoreInfo;
class CLDAPSettings;
class TLDAPFieldType;
class LDAPEngine::CDirectory;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// FORWARD DECLARATIONS
class CContactStoreDomain;
class CContactStoreInfo;
class CContactStoreProperties;
class CContactStoreUri;
class CFieldTypeMappings;
class CSupportedFieldTypes;
class CContact;

/**
*  CContactStore - ldap store implementation.
*
*/
class CContactStore : public CBase,
                      public MVPbkContactStore,
                      public LDAPEngine::MDirectoryObserver
    {
    public:

        // Store state
        enum TState
            {
            EIdle = 0,          // Idle is unspecified state
            EInitializing,      // Initializing ldap
            EInitialized,       // Initialized
            EOpening,           // Opening directory
            EOpened,            // Opened
            ESearching,         // Finder is on
            ERetrieving,        // Retrieving results
            ERetrieved,         // Retrieved results
            EClosing,           // Closing
            EClosed,            // Closed
            EShutdown           // Shutdown
            };

    public:     // CContactStore public constructors and destructor
        /**
        @function   NewL
        @discussion Create CContactStore object
        @param      aURI Store url
        @param      aStoreDomain Store domain
        @return     Pointer to instance of CContactStore
        */
        static CContactStore* NewL(const TVPbkContactStoreUriPtr& aURI,
                                    CContactStoreDomain& aStoreDomain);

        /**
        @function   NewLC
        @discussion Create CContactStore object
        @param      aURI Store url
        @param      aStoreDomain Store domain
        @return     Pointer to instance of CContactStore
        */
        static CContactStore* NewLC(const TVPbkContactStoreUriPtr& aURI,
                                    CContactStoreDomain& aStoreDomain);

        /**
        @function  ~CContactStore
        @discussion CContactStore destructor
        */
        ~CContactStore();

    public:     // CContactStore public methods

        /**
        @function   AddObserverL
        @discussion Add store observer
        @param      aObserver Store observer
        */
        void AddObserverL(MVPbkContactStoreObserver& aObserver);
        /**
        @function   RemoveObserver
        @discussion Remove store observer
        @param      aObserver Store observer
        */
        void RemoveObserver(MVPbkContactStoreObserver& aObserver);

        /**
        @function   CreateContactL
        @discussion Creates copy of contact from index position.
        @param      aIndex Contact index
        @return     Contact at index
        */
        CContact* CreateContactL(TInt aIndex);

        /**
        @function   ContactCount
        @discussion Returns number of contacts in store.
        @return     Number of contacts in store.
        */
        TInt ContactCount();

        /**
        @function   ContactAt
        @discussion Returns contact from position.
        @param      aIndex Contact index
        @return     Contact refernce at index
        */
        CContact& ContactAt(TInt aIndex);

        /**
        @function   FieldTypeMappings
        @discussion Returns store field type map.
        @return     Field type mappings
        */
        CFieldTypeMappings& FieldTypeMappings();

        /**
        @function   IsInitialized
        @discussion Returns true if store is initialized.
        @return     ETrue, store is initialized.
        */
        TBool IsInitialized();

        /**
        @function   IsOpen
        @discussion Returns true if store is open.
        @return     ETrue, store is open.
        */
        TBool IsOpen();

        /**
        @function   State
        @discussion Returns store state.
        @return     Store state.
        */
        TState State();

        /**
        @function   MatchContactStore
        @discussion Matches this contact store with passed.
        @param      aContactStoreUri the URI to compare
        @return     ETrue if this is the same store
        */
        TBool MatchContactStore(const TDesC& aContactStoreUri) const;
        
        /**
        @function   MatchContactStore
        @discussion Matches this contact store domain with passed.
        @param      aContactStoreDomain the domain to compare
        @return     ETrue if this store is in the same domain
        */
        TBool MatchContactStoreDomain(const TDesC& aContactStoreDomain) const;

        /**
        @function   Name
        @discussion Store name
        @return     Store name
        */
        TPtrC Name();

        /**
        @function   StoreInitializeL
        @discussion Initialize store callback
        @return     ETrue initializing or EFalse done
        */
        TBool StoreInitializeL();

        /**
        @function   StoreReady
        @discussion Store ready notfier
        */
        void StoreReady();

    public:     // Methods from MVPbkContactOperationFactory

        /**
        @function   CreateContactRetrieverL
        @discussion Creates a command for asynchronous contact link retrieval.
        @param      aLink Link to retrieve the contact from.
        @param      aObserver Observer for the retrieval operation.
        @return     Created retrieval operation.
        */
        MVPbkContactOperation* CreateContactRetrieverL( 
            const MVPbkContactLink& aLink, 
            MVPbkSingleContactOperationObserver& aObserver);

        /**
        @function   CreateDeleteContactsOperationL
        @discussion Creates an operation for deleting multiple contacts.
        @param      aContactLinks Array of contact links to delete.
        @param      aObserver Observer for the deletion operation.
        @return     Created delete operation.
        */
        MVPbkContactOperation* CreateDeleteContactsOperationL(
            const MVPbkContactLinkArray& aContactLinks, 
            MVPbkBatchOperationObserver& aObserver);

        /**
        @function   CreateCommitContactsOperationL
        @discussion Creates an operation for committing multiple contacts.
        @param      aContacts Array of contact links to commit.
        @param      aObserver Observer for the commit operation.
        @return     Created commit operation.
        */
         MVPbkContactOperation* CreateCommitContactsOperationL(
            const TArray<MVPbkStoreContact*>& aContacts,
            MVPbkBatchOperationObserver& aObserver);

        /**
        @function   CreateMatchPhoneNumberOperationL
        @discussion Creates an operation for asynchronous phone number matching.
        @param      aPhoneNumber    Phone number to search for.
        @param      aMaxMatchDigits Number of digits to match from right.
        @param      aObserver       Observer for the find operation.
        @return     Created find operation.
        */        
        MVPbkContactOperation* CreateMatchPhoneNumberOperationL(
            const TDesC& aPhoneNumber, 
            TInt aMaxMatchDigits,
            MVPbkContactFindObserver& aObserver );

        /**
        @function   CreateFindOperationL
        @discussion Creates an operation for asynchronous contact finding.
        @param      aSearchString String to search for.
        @param      aFieldTypes   List of field types that the search will include.
        @param      aObserver     Observer for the find operation.
        @return     Created find operation.
        */        
        MVPbkContactOperation* CreateFindOperationL(
            const TDesC& aSearchString,
            const MVPbkFieldTypeList& aFieldTypes,
            MVPbkContactFindObserver& aObserver);

        /**
        * TODO comments
        */
        MVPbkContactOperation* CreateFindOperationL(
                const MDesC16Array& aSearchStrings, 
                const MVPbkFieldTypeList& aFieldTypes, 
                MVPbkContactFindFromStoresObserver& aObserver, 
                const TCallBack& aWordParserCallBack);

        /**
        @function   CreateCompressStoresOperationL
        @discussion Creates an operation for compressing all the stores.
        @param      aObserver Observer for the compress operation.
        @return     Created operation.
        */        
        MVPbkContactOperation* CreateCompressStoresOperationL( 
            MVPbkBatchOperationObserver& aObserver);

    public:     // Methods from MVPbkContactStore
 
        /**
        @function   StoreProperties
        @discussion Returns fixed properties of this contact store.
        @return     Store properties
        */
        const MVPbkContactStoreProperties& StoreProperties() const;

        /**
        @function   OpenL
        @discussion Opens this store asynchronously.
        @param      aObserver Observer for the open process.
        @exception  KErrInUse Store is already opening.
        */
        void OpenL(MVPbkContactStoreObserver& aObserver);

        /**
        @function   ReplaceL
        @discussion Replaces an existing store and opens it asynchronously.
        @param      aObserver Observer for the open process.
        */
        void ReplaceL(MVPbkContactStoreObserver& aObserver);

        /**
        @function   Close
        @discussion Closes this contact store from a single observer.
        @param      aObserver Observer for the open process.
        */
        void Close(MVPbkContactStoreObserver& aObserver);

        /**
        @function   CreateNewContactLC
        @discussion Creates a new contact associated to this store.
        @return     Contact associated to this store.
        */
        MVPbkStoreContact* CreateNewContactLC();
        
        /**
        @function   CreateNewContactGroupLC
        @discussion Creates a new contact group associated to this store.
        @return     Contact group associated to this store.
        */
        MVPbkContactGroup* CreateNewContactGroupLC();

        /**
        @function   CreateViewLC
        @discussion Creates a new contact view.
        @return     Contact view.
        */
        MVPbkContactView* CreateViewLC(
            const CVPbkContactViewDefinition& aViewDefinition,
            MVPbkContactViewObserver& aObserver, 
            const MVPbkFieldTypeList& aSortOrder);

        /**
        @function   ContactGroupsLC
        @discussion Return contact groups from this store.
        @return     Contact groups from this store.
        */
        MVPbkContactLinkArray* ContactGroupsLC() const;

        /**
        @function   StoreInfo
        @discussion Returns a contact store information.
        @return     Contact store information.
        */
        const MVPbkContactStoreInfo& StoreInfo() const;

        /**
        @function   CreateLinkFromInternalsLC
        @discussion Returns contact link.
        @param      aStream Stream containing the link internals
        @return     Contact link
        */
        MVPbkContactLink* CreateLinkFromInternalsLC(RReadStream& aStream) const;

    public:  // Methods from MDirectoryObserver

        /**
        @function    ActionDoneL
        @description This method is called after directory action completed.
        @param       aDirectory Directory notifiying action completed.
        @param       aAction    Action completed.
        @param       aStatus    Status code.
        @param       aErrorMsg  Server error message
        */
        void ActionDoneL(LDAPEngine::CDirectory& aDirectory,
            LDAPEngine::TActionType aAction,TInt aStatus,TPtrC8 aErrorMsg);
        /**
        @function    ConnectionStateChanged 
        @description Connection state changed callback
        @param       aDirectory Directory notfying state change
        @param       aState Changed to state
        */
        void ConnectionStateChangedL(LDAPEngine::CDirectory& aDirectory,
                                     LDAPEngine::TConnectionState aState);

    private:    // CContactStore private constructors

        /**
        @function   CContactStore
        @discussion CContactStore default contructor
        @param      aStoreDomain Store domain
        */
        CContactStore(CContactStoreDomain& aStoreDomain);

        /**
        @function   ConstructL
        @discussion Perform the second stage construction of CContactStore 
        @param      aURI Store url
        */
        void ConstructL(const TVPbkContactStoreUriPtr& aURI);

    private:    // CContactStore private methods

        /**
        @function   InitializeFieldMappingL
        @discussion Initialize LDAP attribute to contact model mapping
        */
        void InitializeFieldMappingL();

        /**
        @function   InitializeFieldMappingL
        @discussion Initialize store supported field types
        */
        void InitializeSupportedFieldsL();

        /**
        @function   StoreReady
        @discussion Store ready notfier
        @param      aObserver Store observer
        */
        void StoreReady(MVPbkContactStoreObserver& aObserver);

        /**
        @function   RetrieveContactsL
        @discussion Get contacts from directory. 
        */
        void RetrieveContactsL();


    private:    // CContactStore private member variables

        /// Reference to the ldap store domain
        CContactStoreDomain& iStoreDomain;

        /// Store URI
        CContactStoreUri* iStoreURI;

        /// Static properties of the store
        CContactStoreProperties* iProperties;

        /// Store information
        CContactStoreInfo* iStoreInfo;

        /// Settings
        CLDAPSettings* iSettings;

        /// Directory
        LDAPEngine::CDirectory* iDirectory;

        /// Timer callback
        CIdle* iIdle;

        /// Observers that are notified after asynchronous store event.
        RPointerArray<MVPbkContactStoreObserver> iStoreObservers;

        /// Store state
        TState iState;

        /// Array of field type mapping
        RArray<TLDAPFieldType> iFieldTypes;

        /// Supported virtual phonebook field types
        CSupportedFieldTypes* iSupportedFieldTypes;

        /// Array of contacts
        RPointerArray<CContact> iContacts;

    };
} // End of namespace LDAPStore
#endif // __CONTACTSTORE_H__
