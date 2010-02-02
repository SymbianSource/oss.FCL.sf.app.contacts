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
* Description:  Phonebook 2 voice tag UI extension plug-in.
*
*/


#include "cpvtuiextensionplugin.h"

// Phonebook 2
#include "pvtvoicetagui.hrh"
#include "cpvtplayvoicetagcmd.h"
#include <pbk2voicetaguires.rsg>
#include <cpbk2uiextensionview.h>
#include <mpbk2contactuicontrol.h>
#include <pbk2uicontrols.rsg>
#include <pbk2commands.rsg>
#include <cpbk2presentationcontact.h>
#include <cpbk2presentationcontactfieldcollection.h>
#include <cpbk2fieldpropertyarray.h>
#include <vpbkcontactstoreuris.h>

// Virtual Phonebook
#include <mvpbkstorecontact.h>
#include <mvpbkcontactattributemanager.h>
#include <cvpbkcontactmanager.h>
#include <cvpbkvoicetagattribute.h>
#include <mvpbkfieldtype.h>
#include <cvpbkfieldtypeselector.h>
#include <cvpbkfilteredcontactview.h>
#include <mvpbkcontactstoreproperties.h>
#include <mvpbkcontactstore.h>

// System includes
#include <coemain.h>
#include <featmgr.h>
#include <eikmenup.h>
#include <barsread.h>

// Debugging headers
#include <pbk2debug.h>

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::CPvtUIExtensionPlugin
// --------------------------------------------------------------------------
//
inline CPvtUIExtensionPlugin::CPvtUIExtensionPlugin()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::~CPvtUIExtensionPlugin
// --------------------------------------------------------------------------
//
CPvtUIExtensionPlugin::~CPvtUIExtensionPlugin()
    {
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::ConstructL
// --------------------------------------------------------------------------
//
inline void CPvtUIExtensionPlugin::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::NewL
// --------------------------------------------------------------------------
//
CPvtUIExtensionPlugin* CPvtUIExtensionPlugin::NewL()
    {
    CPvtUIExtensionPlugin* self = new ( ELeave ) CPvtUIExtensionPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::CreateExtensionViewL
// --------------------------------------------------------------------------
//
MPbk2UIExtensionView* CPvtUIExtensionPlugin::CreateExtensionViewL(
        TUid /*aViewId*/,
        CPbk2UIExtensionView& /*aView*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPvtUIExtensionPlugin::DynInitMenuPaneL( TInt aResourceId,
            CEikMenuPane* aMenuPane, MPbk2ContactUiControl& aControl )
    {
    switch (aResourceId)
        {
        // Names list menu filtering is done by extension resouce flags
        // Contact info menu filtering
        case R_PHONEBOOK2_CONTACTINFO_EDIT_MENU:
            {
            TInt ignored(KErrNotFound);

            if (FeatureManager::FeatureSupported(KFeatureIdSind))
                {
                // Dim "Play command" command if
                // 1. Store not support voice tag
                // 2. Command exists     
                const MVPbkContactStoreProperties& properties = 
                    aControl.FocusedStoreContact()->
                        ParentStore().StoreProperties();                           
                if ( !properties.SupportsVoiceTags() &&
                     aMenuPane->MenuItemExists
                        ( EPvtCmdPlaybackVoiceTag, ignored ) )
                    { // Panic fix, need to resolve before build                
                    aMenuPane->SetItemDimmed
                        ( EPvtCmdPlaybackVoiceTag, ETrue );
                    }
                }

            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::UpdateStorePropertiesL
// --------------------------------------------------------------------------
//
void CPvtUIExtensionPlugin::UpdateStorePropertiesL
        ( CPbk2StorePropertyArray& /*aPropertyArray*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::CreatePbk2ContactEditorExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorExtension*
        CPvtUIExtensionPlugin::CreatePbk2ContactEditorExtensionL(
            CVPbkContactManager& /*aContactManager*/,
            MVPbkStoreContact& /*aContact*/,
            MPbk2ContactEditorControl& /*aEditorControl*/ )
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::CreatePbk2UiControlExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactUiControlExtension*
        CPvtUIExtensionPlugin::CreatePbk2UiControlExtensionL
            ( CVPbkContactManager& /*aContactManager*/ )
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::CreatePbk2SettingsViewExtensionL
// --------------------------------------------------------------------------
//
MPbk2SettingsViewExtension*
        CPvtUIExtensionPlugin::CreatePbk2SettingsViewExtensionL
            ( CVPbkContactManager& /*aContactManager*/ )
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::CreatePbk2AppUiExtensionL
// --------------------------------------------------------------------------
//
MPbk2AppUiExtension* CPvtUIExtensionPlugin::CreatePbk2AppUiExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::CreatePbk2CommandForIdL
// --------------------------------------------------------------------------
//
MPbk2Command* CPvtUIExtensionPlugin::CreatePbk2CommandForIdL
        ( TInt aCommandId, MPbk2ContactUiControl& aUiControl) const
    {
    MPbk2Command* result = NULL;
    switch (aCommandId)
        {
       case EPvtCmdPlaybackVoiceTag: // SDND and SIND
            {
            result = CPvtPlayVoiceTagCmd::NewL(aUiControl);
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
// CPvtUIExtensionPlugin::CreatePbk2AiwInterestForIdL
// --------------------------------------------------------------------------
//
MPbk2AiwInterestItem* CPvtUIExtensionPlugin::CreatePbk2AiwInterestForIdL
        ( TInt /*aInterestId*/,
          CAiwServiceHandler& /*aServiceHandler*/ ) const
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::GetHelpContextL
// --------------------------------------------------------------------------
//
TBool CPvtUIExtensionPlugin::GetHelpContextL
        ( TCoeHelpContext& /*aContext*/, const CPbk2AppViewBase& /*aView*/,
          MPbk2ContactUiControl& /*aUiControl*/ )
    {
    // Do nothing
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::ApplyDynamicViewGraphChangesL
// --------------------------------------------------------------------------
//
void CPvtUIExtensionPlugin::ApplyDynamicViewGraphChangesL
        ( CPbk2ViewGraph& /*aViewGraph*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPvtUIExtensionPlugin::ApplyDynamicPluginInformationDataL
// --------------------------------------------------------------------------
//
void CPvtUIExtensionPlugin::ApplyDynamicPluginInformationDataL
        ( CPbk2UIExtensionInformation& /*aUiExtensionInformation*/ )
    {
    // Do nothing
    }

// End of File
