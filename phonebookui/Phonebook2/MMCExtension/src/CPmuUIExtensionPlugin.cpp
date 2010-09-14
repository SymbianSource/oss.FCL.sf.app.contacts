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
* Description:  Phonebook 2 MMC UI extension plug-in.
*
*/


// INCLUDE FILES
#include "CPmuUIExtensionPlugin.h"

// Phonebook 2
#include "CPmuCopyToMmcCmd.h"
#include "CPmuCopyFromMmcCmd.h"
#include "PmuCommands.hrh"
#include <CPbk2AppUiBase.h>
#include <Pbk2Commands.hrh>
#include <MPbk2ContactUiControl.h>
#include <CPbk2AppViewBase.h>
#include <pbk2commands.rsg>
#include <pbk2mmcuires.rsg>

// System includes
#include <coemain.h>
#include <eikmenub.h>

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::CPmuUIExtensionPlugin
// --------------------------------------------------------------------------
//
CPmuUIExtensionPlugin::CPmuUIExtensionPlugin()
    {
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::~CPmuUIExtensionPlugin
// --------------------------------------------------------------------------
//
CPmuUIExtensionPlugin::~CPmuUIExtensionPlugin()
    {
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::NewL
// --------------------------------------------------------------------------
//
CPmuUIExtensionPlugin* CPmuUIExtensionPlugin::NewL()
    {
    CPmuUIExtensionPlugin* self = new ( ELeave ) CPmuUIExtensionPlugin();
    return self;
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::CreateExtensionViewL
// --------------------------------------------------------------------------
//
MPbk2UIExtensionView* CPmuUIExtensionPlugin::CreateExtensionViewL
        ( TUid /*aViewId*/, CPbk2UIExtensionView& /*aView*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPmuUIExtensionPlugin::DynInitMenuPaneL( TInt aResourceId,
        CEikMenuPane* aMenuPane, MPbk2ContactUiControl& aControl )
    {
    switch (aResourceId)
        {
        case R_PMU_CASCADING_COPY_CONTACT_CARD_MENU :
            {
            TBool marked = aControl.ContactsMarked();
            // Menu item "To other memory" is not displayed if there is no marked item in the Names List
            // Menu item "From other memory" is not displayed if there is marked item in the Names List
            TInt pos = 0;
            if ( marked )  
                {
                if ( aMenuPane->MenuItemExists( EPmuCmdImportFromMemoryCard, pos ) )
                    {
                    aMenuPane->SetItemDimmed( EPmuCmdImportFromMemoryCard, ETrue );
                    }
                }
            else
                {
                if ( aMenuPane->MenuItemExists( EPmuCmdExportToMemoryCard, pos ) )
                    {
                    aMenuPane->SetItemDimmed( EPmuCmdExportToMemoryCard, ETrue );
                    }
                }
            break;
            }
            
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::UpdateStorePropertiesL
// --------------------------------------------------------------------------
//
void CPmuUIExtensionPlugin::UpdateStorePropertiesL
        ( CPbk2StorePropertyArray& /*aPropertyArray*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::CreatePbk2ContactEditorExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorExtension*
    CPmuUIExtensionPlugin::CreatePbk2ContactEditorExtensionL
        ( CVPbkContactManager& /*aContactManager*/,
          MVPbkStoreContact& /*aContact*/,
          MPbk2ContactEditorControl& /*aEditorControl*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::CreatePbk2UiControlExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactUiControlExtension*
    CPmuUIExtensionPlugin::CreatePbk2UiControlExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::CreatePbk2SettingsViewExtensionL
// --------------------------------------------------------------------------
//
MPbk2SettingsViewExtension*
        CPmuUIExtensionPlugin::CreatePbk2SettingsViewExtensionL
            ( CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::CreatePbk2AppUiExtensionL
// --------------------------------------------------------------------------
//
MPbk2AppUiExtension* CPmuUIExtensionPlugin::CreatePbk2AppUiExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::CreatePbk2CommandForIdL
// --------------------------------------------------------------------------
//
MPbk2Command* CPmuUIExtensionPlugin::CreatePbk2CommandForIdL
        ( TInt aCommandId, MPbk2ContactUiControl& aUiControl ) const
    {
    MPbk2Command* result = NULL;

    switch ( aCommandId )
        {
        case EPmuCmdExportToMemoryCard:
            {
            result = CPmuCopyToMmcCmd::NewL( aUiControl );
            break;
            }
        case EPmuCmdImportFromMemoryCard:
            {
            result = CPmuCopyFromMmcCmd::NewL( aUiControl );
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::CreatePbk2AiwInterestForIdL
// --------------------------------------------------------------------------
//
MPbk2AiwInterestItem* CPmuUIExtensionPlugin::CreatePbk2AiwInterestForIdL
        ( TInt /*aInterestId*/,
          CAiwServiceHandler& /*aServiceHandler*/ ) const
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::GetHelpContextL
// --------------------------------------------------------------------------
//
TBool CPmuUIExtensionPlugin::GetHelpContextL
        ( TCoeHelpContext& /*aContext*/, const CPbk2AppViewBase& /*aView*/,
          MPbk2ContactUiControl& /*aUiControl*/ )
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::ApplyDynamicViewGraphChangesL
// --------------------------------------------------------------------------
//
void CPmuUIExtensionPlugin::ApplyDynamicViewGraphChangesL
        ( CPbk2ViewGraph& /*aViewGraph*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::ApplyDynamicPluginInformationDataL
// --------------------------------------------------------------------------
//
void CPmuUIExtensionPlugin::ApplyDynamicPluginInformationDataL
        ( CPbk2UIExtensionInformation& /*aUiExtensionInformation*/ )
    {
    // Do nothing
    }

// End of File
