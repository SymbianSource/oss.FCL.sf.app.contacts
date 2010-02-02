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
* Description:  Load UI extensions
*
*/


#include "CPbk2UIExtensionLoader.h"
#include "Pbk2InternalUID.h"

// Phonebook 2
#include "CPbk2UIExtensionPlugin.h"
#include "CPbk2UIExtensionInformation.h"
#include <Pbk2ExtensionUID.h>
#include <Pbk2DataCaging.hrh>

// System includes
#include <coemain.h>
#include <bautils.h>    // BaflUtils::NearestLanguageFile
#include <featmgr.h>            

// Debugging headers

#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

/// Unnamed namespace for local definitions
namespace {

/// Resource file containing shared extension resources
_LIT( KPbk2UIExtCommonResourceFile, "Pbk2UIExtCommon.rsc" );

/**
 * Custom cleanup function.
 *
 * @param aObj  Object to clean.
 */
void CleanupResetAndDestroy( TAny* aObj )
    {
    if ( aObj )
        {
        static_cast<RImplInfoPtrArray*>( aObj )->ResetAndDestroy();
        }
    }
} /// namespace

// --------------------------------------------------------------------------
// CPbk2UIExtensionLoader::CPbk2UIExtensionLoader
// --------------------------------------------------------------------------
//
CPbk2UIExtensionLoader::CPbk2UIExtensionLoader() :
            iCommonResourceFile( *CCoeEnv::Static() )
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionLoader::~CPbk2UIExtensionLoader
// --------------------------------------------------------------------------
//
CPbk2UIExtensionLoader::~CPbk2UIExtensionLoader()
    {
    iPluginInformation.ResetAndDestroy();
    iPluginArray.ResetAndDestroy();
    iCommonResourceFile.Close();
    FeatureManager::UnInitializeLib(); 
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionLoader::NewL
// --------------------------------------------------------------------------
//
CPbk2UIExtensionLoader* CPbk2UIExtensionLoader::NewL()
    {
    CPbk2UIExtensionLoader* self = new( ELeave ) CPbk2UIExtensionLoader();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// --------------------------------------------------------------------------
// CPbk2UIExtensionLoader::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2UIExtensionLoader::ConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionLoader::ConstructL(0x%x)"), this);

    FeatureManager::InitializeLibL(); 
    
    PBK2_PROFILE_START(Pbk2Profile::EUiExtensionloaderLoadResourceFile);
    iCommonResourceFile.OpenL(KPbk2RomFileDrive,
        KDC_RESOURCE_FILES_DIR, KPbk2UIExtCommonResourceFile);
    PBK2_PROFILE_END(Pbk2Profile::EUiExtensionloaderLoadResourceFile);

    PBK2_PROFILE_START(Pbk2Profile::EUiExtensionloaderListImplementations);
    RImplInfoPtrArray implementations;
    REComSession::ListImplementationsL(TUid::Uid(
            KPbk2UiExtensionInterfaceUID), implementations);
    CleanupStack::PushL(TCleanupItem(
            CleanupResetAndDestroy, &implementations));
    PBK2_PROFILE_END(Pbk2Profile::EUiExtensionloaderListImplementations);

    // loop through UI extensions in reverse order
    PBK2_PROFILE_START(Pbk2Profile::EUiExtensionloaderLoopExtensionPlugins);
    const TInt count = implementations.Count();
    for (TInt i = count - 1; i >= 0; --i)
        {
        CImplementationInformation* implInfo = implementations[i];
        CleanupStack::PushL(implInfo);
        implementations.Remove(i);

        TFileName resourceFileName;
        resourceFileName.Copy(implInfo->OpaqueData());
        BaflUtils::NearestLanguageFile( 
                CCoeEnv::Static()->FsSession(), resourceFileName );

        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2UIExtensionLoader::ConstructL(0x%x), implUID %x"), 
                    this, implInfo->ImplementationUid().iUid );

        // add the UI Extension plugin information to the plugin info array
        TEntry unusedEntry;
        RFs& fs = CCoeEnv::Static()->FsSession();
        if (fs.IsValidName(resourceFileName) &&
            fs.Entry(resourceFileName, unusedEntry) == KErrNone)
            {
            // add the plugin information from the plugin resource structure
            iPluginInformation.AppendL(
                    CPbk2UIExtensionInformation::NewLC(resourceFileName));
            CleanupStack::Pop(); // menu resource
            }
        CleanupStack::PopAndDestroy(implInfo);
        }
    CleanupStack::PopAndDestroy(); // implementations
    PBK2_PROFILE_END(Pbk2Profile::EUiExtensionloaderLoopExtensionPlugins);

    /// Load extension that have the loading policy KPbk2LoadInStartup
    PBK2_PROFILE_START(Pbk2Profile::EUiExtensionloaderLoadStartupExtensions);
    LoadStartUpExtensionsL();
    PBK2_PROFILE_END(Pbk2Profile::EUiExtensionloaderLoadStartupExtensions);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2UIExtensionLoader::ConstructL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionLoader::IsLoaded
// --------------------------------------------------------------------------
//
TBool CPbk2UIExtensionLoader::IsLoaded(TUid aImplementationUid)
    {
    if ( LoadedPlugin(aImplementationUid) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }    
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionLoader::EnsureLoadedL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionLoader::EnsureLoadedL(
        TUid aImplementationUid)
    {
    if (!IsLoaded(aImplementationUid))
        {
        LoadExtensionL(aImplementationUid);
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionLoader::LoadExtensionL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionLoader::LoadExtensionL(
        TUid aImplementationUid )
    {
    // instantiate the plugin object
    
    if ( IsOkToLoadL( aImplementationUid ))  
        {                                       
        CPbk2UIExtensionPlugin* plugin =
                CPbk2UIExtensionPlugin::NewL( aImplementationUid );
        if ( plugin )
            {
            CleanupStack::PushL(plugin);
            iPluginArray.AppendL(plugin);
            CleanupStack::Pop();
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2UIExtensionLoader::LoadExtensionL(0x%x) %x loaded"), 
                this, aImplementationUid.iUid );
            }
        }  
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionLoader::LoadStartUpExtensionsL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionLoader::LoadStartUpExtensionsL()
    {
    // loop through the plugin information array and check
    // if any item has the loading policy set so that they should be
    // loaded startup.
    const TInt count = iPluginInformation.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iPluginInformation[i]->LoadingPolicy() & KPbk2LoadInStartup)
            {
            // load plugin that have the start-up policy set as load
            // in start-up
            TUid implUid = iPluginInformation[i]->ImplementationUid();
            LoadExtensionL(implUid);
            // apply dynamic plugin information changes from extension
            CPbk2UIExtensionPlugin* plugin = LoadedPlugin(implUid);
            if(plugin)                       
                {                            
                plugin->ApplyDynamicPluginInformationDataL(*iPluginInformation[i]);
                }                            
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionLoader::LoadedPlugin
// --------------------------------------------------------------------------
//    
CPbk2UIExtensionPlugin* CPbk2UIExtensionLoader::LoadedPlugin
        (TUid aImplementationUid) const
    {
    const TInt count = iPluginArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iPluginArray[i]->ImplementationUid() == aImplementationUid)
            {
            return iPluginArray[i];
            }
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionLoader::PluginInformation
// --------------------------------------------------------------------------
//
TArray<CPbk2UIExtensionInformation*>
        CPbk2UIExtensionLoader::PluginInformation() const
    {
    return iPluginInformation.Array();
    }
    
// --------------------------------------------------------------------------
// CPbk2UIExtensionLoader::Plugins
// --------------------------------------------------------------------------
//    
TArray<CPbk2UIExtensionPlugin*> CPbk2UIExtensionLoader::Plugins() const
    {
    return iPluginArray.Array();
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionLoader::IsOkToLoadL
// --------------------------------------------------------------------------
//
TBool CPbk2UIExtensionLoader::IsOkToLoadL(TUid aImplementationUid )
    {
    TBool ret(ETrue);
    
    //Don't unnecessarily load plugins that are not needed in current build
    switch(aImplementationUid.iUid)
        {
        case KFscRclSettingExtensionImplementationUID:
            if( !FeatureManager::FeatureSupported( KFeatureIdFfContactsRemoteLookup ) )  
                {
                ret = EFalse;
                }
            break;
            
        default:
            ret = ETrue;
            break;
        }
    return ret;
    }

// End of File
