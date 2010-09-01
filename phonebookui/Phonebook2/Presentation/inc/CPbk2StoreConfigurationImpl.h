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
* Description: 
*     Phonebook 2 contact store configurations.
*
*/


#ifndef CPBK2STORECONFIGURATIONIMPL_H
#define CPBK2STORECONFIGURATIONIMPL_H

// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>
#include <MPbk2StoreConfigurationObserver.h>
#include <MPbk2DefaultSavingStoreObserver.h>

// FORWARD DECLARATIONS
class CRepository;
class CPbk2StoreConfigurationMonitor;
class TVPbkContactStoreUriPtr;
class CVPbkContactStoreUriArray;
class CPbk2DefaultSavingStoreMonitor;

/**
 * Phonebook 2 contact store configurations.
 * This classes responsability is to retrieve the store configuration
 * of the device and to monitor changes to the configuration and relay them
 * to registered observers.
 */
NONSHARABLE_CLASS( CPbk2StoreConfigurationImpl )  :
            public CBase,
            private MPbk2StoreConfigurationObserver,
            private MPbk2DefaultSavingStoreObserver
    {
	public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return A new instance of this class.
         */
		static CPbk2StoreConfigurationImpl* NewL();

        /**
         * Destructor.
         */
        ~CPbk2StoreConfigurationImpl();

    public: // Interface
        /**
         * Fetches an array of configured contact store URIs.
         *
         * @return Array of URIs that is the current configuration.
         */
        CVPbkContactStoreUriArray* CurrentConfigurationL() const;

        /**
         * Fetches an array of contact store URIs to be used
         * in searching, for example phone number matching.
         *
         * @return Array of URIs to be used for searching.
         */
        CVPbkContactStoreUriArray* SearchStoreConfigurationL();

        /**
         * Fetches an array of supported contact stores URIs.
         * NOTE: All returned stores are not necessarily supported by
         * the current HW / SW configuration of the phone.
         *
         * @return Array of URIs that is the supported store configuration.
         */
        CVPbkContactStoreUriArray* SupportedStoreConfigurationL() const;

        /**
         * Returns URI of the default store for saving contacts.
         *
         * @return URI of the default store.
         */
        TVPbkContactStoreUriPtr DefaultSavingStoreL() const;

        /**
         * Adds a new contact store URI to current configuration.
         *
         * @param aURI  URI to add to current configuration.
         */
        void AddContactStoreURIL(
            TVPbkContactStoreUriPtr aURI);

        /**
         * Removes a contact store URI from current configuration.
         *
         * @param aURI  URI to remove from current configuration.
         */
        void RemoveContactStoreURIL(
            TVPbkContactStoreUriPtr aURI);

        /**
         * Adds a new configuration observer.
         *
         * @param aObserver The observer to add.
         */
        void AddObserverL(
            MPbk2StoreConfigurationObserver& aObserver);

        /**
         * Removes a configuration observer.
         *
         * @param aObserver The observer to remove.
         */
        void RemoveObserver(
            MPbk2StoreConfigurationObserver& aObserver);

        /**
         * Adds a default saving store observer.
         *
         * @param aObserver The observer to add.
         */
        void AddDefaultSavingStoreObserverL(
            MPbk2DefaultSavingStoreObserver& aObserver);

        /**
         * Removes a default saving store observer.
         *
         * @param aObserver The observer to remove.
         */
        void RemoveDefaultSavingStoreObserver(
            MPbk2DefaultSavingStoreObserver& aObserver);

        /**
         * Adds a new contact store URI to supported configuration.
         * Use AddContactStoreURIL to add the store to current
         * configuration if desired.
         *
         * @param aURI      The URI to add.
         */
        void AddSupportedContactStoreURIL(
                TVPbkContactStoreUriPtr aURI );

        /**
         * Removes a contact store URI from supported configuration.
         * Use RemoveContactStoreUriL to remove the store from
         * current configuration if desired.
         *
         * @param aURI      The URI to remove.
         */
        void RemoveSupportedContactStoreURIL(
                TVPbkContactStoreUriPtr aURI );

    private: // From MPbk2StoreConfigurationObserver
        void ConfigurationChanged();
        void ConfigurationChangedComplete();

    private: // From MPbk2DefaultSavingStoreObserver
        void SavingStoreChanged();

    private: // Implementation
        CPbk2StoreConfigurationImpl();
        void ConstructL();
        TBool AlreadyRegisteredL(
                TVPbkContactStoreUriPtr aURI,
                RArray<TUint32> aKeys );

	private: // Data
        /// Own: Repository for fetching data
        CRepository* iRepository;
        /// Own: Configuration monitor
        CPbk2StoreConfigurationMonitor* iConfigurationMonitor;
        /// Own: Default saving store monitor
        CPbk2DefaultSavingStoreMonitor* iDefaultSavingStoreMonitor;
        /// Own: Array of observers
        RPointerArray<MPbk2StoreConfigurationObserver> iObservers;
        /// Own: Array of default saving store observers
        RPointerArray<MPbk2DefaultSavingStoreObserver> iDefaultStoreObservers;
	};

#endif // CPBK2STORECONFIGURATIONIMPL_H

// End of File
