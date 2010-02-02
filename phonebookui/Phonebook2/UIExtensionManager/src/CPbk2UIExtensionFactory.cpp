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
* Description:  Phonebook 2 UI extension factory.
*
*/


// INCLUDES
#include "CPbk2UIExtensionFactory.h"

// Phonebook 2
#include "CPbk2SettingsViewExtension.h"
#include "CPbk2UIExtensionMenuManager.h"
#include "CPbk2UIExtensionLoader.h"
#include "CPbk2ContactEditorExtension.h"
#include "CPbk2UIExtensionInformation.h"
#include "CPbk2AppUiExtension.h"
#include "CPbk2ContactUiControlExtension.h"
#include <CPbk2UIExtensionView.h>
#include <CPbk2UIExtensionManager.h>
#include <CPbk2UIExtensionPlugin.h>


// --------------------------------------------------------------------------
// CPbk2UIExtensionFactory::CPbk2UIExtensionFactory
// --------------------------------------------------------------------------
//
CPbk2UIExtensionFactory::CPbk2UIExtensionFactory
        ( CPbk2UIExtensionLoader& aExtensionLoader,
          CPbk2ThinUIExtensionLoader& aThinExtensionLoader,
          CPbk2UIExtensionManager& aExtensionManager ) :
            iExtensionLoader( aExtensionLoader ),
            iThinExtensionLoader( aThinExtensionLoader ),
            iExtensionManager( aExtensionManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionFactory::~CPbk2UIExtensionFactory
// --------------------------------------------------------------------------
//
CPbk2UIExtensionFactory::~CPbk2UIExtensionFactory()
    {
    iPluginUidArray.Close();
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionFactory::NewL
// --------------------------------------------------------------------------
//
CPbk2UIExtensionFactory* CPbk2UIExtensionFactory::NewL
        ( CPbk2UIExtensionLoader& aExtensionLoader,
          CPbk2ThinUIExtensionLoader& aThinExtensionLoader,
          CPbk2UIExtensionManager& aExtensionManager )
    {
    return new( ELeave ) CPbk2UIExtensionFactory
        ( aExtensionLoader, aThinExtensionLoader, aExtensionManager );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionFactory::CreatePbk2ContactEditorExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorExtension*
        CPbk2UIExtensionFactory::CreatePbk2ContactEditorExtensionL
            ( CVPbkContactManager& aContactManager,
              MVPbkStoreContact& aContact,
              MPbk2ContactEditorControl& aEditorControl )
    {
    return CPbk2ContactEditorExtension::NewL
        ( iExtensionLoader, aContactManager, aContact, aEditorControl );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionFactory::CreatePbk2UiControlExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactUiControlExtension*
        CPbk2UIExtensionFactory::CreatePbk2UiControlExtensionL
            ( CVPbkContactManager& aContactManager )
    {
    return CPbk2ContactUiControlExtension::NewL
        ( aContactManager, iExtensionLoader );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionFactory::CreatePbk2SettingsViewExtensionL
// --------------------------------------------------------------------------
//
MPbk2SettingsViewExtension*
        CPbk2UIExtensionFactory::CreatePbk2SettingsViewExtensionL
            ( CVPbkContactManager& aContactManager )
    {
    return CPbk2SettingsViewExtension::NewL
        ( aContactManager, iExtensionLoader );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionFactory::CreatePbk2AppUiExtensionL
// --------------------------------------------------------------------------
//
MPbk2AppUiExtension* CPbk2UIExtensionFactory::CreatePbk2AppUiExtensionL
        ( CVPbkContactManager& aContactManager )
    {
    return CPbk2AppUiExtension::NewL
        ( aContactManager, iExtensionLoader, iThinExtensionLoader );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionFactory::CreatePbk2CommandForIdL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2UIExtensionFactory::CreatePbk2CommandForIdL
        ( TInt aCommandId, MPbk2ContactUiControl& aUiControl ) const
    {
    MPbk2Command* result = NULL;
    iPluginUidArray.Reset();

    CPbk2UIExtensionMenuManager& menuManager =
        iExtensionManager.MenuManager();

    // Get extensions that possible implements the command
    menuManager.PluginForCommandIdL(aCommandId, iPluginUidArray);
    TInt count = iPluginUidArray.Count();
    for (TInt i = 0; i < count && !result; ++i)
        {
        // Load those extensions
        iExtensionLoader.EnsureLoadedL(iPluginUidArray[i]);
        }

    // Get first extension that implements the command
    TArray<CPbk2UIExtensionInformation*> infos =
        iExtensionLoader.PluginInformation();
    count = infos.Count();
    for (TInt i = 0; i < count && !result; ++i)
        {
        CPbk2UIExtensionPlugin* plugin =
            iExtensionLoader.LoadedPlugin(infos[i]->ImplementationUid());
        if (plugin)
            {
            result = plugin->CreatePbk2CommandForIdL
                ( aCommandId, aUiControl );
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionFactory::CreatePbk2AiwInterestForIdL
// --------------------------------------------------------------------------
//
MPbk2AiwInterestItem* CPbk2UIExtensionFactory::CreatePbk2AiwInterestForIdL
        ( TInt aInterestId, CAiwServiceHandler& aServiceHandler ) const
    {
    MPbk2AiwInterestItem* result = NULL;

    // Load extensions that use proper loading policy
    TArray<CPbk2UIExtensionInformation*> pluginInfo =
        iExtensionLoader.PluginInformation();
    TInt count = pluginInfo.Count();
    for (TInt i = 0; i < count; ++i)
        {
        TUint32 loadFlags = KPbk2LoadOnAiwInitialisation;
        if ( pluginInfo[i]->LoadingPolicy() & loadFlags )
            {
            TUid pluginUid = pluginInfo[i]->ImplementationUid();
            iExtensionLoader.EnsureLoadedL( pluginUid );
            }
        }

    // Get first extension that consumes the interest
    TArray<CPbk2UIExtensionInformation*> infos =
        iExtensionLoader.PluginInformation();
    count = infos.Count();
    for (TInt i = 0; i < count && !result; ++i)
        {
        CPbk2UIExtensionPlugin* plugin =
            iExtensionLoader.LoadedPlugin( infos[i]->ImplementationUid() );
        if ( plugin )
            {
            result = plugin->CreatePbk2AiwInterestForIdL
                ( aInterestId, aServiceHandler );
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionFactory::CreatePhonebook2ViewL
// --------------------------------------------------------------------------
//
CAknView* CPbk2UIExtensionFactory::CreatePhonebook2ViewL( TUid aId )
    {
    return CPbk2UIExtensionView::NewL( aId, iExtensionLoader );
    }

// End of File
