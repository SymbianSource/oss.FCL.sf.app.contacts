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
* Description:  Contacts Model store contact store implementation.
*
*/


#ifndef VPBKCNTMODEL_CCONTACTSTORE_H
#define VPBKCNTMODEL_CCONTACTSTORE_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStore2.h>
#include <MVPbkContactOperationFactory.h>
#include <VPbkFieldType.hrh>
#include <MVPbkContactStoreObserver.h>
#include <cntdb.h>
#include <cntviewbase.h>
#include "CContactStoreDomain.h"

// FORWARD DECLARATIONS
class MVPbkFieldType;
class CVPbkContactStoreUri;
class MVPbkContactLink;
class MVPbkContactObserver;
class CVPbkContactStoreProperties;
class MVPbkContactBookmark;
template<typename MVPbkContactStoreObserver> class CVPbkAsyncObjectOperation;

namespace VPbkEngUtils {
class CVPbkDiskSpaceCheck;
} /// namespace

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CFieldTypeMap;
class CContact;
class CFieldFactory;
class CAsyncContactOperation;
class CFieldsInfo;
class CContactStoreInfo;
class CNamedRemoteViewHandle;

#ifdef _DEBUG
enum TContactStorePanic
	{
	EPreCond_NativeDatabase,
	EPreCond_ContactDestroyed,
	EPreCond_CreateNewContactGroupLC,
	EPreCond_ContactGroupsLC,
	EPreCond_OwnContactLinkL
	};

void ContactStorePanic( TContactStorePanic aPanic );
#endif // _DEBUG


// CLASS DECLARATIONS

/**
 * Maps a Contact Model database to a virtual Phonebook contact store.
 */
