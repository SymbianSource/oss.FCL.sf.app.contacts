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
* Description:  Phonebook 2 store manager.
*
*/


#ifndef CPBK2STOREMANAGER_H
#define CPBK2STOREMANAGER_H

// INCLUDES
#include <e32base.h>
#include <MPbk2StoreObservationRegister.h>
#include <MVPbkContactStoreListObserver.h>
#include <MPbk2StoreConfigurationObserver.h>
#include <MPbk2DefaultSavingStoreObserver.h>
#include <MPbk2StoreValidityInformer.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkContactStoreList;
class CPbk2StoreConfiguration;
class MPbk2CompressPolicyManager;

// CLASS DECLARATION

/**
 * Phonebook 2 store manager.
 * The responsibilities of this class are to retrieve and observer the
 * device contact store configuration and to open these stores through
 * the Virtual Phonebook contact manager. It also manages the
 * compression of the contact stores.
 */
class CPbk2StoreManager : public CBase,
                          public MPbk2StoreObservationRegister,
                          public MPbk2StoreValidityInformer,
                          public MVPbkContactStoreListObserver,
                          public MPbk2StoreConfigurationObserver,
                          public MPbk2DefaultSavingStoreObserver
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager           Virtual Phonebook
         *                                  contact manager.
         * @param aStoreConfiguration       Contact store configuration.
         * @param aActOnConfigurationEvents Indicates whether to act on
         *                                  configuration events.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StoreManager* NewL(
                CVPbkContactManager& aContactManager,
                CPbk2StoreConfiguration& aStoreConfiguration,
                TBool aActOnConfigurationEvents );

        /**
         * Destructor.
         */
        ~CPbk2StoreManager();

    public: // Interface

        /**
         * Opens all the contact stores that are loaded by
         * Virtual Phonebook contact manager.
         */
        IMPORT_C void OpenStoresL();

        /**
         * Enables and disables compression.
         *
         * @param aEnable   Indicates whether to enable or
         *                  disable compression.
         */
        IMPORT_C void EnableCompression(
                TBool aEnable );

        /**
         * Ensures default saving store is loaded
         */
        IMPORT_C void EnsureDefaultSavingStoreIncludedL();

        /**
         * Ensures current configuation stores are loaded
         */
        IMPORT_C void EnsureCurrentConfigurationStoresIncludedL();

    public: // From MPbk2ContactStoreObservationRegister
        void RegisterStoreEventsL(
                const MVPbkContactStoreObserver& aObserver );
        void DeregisterStoreEvents(
                const MVPbkContactStoreObserver& aObserver );

    public: // From MPbk2StoreValidityInformer
        CVPbkContactStoreUriArray* CurrentlyValidStoresL() const;
        CVPbkContactStoreUriArray* CurrentlyValidShownStoresL() const;

    private: // From MVPbkContactStoreListObserver
        void OpenComplete();
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aEvent );

    private: // From MPbk2StoreConfigurationObserver
        void ConfigurationChanged();
        void ConfigurationChangedComplete();

    private: // From MPbk2DefaultSavingStoreObserver
        void SavingStoreChanged();

    private: // Implementation
        CPbk2StoreManager(
                CVPbkContactManager& aContactManager,
                CPbk2StoreConfiguration& aStoreConfiguration,
                TBool aActOnConfigurationEvents );
        void ConstructL();
        void HandleConfigurationChangedL();
        void EnsureCurrentConfigurationLoadedAndOpenL();
        void CloseUnnecessaryStoresL();
        void HandleSavingStoreChangedL();
        TBool IsStoreOpen(
                MVPbkContactStore& aContactStore ) const;
        void ReplaceStoreL(
                MVPbkContactStore& aContactStore );

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Contact stores, owned by the manager
        MVPbkContactStoreList* iContactStores;
        /// Own: An array of stores that failed to open
        RPointerArray<MVPbkContactStore> iFailedStores;
        /// Own: An array of store event observers
        RPointerArray<MVPbkContactStoreObserver> iStoreObservers;
        /// Ref: Store configuration
        CPbk2StoreConfiguration& iStoreConfiguration;
        /// Own: Store compression policy manager
        MPbk2CompressPolicyManager* iCompressPolicy;
        /// Ref: list of opened contact stores
        RPointerArray<MVPbkContactStore> iOpenedStores;
        /// Own: Indicates whether to act on configuration events
        TBool iActOnConfigurationEvents;
    };

#endif // CPBK2STOREMANAGER_H

// End of File
