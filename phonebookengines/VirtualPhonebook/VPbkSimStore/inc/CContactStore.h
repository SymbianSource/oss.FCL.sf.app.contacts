/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The virtual phonebook sim store implementation
*
*/


#ifndef VPBKSIMSTORE_CCONTACTSTORE_H
#define VPBKSIMSTORE_CCONTACTSTORE_H


// INCLUDES
#include <e32base.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStore2.h>
#include <MVPbkContactObserver.h>
#include <MVPbkSimStoreObserver.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CVPbkContactStoreUri;
class MVPbkContactObserver;
class MVPbkContactOperation;
class MVPbkBatchOperationObserver;
class MVPbkSingleContactOperationObserver;
class MVPbkContactFindObserver;
class MVPbkFieldTypeList;
class MVPbkContactBookmark;
class CVPbkContactStoreProperties;
class TVPbkContactStoreUriPtr;
class MVPbkSimCntStore;
struct TVPbkGsmStoreProperty;
template<typename MVPbkContactStoreObserver> class CVPbkAsyncObjectOperation;

namespace VPbkSimStore {

// DATA TYPES
enum TSimStoreState
    {
    EStoreNotOpen,
    EStoreOpen,
    EStoreNotAvailable
    };

// FORWARD DECLARATIONS
class CContact;
class CFieldTypeMappings;
class CContactStoreDomain;
class CSupportedFieldTypes;
class CRemoteStore;
class CContactStoreInfo;

// CLASS DECLARATIONS

/**
*  The virtual phonebook store implementation
*
*/
NONSHARABLE_CLASS( CContactStore ):
        public CBase,
        public MVPbkContactStore,
        protected MVPbkSimStoreObserver

    {
    public:  // Constructor and destructor
        /**
         * Creates a new instance of this class.
         * @param aURI  Filename URI of the SIM database to map.
         * @param aStoreDomain the sim store domain
         * @return A new instance of this class.
         */
        static CContactStore* NewL( const TVPbkContactStoreUriPtr& aURI,
            CContactStoreDomain& aStoreDomain );

        ~CContactStore();

    public:  // New functions

        /**
        * @return supported field types of the sim store
        */
        inline const CSupportedFieldTypes& SupportedFieldTypes() const;

        inline TUint32 SimStoreCapabilities() const;

        /**
        * Returns the field type mappings vpbk <-> sim
        * @return the field type mappings vpbk <-> sim
        */
        CFieldTypeMappings& FieldTypeMappings();

        /**
        * @return the master field type list
        */
        const MVPbkFieldTypeList& MasterFieldTypeList() const;

        /**
        * Returns the remote store
        * @return  the remote store
        */
        MVPbkSimCntStore& NativeStore();

        /**
        * Creates a sim contact link
        * @param aSimIndex the sim index of the contact
        * @return a contact link to given sim index
        */
        MVPbkContactLink* CreateLinkLC( TInt aSimIndex ) const;

        /**
        * Reads a contact from the store. Caller takes the ownership
        * @param aSimIndex the index of the contact in the sim
        * @return a contact
        */
        CContact* ReadContactL( TInt aSimIndex );

        /**
        * Matches contact store
        * @param aContactStoreUri the URI to compare
        * @return ETrue if this is the same store
        */
        TBool MatchContactStore( const TDesC& aContactStoreUri ) const;

        /**
        * Matches contact store domain
        * @param aContactStoreUri the domain to compare
        * @return ETrue if this store is in the same domain
        */
        TBool MatchContactStoreDomain( const TDesC& aContactStoreDomain ) const;

        /**
        * Creates a contact bookmark
        * @param aSimIndex the sim index
        * @return bookmark
        */
        MVPbkContactBookmark* CreateBookmarkLC( TInt aSimIndex ) const;

        /**
        * Returns file session
        * @return file session
        */
        RFs& ContactStoreDomainFsSession();


    public:  // Functions from base classes

        /**
        * From MVPbkContactOperationFactory
        */
        MVPbkContactOperation* CreateContactRetrieverL(
            const MVPbkContactLink& aLink,
            MVPbkSingleContactOperationObserver& aObserver );

        /**
        * From MVPbkContactOperationFactory
        */
        MVPbkContactOperation* CreateDeleteContactsOperationL(
            const MVPbkContactLinkArray& aContactLinks,
            MVPbkBatchOperationObserver& aObserver );

        /**
        * From MVPbkContactOperationFactory
        */
        MVPbkContactOperation* CreateCommitContactsOperationL(
            const TArray<MVPbkStoreContact*>& aContacts,
            MVPbkBatchOperationObserver& aObserver );

        /**
        * From MVPbkContactOperationFactory
        */
        MVPbkContactOperation* CreateMatchPhoneNumberOperationL(
            const TDesC& aPhoneNumber,
            TInt aMaxMatchDigits,
            MVPbkContactFindObserver& aObserver );

        /**
        * From MVPbkContactOperationFactory
        */
        MVPbkContactOperation* CreateFindOperationL(
            const TDesC& aSearchString,
            const MVPbkFieldTypeList& aFieldTypes,
            MVPbkContactFindObserver& aObserver );

        /**
        * From MVPbkContactOperationFactory
        */
        MVPbkContactOperation* CreateFindOperationL(
            const MDesC16Array& aSearchStrings,
            const MVPbkFieldTypeList& aFieldTypes,
            MVPbkContactFindFromStoresObserver& aObserver,
            const TCallBack& aWordParserCallBack );

        /**
         * From MVPbkContactOperationFactory
         */
        MVPbkContactOperation* CreateCompressStoresOperationL(
        		MVPbkBatchOperationObserver& aObserver );

        /**
        * From MVPbkContactStore
        */
        const MVPbkContactStoreProperties& StoreProperties() const;

        /**
        * From MVPbkContactStore
        */
        void OpenL(MVPbkContactStoreObserver& aObserver );

        /**
        * From MVPbkContactStore
        */
        void ReplaceL(MVPbkContactStoreObserver& aObserver );

        /**
        * From MVPbkContactStore
        */
        void Close(MVPbkContactStoreObserver& aObserver );

        /**
        * From MVPbkContactStore
        */
        MVPbkStoreContact* CreateNewContactLC();

        /**
        * From MVPbkContactStore
        */
        MVPbkContactGroup* CreateNewContactGroupLC();

        /**
        * From MVPbkContactStore
        */
        MVPbkContactView* CreateViewLC(
            const CVPbkContactViewDefinition& aViewDefinition,
            MVPbkContactViewObserver& aObserver,
            const MVPbkFieldTypeList& aSortOrder );

        /**
        * From MVPbkContactStore
        */
        MVPbkContactLinkArray* ContactGroupsLC() const;

        /**
        * From MVPbkContactStore
        */
        const MVPbkContactStoreInfo& StoreInfo() const;

        /**
        * From MVPbkContactStore
        */
        MVPbkContactLink* CreateLinkFromInternalsLC(
            RReadStream& aStream) const;

    protected:  // Functions from base classes

        /**
        * From MVPbkSimStoreObserver
        */
        void StoreReady( MVPbkSimCntStore& aStore );

        /**
        * From MVPbkSimStoreObserver
        */
        void StoreError( MVPbkSimCntStore& aStore, TInt aError );

        /**
        * From MVPbkSimStoreObserver
        */
        void StoreNotAvailable( MVPbkSimCntStore& aStore );

        /**
         * From MVPbkSimStoreObserver
         */
        void StoreContactEvent( TEvent aEvent, TInt aSimIndex );

    private:  // Implementation
        /**
        * C++ constructor
        */
        CContactStore( CContactStoreDomain& aStoreDomain );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TVPbkContactStoreUriPtr& aURI );

