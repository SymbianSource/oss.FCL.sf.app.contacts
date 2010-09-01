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
* Description:  Phonebook 2 UI extension manager.
*
*/


// INCLUDE FILES
#include "CPbk2UIExtensionManager.h"

// Phonebook2
#include "CPbk2UIExtensionMenuManager.h"
#include "CPbk2UIExtensionInformation.h"
#include "CPbk2UIExtensionLoader.h"
#include "CPbk2UIExtensionFactory.h"
#include "CPbk2UIExtensionIconSupport.h"
#include "CPbk2UIExtensionPlugin.h"
#include "CPbk2ThinUIExtensionLoader.h"
#include "CPbk2UIExtensionConfigurationObserver.h"
#include <CPbk2ViewGraph.h>
#include <CPbk2AppViewBase.h>
#include <CPbk2AppUiBase.h>
#include "CPbk2ApplicationServices.h"

// System includes
#include <ecom/ecom.h>
#include <barsread.h>

// Debugging headers
#include <Pbk2Config.hrh>
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * Returns the global instance of CPbk2UIExtensionManager.
 *
 * @return  Instance of CPbk2UIExtensionManager.
 */
inline CPbk2UIExtensionManager* Instance()
    {
    return static_cast<CPbk2UIExtensionManager*>( Dll::Tls() );
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::CPbk2UIExtensionManager
// --------------------------------------------------------------------------
//
CPbk2UIExtensionManager::CPbk2UIExtensionManager()
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::~CPbk2UIExtensionManager
// --------------------------------------------------------------------------
//
CPbk2UIExtensionManager::~CPbk2UIExtensionManager()
    {
    delete iExtensionFactory;
    delete iMenuManager;
    delete iIconSupport;
    delete iThinExtensionLoader;
    delete iExtensionLoader;
    delete iStoreConfigurationObserver;

    Release( iAppServices );

    // Cleanup ECom session
    REComSession::FinalClose();
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2UIExtensionManager::ConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionManager::ConstructL(0x%x)"), this);

    PBK2_PROFILE_START(Pbk2Profile::EUiExtensionManagerCreateExtensionLoader);
    iExtensionLoader = CPbk2UIExtensionLoader::NewL();
    PBK2_PROFILE_END(Pbk2Profile::EUiExtensionManagerCreateExtensionLoader);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionManager::ConstructL(0x%x) loader instantiated"),
        this);

    PBK2_PROFILE_START
        (Pbk2Profile::EUiExtensionManagerCreateConfigurationObserver);

    iAppServices = CPbk2ApplicationServices::InstanceL();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionManager::ConstructL(0x%x) application services fetched"),
        this);

    iStoreConfigurationObserver =
        CPbk2UIExtensionConfigurationObserver::NewL
            ( iAppServices->StoreProperties(),
              *iExtensionLoader, iAppServices->StoreConfiguration() );

    PBK2_PROFILE_END
        (Pbk2Profile::EUiExtensionManagerCreateConfigurationObserver);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionManager::ConstructL(0x%x) configuration observer instantiated"),
        this);

    PBK2_PROFILE_START
        (Pbk2Profile::EUiExtensionManagerCreateThinExtensionLoader);
    iThinExtensionLoader = CPbk2ThinUIExtensionLoader::NewL();
    PBK2_PROFILE_END
        (Pbk2Profile::EUiExtensionManagerCreateThinExtensionLoader);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionManager::ConstructL(0x%x) thin loader instantiated"),
        this);

    PBK2_PROFILE_START(Pbk2Profile::EUiExtensionManagerCreateMenuManager);
    iMenuManager = CPbk2UIExtensionMenuManager::NewL
        ( *iExtensionLoader, *iThinExtensionLoader );
    PBK2_PROFILE_END(Pbk2Profile::EUiExtensionManagerCreateMenuManager);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionManager::ConstructL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::InstanceL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2UIExtensionManager* CPbk2UIExtensionManager::InstanceL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionManager::InstanceL()"));

    CPbk2UIExtensionManager* instance = Instance();

    if ( !instance )
        {
        instance = new ( ELeave ) CPbk2UIExtensionManager;
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

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionManager::InstanceL() end"));

    return instance;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::FactoryL
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2UIExtensionFactory* CPbk2UIExtensionManager::FactoryL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionManager::FactoryL(0x%x)"), this);

    if (!iExtensionFactory)
        {
        iExtensionFactory = CPbk2UIExtensionFactory::NewL
            ( *iExtensionLoader, *iThinExtensionLoader, *this );
        }

    return iExtensionFactory;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::ApplyExtensionViewGraphChangesL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2UIExtensionManager::ApplyExtensionViewGraphChangesL(
        CPbk2ViewGraph& aViewGraph)
    {
    TArray<CPbk2UIExtensionInformation*> extInfoArray =
            iExtensionLoader->PluginInformation();
    const TInt count = extInfoArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        // loop through all the UI extensions and check if they have
        // modifications to the view graph
        CPbk2UIExtensionInformation* extInfo = extInfoArray[i];
        if (extInfo->HasViewGraphModifications())
            {
            TResourceReader reader;
            extInfo->CreateViewGraphModificationReaderLC(reader);
            aViewGraph.ModifyViewGraphL(reader);
            CleanupStack::PopAndDestroy(); // reader
            }
        if (extInfo->LoadingPolicy() & KPbk2LoadInStartup &&
            iExtensionLoader->IsLoaded(extInfo->ImplementationUid()))
            {
            // if extension is loaded during start up, ask it to
            // apply dynamic view graph changes
            CPbk2UIExtensionPlugin* plugin = iExtensionLoader->LoadedPlugin(
                    extInfo->ImplementationUid());
            if(plugin)   
                {        
                plugin->ApplyDynamicViewGraphChangesL(aViewGraph);                    
                }        
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::IconSupportL
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2UIExtensionIconSupport& CPbk2UIExtensionManager::IconSupportL()
    {
    if (!iIconSupport)
        {
        iIconSupport =
            new(ELeave) CPbk2UIExtensionIconSupport(*iExtensionLoader);
        }
    return *iIconSupport;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2UIExtensionManager::DynInitMenuPaneL
        ( TInt aResourceId, CEikMenuPane* aMenuPane,
          CPbk2AppViewBase& aViewBase,
          MPbk2ContactUiControl& aControl )
    {
    iMenuManager->DynInitMenuPaneL( aViewBase.Id(),
        aViewBase.GetMenuFilteringFlagsL(), aResourceId,
        aMenuPane, aControl);
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::GetHelpContextL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2UIExtensionManager::GetHelpContextL
        ( TCoeHelpContext& aContext, const CPbk2AppViewBase& aView,
          MPbk2ContactUiControl& aUiControl )
    {
    TArray<CPbk2UIExtensionPlugin*> pluginArray =
        iExtensionLoader->Plugins();
    TInt count = pluginArray.Count();
    TBool found = EFalse;
    for (TInt i = 0; i < count && !found; ++i )
        {
        found = ( pluginArray[i] )->GetHelpContextL
            ( aContext, aView, aUiControl );
        }
    return found;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::MenuManager
// --------------------------------------------------------------------------
//
CPbk2UIExtensionMenuManager& CPbk2UIExtensionManager::MenuManager() const
    {
    return *iMenuManager;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::DoRelease
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionManager::DoRelease()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionManager::DoRelease(0x%x)"), this);

    if (DecRef() == 0)
        {
        Dll::SetTls(NULL);
        delete this;
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::IncRef
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionManager::IncRef()
    {
    ++iRefCount;

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionManager::IncRef(0x%x) %d"), this, iRefCount);
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionManager::DecRef
// --------------------------------------------------------------------------
//
TInt CPbk2UIExtensionManager::DecRef()
    {
    return --iRefCount;
    }

// End of File
