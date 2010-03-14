/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 server application contact store manager.
*
*/


#ifndef CPBK2SERVERAPPSTOREMANAGER_H
#define CPBK2SERVERAPPSTOREMANAGER_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactViewObserver.h>
#include <VPbkContactView.hrh>
#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATIONS
class CPbk2ServerAppMonitor;
class CVPbkContactStoreUriArray;
class MVPbkContactViewBase;
class CPbk2StoreManager;
class CVPbkFieldTypeSelector;
class CVPbkContactManager;
class MPbk2ServerAppStartupObserver;
class CPbk2StorePropertyArray;
class CPbk2SortOrderManager;
class MVPbkStoreContact;
class CPbk2StoreConfiguration;
class CVPbkSortOrderAcquirer;
class MVPbkContactLinkArray;
class CPbk2MyCardFilter;

// CLASS DECLARATION

/**
 * Phonebook 2 server application contact store manager.
 */
class CPbk2ServerAppStoreManager : public CBase,
                                   public MVPbkContactViewObserver,
                                   public MVPbkContactStoreObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aStoreManager     Contact store manager.
         * @param aContactManager   Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2ServerAppStoreManager* NewL(
                CPbk2StoreManager& aStoreManager,
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2ServerAppStoreManager();

    public: // Interface

        /**
         * Loads and opens given contact stores.
         *
         * @param aStoreUris                The URIs of the stores to open.
         * @param aObserver                 Start-up observer.
         * @param aLoadSavingMemoriesAlways If Etrue default saving memory and
         *                                  backup saving memory are loaded and
         *                                  opened always.
         */
        void LoadAndOpenContactStoresL(
                CVPbkContactStoreUriArray& aStoreUris,
                MPbk2ServerAppStartupObserver& aObserver,
                TBool aLoadSavingMemoriesAlways );

        /**
         * Builds a contact view to be used in the fetch phase.
         *
         * @param aViewType             View type, only group and
         *                              contacts types are supported.
         * @param aStoreUris            URIs of the stores to include
         *                              in the view.
         * @param aViewFilter           Contact view filter.
         * @param aStoreProperties      Phonebook contact store properties.
         * @param aSortOrderManager     Sort order manager.
         * @param aFlags                Contact view flags 
         *                              @see TVPbkContactViewFlag
         * @return  Contact view.
         */
        MVPbkContactViewBase* BuildFetchViewL(
                TVPbkContactViewType aViewType,
                const CVPbkContactStoreUriArray& aStoreUris,
                CVPbkFieldTypeSelector* aViewFilter,
                CPbk2StorePropertyArray& aStoreProperties,
                const CPbk2SortOrderManager& aSortOrderManager,
                const TUint32 aFlags );

        /**
         * Removes read-only stores from the given store URI list.
         *
         * @param aStoreUriArray    Store URI array to operate with.
         * @param aStoreProperties  Phonebook store properties.
         */
        void RemoveReadOnlyStoreUris(
                CVPbkContactStoreUriArray& aStoreUriArray,
                CPbk2StorePropertyArray& aStoreProperties ) const;

        /**
         * Checks whether the given contact is included in the
         * current configuration.
         *
         * @param aContact              The contact to inspect.
         * @param aStoreConfiguration   Phonebook store configuration.
         * @return  ETrue if contact belongs to the configuration.
         */
        TBool IsContactIncludedInCurrentConfigurationL(
                const MVPbkStoreContact& aContact,
                const CPbk2StoreConfiguration& aStoreConfiguration ) const;

        /**
         * Checks are given contacts from stores that are available.
         *
         * @param aContactLinks     Contacts whose stores to inspect.
         * @return  EFalse if any of the store that was
         *          inspected is unavailable.
         */
        TBool ContactsAvailableL(
                const MVPbkContactLinkArray& aContactLinks ) const;

        /**
         * Returns currently valid stores.
         *
         * @return  Currently valid stores.
         */
        CVPbkContactStoreUriArray* CurrentlyValidStoresL() const;

    private: // From MVPbkContactViewObserver
        void ContactViewReady(
                MVPbkContactViewBase& aView );
        void ContactViewUnavailable(
                MVPbkContactViewBase& aView );
        void ContactAddedToView(
                MVPbkContactViewBase& aView, 
                TInt aIndex, 
                const MVPbkContactLink& aContactLink );
        void ContactRemovedFromView(
                MVPbkContactViewBase& aView, 
                TInt aIndex, 
                const MVPbkContactLink& aContactLink );
        void ContactViewError(
                MVPbkContactViewBase& aView, 
                TInt aError, 
                TBool aErrorNotified );

    private: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aEvent );

    private: // Implementation
        CPbk2ServerAppStoreManager(
                CPbk2StoreManager& aStoreManager,
                CVPbkContactManager& aContactManager );
        void ConstructL();
    
    private: // Data
        /// Ref: Store manager
        CPbk2StoreManager& iStoreManager;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Own: Server app monitor
        CPbk2ServerAppMonitor* iMonitor;
        /// Own: View for fetch names list
        MVPbkContactViewBase* iFetchView;
        /// Own: Sort order aquirerer for group extension
        CVPbkSortOrderAcquirer* iSortOrderAcquirer;
        /// Ref: list of opened contact stores
        RPointerArray<MVPbkContactStore> iOpenedStores;
        /// Own: 
        CPbk2MyCardFilter* iMyCardFilter;
    };

#endif // CPBK2SERVERAPPSTOREMANAGER_H

// End of File
