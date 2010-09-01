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
* Description:  Phonebook 2 application services.
*
*/


#ifndef CPBK2APPLICATIONSERVICES_H
#define CPBK2APPLICATIONSERVICES_H

// INCLUDES
#include <e32base.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ApplicationServices2.h>

// FORWARD DECLARATIONS
class CPbk2StoreManager;
class CPbk2ContactViewSupplier;
class CPbk2ServiceManager;

// CLASS DECLARATION

/**
 * Phonebook 2 application services.
 * Responsible for owning and delivering commonly used shared objects
 * and providing some common application level services.
 */
NONSHARABLE_CLASS(CPbk2ApplicationServices) : public CBase,
                                 public MPbk2ApplicationServices,
                                 public MPbk2ApplicationServices2

    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2ApplicationServices* InstanceL();
        
        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2ApplicationServices* InstanceLC();

        /**
         * Destructor.
         */
        ~CPbk2ApplicationServices();

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
        TBool LocallyVariatedFeatureEnabled(
            TVPbkLocalVariantFlags aFeatureFlag);
        TAny* MPbk2ApplicationServicesExtension( TUid aExtensionUid );

    public: // From MPbk2ApplicationServices2
        CPbk2StoreManager& StoreManager() const;
        CPbk2ServiceManager& ServiceManager() const;

    public:
        /**
         * Pushes this object to the cleanup stack for releasing.
         */
        inline void PushL();

    private: // Release interface
        friend void Release( CPbk2ApplicationServices* aObj );
        virtual void DoRelease();
        static void CleanupRelease( TAny* aObj );

    private: // Implementation
        CPbk2ApplicationServices();
        void ConstructL();
        void CreateManagerL();
        void CreateSortOrderManagerL();
        void CreateStorePropertyArrayL();
        void CreateFieldPropertyArrayL();
        void CreateNameFormatterL();
        void CreateViewSupplierL();
        void CreateStoreConfigurationL();
        void GetLocalVariationFlags();

        void IncRef();
        TInt DecRef();

    private: // Data
        /// Own: Reference count
        TInt iRefCount;
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
        /// Own: local variation flags
        TInt iLocalVariationFlags;

    };

// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// Release
// --------------------------------------------------------------------------
//
inline void Release( CPbk2ApplicationServices* aObj )
    {
    if ( aObj )
        {
        aObj->DoRelease();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::PushL
// --------------------------------------------------------------------------
//
inline void CPbk2ApplicationServices::PushL()
    {
    CleanupStack::PushL( TCleanupItem( CleanupRelease, this ) );
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::CleanupRelease
// --------------------------------------------------------------------------
//
inline void CPbk2ApplicationServices::CleanupRelease( TAny* aObj )
    {
    Release( static_cast<CPbk2ApplicationServices*>( aObj ) );
    }

#endif // CPBK2APPLICATIONSERVICES_H

// End of File
