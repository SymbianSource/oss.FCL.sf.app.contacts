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


// INCLUDE FILES
#include "CPbk2ApplicationServices.h"

// Phonebook 2
#include <CPbk2StoreManager.h>
#include <CPbk2SortOrderManager.h>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2CommandHandler.h>
#include <MPbk2ViewExplorer.h>
#include <Pbk2ContactNameFormatterFactory.h>
#include <pbk2commonui.rsg>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2FieldPropertyArray.h>
#include <Pbk2MenuFilteringFlags.hrh>
#include <Phonebook2PrivateCRKeys.h>
#include <CPbk2ContactViewSupplier.h>
#include <CPbk2StoreConfiguration.h>
#include <CPbk2ServiceManager.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>

// System includes
#include <StringLoader.h>
#include <sendui.h>
#include <centralrepository.h>

// Debugging headers
#include <Pbk2Config.hrh>
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * Returns the global instance of CPbk2ApplicationServices.
 *
 * @return  Instance of CPbk2ApplicationServices.
 */
inline CPbk2ApplicationServices* Instance()
    {
    return static_cast<CPbk2ApplicationServices*>( Dll::Tls() );
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::CPbk2ApplicationServices
// --------------------------------------------------------------------------
//
CPbk2ApplicationServices::CPbk2ApplicationServices()
    : iLocalVariationFlags(KErrNotFound)
    {
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::~CPbk2ApplicationServices
// --------------------------------------------------------------------------
//
CPbk2ApplicationServices::~CPbk2ApplicationServices()
    {
    // Command handler is calling iStoreConfiguration's RemoveObserver
    // in its destructor. So delete command handler before store
    // configuration.
    delete iServiceManager;
    delete iSortOrderManager;
    delete iNameFormatter;
    delete iFieldProperties;
    delete iViewSupplier;
    delete iStoreManager;
    delete iStoreConfiguration;
    delete iStoreProperties;
    delete iContactManager;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ApplicationServices::ConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ( "CPbk2ApplicationServices::ConstructL(0x%x)"), this );

    // Create Virtual Phonebook contact manager for Phonebook 2
    CreateManagerL();

    CreateSortOrderManagerL();
    CreateStorePropertyArrayL();
    CreateStoreConfigurationL();
    CreateViewSupplierL();

    PBK2_PROFILE_START(Pbk2Profile::EAppServicesCreateStoreManager);
    iStoreManager = CPbk2StoreManager::NewL
        ( *iContactManager, *iStoreConfiguration, ETrue );
    PBK2_PROFILE_END(Pbk2Profile::EAppServicesCreateStoreManager);

    // Set core Phonebook 2 stores
    iStoreManager->EnsureDefaultSavingStoreIncludedL();
    iStoreManager->EnsureCurrentConfigurationStoresIncludedL();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ( "CPbk2ApplicationServices::ConstructL(0x%x) Store manager created"),
          this );

    iServiceManager = CPbk2ServiceManager::NewL(iContactManager->FsSession());

    CreateFieldPropertyArrayL();
    CreateNameFormatterL();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ApplicationServices::ConstructL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ApplicationServices* CPbk2ApplicationServices::InstanceL()
    {
    CPbk2ApplicationServices* instance = Instance();

    if ( !instance )
        {
        instance = new ( ELeave ) CPbk2ApplicationServices;
        CleanupStack::PushL( instance );
        User::LeaveIfError( Dll::SetTls( instance ) );
        TRAPD( error, instance->ConstructL(); );
        if( error != KErrNone )
            {
            Dll::SetTls( NULL );
            User::Leave( error );
            }

        CleanupStack::Pop( instance );
        }

    instance->IncRef();

    return instance;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::InstanceLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ApplicationServices* CPbk2ApplicationServices::InstanceLC()
	{
	CPbk2ApplicationServices* appServices = InstanceL();
	appServices->PushL();
	return appServices;
	}

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::ContactManager
// --------------------------------------------------------------------------
//
CVPbkContactManager& CPbk2ApplicationServices::ContactManager() const
    {
    return *iContactManager;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::CommandHandlerL
// --------------------------------------------------------------------------
//
MPbk2CommandHandler* CPbk2ApplicationServices::CommandHandlerL()
    {
    // Not supported in Base Application Services
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::NameFormatter
// --------------------------------------------------------------------------
//
MPbk2ContactNameFormatter& CPbk2ApplicationServices::NameFormatter() const
    {
    return *iNameFormatter;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::SortOrderManager
// --------------------------------------------------------------------------
//
CPbk2SortOrderManager& CPbk2ApplicationServices::SortOrderManager() const
    {
    return *iSortOrderManager;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::SendUiL
// --------------------------------------------------------------------------
//
CSendUi* CPbk2ApplicationServices::SendUiL()
    {
    // Not supported in Base Application Services
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::StoreProperties
// --------------------------------------------------------------------------
//
CPbk2StorePropertyArray& CPbk2ApplicationServices::StoreProperties() const
    {
    return *iStoreProperties;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::FieldProperties
// --------------------------------------------------------------------------
//
CPbk2FieldPropertyArray& CPbk2ApplicationServices::FieldProperties() const
    {
    return *iFieldProperties;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::StoreConfiguration
// --------------------------------------------------------------------------
//
CPbk2StoreConfiguration& CPbk2ApplicationServices::StoreConfiguration() const
    {
    return *iStoreConfiguration;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::StoreObservationRegister
// --------------------------------------------------------------------------
//
MPbk2StoreObservationRegister&
        CPbk2ApplicationServices::StoreObservationRegister() const
    {
    return *iStoreManager;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::StoreValidityInformer
// --------------------------------------------------------------------------
//
MPbk2StoreValidityInformer&
        CPbk2ApplicationServices::StoreValidityInformer() const
    {
    return *iStoreManager;
    }


// --------------------------------------------------------------------------
// CPbk2ApplicationServices::ViewSupplier
// --------------------------------------------------------------------------
//
MPbk2ContactViewSupplier& CPbk2ApplicationServices::ViewSupplier() const
    {
    return *iViewSupplier;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::GlobalMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2ApplicationServices::GlobalMenuFilteringFlagsL() const
    {
    return KPbk2MenuFilteringFlagsNone;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::LocallyVariatedFeatureEnabled
// --------------------------------------------------------------------------
//
TBool CPbk2ApplicationServices::LocallyVariatedFeatureEnabled(
    TVPbkLocalVariantFlags aFeatureFlag)
    {
    TBool ret = EFalse;

    if ( iLocalVariationFlags == KErrNotFound )
        {
        GetLocalVariationFlags();
        }

    if ( iLocalVariationFlags != KErrNotFound )
        {
        ret = (iLocalVariationFlags & aFeatureFlag);
        }

    return ret;
}

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::MPbk2ApplicationServicesExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2ApplicationServices::MPbk2ApplicationServicesExtension(
    TUid aExtensionUid )
    {
    if ( aExtensionUid == KMPbk2ApplicationServicesExtension2Uid )
        {
        return static_cast<MPbk2ApplicationServices2*>( this );
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::StoreManager
// --------------------------------------------------------------------------
//
CPbk2StoreManager& CPbk2ApplicationServices::StoreManager() const
    {
    return *iStoreManager;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::ServicesManager
// --------------------------------------------------------------------------
//
CPbk2ServiceManager& CPbk2ApplicationServices::ServiceManager() const
    {
    return *iServiceManager;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::CreateManagerL
// --------------------------------------------------------------------------
//
inline void CPbk2ApplicationServices::CreateManagerL()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2ApplicationServices:CreateManagerL(0x%x)" ), this );

    // Initialize contact manager with empty URI array
    CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewL();
    CleanupStack::PushL( uriArray );

    TSecurityInfo secInfo;
    
    // Using the security info of the current process to create contact manager 
    secInfo.SetToCurrentInfo();
    
    iContactManager = CVPbkContactManager::NewL( secInfo, *uriArray );
    //iContactManager = CVPbkContactManager::NewL( *uriArray );

    CleanupStack::PopAndDestroy(); // uriArray

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ApplicationServices::CreateManagerL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::CreateSortOrderManagerL
// --------------------------------------------------------------------------
//
inline void CPbk2ApplicationServices::CreateSortOrderManagerL()
    {
    PBK2_PROFILE_START( Pbk2Profile::EAppServicesCreateSortOrderManager );

    iSortOrderManager = CPbk2SortOrderManager::NewL
        ( iContactManager->FieldTypes() );

    PBK2_PROFILE_END( Pbk2Profile::EAppServicesCreateSortOrderManager );
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::CreateStorePropertyArrayL
// --------------------------------------------------------------------------
//
inline void CPbk2ApplicationServices::CreateStorePropertyArrayL()
    {
    PBK2_PROFILE_START( Pbk2Profile::EAppServicesCreateStorePropertyArray );
    iStoreProperties = CPbk2StorePropertyArray::NewL();
    PBK2_PROFILE_END( Pbk2Profile::EAppServicesCreateStorePropertyArray );

    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2ApplicationServices::CreateStorePropertyArrayL() done ") );
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::CreateFieldPropertyArrayL
// --------------------------------------------------------------------------
//
inline void CPbk2ApplicationServices::CreateFieldPropertyArrayL()
    {
    PBK2_PROFILE_START( Pbk2Profile::EAppServicesCreateFieldPropertyArray );
    iFieldProperties = CPbk2FieldPropertyArray::NewL
        ( iContactManager->FieldTypes(), iContactManager->FsSession(),
          iServiceManager );
    PBK2_PROFILE_END( Pbk2Profile::EAppServicesCreateFieldPropertyArray );
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::CreateNameFormatterL
// --------------------------------------------------------------------------
//
inline void CPbk2ApplicationServices::CreateNameFormatterL()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2ApplicationServices::CreateNameFormatterL(0x%x)" ), this );

    HBufC* unnamedBuf = StringLoader::LoadLC( R_QTN_PHOB_UNNAMED );
    TPtr unnamed = unnamedBuf->Des();
    MPbk2ContactNameFormatter* nameFormatter =
        Pbk2ContactNameFormatterFactory::CreateL
            ( unnamed, iContactManager->FieldTypes(), *iSortOrderManager,
              &CCoeEnv::Static()->FsSession() );
    CleanupStack::PopAndDestroy( unnamedBuf );

    delete iNameFormatter;
    iNameFormatter = nameFormatter;

    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2ApplicationServices::CreateNameFormatterL(0x%x) end" ),
          this );
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::CreateViewSupplierL
// --------------------------------------------------------------------------
//
inline void CPbk2ApplicationServices::CreateViewSupplierL()
    {
    iViewSupplier = CPbk2ContactViewSupplier::NewL
        ( *iContactManager, *iSortOrderManager, *iStoreConfiguration,
          *iStoreProperties, ETrue );
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::CreateStoreConfigurationL
// --------------------------------------------------------------------------
//
inline void CPbk2ApplicationServices::CreateStoreConfigurationL()
    {
    iStoreConfiguration = CPbk2StoreConfiguration::NewL();
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::GetLocalVariationFlags
// --------------------------------------------------------------------------
//
void CPbk2ApplicationServices::GetLocalVariationFlags()
    {
    CRepository* key = NULL;

    // Read local variation flags
    TRAPD( err, key =
        CRepository::NewL( TUid::Uid( KCRUidPhonebook ) ) );

    // If NewL fails do not set iLocalVariationFlags, new query is made next time
    if ( err == KErrNone )
        {
        err = key->Get( KPhonebookLocalVariationFlags, iLocalVariationFlags );
        if ( err != KErrNone )
            {
            // If there were problems reading the flags,
            // assume everything is off
            iLocalVariationFlags = 0;
            }
        delete key;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::DoRelease
// --------------------------------------------------------------------------
//
void CPbk2ApplicationServices::DoRelease()
    {
    if (DecRef() == 0)
        {
        Dll::SetTls(NULL);
        delete this;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::IncRef
// --------------------------------------------------------------------------
//
void CPbk2ApplicationServices::IncRef()
    {
    ++iRefCount;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::DecRef
// --------------------------------------------------------------------------
//
TInt CPbk2ApplicationServices::DecRef()
    {
    return --iRefCount;
    }

// End of File
