/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A list of store domains.
*
*/


#ifndef CVPBKCONTACTSTOREDOMAINLIST_H
#define CVPBKCONTACTSTOREDOMAINLIST_H

// INCLUDES
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreListObserver.h>
#include <MVPbkContactOperationFactory.h>
#include <e32base.h>
#include <bamdesca.h>
#include <badesca.h>
#include <MVPbkContactOperationFactory2.h>

// FORWARD DECLARATIONS
class CVPbkContactStoreDomain;
class MVPbkContactStore;
class MVPbkContactLink;
class MVPbkStoreContact;
class MVPbkFieldTypeList;
class CImplementationInformation;
class TVPbkContactStoreUriPtr;
class MVPbkContactOperation;
class MVPbkSingleContactOperationObserver;
class CVPbkContactStoreUriArray;

// CLASS DECLARATIONS

/**
 * Virtual Phonebook Contact Manager Store list.
 */
NONSHARABLE_CLASS(CVPbkContactStoreDomainList) : 
        public CBase, 
        public MVPbkContactStoreList,
        public MVPbkContactOperationFactory,
        public MVPbkContactOperationFactory2,
        private MVPbkContactStoreListObserver
    {
    public: // Constrcutor and destructor
        /**
         * Creates a contact store domain list.
         * @param aURIList          List of contact stores to load.
         * @param aFieldTypeList    File type list that contains all 
         *                          possible field types in vpbk.
         * @param aSecurityInfo     Security info of client process.
         */
        static CVPbkContactStoreDomainList* NewL(
                const CVPbkContactStoreUriArray& aURIList,
                const MVPbkFieldTypeList& aFieldTypeList,
                const TSecurityInfo& aSecurityInfo );

        ~CVPbkContactStoreDomainList();

    public: // interface
        /**
         * Adds aStore to this container. Passes ownership.
         */
        void AppendL(CVPbkContactStoreDomain* aStore);

        /**
         * Returns the count of loaded domains.
         */
        TInt DomainCount() const;
       
        /**
         * Loads the contact store domain corresponding to aURIList.
         */
        void LoadPluginStoreDomainsL(
            const CVPbkContactStoreUriArray& aURIList, 
            const MVPbkFieldTypeList& aFieldTypeList,
            const TSecurityInfo& aSecurityInfo );
                
    public: // from MVPbkContactOperationFactory
        MVPbkContactOperation* CreateContactRetrieverL(
                const MVPbkContactLink& aLink,
                MVPbkSingleContactOperationObserver& aObserver);
        MVPbkContactOperation* CreateContactRetrieverL(
                const TDesC8& aLinkBuf,
                MVPbkSingleContactOperationObserver& aObserver);
        MVPbkContactLink* CreateLinkLC(
                RReadStream& aStream) const;
        MVPbkContactOperation* CreateDeleteContactsOperationL(
                const MVPbkContactLinkArray& aContactLinks, 
                MVPbkBatchOperationObserver& aObserver);
        MVPbkContactOperation* CreateCommitContactsOperationL(
                const TArray<MVPbkStoreContact*>& aContacts,
                MVPbkBatchOperationObserver& aObserver);
        MVPbkContactOperation* CreateMatchPhoneNumberOperationL(
                const TDesC& aPhoneNumber, 
                TInt aMaxMatchDigits,
                MVPbkContactFindObserver& aObserver);
        MVPbkContactOperation* CreateFindOperationL(
                const TDesC& aSearchString,
                const MVPbkFieldTypeList& aFieldTypes,
                MVPbkContactFindObserver& aObserver);
        MVPbkContactOperation* CreateFindOperationL(
                const MDesC16Array& aSearchStrings, 
                const MVPbkFieldTypeList& aFieldTypes, 
                MVPbkContactFindFromStoresObserver& aObserver, 
                const TCallBack& aWordParserCallBack );
        MVPbkContactOperation* CreateCompressStoresOperationL(
        		MVPbkBatchOperationObserver& aObserver);
       
        TAny* ContactOperationFactoryExtension( TUid aExtensionUid );
       
    public: // from MVPbkContactOperationFactory2   	
      	MVPbkContactOperation* CreateFindOperationL(
                const MDesC16Array& aSearchStrings, 
                const MVPbkFieldTypeList& aFieldTypes, 
                MVPbkContactFindFromStoresObserver& aObserver, 
                const TCallBack& aWordParserCallBack,
                const CDesC16ArrayFlat& aStoreEntriesArray);

    public: // From MVPbkContactStoreList
        TInt Count() const;
        MVPbkContactStore& At(TInt aIndex) const;
        MVPbkContactStore* Find(const TVPbkContactStoreUriPtr& aUri) const;
        void OpenAllL(MVPbkContactStoreListObserver& aObserver);
        void CloseAll(MVPbkContactStoreListObserver& aObserver);

    private: // From MVPbkContactStoreListObserver
        void StoreReady(MVPbkContactStore& aContactStore);
        void StoreUnavailable(MVPbkContactStore& aContactStore, TInt aReason);
        void HandleStoreEventL(MVPbkContactStore& aContactStore, 
                               TVPbkContactStoreEvent aStoreEvent);
        void OpenComplete();

    private: // For CIdle callback
        static TInt AsyncSignalOpenComplete( TAny *aDomainList );

    private: // implementation
        CVPbkContactStoreDomainList();
        void ConstructL(
            const CVPbkContactStoreUriArray& aURIList, 
            const MVPbkFieldTypeList& aFieldTypeList,
            const TSecurityInfo& aSecurityInfo);
        void LoadImplementationStoreDomainsL(
            const CVPbkContactStoreUriArray& aURIList, 
            const CImplementationInformation& aImplInfo,
            const MVPbkFieldTypeList& aFieldTypeList, 
            const TSecurityInfo& aSecurityInfo );
        CVPbkContactStoreDomain* FindDomain(const TDesC& aStoreDomain) const;
        void SignalOpenComplete();

    private: // data members
        /// Own: Array of store domains
        RPointerArray<CVPbkContactStoreDomain> iStoreDomains;
        /// Not Own: Listens Store list events
        MVPbkContactStoreListObserver* iListObserver;
        /// Own: Array of stores to be opened
        RPointerArray<MVPbkContactStore> iStoresToOpen;
        /// Own: If no stores are opened, this is used to make call async
        CIdle* iAsyncCallBack;
        
    };

#endif // CVPBKCONTACTSTOREDOMAINLIST_H
//End of file

