
/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for loading and handling the plugins
*
*/

#include <e32def.h>
#include <centralrepository.h>

#include <mccaparentcleaner.h>
#include <mccapluginfactory.h>

#include <cpbk2applicationservices.h>
#include <CPbk2StoreConfiguration.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStore.h>
#include "Phonebook2PrivateCRKeys.h"
#include "ccappheaders.h"
#include "tccapluginsorderinfo.h"
#include "ccapluginfactoryowner.h"
#include "../../ccadetailsviewplugin/inc/ccappdetailsviewpluginuids.hrh"
#include "../../ccacommlauncherplugin/inc/ccappcommlauncherpluginuids.hrh"
#include "../inc/ccappmycardpluginuids.hrh"

#include <Pbk2DataCaging.hrh>


// ======== CONSTANTS ==============
const TInt KMaxPlugins = 255;
//TODO: put these to common header
_LIT8( KCcaOpaqueNameDelimiter,     "\t" ); //Name=value pairs separated by tabs
_LIT8( KCcaOpaqueValueDelimiter,    "=" );  //Names and values separated by =
_LIT8( KCcaOpaqueTABP,              "TABP" );  //Tab position

_LIT(KPbk2CommandsDllResFileName,   "Pbk2Commands.rsc");
_LIT(KPbk2UiControlsDllResFileName, "Pbk2UiControls.rsc");
_LIT(KPbk2CommonUiDllResFileName,   "Pbk2CommonUi.rsc"  );

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// OrderOfPlugins (local function)
//
// ---------------------------------------------------------------------------
//
TInt OrderOfPlugins( const CImplementationInformation& aFirst,
            const CImplementationInformation& aSecond )
    {
    TInt firstVal = KMaxPlugins;
    TInt secondVal = KMaxPlugins;

    TLex8 lex( aFirst.OpaqueData() );
    lex.Val( firstVal );

    lex = aSecond.OpaqueData();
    lex.Val( secondVal );

    if( firstVal < secondVal )
        {
        return -1;
        }
    else if( firstVal == secondVal )
        {
        return 0;
        }
    else
        {
        return 1;
        }
    }

