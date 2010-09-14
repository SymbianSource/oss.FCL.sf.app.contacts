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
#include "CPbk2UIApplicationServices.h"
#include "tpbk2multidriveavailable.h"

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

// Debugging headers
#include <Pbk2Config.hrh>
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::CPbk2UIApplicationServices
// --------------------------------------------------------------------------
//
CPbk2UIApplicationServices::CPbk2UIApplicationServices() :
iFsSession( CCoeEnv::Static()->FsSession() )
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::~CPbk2UIApplicationServices
// --------------------------------------------------------------------------
//
CPbk2UIApplicationServices::~CPbk2UIApplicationServices()
    {
    // Command handler is calling iStoreConfiguration's RemoveObserver
    // in its destructor. So delete command handler before store
    // configuration.
    delete iCommandHandler;
    delete iSendUi;

    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2UIApplicationServices::ConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ( "CPbk2UIApplicationServices::ConstructL(0x%x)"), this );

    iAppServices = CPbk2ApplicationServices::InstanceL();

    // Set core Phonebook 2 stores
    //iAppServices->StoreManager().EnsureDefaultSavingStoreIncludedL();
    //iAppServices->StoreManager().EnsureCurrentConfigurationStoresIncludedL();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIApplicationServices::ConstructL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::NewL
// --------------------------------------------------------------------------
//
CPbk2UIApplicationServices* CPbk2UIApplicationServices::NewL()
    {
    CPbk2UIApplicationServices* self =
        new ( ELeave ) CPbk2UIApplicationServices();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::ContactManager
// --------------------------------------------------------------------------
//
CVPbkContactManager& CPbk2UIApplicationServices::ContactManager() const
    {
    return iAppServices->ContactManager();
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::CommandHandlerL
// --------------------------------------------------------------------------
//
MPbk2CommandHandler* CPbk2UIApplicationServices::CommandHandlerL()
    {
    if ( !iCommandHandler )
        {
        iCommandHandler = CPbk2CommandHandler::NewL();
        }

    return iCommandHandler;
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::NameFormatter
// --------------------------------------------------------------------------
//
MPbk2ContactNameFormatter& CPbk2UIApplicationServices::NameFormatter() const
    {
    return iAppServices->NameFormatter();
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::SortOrderManager
// --------------------------------------------------------------------------
//
CPbk2SortOrderManager& CPbk2UIApplicationServices::SortOrderManager() const
    {
    return iAppServices->SortOrderManager();
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::SendUiL
// --------------------------------------------------------------------------
//
CSendUi* CPbk2UIApplicationServices::SendUiL()
    {
    if ( !iSendUi )
        {
        iSendUi = CSendUi::NewL();
        PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
            ( "CPbk2UIApplicationServices::SendUiL(0x%x) SendUI created"),
              this );
        }
    return iSendUi;
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::StoreProperties
// --------------------------------------------------------------------------
//
CPbk2StorePropertyArray& CPbk2UIApplicationServices::StoreProperties() const
    {
    return iAppServices->StoreProperties();
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::FieldProperties
// --------------------------------------------------------------------------
//
CPbk2FieldPropertyArray& CPbk2UIApplicationServices::FieldProperties() const
    {
    return iAppServices->FieldProperties();
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::StoreConfiguration
// --------------------------------------------------------------------------
//
CPbk2StoreConfiguration& CPbk2UIApplicationServices::StoreConfiguration() const
    {
    return iAppServices->StoreConfiguration();
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::StoreObservationRegister
// --------------------------------------------------------------------------
//
MPbk2StoreObservationRegister&
        CPbk2UIApplicationServices::StoreObservationRegister() const
    {
    return iAppServices->StoreManager();
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::StoreValidityInformer
// --------------------------------------------------------------------------
//
MPbk2StoreValidityInformer&
        CPbk2UIApplicationServices::StoreValidityInformer() const
    {
    return iAppServices->StoreManager();
    }


// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::ViewSupplier
// --------------------------------------------------------------------------
//
MPbk2ContactViewSupplier& CPbk2UIApplicationServices::ViewSupplier() const
    {
    return iAppServices->ViewSupplier();
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::GlobalMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2UIApplicationServices::GlobalMenuFilteringFlagsL() const
    {
    TInt result = KPbk2MenuFilteringFlagsNone;

    if ( TPbk2MultiDriveAvailable::MultiDriveAvailable( iFsSession ) )
        {
        result |= KPbk2MultiDriveAvailable;
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::LocallyVariatedFeatureEnabled
// --------------------------------------------------------------------------
//
TBool CPbk2UIApplicationServices::LocallyVariatedFeatureEnabled(
    TVPbkLocalVariantFlags aFeatureFlag)
    {
    return iAppServices->LocallyVariatedFeatureEnabled(aFeatureFlag);
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::MPbk2ApplicationServicesExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2UIApplicationServices::MPbk2ApplicationServicesExtension(
    TUid aExtensionUid )
    {
    if ( aExtensionUid == KMPbk2ApplicationServicesExtension2Uid )
        {
        return static_cast<MPbk2ApplicationServices2*>( this );
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::StoreManager
// --------------------------------------------------------------------------
//
CPbk2StoreManager& CPbk2UIApplicationServices::StoreManager() const
    {
    return iAppServices->StoreManager();
    }

// --------------------------------------------------------------------------
// CPbk2UIApplicationServices::ServicesManager
// --------------------------------------------------------------------------
//
CPbk2ServiceManager& CPbk2UIApplicationServices::ServiceManager() const
    {
    return iAppServices->ServiceManager();
    }

// End of File
