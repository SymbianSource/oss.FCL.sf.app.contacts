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
#include "cpbk2serverapplicationservices.h"

// Phonebook 2
#include <cpbk2storemanager.h>
#include <cpbk2sortordermanager.h>
#include <mpbk2contactnameformatter.h>
#include <mpbk2viewexplorer.h>
#include <pbk2contactnameformatterfactory.h>
#include <pbk2commonui.rsg>
#include <cpbk2storepropertyarray.h>
#include <cpbk2fieldpropertyarray.h>
#include <pbk2menufilteringflags.hrh>
#include <phonebook2privatecrkeys.h>
#include <cpbk2contactviewsupplier.h>
#include <cpbk2storeconfiguration.h>
#include <cpbk2servicemanager.h>

// Virtual Phonebook
#include <cvpbkcontactmanager.h>
#include <cvpbkcontactstoreuriarray.h>

// System includes
#include <stringloader.h>
#include <centralrepository.h>

// Debugging headers
#include <pbk2debug.h>
#include <pbk2profile.h>

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::CPbk2ServerApplicationServices
// --------------------------------------------------------------------------
//
CPbk2ServerApplicationServices::CPbk2ServerApplicationServices()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::~CPbk2ServerApplicationServices
// --------------------------------------------------------------------------
//
CPbk2ServerApplicationServices::~CPbk2ServerApplicationServices()
    {
    delete iServiceManager;
    // sort order manager can use the views from iViewSupplier,
    // so delete iSortOrderManager before iViewSupplier 
    delete iSortOrderManager;
    delete iViewSupplier;
    delete iNameFormatter;
    delete iFieldProperties;
    delete iStoreManager;
    delete iStoreProperties;
    delete iContactManager;
    delete iStoreConfiguration;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ServerApplicationServices::ConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ( "CPbk2ServerApplicationServices::ConstructL(0x%x)"), this );

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

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ( "CPbk2ServerApplicationServices::ConstructL(0x%x) Store manager created"),
          this );

 
    iServiceManager = CPbk2ServiceManager::NewL(iContactManager->FsSession());
    CreateFieldPropertyArrayL();
    CreateNameFormatterL();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ServerApplicationServices::ConstructL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::NewL
