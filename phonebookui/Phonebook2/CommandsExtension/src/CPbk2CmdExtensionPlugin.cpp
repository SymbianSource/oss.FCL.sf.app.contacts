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
* Description:  Phonebook 2 commands extension plug-in.
*
*/


#include "CPbk2CmdExtensionPlugin.h"

// Phonebook 2
#include "CPbk2CommandFactory.h"
#include <CPbk2AppUiBase.h>
#include <Pbk2Commands.hrh>

// System includes
#include <coemain.h>

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::CPbk2CmdExtensionPlugin
// --------------------------------------------------------------------------
//
CPbk2CmdExtensionPlugin::CPbk2CmdExtensionPlugin()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::~CPbk2CmdExtensionPlugin
// --------------------------------------------------------------------------
//
CPbk2CmdExtensionPlugin::~CPbk2CmdExtensionPlugin()
    {
    delete iCommandFactory;
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2CmdExtensionPlugin::ConstructL()
    {
    iCommandFactory = CPbk2CommandFactory::NewL();
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::NewL
// --------------------------------------------------------------------------
//
CPbk2CmdExtensionPlugin* CPbk2CmdExtensionPlugin::NewL()
    {
    CPbk2CmdExtensionPlugin* self = new ( ELeave ) CPbk2CmdExtensionPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::CreateExtensionViewL
// --------------------------------------------------------------------------
//
MPbk2UIExtensionView* CPbk2CmdExtensionPlugin::CreateExtensionViewL
        ( TUid /*aViewId*/, CPbk2UIExtensionView& /*aView*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2CmdExtensionPlugin::DynInitMenuPaneL
        ( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/,
          MPbk2ContactUiControl& /*aControl*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::UpdateStorePropertiesL
// --------------------------------------------------------------------------
//
void CPbk2CmdExtensionPlugin::UpdateStorePropertiesL
        ( CPbk2StorePropertyArray& /*aPropertyArray*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::CreatePbk2ContactEditorExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorExtension*
    CPbk2CmdExtensionPlugin::CreatePbk2ContactEditorExtensionL
        ( CVPbkContactManager& /*aContactManager*/,
          MVPbkStoreContact& /*aContact*/,
          MPbk2ContactEditorControl& /*aEditorControl*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::CreatePbk2UiControlExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactUiControlExtension*
    CPbk2CmdExtensionPlugin::CreatePbk2UiControlExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::CreatePbk2SettingsViewExtensionL
// --------------------------------------------------------------------------
//
MPbk2SettingsViewExtension* CPbk2CmdExtensionPlugin::
        CreatePbk2SettingsViewExtensionL
            ( CVPbkContactManager& /*aContactManager */ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::CreatePbk2AppUiExtensionL
// --------------------------------------------------------------------------
//
MPbk2AppUiExtension* CPbk2CmdExtensionPlugin::CreatePbk2AppUiExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::CreatePbk2CommandForIdL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CmdExtensionPlugin::CreatePbk2CommandForIdL
        ( TInt aCommandId,  MPbk2ContactUiControl& aUiControl ) const
    {
    return iCommandFactory->CreateCommandForIdL
        ( TPbk2CommandId( aCommandId ), aUiControl );
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::CreatePbk2AiwInterestForIdL
// --------------------------------------------------------------------------
//
MPbk2AiwInterestItem* CPbk2CmdExtensionPlugin::CreatePbk2AiwInterestForIdL
        ( TInt /*aInterestId*/,
          CAiwServiceHandler& /*aServiceHandler*/ ) const
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::GetHelpContextL
// --------------------------------------------------------------------------
//
TBool CPbk2CmdExtensionPlugin::GetHelpContextL
        ( TCoeHelpContext& /*aContext*/, const CPbk2AppViewBase& /*aView*/,
          MPbk2ContactUiControl& /*aUiControl*/ )
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::ApplyDynamicViewGraphChangesL
// --------------------------------------------------------------------------
//
void CPbk2CmdExtensionPlugin::ApplyDynamicViewGraphChangesL
        ( CPbk2ViewGraph& /*aViewGraph*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CmdExtensionPlugin::ApplyDynamicPluginInformationDataL
// --------------------------------------------------------------------------
//
void CPbk2CmdExtensionPlugin::ApplyDynamicPluginInformationDataL
        ( CPbk2UIExtensionInformation& /*aUiExtensionInformation*/ )
    {
    // Do nothing
    }

// End of File