    private:    // New functions
        /// Callback function for OpenL
        void DoOpenL( MVPbkContactStoreObserver& aObserver );
        /// An error handling of DoOpenL
        void DoOpenError( MVPbkContactStoreObserver& aObserver, TInt aError );
        /// Requests properties from remote store and set vpbk property
        void SetStaticProperties( TVPbkGsmStoreProperty& aProperties );

    private:  // Data
        /// A reference to the sim store domain
        CContactStoreDomain& iStoreDomain;
        /// The URI of this store
        CVPbkContactStoreUri* iStoreURI;
        /// Own: static properties of the store
        CVPbkContactStoreProperties* iProperties;
        /// Own: the remote store
        CRemoteStore* iNativeStore;
        /// An array of observers that are notified after
        /// asynchronous store event.
        RPointerArray<MVPbkContactStoreObserver> iStoreObservers;
        /// Own: an asynchronous call back that is needed if observer
        /// must be notified e.g from OpenL
        CVPbkAsyncObjectOperation<MVPbkContactStoreObserver>* iAsyncOpenOp;
        /// Own: the supported virtual phonebook field types
        CSupportedFieldTypes* iSupportedFieldTypes;
        /// Internal state of the store
        TSimStoreState iStoreState;
        /// Own: the contact store info
        CContactStoreInfo* iStoreInfo;
        /// Sim store static capabilities that set when store is ready
        TUint32 iSimStoreCapabilities;
    };

// INLINE FUNCTIONS
inline const CSupportedFieldTypes& CContactStore::SupportedFieldTypes() const
    {
    return *iSupportedFieldTypes;
    }

inline TUint32 CContactStore::SimStoreCapabilities() const
    {
    return iSimStoreCapabilities;
    }
} // namespace VPbkSimStore

#endif  // VPBKSIMSTORE_CCONTACTSTORE_H
//End of file