// --------------------------------------------------------------------------
//
CPbk2ServerApplicationServices* CPbk2ServerApplicationServices::NewL()
    {
    CPbk2ServerApplicationServices* self =
        new ( ELeave ) CPbk2ServerApplicationServices();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::ContactManager
// --------------------------------------------------------------------------
//
CVPbkContactManager& CPbk2ServerApplicationServices::ContactManager() const
    {
    return *iContactManager;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::CommandHandlerL
// --------------------------------------------------------------------------
//
MPbk2CommandHandler* CPbk2ServerApplicationServices::CommandHandlerL()
    {
    // Not supported in Server App
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::NameFormatter
// --------------------------------------------------------------------------
//
MPbk2ContactNameFormatter&
        CPbk2ServerApplicationServices::NameFormatter() const
    {
    return *iNameFormatter;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::SortOrderManager
// --------------------------------------------------------------------------
//
CPbk2SortOrderManager&
        CPbk2ServerApplicationServices::SortOrderManager() const
    {
    return *iSortOrderManager;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::SendUiL
// --------------------------------------------------------------------------
//
CSendUi* CPbk2ServerApplicationServices::SendUiL()
    {
    // Not supported in Server App
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::StoreProperties
// --------------------------------------------------------------------------
//
CPbk2StorePropertyArray&
        CPbk2ServerApplicationServices::StoreProperties() const
    {
    return *iStoreProperties;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::FieldProperties
// --------------------------------------------------------------------------
//
CPbk2FieldPropertyArray&
        CPbk2ServerApplicationServices::FieldProperties() const
    {
    return *iFieldProperties;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::StoreConfiguration
// --------------------------------------------------------------------------
//
CPbk2StoreConfiguration&
        CPbk2ServerApplicationServices::StoreConfiguration() const
    {
    return *iStoreConfiguration;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::StoreObservationRegister
// --------------------------------------------------------------------------
//
MPbk2StoreObservationRegister&
        CPbk2ServerApplicationServices::StoreObservationRegister() const
    {
    return *iStoreManager;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::StoreValidityInformer
// --------------------------------------------------------------------------
//
MPbk2StoreValidityInformer&
        CPbk2ServerApplicationServices::StoreValidityInformer() const
    {
    return *iStoreManager;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::ViewSupplier
// --------------------------------------------------------------------------
//
MPbk2ContactViewSupplier&
        CPbk2ServerApplicationServices::ViewSupplier() const
    {
    return *iViewSupplier;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::GlobalMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2ServerApplicationServices::GlobalMenuFilteringFlagsL() const
    {
    return KPbk2MenuFilteringFlagsNone;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::MPbk2ApplicationServicesExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2ServerApplicationServices::MPbk2ApplicationServicesExtension(
    TUid aExtensionUid )
    {
    if ( aExtensionUid == KMPbk2ApplicationServicesExtension2Uid )
        {
        return static_cast<MPbk2ApplicationServices2*>( this );
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::StoreManager
// --------------------------------------------------------------------------
//
CPbk2StoreManager& CPbk2ServerApplicationServices::StoreManager() const
    {
    return *iStoreManager;
    }

// --------------------------------------------------------------------------
// CPbk2ApplicationServices::ServicesManager
// --------------------------------------------------------------------------
//
CPbk2ServiceManager& CPbk2ServerApplicationServices::ServiceManager() const
    {
    return *iServiceManager;
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::CreateManagerL
// --------------------------------------------------------------------------
//
inline void CPbk2ServerApplicationServices::CreateManagerL()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2ServerApplicationServices:CreateManagerL(0x%x)" ), this );

    // Initialize contact manager with empty URI array
    CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewL();
    CleanupStack::PushL( uriArray );

    TSecurityInfo secInfo;
    secInfo.SetToCreatorInfo();
    iContactManager = CVPbkContactManager::NewL( secInfo, *uriArray );

    CleanupStack::PopAndDestroy(); // uriArray

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ServerApplicationServices::CreateManagerL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::CreateSortOrderManagerL
// --------------------------------------------------------------------------
//
inline void CPbk2ServerApplicationServices::CreateSortOrderManagerL()
    {
    PBK2_PROFILE_START( Pbk2Profile::EAppServicesCreateSortOrderManager );

    iSortOrderManager = CPbk2SortOrderManager::NewL
        ( iContactManager->FieldTypes() );

    PBK2_PROFILE_END( Pbk2Profile::EAppServicesCreateSortOrderManager );
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::CreateStorePropertyArrayL
// --------------------------------------------------------------------------
//
inline void CPbk2ServerApplicationServices::CreateStorePropertyArrayL()
    {
    PBK2_PROFILE_START( Pbk2Profile::EAppServicesCreateStorePropertyArray );
    iStoreProperties = CPbk2StorePropertyArray::NewL();
    PBK2_PROFILE_END( Pbk2Profile::EAppServicesCreateStorePropertyArray );

    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2ServerApplicationServices::CreateStorePropertyArrayL() done ") );
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::CreateFieldPropertyArrayL
// --------------------------------------------------------------------------
//
inline void CPbk2ServerApplicationServices::CreateFieldPropertyArrayL()
    {
    PBK2_PROFILE_START( Pbk2Profile::EAppServicesCreateFieldPropertyArray );
    iFieldProperties = CPbk2FieldPropertyArray::NewL
        ( iContactManager->FieldTypes(), iContactManager->FsSession(),
          iServiceManager );
    PBK2_PROFILE_END( Pbk2Profile::EAppServicesCreateFieldPropertyArray );
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::CreateNameFormatterL
// --------------------------------------------------------------------------
//
inline void CPbk2ServerApplicationServices::CreateNameFormatterL()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2ServerApplicationServices::CreateNameFormatterL(0x%x)" ),
          this );

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
        ( "CPbk2ServerApplicationServices::CreateNameFormatterL(0x%x) end" ),
          this );
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::CreateViewSupplierL
// --------------------------------------------------------------------------
//
inline void CPbk2ServerApplicationServices::CreateViewSupplierL()
    {
    iViewSupplier = CPbk2ContactViewSupplier::NewL
        ( *iContactManager, *iSortOrderManager, *iStoreConfiguration,
          *iStoreProperties, EFalse );
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplicationServices::CreateStoreConfigurationL
// --------------------------------------------------------------------------
//
inline void CPbk2ServerApplicationServices::CreateStoreConfigurationL()
    {
    iStoreConfiguration = CPbk2StoreConfiguration::NewL();
    }

// End of File
