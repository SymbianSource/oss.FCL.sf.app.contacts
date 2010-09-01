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
* Description:  Phonebook 2 server application services.
*
*/


#ifndef CPBK2SERVERAPPLICATIONSERVICES_H
#define CPBK2SERVERAPPLICATIONSERVICES_H

// INCLUDES
#include <e32base.h>
#include <mpbk2applicationservices.h>
#include <mpbk2applicationservices2.h>

// FORWARD DECLARATIONS
class CPbk2StoreManager;
class CPbk2ContactViewSupplier;
class CPbk2ServiceManager;

// CLASS DECLARATION

/**
 * Phonebook 2 server application services.
 * Responsible for owning and delivering commonly used shared objects
 * and providing some common application level services.
 */
class CPbk2ServerApplicationServices : public CBase,
                                       public MPbk2ApplicationServices,
                                       public MPbk2ApplicationServices2
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2ServerApplicationServices* NewL();

        /**
         * Destructor.
         */
        ~CPbk2ServerApplicationServices();

    public: // From MPbk2ApplicationServices
        CVPbkContactManager& ContactManager() const;
        MPbk2CommandHandler* CommandHandlerL();
        MPbk2ContactNameFormatter& NameFormatter() const;
        CPbk2SortOrderManager& SortOrderManager() const;
        CSendUi* SendUiL();
        CPbk2StorePropertyArray& StoreProperties() const;
        CPbk2FieldPropertyArray& FieldProperties() const;
        CPbk2StoreConfiguration& StoreConfiguration() const;
        MPbk2StoreObservationRegister& StoreObservationRegister() const;
        MPbk2StoreValidityInformer& StoreValidityInformer() const;
        MPbk2ContactViewSupplier& ViewSupplier() const;
        TInt GlobalMenuFilteringFlagsL() const;
        TAny* MPbk2ApplicationServicesExtension( TUid aExtensionUid );    
    
    public: // From MPbk2ApplicationServices2
        CPbk2StoreManager& StoreManager() const;
        CPbk2ServiceManager& ServiceManager() const;

    private: // Implementation
        CPbk2ServerApplicationServices();
        void ConstructL();
        void CreateManagerL();
        void CreateSortOrderManagerL();
        void CreateStorePropertyArrayL();
        void CreateFieldPropertyArrayL();
        void CreateNameFormatterL();
        void CreateViewSupplierL();
        void CreateStoreConfigurationL();

    private: // Data
        /// Own: Virtual Phonebook contact manager
        CVPbkContactManager* iContactManager;
        /// Own: Store manager
        CPbk2StoreManager* iStoreManager;
        /// Own: Sort order manager
        CPbk2SortOrderManager* iSortOrderManager;
        /// Own: Contact name formatter
        MPbk2ContactNameFormatter* iNameFormatter;
        /// Own: Store properties
        CPbk2StorePropertyArray* iStoreProperties;
        /// Own: Field properties
        CPbk2FieldPropertyArray* iFieldProperties;
        /// Own: View supplier
        CPbk2ContactViewSupplier* iViewSupplier;
        /// Own: Contact store configuration
        CPbk2StoreConfiguration* iStoreConfiguration;
        /// Own: Service manager
        CPbk2ServiceManager* iServiceManager;
    };

#endif // CPBK2SERVERAPPLICATIONSERVICES_H

// End of File
