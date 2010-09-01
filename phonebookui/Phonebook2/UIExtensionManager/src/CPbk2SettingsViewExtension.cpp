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
* Description:  Phonebook 2 settings view composite UI extension.
*
*/


#include "CPbk2SettingsViewExtension.h"

// Phonebook 2
#include "CPbk2UIExtensionLoader.h"
#include "ForEachUtil.h"
#include "CPbk2UIExtensionInformation.h"
#include "CPbk2UIExtensionPlugin.h"
#include "CPbk2SettingsControlExtension.h"
#include <Pbk2UIExtension.hrh>

// -----------------------------------------------------------------------------
// CPbk2SettingsViewExtension::CPbk2SettingsViewExtension
// -----------------------------------------------------------------------------
//
CPbk2SettingsViewExtension::CPbk2SettingsViewExtension
        ( CVPbkContactManager& aContactManager,
          CPbk2UIExtensionLoader& aExtensionLoader ) :
            iContactManager( aContactManager ),
            iExtensionLoader( aExtensionLoader )
    {
    }

// -----------------------------------------------------------------------------
// CPbk2SettingsViewExtension::~CPbk2SettingsViewExtension
// -----------------------------------------------------------------------------
//
CPbk2SettingsViewExtension::~CPbk2SettingsViewExtension()
    {
    iSettingExtensions.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CPbk2SettingsViewExtension::ConstructL
// -----------------------------------------------------------------------------
//
inline void CPbk2SettingsViewExtension::ConstructL()
    {
    LoadExtensionsL();
    }

// -----------------------------------------------------------------------------
// CPbk2SettingsViewExtension::NewL
// -----------------------------------------------------------------------------
//
CPbk2SettingsViewExtension* CPbk2SettingsViewExtension::NewL
        ( CVPbkContactManager& aContactManager,
          CPbk2UIExtensionLoader& aExtensionLoader )
    {
    CPbk2SettingsViewExtension* self = 
        new ( ELeave ) CPbk2SettingsViewExtension
            ( aContactManager, aExtensionLoader );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPbk2SettingsViewExtension::DoRelease
// -----------------------------------------------------------------------------
//
void CPbk2SettingsViewExtension::DoRelease()
    {
    delete this;
    }
    
// -----------------------------------------------------------------------------
// CPbk2SettingsViewExtension::CreatePbk2SettingsControlExtensionL
// -----------------------------------------------------------------------------
//
MPbk2SettingsControlExtension*
            CPbk2SettingsViewExtension::CreatePbk2SettingsControlExtensionL()
    {
    return CPbk2SettingsControlExtension::NewL( iSettingExtensions.Array() );
    }

// -----------------------------------------------------------------------------
// CPbk2SettingsViewExtension::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CPbk2SettingsViewExtension::DynInitMenuPaneL
        ( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    ForEachL( iSettingExtensions,
              VoidMemberFunction
              ( &MPbk2SettingsViewExtension::DynInitMenuPaneL ),
                aResourceId, aMenuPane );
    }
            
// -----------------------------------------------------------------------------
// CPbk2SettingsViewExtension::HandleCommandL
// -----------------------------------------------------------------------------
//
TBool CPbk2SettingsViewExtension::HandleCommandL( TInt aCommandId )
    {
    return TryEachL( iSettingExtensions, 
        MemberFunction( &MPbk2SettingsViewExtension::HandleCommandL ),
            aCommandId );
    }

// -----------------------------------------------------------------------------
// CPbk2SettingsViewExtension::LoadExtensionsL
// -----------------------------------------------------------------------------
//            
void CPbk2SettingsViewExtension::LoadExtensionsL()
    {
    TArray<CPbk2UIExtensionInformation*> pluginInfo = 
        iExtensionLoader.PluginInformation();
    const TInt count = pluginInfo.Count();
    for (TInt i = 0; i < count; ++i)
        {
        TUint32 loadFlags = KPbk2LoadInStartup | KPbk2LoadOnSettingsExecution;
        if (pluginInfo[i]->LoadingPolicy() & loadFlags)
            {
            TUid pluginUid = pluginInfo[i]->ImplementationUid();
            iExtensionLoader.EnsureLoadedL(pluginUid);
            CPbk2UIExtensionPlugin* plugin =
                iExtensionLoader.LoadedPlugin(pluginUid);
            if(plugin)                                      
                {                                           
                MPbk2SettingsViewExtension* extension =
                    plugin->CreatePbk2SettingsViewExtensionL( iContactManager );
                if ( extension )
                    {
                    CleanupDeletePushL(extension);
                    iSettingExtensions.AppendL(extension);
                    CleanupStack::Pop(); // extension
                    }
                }                                           
            }
        }
    }

// End of File
