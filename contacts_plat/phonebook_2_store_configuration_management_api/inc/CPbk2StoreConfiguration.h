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
* Description:  Phonebook 2 contact store configuration.
*
*/


#ifndef CPBK2STORECONFIGURATION_H
#define CPBK2STORECONFIGURATION_H

// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class TVPbkContactStoreUriPtr;
class CVPbkContactStoreUriArray;
class CPbk2StoreConfigurationImpl;
class MPbk2StoreConfigurationObserver;
class MPbk2DefaultSavingStoreObserver;

/**
 * Phonebook 2 contact store configuration.
 * The responsability of this class is to provide a centrally
 * managed location where clients can retrieve the current store configuration
 * of the device and to register as an observer to any store configuration 
 * events.
 */
class CPbk2StoreConfiguration : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StoreConfiguration* NewL();

        /**
         * Destructor.
         */
        ~CPbk2StoreConfiguration();

    public: // Interface

        /**
         * Fetches an array of configured contact store URIs.
         *
         * @return  Array of URIs that is the current configuration.
         */
        IMPORT_C CVPbkContactStoreUriArray* CurrentConfigurationL() const;

        /**
         * Fetches an array of contact store URIs to be used
         * in searching, for example in phone number matching.
         *
         * @return  Array of URIs to be used for searching.
         */
        IMPORT_C CVPbkContactStoreUriArray* SearchStoreConfigurationL();

        /**
         * Fetches an array of supported contact stores URIs.
         * NOTE: All returned stores are not necessarily supported by
         * the current HW / SW configuration of the phone.
         *
         * @return  Array of URIs that is the supported store configuration.
         */
        IMPORT_C CVPbkContactStoreUriArray* SupportedStoreConfigurationL() const;

        /**
         * Returns the URI of the default store for saving contacts.
         *
         * @return  URI of the default store.
         */
        IMPORT_C TVPbkContactStoreUriPtr DefaultSavingStoreL() const;

        /**
         * Adds a new contact store URI to current configuration.
         * Then use AddSupportedContactStoreUriL to add the store to
         * supported configuration if desired.
         *
         * @param aURI      The URI to add.
         */
        IMPORT_C void AddContactStoreURIL(
                TVPbkContactStoreUriPtr aURI );

        /**
         * Removes a contact store URI from current configuration.
         * Then use RemoveSupportedContactStoreUriL to remove the store from
         * supported configuration if desired.
         *
         * @param aURI      The URI to remove.
         */
        IMPORT_C void RemoveContactStoreURIL(
                TVPbkContactStoreUriPtr aURI );

        /**
         * Adds a new configuration observer.
         *
         * @param aObserver     The observer to add.
         */
        IMPORT_C void AddObserverL(
                MPbk2StoreConfigurationObserver& aObserver );

        /**
         * Removes a configuration observer.
         *
         * @param aObserver     The observer to remove.
         */
        IMPORT_C void RemoveObserver(
                MPbk2StoreConfigurationObserver& aObserver );

        /**
         * Adds a default saving store observer.
         *
         * @param aObserver     The observer to add.
         */
        IMPORT_C void AddDefaultSavingStoreObserverL(
                MPbk2DefaultSavingStoreObserver& aObserver );

        /**
         * Removes a default saving store observer.
         *
         * @param aObserver     The observer to remove.
         */
        IMPORT_C void RemoveDefaultSavingStoreObserver(
                MPbk2DefaultSavingStoreObserver& aObserver );

        /**
         * Adds a new contact store URI to supported configuration.
         * Use AddContactStoreURIL to add the store to current
         * configuration if desired.
         *
         * @param aURI      The URI to add.
         */
        IMPORT_C void AddSupportedContactStoreURIL(
                TVPbkContactStoreUriPtr aURI );

        /**
         * Removes a contact store URI from supported configuration.
         * Use RemoveContactStoreUriL to remove the store from
         * current configuration if desired.
         *
         * @param aURI      The URI to remove.
         */
        IMPORT_C void RemoveSupportedContactStoreURIL(
                TVPbkContactStoreUriPtr aURI );

    private: // Implementation
        CPbk2StoreConfiguration();
        void ConstructL();

    private: // Data
        /// Own: Implementation
        CPbk2StoreConfigurationImpl* iImplementation;
    };

#endif // CPBK2STORECONFIGURATION_H

// End of File
