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
* Description:  Phonebook 2 Group UI extension plug-in.
*
*/


// INCLUDE FILES
#include "CPguUIExtensionPlugin.h"

// Phonebook 2
#include "CPguGroupView.h"
#include "CPguGroupMembersView.h"
#include "Pbk2GroupUi.hrh"
#include "CPguCreateNewGroupCmd.h"
#include "CPguRenameGroupCmd.h"
#include "CPguBelongsToGroupsCmd.h"
#include "CPguDeleteGroupCmd.h"
#include "CPguSendMessageGroupCmd.h"
#include "CPguSetToneCmd.h"
#include <MPbk2AppUi.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ViewExplorer.h>
#include <CPbk2ViewState.h>

// System includes
#include <coemain.h>
#include <avkon.hrh>
#include <avkon.rsg>

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::CPguUIExtensionPlugin
// --------------------------------------------------------------------------
//
CPguUIExtensionPlugin::CPguUIExtensionPlugin():
    iEndCallKeyPressed ( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::~CPguUIExtensionPlugin
// --------------------------------------------------------------------------
//
CPguUIExtensionPlugin::~CPguUIExtensionPlugin()
    {
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::NewL
// --------------------------------------------------------------------------
//
CPguUIExtensionPlugin* CPguUIExtensionPlugin::NewL()
    {
    CPguUIExtensionPlugin* self = new ( ELeave ) CPguUIExtensionPlugin();
    return self;
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::CreateExtensionViewL
// --------------------------------------------------------------------------
//
MPbk2UIExtensionView* CPguUIExtensionPlugin::CreateExtensionViewL
        ( TUid aViewId, CPbk2UIExtensionView& aView )
    {
    MPbk2UIExtensionView* result = NULL;

    if ( aViewId == TUid::Uid( EPbk2GroupsListViewId ) )
        {
        result = CPguGroupView::NewL( aView );
        }
    else if ( aViewId == TUid::Uid( EPbk2GroupMembersListViewId ) )
        {
        result = CPguGroupMembersView::NewL( aView );
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPguUIExtensionPlugin::DynInitMenuPaneL(TInt /*aResourceId*/,
        CEikMenuPane* /*aMenuPane*/, MPbk2ContactUiControl& /*aControl*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::UpdateStorePropertiesL
// --------------------------------------------------------------------------
//
void CPguUIExtensionPlugin::UpdateStorePropertiesL
        ( CPbk2StorePropertyArray& /*aPropertyArray*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::CreatePbk2ContactEditorExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorExtension*
    CPguUIExtensionPlugin::CreatePbk2ContactEditorExtensionL
        ( CVPbkContactManager& /*aContactManager*/,
          MVPbkStoreContact& /*aContact*/,
          MPbk2ContactEditorControl& /*aEditorControl*/ )
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::CreatePbk2UiControlExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactUiControlExtension*
    CPguUIExtensionPlugin::CreatePbk2UiControlExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::CreatePbk2SettingsViewExtensionL
// --------------------------------------------------------------------------
//
MPbk2SettingsViewExtension*
        CPguUIExtensionPlugin::CreatePbk2SettingsViewExtensionL
            ( CVPbkContactManager& /*aContactManager*/ )
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::CreatePbk2AppUiExtensionL
// --------------------------------------------------------------------------
//
MPbk2AppUiExtension* CPguUIExtensionPlugin::CreatePbk2AppUiExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::IsEndCallKeyPressed
// --------------------------------------------------------------------------
//
TBool CPguUIExtensionPlugin::IsEndCallKeyPressed()
    {
    return iEndCallKeyPressed;
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::CreatePbk2CommandForIdL
// --------------------------------------------------------------------------
//
MPbk2Command* CPguUIExtensionPlugin::CreatePbk2CommandForIdL
        ( TInt aCommandId, MPbk2ContactUiControl& aUiControl ) const
    {
    MPbk2Command* result = NULL;

    switch ( aCommandId )
        {
        case EPbk2CmdCreateNewGroup:
            {
            result = CPguCreateNewGroupCmd::NewL( aUiControl );
            break;
            }
        case EPbk2CmdRenameGroup:
            {
            result = CPguRenameGroupCmd::NewL( aUiControl );
            break;
            }
        case EPbk2CmdBelongsToGroups:
            {
            result = CPguBelongsToGroupsCmd::NewL( aUiControl );
            break;
            }
        case EPbk2CmdOpenGroup:
            {
            CmdOpenGroupL( aUiControl );
            break;
            }
        case EPbk2CmdRemoveGroup:
            {
            result = CPguDeleteGroupCmd::NewL( aUiControl );
            break;
            }
        case EPbk2CmdWriteGroup:
            {
            result = CPguSendMessageGroupCmd::NewL( aUiControl,
                         const_cast<CPguUIExtensionPlugin*>(this) );
            break;
            }
        case EPbk2CmdSetToneToGroup:
            {
            result = CPguSetToneCmd::NewL( aUiControl );
            break;
            }
        default:
            {
            // Remaining commands are handled in Pbk2Commands
            break;
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::CreatePbk2AiwInterestForIdL
// --------------------------------------------------------------------------
//
MPbk2AiwInterestItem* CPguUIExtensionPlugin::CreatePbk2AiwInterestForIdL
        ( TInt /*aInterestId*/,
          CAiwServiceHandler& /*aServiceHandler*/ ) const
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::GetHelpContextL
// --------------------------------------------------------------------------
//
TBool CPguUIExtensionPlugin::GetHelpContextL
        ( TCoeHelpContext& /*aContext*/,  const CPbk2AppViewBase& /*aView*/,
          MPbk2ContactUiControl& /*aUiControl*/ )
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::ApplyDynamicViewGraphChangesL
// --------------------------------------------------------------------------
//
void CPguUIExtensionPlugin::ApplyDynamicViewGraphChangesL
        ( CPbk2ViewGraph& /*aViewGraph*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::ApplyDynamicPluginInformationDataL
// --------------------------------------------------------------------------
//
void CPguUIExtensionPlugin::ApplyDynamicPluginInformationDataL
        ( CPbk2UIExtensionInformation& /*aUiExtensionInformation*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::CmdOpenGroupL
// --------------------------------------------------------------------------
//
void CPguUIExtensionPlugin::CmdOpenGroupL
        ( MPbk2ContactUiControl& aUiControl ) const
    {
    if ( !aUiControl.ContactsMarked() )
        {
        CPbk2ViewState* state = aUiControl.ControlStateL();
        CleanupStack::PushL( state );
        Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->
            ActivatePhonebook2ViewL( TUid::Uid(
                EPbk2GroupMembersListViewId ), state );
        CleanupStack::PopAndDestroy( state );
        aUiControl.UpdateAfterCommandExecution();
        }
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::UIExtensionPluginExtension
// --------------------------------------------------------------------------
//
TAny* CPguUIExtensionPlugin::UIExtensionPluginExtension( TUid aExtensionUid )
    {
    TAny* ret = NULL;
    if (aExtensionUid == ImplementationUid())
        {
        ret = static_cast<MPbk2PluginCommandListerner*>( this );
        }    
    return ret;
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::ClearEndCallKeyPressedFlag
// --------------------------------------------------------------------------
//
void CPguUIExtensionPlugin::ClearEndCallKeyPressedFlag()
    {
    iEndCallKeyPressed = EFalse;
    }

// --------------------------------------------------------------------------
// CPguUIExtensionPlugin::HandlePbk2Command
// --------------------------------------------------------------------------
//
void CPguUIExtensionPlugin::HandlePbk2Command( TInt aCommand )
    {
    switch( aCommand )
        {
        // If Pbk2 goes to the backgound, then close the SendMessageGroupCmd
        case EAknCmdHideInBackground:
            {
            iEndCallKeyPressed = ETrue; 
            }
            break;
        
        default:
            break;
        }
    }

//  End of File