NONSHARABLE_CLASS( CContactStore ): public CBase,
                                    public MVPbkContactStore,
                                    public MVPbkContactStore2,
                                    private MContactDbObserver
    {
    public: // Constructor and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aURI          Filename URI of the Contacts Model
         *                      database to map.
         * @param aStoreDomain  Contact store domain.
         * @return  A new instance of this class.
         */
        static CContactStore* NewL(
                const TVPbkContactStoreUriPtr& aURI,
                CContactStoreDomain& aStoreDomain );

        /**
         * Destructor.
         */
        ~CContactStore();

    public: // Interface
        /**
         * Returns the field type map to use for mapping Contact Model field
         * types to Virtual Phonebook field types.
         *
         * @return  Field type map.
         */
        const CFieldTypeMap& FieldTypeMap() const;

        /**
         * Returns the field factory.
         *
         * @return  Field factory.
         */
        const CFieldFactory& FieldFactory() const;

        /**
         * Returns the master field type list of Virtual Phonebook.
         *
         * @return  Master field type list.
         */
        const MVPbkFieldTypeList& MasterFieldTypeList() const;

        /**
         * Creates and returns a new Contacts Model field based on Virtual
         * Phonebook generic field type.
         *
         * @param aFieldType    Field type.
         * @return  Contacts Model field.
         */
        CContactItemField* CreateFieldLC(
                const MVPbkFieldType& aFieldType ) const;

        /**
         * Reads a contact from the database.
         *
         * @param aContactId    Id of the contact to read.
         * @param aObserver     Observer.
         */
        void ReadContactL(
                TContactItemId aContactId,
                MVPbkContactObserver& aObserver );

        /**
         * Locks a contact in the database.
         *
         * @param aClient   The contact to lock.
         * @param aObserver Observer.
         */
        void LockContactL(
                const CContact& aClient,
                MVPbkContactObserver& aObserver );

        /**
         * Reads and locks a contact in the database.
         *
         * @param aContactId    Id of the contact to read and lock.
         * @param aObserver     Observer.
         */
        void ReadAndLockContactL(
                TContactItemId aContactId,
                MVPbkContactObserver& aObserver );

        /**
         * Deletes a contact from the database.
         *
         * @param aContactId    Id of the contact to delete.
         * @param aObserver     Observer.
         */
        void DeleteContactL(
                TContactItemId aContactId,
                MVPbkContactObserver& aObserver );

        /**
         * Commits changes to a contact to the database.
         *
         * @param aContactItem  The contact to commit.
         * @param aObserver     Observer.
         */
        void CommitContactL(
                const CContact& aContactItem,
                MVPbkContactObserver& aObserver );

        /**
         * Creates a contact link.
         *
         * @param aContactId    Id of the contact to link.
         * @return  Contact link.
         */
        MVPbkContactLink* CreateLinkLC(
                TContactItemId aContactId ) const;

        /**
         * Called from CContact destructor. aCloseContact should be
         * set for contacts that were locked. Read-only contacts
         * should provide EFalse for aCloseContact in order to avoid
         * resetting lock of another CContact instance for the same
         * contact model item.
         *
         * @param aContactItem  The destroyed contact.
         * @param aCloseContact Indicates that contact should be closed,
         * 						necessary for locked contacts.
         */
        void ContactDestroyed(
                CContactItem* aContactItem, TBool aCloseContact );

        /**
         * Returns the native contacts database that
         * this store is connected to.
         *
         * @return  Contact database.
         */
        CContactDatabase& NativeDatabase() const;

        /**
         * Compares contact stores.
         *
         * @param aContactStoreUri  The URI to compare.
         * @return  ETrue if this is the same store.
         */
        TBool MatchContactStore(
                const TDesC& aContactStoreUri ) const;

        /**
         * Compares contact store domains.
         *
         * @param aContactStoreDomain   The domain to compare.
         * @return  ETrue if this store is in the same domain.
         */
        TBool MatchContactStoreDomain(
                const TDesC& aContactStoreDomain ) const;

        /**
         * Creates a contact bookmark
         *
         * @param aContactId    Id of the contact to bookmark.
         * @return  Contact bookmark.
         */
        MVPbkContactBookmark* CreateBookmarkLC(
                TContactItemId aContactId ) const;

        /**
         * Set contact as the store's current own contact.
         *
         * @param aContactItem  The contact to set as own.
         * @param aObserver     Observer.
         */
        void SetAsOwnL(
                const CContact& aContactItem,
                MVPbkContactObserver& aObserver );

    public: // From MVPbkContactStore
        const MVPbkContactStoreProperties& StoreProperties() const;
        void OpenL(
                MVPbkContactStoreObserver& aObserver );
        void ReplaceL(
                MVPbkContactStoreObserver& aObserver );
        void Close(
                MVPbkContactStoreObserver& aObserver );
        MVPbkStoreContact* CreateNewContactLC();
        MVPbkContactGroup* CreateNewContactGroupLC();
        MVPbkContactView* CreateViewLC(
                const CVPbkContactViewDefinition& aViewDefinition,
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder );
        MVPbkContactLinkArray* ContactGroupsLC() const;
        const MVPbkContactStoreInfo& StoreInfo() const;
        MVPbkContactLink* CreateLinkFromInternalsLC(
                RReadStream& aStream ) const;
        TAny* ContactStoreExtension( TUid aExtensionUid );

    public: // From MVPbkContactStore2
        MVPbkContactOperationBase* OwnContactLinkL(
               MVPbkSingleContactLinkOperationObserver& aObserver) const;
        
    public: // Contact operations (MVPbkContactOperationFactory)
        MVPbkContactOperation* CreateContactRetrieverL(
                const MVPbkContactLink& aLink,
                MVPbkSingleContactOperationObserver& aObserver );
        MVPbkContactOperation* CreateDeleteContactsOperationL(
                const MVPbkContactLinkArray& aContactLinks,
                MVPbkBatchOperationObserver& aObserver );
        MVPbkContactOperation* CreateCommitContactsOperationL(
                const TArray<MVPbkStoreContact*>& aContacts,
                MVPbkBatchOperationObserver& aObserver );
        MVPbkContactOperation* CreateMatchPhoneNumberOperationL(
                const TDesC& aPhoneNumber,
                TInt aMaxMatchDigits,
                MVPbkContactFindObserver& aObserver );
        MVPbkContactOperation* CreateFindOperationL(
                const TDesC& aSearchString,
                const MVPbkFieldTypeList& aFieldTypes,
                MVPbkContactFindObserver& aObserver );
        MVPbkContactOperation* CreateFindOperationL(
                const MDesC16Array& aSearchStrings,
                const MVPbkFieldTypeList& aFieldTypes,
                MVPbkContactFindFromStoresObserver& aObserver,
                const TCallBack& aWordParserCallBack );
        MVPbkContactOperation* CreateCompressStoresOperationL(
			    MVPbkBatchOperationObserver& aObserver );


    public: // Interface for VPbkCntModel component
        void AddObserverL(
                MVPbkContactStoreObserver& aObserver );
        void RemoveObserver(
                MVPbkContactStoreObserver& aObserver );
        TBool IsOpened() const;
        RFs& ContactStoreDomainFsSession();

    private: // From MContactDbObserver
        void HandleDatabaseEventL(
                TContactDbObserverEvent aEvent );

    private:  // Implementation
        CContactStore(
                CContactStoreDomain& aStoreDomain );
        void ConstructL(
                const TVPbkContactStoreUriPtr& aURI );
        void DoOpenL(
                MVPbkContactStoreObserver& aObserver,
                TBool aReplace );
        void OpenError(
                MVPbkContactStoreObserver& aObserver,
                TInt aError );
        void SetStaticPropertiesL();
        void DoAddFieldTypesL();
        void CloseSystemTemplate(
                CContactDatabase* aContactDb );
        CContactDatabase* OpenInternalL();
        void DoActivateRemoteViewsL();
        RContactViewSortOrder CreateSortOrderL(
                const MVPbkFieldTypeList& aSortOrder) const;
        TContactViewPreferences ConvertViewPrefsL(const TDesC8& aData);

    private:  // Data
        /// Ref: Store domain
        CContactStoreDomain& iStoreDomain;
        /// Own: URI of this store
        CVPbkContactStoreUri* iStoreURI;
        /// Own: Store properties
        CVPbkContactStoreProperties* iProperties;
        /// Own: Stores asynchronous operation
        CVPbkAsyncObjectOperation<MVPbkContactStoreObserver>* iAsyncOpenOp;
        /// Own: Store observers
        RPointerArray<MVPbkContactStoreObserver> iObservers;
        /// Own: Contact database for this store
        CContactDatabase* iContactDb;
        /// Own: Contact database change notifier
        CContactChangeNotifier* iDbNotifier;
        /// Own: Virtual Phonebook to contact model field type mapping
        CFieldFactory* iFieldFactory;
        /// Own: Asynchronous operation
        CAsyncContactOperation* iAsyncContactOperation;
        /// Own: Field info of Contacts Model fields
        CFieldsInfo* iFieldsInfo;
        /// Own: the store info
        CContactStoreInfo* iStoreInfo;
        /// Own: Golden template contact item
        CContactItem* iSysTemplate;
        /// Own: Disk space checker
        VPbkEngUtils::CVPbkDiskSpaceCheck* iDiskSpaceCheck;
        /// Own: named remote view container
        RPointerArray<CNamedRemoteViewHandle> iNamedViewContainer;
    };


// INLINE FUNCTIONS

inline CContactDatabase& CContactStore::NativeDatabase() const
    {
    __ASSERT_DEBUG( iContactDb, ContactStorePanic( EPreCond_NativeDatabase ) );
    return *iContactDb;
    }

} /// namespace VPbkCntModel

#endif  // VPBKCNTMODEL_CCONTACTSTORE_H

//End of File


