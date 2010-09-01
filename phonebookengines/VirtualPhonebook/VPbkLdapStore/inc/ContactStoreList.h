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
* Description:  CContactStoreList - Contact store list for LDAP stores.
*
*/


#ifndef __CONTACTSTORELIST_H__
#define __CONTACTSTORELIST_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <mvpbkcontactstorelist.h>
#include <mvpbkcontactstoreobserver.h>

// FORWARD DECLARATIONS
class TVPbkContactStoreUriPtr;
class MVPbkContactOperation;
class MVPbkStoreContact;
class MVPbkBatchOperationObserver;
class MVPbkSingleContactOperationObserver;
class MVPbkContactLink;
class MVPbkContactFindObserver;
class MVPbkFieldTypeList;
class MVPbkContactLinkArray;
class CVPbkContactStoreUriArray;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// FORWARD DECLARATIONS
class CContactStore;
class CContactStoreDomain;

/**
*  CContactStoreList - contact store list for ldap stores.
*
*/
class CContactStoreList : public CBase,
                          public MVPbkContactStoreList,
                          public MVPbkContactStoreObserver
//                        public MVPbkContactOperationFactory
    {
    public:     // CContactStoreList public constructors and destructor

        /**
        @function   NewL
        @discussion Create CContactStoreList object
        @return     Pointer to instance of CContactStoreList
        */
        static CContactStoreList* NewL(
            const CVPbkContactStoreUriArray& aURIList,
            CContactStoreDomain& aStoreDomain);

        /**
        @function   NewLC
        @discussion Create CContactStoreList object
        @return     Pointer to instance of CContactStoreList
        */
        static CContactStoreList* NewLC(
            const CVPbkContactStoreUriArray& aURIList,
            CContactStoreDomain& aStoreDomain);

        /**
        @function   ~CContactStoreList
        @discussion CContactStoreList destructor
        */
        ~CContactStoreList();

    public:     // CContactStoreList public methods

        /**
        @function   AppendL
        @discussion Appends contact store to the store list.
        @param      aContactStore Contact store to append.
        */
        void AppendL(CContactStore* aContactStore);

    public:     // Methods from MVPbkContactStoreList

        /**
        @function   Count
        @discussion Returns number of stores in list
        @return     Number of stores in list
        */
        TInt Count() const;

        /**
        @function   At
        @discussion Returns contact store at index
        @param      aIndex Index to get store
        @return     Contact store
        */
        MVPbkContactStore& At(TInt aIndex) const;

        /**
        @function   Find
        @discussion Returns the contact store by URL
        @param      aUri Contact stores URI
        @return     Number of stores in list
        */
        MVPbkContactStore* Find(const TVPbkContactStoreUriPtr& aUri) const;

        /**
        @function   OpenAllL
        @discussion Opens all stores in this list asynchronously.
        @param      aObserver Observer for the open process.
        @exception  KErrInUse Store list is already opening.
        */
        void OpenAllL(MVPbkContactStoreListObserver& aObserver);

        /**
        @function   CloseAll
        @discussion Closes all stores in the list.
        @param      aObserver Observer for the close process.
        @exception  KErrInUse Store list is already closing.
        */
        void CloseAll(MVPbkContactStoreListObserver& aObserver);

    public:     // Methods from MVPbkContactStoreObserver

        /**
        @function   StoreReady
        @discussion Called when a contact store is ready to use.
        @param      aContactStore Store that is ready to use
        */
        void StoreReady(MVPbkContactStore& aContactStore);

        /**
        @function   StoreUnavailable
        @discussion Called when a contact store becomes unavailable.
        @param      aContactStore The store that became unavailable.
        @param      aReason The reason why the store is unavailable.
                            This is one of the system wide error codes.
        */
        void StoreUnavailable(MVPbkContactStore& aContactStore,TInt aReason);

        /**
        @function   HandleStoreEventL
        @discussion Called when changes occur in the contact store.
        @see        TVPbkContactStoreEvent
        @param      aContactStore The store that became unavailable.
        @param      aStoreEvent   Event that has occured.
        */
        void HandleStoreEventL(MVPbkContactStore& aContactStore, 
                            TVPbkContactStoreEvent aStoreEvent);

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
        @function   CreateCompressStoresOperationL
        @discussion Creates an operation for compressing all the stores.
        @param      aObserver Observer for the compress operation.
        @return     Created operation.
        */        
        MVPbkContactOperation* CreateCompressStoresOperationL(
                MVPbkBatchOperationObserver& aObserver);

    private:    // CContactStoreList private constructors

        /**
        @function   CContactStoreList
        @discussion CContactStoreList default contructor
        */
        CContactStoreList(CContactStoreDomain& aStoreDomain);

        /**
        @function   ConstructL
        @discussion Perform the second stage construction of CContactStoreList 
        */
        void ConstructL(const CVPbkContactStoreUriArray& aURIList);

    private:    // CContactStoreList private methods

        /**
        @function   NotifyIfAllStoresReady
        @discussion Notify stores ready
        */
        void NotifyIfAllStoresReady();

    private:    // CContactStoreList private member variables

        /// Contact stores owned by this list
        RPointerArray<CContactStore> iStores;

        /// The store domain reference
        CContactStoreDomain& iStoreDomain;

        /// The observer that is notified when stores are open
        MVPbkContactStoreListObserver* iObserver;

        /// Open store count
        TInt iOpenCount;

    };
} // End of namespace LDAPStore
#endif // __CONTACTSTORELIST_H__
