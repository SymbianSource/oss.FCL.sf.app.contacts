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
* Description:  Map extension plugin.
*
*/

#include "cpmapuiextensionplugin.h"

// Phonebook 2
#include "cpmapcmd.h"
#include "pbk2mapcommands.hrh"
#include <CPbk2AppUiBase.h>
#include <Pbk2Commands.hrh>
#include <MPbk2CommandHandler.h>
#include <MPbk2ApplicationServices.h>
#include <Pbk2MapUIRes.rsg>
#include <Pbk2Commands.rsg>
#include <Pbk2UIControls.hrh>
#include <Pbk2UIControls.rsg>
#include <cpmapcontacteditorextension.h>
#include "cpmapcmd.h"

// System includes
#include <coemain.h>
#include <eikmenub.h>
#include <AiwCommon.hrh>
#include <AiwServiceHandler.h>

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::CPmapUIExtensionPlugin
// --------------------------------------------------------------------------
//
CPmapUIExtensionPlugin::CPmapUIExtensionPlugin()
    {
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::~CPmapUIExtensionPlugin
// --------------------------------------------------------------------------
//
CPmapUIExtensionPlugin::~CPmapUIExtensionPlugin()
    {
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::NewL
// --------------------------------------------------------------------------
//
CPmapUIExtensionPlugin* CPmapUIExtensionPlugin::NewL()
    {
    CPmapUIExtensionPlugin* self = new ( ELeave ) CPmapUIExtensionPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::ConstructL
// --------------------------------------------------------------------------
//
void CPmapUIExtensionPlugin::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::CreateExtensionViewL
// --------------------------------------------------------------------------
//
MPbk2UIExtensionView* CPmapUIExtensionPlugin::CreateExtensionViewL
        ( TUid /*aViewId*/, CPbk2UIExtensionView& /*aView*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPmapUIExtensionPlugin::DynInitMenuPaneL( TInt /*aResourceId*/,
        CEikMenuPane* /*aMenuPane*/, MPbk2ContactUiControl& /*aControl*/ )
    {        
   
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::UpdateStorePropertiesL
// --------------------------------------------------------------------------
//
void CPmapUIExtensionPlugin::UpdateStorePropertiesL
        ( CPbk2StorePropertyArray& /*aPropertyArray*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::CreatePbk2ContactEditorExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorExtension*
    CPmapUIExtensionPlugin::CreatePbk2ContactEditorExtensionL
        ( CVPbkContactManager& aContactManager,
          MVPbkStoreContact& aContact,
          MPbk2ContactEditorControl& aEditorControl )
    {
    return CPmapContactEditorExtension::NewL
        ( aContactManager, aContact, aEditorControl );
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::CreatePbk2UiControlExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactUiControlExtension*
    CPmapUIExtensionPlugin::CreatePbk2UiControlExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::CreatePbk2SettingsViewExtensionL
// --------------------------------------------------------------------------
//
MPbk2SettingsViewExtension*
        CPmapUIExtensionPlugin::CreatePbk2SettingsViewExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::CreatePbk2AppUiExtensionL
// --------------------------------------------------------------------------
//
MPbk2AppUiExtension* CPmapUIExtensionPlugin::CreatePbk2AppUiExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::CreatePbk2CommandForIdL
// --------------------------------------------------------------------------
//
MPbk2Command* CPmapUIExtensionPlugin::CreatePbk2CommandForIdL
        ( TInt aCommandId, MPbk2ContactUiControl& aUiControl ) const
    {
    switch(aCommandId)
    	{
    	case EPbk2ExtensionAssignFromMapSelect:
    	case EPbk2ExtensionAssignFromMap:
    	case EPbk2ExtensionShowOnMap:
    		return CPmapCmd::NewL( aUiControl, aCommandId );
    	default:
    		break;
    	}
    return NULL;
    }
    
// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::CreatePbk2AiwInterestForIdL
// --------------------------------------------------------------------------
//
MPbk2AiwInterestItem* CPmapUIExtensionPlugin::CreatePbk2AiwInterestForIdL(
        TInt /*aInterestId*/,
        CAiwServiceHandler& /*aServiceHandler*/ ) const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::GetHelpContextL
// --------------------------------------------------------------------------
//
TBool CPmapUIExtensionPlugin::GetHelpContextL
        ( TCoeHelpContext& /*aContext*/, const CPbk2AppViewBase& /*aView*/,
          MPbk2ContactUiControl& /*aUiControl*/ )
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::ApplyDynamicViewGraphChangesL
// --------------------------------------------------------------------------
//
void CPmapUIExtensionPlugin::ApplyDynamicViewGraphChangesL
        ( CPbk2ViewGraph& /*aViewGraph*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPmapUIExtensionPlugin::ApplyDynamicPluginInformationDataL
// --------------------------------------------------------------------------
//
void CPmapUIExtensionPlugin::ApplyDynamicPluginInformationDataL
        ( CPbk2UIExtensionInformation& /*aUiExtensionInformation*/ )
    {
    // Do nothing
    }

// End of File