// ---------------------------------------------------------------------------
// CleanupResetAndDestroy (local function)
// ---------------------------------------------------------------------------
//
void CleanupResetAndDestroy(TAny* aObj)
    {
    if (aObj)
        {
        static_cast<RImplInfoPtrArray*>(aObj)->ResetAndDestroy();
        }
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::CCCAppPluginLoader
// ---------------------------------------------------------------------------
//
CCCAppPluginLoader::CCCAppPluginLoader(MCCAppEngine* aAppEngine) : 
	iAppEngine(aAppEngine),
    iCommandsResourceFile( *CCoeEnv::Static() ),
    iUiControlsResourceFile( *CCoeEnv::Static() ),
    iCommonUiResourceFile( *CCoeEnv::Static() )
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppPluginData::CCCAppPluginLoader"));
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::~CCCAppPluginLoader
// ---------------------------------------------------------------------------
//
CCCAppPluginLoader::~CCCAppPluginLoader()
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::~CCCAppPluginLoader"));

    //Nullify pointer to AppEngine in plugins as otherwise it will point to object
    //that doesn't exist anymore at this point of execution (because views are
    //deleted after AppEngine).
    for(TInt i = 0; i < PluginsCount(); i++)
        {
        CCCAppViewPluginBase& plugin(iPluginDataArray[i]->Plugin());
        plugin.SetAppEngine(NULL);
        }

    iPluginDataArray.ResetAndDestroy();
    delete iFactoryTempPtr;
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::~CCCAppPluginLoader"));
    
    if(iAppServices)
        {
        iAppServices->StoreConfiguration().RemoveObserver(*this);
        }
    Release( iAppServices );

    iCommandsResourceFile.Close();
    iUiControlsResourceFile.Close();
    iCommonUiResourceFile.Close();
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::NewL
// ---------------------------------------------------------------------------
//
CCCAppPluginLoader* CCCAppPluginLoader::NewL(
    MCCAppEngine* aAppEngine,
    const TDesC8& aPluginProperties,
    const TDesC8& aTypeFilter )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::NewL"));

    CCCAppPluginLoader* self = new( ELeave ) CCCAppPluginLoader(aAppEngine);
    CleanupStack::PushL( self );
    self->ConstructL(aPluginProperties, aTypeFilter );
    CleanupStack::Pop( self );

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::NewL"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::ConstructL(
    const TDesC8& aPluginProperties,
    const TDesC8& aTypeFilter )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::ConstructL"));

    //PERFORMANCE LOGGING: 3. Loading plugins
    WriteToPerfLog();

    PreparePbk2ApplicationServicesL();
    
    RPointerArray<CImplementationInformation> oldImplInfoArray;
    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy, &oldImplInfoArray));
    RPointerArray<CImplementationInformation> newImplInfoArray;
    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy, &newImplInfoArray));

    FeatureManager::InitializeLibL();
    const TBool myCardSupported( 
            FeatureManager::FeatureSupported( KFeatureIdffContactsMycard ) );    
    FeatureManager::UnInitializeLib();
    
    // A. List legacy and new implementations
    if( myCardSupported && 
            aTypeFilter.Length() > 0 )
        {
        TEComResolverParams params;
        params.SetDataType( aTypeFilter );

        REComSession::ListImplementationsL( 
                TUid::Uid(KCCAppViewPluginBaseInterfaceUID),
            params, oldImplInfoArray );
        REComSession::ListImplementationsL( KCCAppViewFactoryInterfaceUID,
            params, newImplInfoArray );
        }
    else
        {
        REComSession::ListImplementationsL( 
                TUid::Uid(KCCAppViewPluginBaseInterfaceUID),
            oldImplInfoArray );
        REComSession::ListImplementationsL( KCCAppViewFactoryInterfaceUID,
            newImplInfoArray );
        }

    if ( !myCardSupported && 
            !oldImplInfoArray.Count() )  //At least one legacy plugin needed
        {// no plugins, nothing can be done
        CCA_DP( KCCAppLogFile, CCA_L( "::ConstructL - no plugins found!"));
        User::Leave( KErrNotFound );
        }

    // B. Sort implementations into same order as are in Phonebook
    // each stores pluginInfo (from oldImplInfoArray) address and order
    RArray<TCCAPluginsOrderInfo> pluginOrderArray;
    CleanupClosePushL( pluginOrderArray );
    RArray<TPtrC> nameArray; // stores phonebooksXPExtension plugins name
    CleanupClosePushL(nameArray);

    // store phonebook mainview sXPExtension's name from CenRep to nameArray
    CRepository* repository = CRepository::NewLC( TUid::Uid(KCRUidPhonebook));
    HBufC* names = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr namesPtr = names->Des();

    // get key value, which stores phonebook sXPExtension Plugins name
    TInt error = repository->Get(KPhonebooksXPExtensionPluginsName, namesPtr);

    // e.g if the bey value is "MSN|Ovi|Sonera|....",then MSN, Ovi and Sonera
    // will be appended to aPbksXPExtesionNamesArray
    GetPbksXPExtesionNamesL(nameArray, *names); //parses names to namearray


    // copy CImplementationInformation's address from oldImplInfoArray to pluginOrderArray
    GetPluginsInfoL(pluginOrderArray, nameArray, oldImplInfoArray, newImplInfoArray);

    SortPluginsOrderInfoL( pluginOrderArray, nameArray );

    // C. Load implementations in defined order
    LoadAllPlugins(pluginOrderArray, aPluginProperties);

    CleanupStack::PopAndDestroy(names);
    CleanupStack::PopAndDestroy(repository);

    CleanupStack::PopAndDestroy(); //nameArray
    CleanupStack::PopAndDestroy(); //pluginOrderArray
    CleanupStack::PopAndDestroy(&newImplInfoArray);
    CleanupStack::PopAndDestroy(&oldImplInfoArray);
    //PERFORMANCE LOGGING: 4. Plugins loaded
    WriteToPerfLog();

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::ConstructL"));
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::LoadOneMultiViewPluginL
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::LoadOneMultiViewPluginL(
    CImplementationInformation* aImplementationInformation)
    {
    __ASSERT_DEBUG(!iFactoryTempPtr, User::Panic(_L("CCCAppPluginLoad"), KErrAlreadyExists));
    iFactoryTempPtr = CcaPluginFactoryOwner::NewL(aImplementationInformation);
    CcaPluginFactoryOwner* factory = iFactoryTempPtr;

    for(TInt j=0; j < factory->Factory().TabViewCount(); j++)
        {
        CCCAppViewPluginBase* plugin = factory->Factory().CreateTabViewL(j);
        CleanupStack::PushL(plugin);
        CCCAppPluginData* data = CCCAppPluginData::NewLC(*plugin); //Handles plugin ownership
        CleanupStack::Pop(2, plugin);
        CleanupStack::PushL(data);
        iPluginDataArray.AppendL( data );
        CleanupStack::Pop(data);
        data->SetPluginVisibility( CCCAppPluginData::EPluginHidden );

        //Hand over ownership of factoryOwner (shared ownership among tabs created by factory)
        factory->AddedChild();
        plugin->TakeSharedOwnerShip( factory );
        iFactoryTempPtr = NULL; //ownership given
        }

    //If ownership was not given
    delete iFactoryTempPtr;
    iFactoryTempPtr = NULL;
   }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::IsSimStoreUri
// Determine whether a Contact Store URI is SIM Card storage.
// ---------------------------------------------------------------------------
//
TBool CCCAppPluginLoader::IsSimStoreUri( const TVPbkContactStoreUriPtr& aUri ) const
    {
    TBool isSimUri(EFalse);

    isSimUri = !aUri.Compare(VPbkContactStoreUris::SimGlobalAdnUri(),
                    TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) ||
               !aUri.Compare(VPbkContactStoreUris::SimGlobalFdnUri(),
                    TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) ||
               !aUri.Compare(VPbkContactStoreUris::SimGlobalSdnUri(),
                    TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) ||
               !aUri.Compare(VPbkContactStoreUris::SimGlobalOwnNumberUri(),
                    TVPbkContactStoreUriPtr::EContactStoreUriAllComponents);
    return isSimUri;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::IsCurrentContactStoreSupportL
// Determine whether an implementation plug-in is supported by Current Contact Store. 
// ---------------------------------------------------------------------------
//
TBool CCCAppPluginLoader::IsCurrentContactStoreSupportL( const TInt& aImplmentationUid ) const
    {
    TBool isCurrentContactStoreSupport(ETrue);

    if( ( KCCADetailsViewPluginImplmentationUid != aImplmentationUid )
            && ( KCCACommLauncherPluginImplmentationUid != aImplmentationUid )
            && ( KCCAMyCardPluginImplmentationUid != aImplmentationUid ) )
        {
        MVPbkContactLink* link = NULL;
        MVPbkContactLinkArray* contactArray = NULL;
        HBufC& contactData = iAppEngine->Parameter().ContactDataL();
        HBufC8* contactData8 = HBufC8::NewLC(contactData.Size());
        TPtr8 contactData8Ptr(contactData8->Des());
        contactData8Ptr.Copy(contactData.Des());
          
        contactArray = iAppServices->ContactManager().CreateLinksLC(contactData8Ptr);

        if (contactArray->Count() > 0)
            {
            link = contactArray->At(0).CloneLC();
            }

        if (link)
            {
            const MVPbkContactStoreProperties& storeProperties =
                    link->ContactStore().StoreProperties();
            TVPbkContactStoreUriPtr uri = storeProperties.Uri();
            isCurrentContactStoreSupport = !IsSimStoreUri( uri );
            }
            

        if( link )
            {
            CleanupStack::PopAndDestroy(); //link
            }
        if( contactArray )
            {
            CleanupStack::PopAndDestroy(); //contactArray
            }

        CleanupStack::PopAndDestroy(); //contactData8
        }

    return isCurrentContactStoreSupport;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::PreparePbk2ApplicationServicesL
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::PreparePbk2ApplicationServicesL()
    {   
    iCommandsResourceFile.OpenL(
        KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, KPbk2CommandsDllResFileName );
    iUiControlsResourceFile.OpenL(
        KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, KPbk2UiControlsDllResFileName );
    iCommonUiResourceFile.OpenL(
        KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, KPbk2CommonUiDllResFileName );

    iAppServices = CPbk2ApplicationServices::InstanceL();
    iAppServices->StoreConfiguration().AddObserverL(*this);    
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::LoadAllPlugins
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::LoadAllPlugins(
        const RArray<TCCAPluginsOrderInfo>& aPluginOrderInfoArray, const TDesC8& aPluginProperties )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::LoadAllPlugins"));

    const TInt count = aPluginOrderInfoArray.Count();
    CCA_DP( KCCAppLogFile, CCA_L("::LoadAllPlugins - plugin count: %d"), count );
    for ( TInt i = 0; i < count; ++i )
        {
        TInt tempImplmentationUid = aPluginOrderInfoArray[i].iPluginInfor->ImplementationUid().iUid;
        TBool isCurrentContactStoreSupport = EFalse;
        TRAPD( err, isCurrentContactStoreSupport = IsCurrentContactStoreSupportL( tempImplmentationUid ) );
        if( err != KErrNone || !isCurrentContactStoreSupport )
            {
            /* If problems with Contact Link related operations or the Contact Link is in SIM Store,
             * don't load current plugin. Just continuing
             * to load the next one. */
            continue;
            }
        /* If problems with plugin loading, that plugin is
         * not included to the plugin array. Just continuing
         * and trying to load the next one. */
        TRAP_IGNORE( CheckAndLoadPluginL(
                aPluginOrderInfoArray[i], aPluginProperties ));
        }

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::LoadAllPlugins"));
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::CheckAndLoadPluginL
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::CheckAndLoadPluginL(
    const TCCAPluginsOrderInfo& aPluginsOrderInfo,
    const TDesC8& aPluginProperties )
    {
    HBufC8* match = GetmachingPropertiesLC(
            aPluginsOrderInfo.iPluginInfor->OpaqueData(),
            aPluginProperties,
            EFalse );
    TBool loadThis = match->Length() > 0;
    CleanupStack::PopAndDestroy(match);

    //Fallback1 for old plugins that provide only number in opaque data: load them always
    //The below if{} can be removed when legacy plugins support new format of OpaqueData
    if(!loadThis)
        {
        loadThis = OpaqueValueFrom( *aPluginsOrderInfo.iPluginInfor ) > 0;
        }

    if( loadThis )
        {
        TUid uid = aPluginsOrderInfo.iPluginInfor->ImplementationUid();

        if( aPluginsOrderInfo.iOldInterFaceType )
            {
            LoadPluginL(uid);   //lecacy implementations
            }
        else
            {
            LoadOneMultiViewPluginL( aPluginsOrderInfo.iPluginInfor ); //New implementations
            }
        CCA_DP( KCCAppLogFile, CCA_L("::LoadAllPlugins - plugin loaded uid: %d"), uid.iUid );
        }
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::LoadPluginL
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::LoadPluginL(
        TUid aImplementationUid )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::LoadPluginL"));

    CCCAppViewPluginBase* plugin =
            CCCAppViewPluginBase::NewL( aImplementationUid );
    plugin->SetAppEngine(iAppEngine);
    CCA_DP( KCCAppLogFile, CCA_L("::LoadPluginL - plugin created"));
    CleanupStack::PushL( plugin );
    User::LeaveIfError( aImplementationUid == plugin->Id() ? KErrNone : KErrArgument );
    CCCAppPluginData* data = CCCAppPluginData::NewLC( *plugin );
    data->SetPluginVisibility( CCCAppPluginData::EPluginHidden );
    iPluginDataArray.AppendL( data );
    CCA_DP( KCCAppLogFile, CCA_L("::LoadPluginL - plugin added to iPluginDataArray"));
    CleanupStack::Pop( 2, plugin );// data, plugin
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::LoadPluginL"));
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::NextPlugin
// ---------------------------------------------------------------------------
//
CCCAppPluginData* CCCAppPluginLoader::NextPlugin( TBool aOnlyVisiblePlugins )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::NextPlugin"));
    CCA_DP( KCCAppLogFile, CCA_L("::NextPlugin - iCurrentPlugin: %d"), iCurrentPlugin );

    for(TInt i = iCurrentPlugin+1; i < PluginsCount(); i++)
        {
        if( !aOnlyVisiblePlugins )
            {
            return iPluginDataArray[iCurrentPlugin = i];
            }
        else if( iPluginDataArray[i]->PluginVisibility() == CCCAppPluginData::EPluginVisible )
            {
            return iPluginDataArray[iCurrentPlugin = i];
            }
        }
        CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::NextPlugin - no next"));
        return NULL;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::PreviousPlugin
// ---------------------------------------------------------------------------
//
CCCAppPluginData* CCCAppPluginLoader::PreviousPlugin( TBool aOnlyVisiblePlugins )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::PreviousPlugin"));
    CCA_DP( KCCAppLogFile, CCA_L("::PreviousPlugin - iCurrentPlugin: %d"), iCurrentPlugin );

    for(TInt i = iCurrentPlugin-1; i >= 0; i--)
        {
        if( !aOnlyVisiblePlugins )
            {
            return iPluginDataArray[iCurrentPlugin = i];
            }
        else if( iPluginDataArray[i]->PluginVisibility() == CCCAppPluginData::EPluginVisible )
            {
            return iPluginDataArray[iCurrentPlugin = i];
            }
        }
        CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::PreviousPlugin - no previous"));
        return NULL;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::SetPluginInFocus
// ---------------------------------------------------------------------------
//
TInt CCCAppPluginLoader::SetPluginInFocus( const TUid aUid )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::SetPluginInFocus"));
    CCA_DP( KCCAppLogFile, CCA_L("::SetPluginInFocus - searching aUid: %d"), aUid );

    TInt index = KErrNotFound;
    // no need to search if no uid provided
    if ( TUid::Null() == aUid )
        return index;

    const TInt count = iPluginDataArray.Count();
    CCA_DP( KCCAppLogFile, CCA_L("::SetPluginInFocus - plugin count: %d"), count );
    for ( TInt i = 0; i < count; ++i )
        {
        TUid uid( iPluginDataArray[i]->Plugin().Id() );
        CCA_DP( KCCAppLogFile, CCA_L("::SetPluginInFocus - uid: %d found"), uid );
        if( aUid == uid )
            {
            index = i;
            iCurrentPlugin = i;
            break;// found, no need to loop all through
            }
        }
    CCA_DP( KCCAppLogFile, CCA_L("::SetPluginInFocus - index after searching the array %d"), index );
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::SetPluginInFocus"));
    return index;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::PluginInFocus
// ---------------------------------------------------------------------------
//
CCCAppPluginData* CCCAppPluginLoader::PluginInFocus( )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::PluginInFocus"));
    CCA_DP( KCCAppLogFile, CCA_L("::PluginInFocus - iCurrentPlugin: %d"), iCurrentPlugin );

    if ( 0 > iCurrentPlugin || iCurrentPlugin > PluginsCount()-1 )
        {
        CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::PluginInFocus - not in range"));
        return NULL;
        }
    else
        {
        CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::PluginInFocus - found in range"));
        return iPluginDataArray[ iCurrentPlugin ];
        }
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::PluginsCount
// ---------------------------------------------------------------------------
//
TInt CCCAppPluginLoader::PluginsCount( ) const
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::PluginsCount"));
    const TInt count = iPluginDataArray.Count();
    CCA_DP( KCCAppLogFile, CCA_L("::PluginsCount - count %d"), count );
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::PluginsCount"));
    return count;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::VisiblePluginCount
// ---------------------------------------------------------------------------
//
TInt CCCAppPluginLoader::VisiblePluginCount( ) const
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::VisiblePluginCount"));
    TInt count(0);
    for(TInt i = 0; i < iPluginDataArray.Count(); i++ )
        {
        CCCAppPluginData* dta = iPluginDataArray[i];
        if( dta->PluginVisibility() == CCCAppPluginData::EPluginVisible)
            {
            count++;
            }
        }

    CCA_DP( KCCAppLogFile, CCA_L("::VisiblePluginCount - count %d"), count );
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::VisiblePluginCount"));
    return count;
    }


// ---------------------------------------------------------------------------
// CCCAppPluginLoader::VisiblePlugin
// ---------------------------------------------------------------------------
//
CCCAppPluginData* CCCAppPluginLoader::VisiblePlugin( TInt aIndex, TBool aChangefocus )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::VisiblePluginAt"));
    CCA_DP( KCCAppLogFile, CCA_L("::VisiblePluginAt - aIndex %d"), aIndex );
    TInt visiblePlugin(KErrNotFound);

    for(TInt i = 0; i < PluginsCount(); i++)
        {
        if( iPluginDataArray[i]->PluginVisibility() == CCCAppPluginData::EPluginVisible )
            {
            visiblePlugin++;
            }
        if( visiblePlugin == aIndex )
            {
            CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::VisiblePluginAt - found in range"));
            return PluginAt( i, aChangefocus );
            }
        }
        CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::VisiblePluginAt - not in visible range"));
        return NULL;
    }


// ---------------------------------------------------------------------------
// CCCAppPluginLoader::PluginAt
// ---------------------------------------------------------------------------
//
CCCAppPluginData* CCCAppPluginLoader::PluginAt( TInt aIndex, TBool aChangefocus )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::PluginAt"));
    CCA_DP( KCCAppLogFile, CCA_L("::PluginAt - aIndex %d"), aIndex );

    const TInt count = PluginsCount();
    if ( 0 == count || 0 > aIndex || (count - 1) < aIndex )
        {
        CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::PluginAt - not in range"));
        return NULL;
        }
    else
        {
        CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::PluginAt - found in range"));
        if ( aChangefocus )
            iCurrentPlugin = aIndex;
        return iPluginDataArray[ aIndex ];
        }
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::RemovePlugin
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::RemovePlugin( TUid aPlugin )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::RemovePlugin"));
    CCA_DP( KCCAppLogFile, CCA_L("::RemovePlugin - aPlugin %d"), aPlugin );

    TInt count = PluginsCount();

    for ( TInt i = 0; i < count; ++i )
        {
        TUid id = iPluginDataArray[ i ]->Plugin().Id();
        if( aPlugin == id )
            {
            // delete
            delete iPluginDataArray[ i ];
            iPluginDataArray.Remove( i );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::Plugin
// ---------------------------------------------------------------------------
//
CCCAppViewPluginBase* CCCAppPluginLoader::Plugin( TUid aPlugin )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::Plugin"));
    CCA_DP( KCCAppLogFile, CCA_L("::Plugin - aPlugin %d"), aPlugin );

    TInt count = PluginsCount();

    for ( TInt i = 0; i < count; ++i )
        {
        TUid id = iPluginDataArray[ i ]->Plugin().Id();
        if( aPlugin == id )
            {
            return &(iPluginDataArray[ i ]->Plugin());
            }
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::SetPluginVisibility
// ---------------------------------------------------------------------------
//
TBool CCCAppPluginLoader::SetPluginVisibility(
    TUid aPlugin,
    TInt aVisibility)
    {
    TBool ret(EFalse);
    const TInt count = iPluginDataArray.Count();
    CCA_DP( KCCAppLogFile, CCA_L("::SetPluginVisibility - plugin count: %d"), count );
    for ( TInt i = 0; i < count; i++ )
        {
        TUid uid( iPluginDataArray[i]->Plugin().Id() );
        if( aPlugin == uid )
            {
            if( aVisibility != iPluginDataArray[i]->PluginVisibility() )
                {
                iPluginDataArray[i]->SetPluginVisibility( aVisibility );
                ret = ETrue;
                }

            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::SetPluginVisibility
// ---------------------------------------------------------------------------
//
TInt CCCAppPluginLoader::PluginVisibility(
    TUid aPlugin, TInt& aTabNbr )
    {
    TInt visibility(KErrNotFound);
    TInt tabNbr(KErrNotFound);
    const TInt count = iPluginDataArray.Count();
    CCA_DP( KCCAppLogFile, CCA_L("::SetPluginVisibility - plugin count: %d"), count );

    for ( TInt i = 0; i < count; i++ )
        {
        if(iPluginDataArray[i]->PluginVisibility() == CCCAppPluginData::EPluginVisible)
            {
            tabNbr++;
            }
        TUid uid( iPluginDataArray[i]->Plugin().Id() );
        if( aPlugin == uid )
            {
            visibility = iPluginDataArray[i]->PluginVisibility();
            if(visibility == CCCAppPluginData::EPluginVisible)
                {
                aTabNbr = tabNbr;
                }
            break;
            }
        }
    return visibility;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::GetPbksXPExtesionNamesL
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::GetPbksXPExtesionNamesL( RArray<TPtrC>& aPbksXPExtesionNamesArray,
        const TDesC& aNameString)
    {
    CleanupClosePushL( aPbksXPExtesionNamesArray );
    TLex nameString ( aNameString );
    TChar curChar;
    nameString.Mark();
    while( !nameString.Eos() )
        {
        curChar = nameString.Peek();
        if( curChar == '|' )
            {
            TPtrC extractedName = nameString.MarkedToken();
            User::LeaveIfError(aPbksXPExtesionNamesArray.Append(extractedName));

            nameString.Inc();
            nameString.Mark();
            }
        else
            {
            nameString.Inc();
            }
        }
    CleanupStack::Pop();
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::LargestOpaqueFromInHousePlugins
// ---------------------------------------------------------------------------
//
TInt CCCAppPluginLoader::LargestOpaqueFromInHousePlugins( RArray<TCCAPluginsOrderInfo>& aOrderInfoArray )
    {
    TInt largestOpaqueFromInHouse = 0;
    for ( TInt i=0; i<aOrderInfoArray.Count(); i++ )
        {
        // this is for plugins in group ECCAInHousePlugins, the purpose of doing this
        // is find start position for plugins in group ECCAPlugindInBothCCAAndNameList.
        // this means if opaque values in group ECCAInHousePlugins are negative,
        // Initial value if largestOpaqueFromInHouse 0 will be taked to be used.
        // if they are positive values, find the largest
        TInt order = aOrderInfoArray[i].iOrder;
        if (aOrderInfoArray[i].iGroupInfo == TCCAPluginsOrderInfo::ECCAInHousePlugins &&
                order > largestOpaqueFromInHouse )
            {
            largestOpaqueFromInHouse = order;
            }
        }
    return largestOpaqueFromInHouse;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::SmallestOpaqueFromPluginsOnlyInCCA
// ---------------------------------------------------------------------------
//
TInt CCCAppPluginLoader::SmallestOpaqueFromPluginsOnlyInCCA( RArray<TCCAPluginsOrderInfo>& aOrderInfoArray )
    {
    TInt smallestOpaque = 0;
    for ( TInt i=0; i<aOrderInfoArray.Count(); i++ )
        {
        // for plugins in group ECCAPlugindInBothCCAAndNameList, orders are follow as
        // corresponding sXPextension in  Name List view.

        // this is for plugins in group ECCAPluginsOnlyInCCA. the purpose of doing this is
        // find start positions for plugins in group ECCAPluginsOnlyInCCA if there is negative
        // opaque value. if no negative values, the start postion of plugins in this
        // ECCAPluginsOnlyInCCA group should follow the last one's opaque value in group
        // ECCAPlugindInBothCCAAndNameList
        TInt order = aOrderInfoArray[i].iOrder;
        if (aOrderInfoArray[i].iGroupInfo == TCCAPluginsOrderInfo::ECCAPluginsOnlyInCCA &&
                order < smallestOpaque )
            {
            smallestOpaque = order;
            }
        }
    return smallestOpaque;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::UpdateOrdersForPluginsOnlyInCCA
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::UpdateOrdersForPluginsOnlyInCCA(
        const TInt aStartPosition,
        RArray<TCCAPluginsOrderInfo>& aOrderInfoArray)
    {
    for ( TInt i=0; i<aOrderInfoArray.Count(); i++ )
        {
        if( aOrderInfoArray[i].iGroupInfo == TCCAPluginsOrderInfo::ECCAPluginsOnlyInCCA )
          {
          aOrderInfoArray[i].iOrder = aOrderInfoArray[i].iOrder + aStartPosition;
          }
        }
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::UpdateOrdersForPluginsInBothCCAAndNameList
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::UpdateOrdersForPluginsInBothCCAAndNameList(
        TInt& aLastPosition,
        RArray<TCCAPluginsOrderInfo>& aOrderInfoArray,
        const RArray<TPtrC>& aNameListPluginNameArray)
    {
    // go through phonebook main view's sXP Extension plugins name list
    // Group2 means ECCAPlugindInBothCCAAndNameList
    TInt startPosition = aLastPosition;
    for ( TInt i=0; i<aNameListPluginNameArray.Count(); i++ )
       {
       TInt found = EFalse;
       for ( TInt j=0; j<aOrderInfoArray.Count() && !found; j++ )
           {
           if( aNameListPluginNameArray[i].CompareF(aOrderInfoArray[j].iPluginInfor->DisplayName()) == 0
                   && aOrderInfoArray[j].iGroupInfo == TCCAPluginsOrderInfo::ECCAPlugindInBothCCAAndNameList )
               {
               aOrderInfoArray[j].iOrder = startPosition + 1 + i;
               aLastPosition = aOrderInfoArray[j].iOrder;
               found = ETrue;
               }
           }
       }

    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::SortPluginsOrderInfoL
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::SortPluginsOrderInfoL( RArray<TCCAPluginsOrderInfo>& aOrderInfoArray,
        const RArray<TPtrC>& aNameListPluginNameArray)
    {
    // largest opaque from in-house plugins
    TInt largestOpaqueFromGroup1 = LargestOpaqueFromInHousePlugins(aOrderInfoArray);

    // go through phonebook main view's sXP Extension plugins name list
    // Group2 means ECCAPlugindInBothCCAAndNameList
    TInt lastPosInGroup2 = largestOpaqueFromGroup1 + 1;

    UpdateOrdersForPluginsInBothCCAAndNameList(lastPosInGroup2, aOrderInfoArray, aNameListPluginNameArray);

    // Group3 means ECCAPluginsOnlyInCCA
    TInt startPosInGroup3 = lastPosInGroup2 + 1;

    // smallest opaque from ECCAPluginsOnlyInCCA
    TInt smallestOpaqueFromGroup3 = SmallestOpaqueFromPluginsOnlyInCCA(aOrderInfoArray);

    // if there are negative opaque values from group3
    if (smallestOpaqueFromGroup3 < 0)
        {
        startPosInGroup3 = startPosInGroup3 - smallestOpaqueFromGroup3;
        }

    UpdateOrdersForPluginsOnlyInCCA(startPosInGroup3, aOrderInfoArray);

    // after opaque value(iOrder) has been handled for each of three group:
    // ECCAInHousePlugins, ECCAPlugindInBothCCAAndNameList and ECCAPluginsOnlyInCCA
    // finally sort the aOrderInfoArray by iOrder
    TLinearOrder< TCCAPluginsOrderInfo > order( *TCCAPluginsOrderInfo::SortByOrder );
    aOrderInfoArray.Sort( order );
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::FindPluginNameFromNameListByCCAPluginInfo
// ---------------------------------------------------------------------------
//
TInt CCCAppPluginLoader::FindPluginNameFromNameListByCCAPluginInfo(
        const CImplementationInformation& aCCAPluginInfo,
        const RArray<TPtrC>& aNameListPluginNameArray)
    {
    TInt result = KErrNotFound;
    for ( TInt i=0; i<aNameListPluginNameArray.Count() && result == KErrNotFound; i++ )
        {
        if( aNameListPluginNameArray[i].CompareF( aCCAPluginInfo.DisplayName())==0 )
            {
            result = i;
            }
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::GetPluginsInfoL
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::GetPluginsInfoL( RArray<TCCAPluginsOrderInfo>& aOrderInfoArray,
        const RArray<TPtrC>& aNameListPluginNameArray,
        RPointerArray<CImplementationInformation>& aOldPluginInfoArray,
        RPointerArray<CImplementationInformation>& aNewPluginInfoArray)
    {
    //1. For lecagy interface implementations
    AppendOrderInfoL( aOrderInfoArray, aNameListPluginNameArray,
            aOldPluginInfoArray, ETrue );

    //2. For new interface implementations
    AppendOrderInfoL( aOrderInfoArray, aNameListPluginNameArray,
            aNewPluginInfoArray, EFalse );
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::AppendOrderInfoL
// ---------------------------------------------------------------------------
//
void CCCAppPluginLoader::AppendOrderInfoL( RArray<TCCAPluginsOrderInfo>& aOrderInfoArray,
        const RArray<TPtrC>& aNameListPluginNameArray,
        RPointerArray<CImplementationInformation>& aPluginInfoArray,
        TBool aIsOldInterFaceType)
    {
    // copy object CImplementationInformation's address from aPluginInfoArray to pluginOrderInfo
    // copy object CImplementationInformation's opaque value to pluginOrderInfo
    // give identification of TCCAPluginGroup info for each object in pluginOrderInfo
    for ( TInt i=0; i<aPluginInfoArray.Count(); i++ )
        {
        TCCAPluginsOrderInfo pluginOrderInfo;
        pluginOrderInfo.iPluginInfor = aPluginInfoArray[i];
        pluginOrderInfo.iOrder = GetOrderValueL( *aPluginInfoArray[i] );

        //in-house CCA plugins impelemented by ourself
        if (aPluginInfoArray[i]->ImplementationUid()==TUid::Uid(KCCACommLauncherPluginImplmentationUid) ||
            aPluginInfoArray[i]->ImplementationUid()== TUid::Uid(KCCADetailsViewPluginImplmentationUid))
            {
            pluginOrderInfo.iGroupInfo = TCCAPluginsOrderInfo::ECCAInHousePlugins;
            }
        // CCA plugin name found from plugin name list of phonebook main view's sXPExtensions
        else if (FindPluginNameFromNameListByCCAPluginInfo(*aPluginInfoArray[i],aNameListPluginNameArray) != KErrNotFound )
            {
            pluginOrderInfo.iGroupInfo = TCCAPluginsOrderInfo::ECCAPlugindInBothCCAAndNameList;
            }
        // CCA plugin name is not found from plugin name list of phonebook main view's sXPExtensions
        else
            {
            pluginOrderInfo.iGroupInfo = TCCAPluginsOrderInfo::ECCAPluginsOnlyInCCA;
            }

        pluginOrderInfo.iOldInterFaceType = aIsOldInterFaceType;
        User::LeaveIfError(aOrderInfoArray.Append( pluginOrderInfo ));
        }
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::GetOrderValueL
// ---------------------------------------------------------------------------
//
TInt CCCAppPluginLoader::GetOrderValueL( const CImplementationInformation& aInfo )
    {
    HBufC8* match = GetmachingPropertiesLC(
        aInfo.OpaqueData(), TPtrC8(KCcaOpaqueTABP), EFalse);
    TPtrC8 namePtr;
    TPtrC8 valuePtr;
    TPtrC8 mp(*match);
    GetNameValue( namePtr, valuePtr, mp);
    TInt order = 0;
    TLex8 lex( valuePtr );
    lex.Val( order );
    CleanupStack::PopAndDestroy(match);

    //Fallback2 for old plugins that provide only number in opaque data:
    //The below if{} can be removed when all plugins provide new format OpaqueData
    if(!order)
        {
        order = OpaqueValueFrom( aInfo );
        }

    return order;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginLoader::OpaqueValueFrom
// ---------------------------------------------------------------------------
//
TInt CCCAppPluginLoader::OpaqueValueFrom( const CImplementationInformation& aInfo )
    {
    TInt opaqueValue = 0;

    TLex8 lex( aInfo.OpaqueData() );
    lex.Val( opaqueValue );

    return opaqueValue;
    }

// ----------------------------------------------------------------------------
// CCCAppPluginLoader::GetmachingPropertiesLC
// ----------------------------------------------------------------------------
//
HBufC8* CCCAppPluginLoader::GetmachingPropertiesLC(
    const TDesC8& aData, const TDesC8& aDataRef, TBool aMatchValuesToo)
    {
    TPtrC8 dataPtr(aData);
    HBufC8* foundData = HBufC8::NewLC( Max(aData.Length(), aDataRef.Length()) );
    TPtr8 foundPtr = foundData->Des();

    while( dataPtr.Length() > 0 )
        {
        TPtrC8 name;
        TPtrC8 value;
        GetNameValue( name, value, dataPtr);
        TPtrC8 dataRefPtr(aDataRef);

        while( name.Length() > 0 && dataRefPtr.Length() > 0 )
            {
            TPtrC8 nameRef;
            TPtrC8 valueRef;
            GetNameValue( nameRef, valueRef, dataRefPtr);
            TBool nameOk  = !name.Compare(nameRef);
            TBool valueOk = aMatchValuesToo ? !value.Compare(valueRef) : ETrue;

            if(nameOk && valueOk)
                {
                foundPtr.Append(name);
                if(value.Length())
                    {
                    foundPtr.Append(KCcaOpaqueValueDelimiter);
                    foundPtr.Append(value);
                    }
                foundPtr.Append(KCcaOpaqueNameDelimiter);
                }
            }
        }
    return foundData;
    }


// ----------------------------------------------------------------------------
// CCCAppPluginLoader::GetNameValue
// Extracted data is removed from property string
// ----------------------------------------------------------------------------
//
void CCCAppPluginLoader::GetNameValue(
    TPtrC8& aName, TPtrC8& aValue, TPtrC8& aDataPtr )
    {
    TInt nextTokenStart = aDataPtr.Find( KCcaOpaqueNameDelimiter );
    TPtrC8 delim( aDataPtr );

    if( nextTokenStart > aDataPtr.Length() || nextTokenStart < 0 )
        {
        nextTokenStart = aDataPtr.Length();
        }

    //Inspect on the left side of field delimiter token
    TPtrC8 nameValue = aDataPtr.Left( nextTokenStart );
    TInt delimiterStart = nameValue.Find( KCcaOpaqueValueDelimiter );
    if( delimiterStart > aDataPtr.Length() || delimiterStart < 0 )
        {
        aName.Set( nameValue );
        }
    else
        {
        aName.Set( aDataPtr.Left( delimiterStart ) );
        TInt length = nameValue.Length() - delimiterStart - 1;
        aValue.Set( aDataPtr.Mid( delimiterStart + 1, length) );
        }

    //Remaining data on the right side of token
    TInt dataLeft = aDataPtr.Length() - nextTokenStart -1;
    if( dataLeft > 0 )
        {
        //Continue with remaining data on the right side of token
        aDataPtr.Set( aDataPtr.Right(dataLeft) );
        }
    else
        {
        aDataPtr.Set( TPtrC8() );
        }
    }

// ----------------------------------------------------------------------------
// CCCAppPluginLoader::ConfigurationChanged
// ----------------------------------------------------------------------------
//
void CCCAppPluginLoader::ConfigurationChanged()
    {
    //Special case. Store configuration changed, e.g. store containing current 
    //contact removed. So close CCA as current contact may not anymore be available
    CAknAppUi* appUi = static_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAP_IGNORE(appUi->HandleCommandL(EAknCmdExit));
    }

// ----------------------------------------------------------------------------
// CCCAppPluginLoader::ConfigurationChangedComplete
// ----------------------------------------------------------------------------
//
void CCCAppPluginLoader::ConfigurationChangedComplete()
    {
    }

// End of File
