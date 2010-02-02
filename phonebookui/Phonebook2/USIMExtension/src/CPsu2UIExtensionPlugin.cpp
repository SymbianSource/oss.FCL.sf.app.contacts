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
* Description:  Phonebook 2 USIM UI Extension plug-in.
*
*/
 

// INCLUDE FILES
#include "CPsu2UIExtensionPlugin.h"

// Phonebook 2
#include "CPsu2CopyToSimCmd.h"
#include "CPsu2CopyFromPbkCmd.h"
#include "CPsu2LaunchViewCmd.h"
#include "Pbk2USimUI.hrh"
#include "CPsu2ViewManager.h"
#include "CPsu2OwnNumbersView.h"
#include "CPsu2ServiceDialingView.h"
#include "CPsu2FixedDialingView.h"
#include "CPsu2ServiceDialingInfoView.h"
#include "CPsu2FixedDialingInfoView.h"
#include "CPsu2ContactEditorExtension.h"
#include <MPbk2ContactUiControl.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2AppViewBase.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StoreViewDefinition.h>
#include <CPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <MPbk2StoreValidityInformer.h>
#include <Pbk2ViewId.hrh>

// Virtual Phonebook
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkBaseContact.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactViewDefinition.h>

// System includes
#include <barsread.h>
#include <coemain.h>
#include <vwsdef.h>
#include <csxhelp/phob.hlp.hrh>
#include <avkon.hrh>

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::CPsu2UIExtensionPlugin
// --------------------------------------------------------------------------
//
CPsu2UIExtensionPlugin::CPsu2UIExtensionPlugin()
     :iEndKeyPressed( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::~CPsu2UIExtensionPlugin
// --------------------------------------------------------------------------
//
CPsu2UIExtensionPlugin::~CPsu2UIExtensionPlugin()
    {
    delete iViewManager;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::ConstructL
// --------------------------------------------------------------------------
//
inline void CPsu2UIExtensionPlugin::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();

    iViewManager = CPsu2ViewManager::NewL();
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::NewL
// --------------------------------------------------------------------------
//
CPsu2UIExtensionPlugin* CPsu2UIExtensionPlugin::NewL()
    {
    CPsu2UIExtensionPlugin* self = new( ELeave ) CPsu2UIExtensionPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::CreateExtensionViewL
// --------------------------------------------------------------------------
//
MPbk2UIExtensionView* CPsu2UIExtensionPlugin::CreateExtensionViewL
        ( TUid aViewId, CPbk2UIExtensionView& aView )
    {
    MPbk2UIExtensionView* ret = NULL;

    if ( aViewId == TUid::Uid( EPsu2OwnNumberViewId ) )
        {
        ret = CPsu2OwnNumbersView::NewL( aView, *iViewManager );
        }
    else if ( aViewId == TUid::Uid( EPsu2ServiceDialingViewId ) )
        {
        ret = CPsu2ServiceDialingView::NewL( aView, *iViewManager );
        }
    else if ( aViewId == TUid::Uid( EPsu2FixedDialingViewId ) )
        {
        ret = CPsu2FixedDialingView::NewL( aView, *iViewManager );
        }
    else if ( aViewId == TUid::Uid( EPsu2ServiceDialingInfoViewId ) )
        {
        ret = CPsu2ServiceDialingInfoView::NewL( aView, *iViewManager );
        }
    else if ( aViewId == TUid::Uid( EPsu2FixedDialingInfoViewId ) )
        {
        ret = CPsu2FixedDialingInfoView::NewL( aView, *iViewManager );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPsu2UIExtensionPlugin::DynInitMenuPaneL
        ( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/,
          MPbk2ContactUiControl& /*aControl*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::UpdateStorePropertiesL
// --------------------------------------------------------------------------
//
void CPsu2UIExtensionPlugin::UpdateStorePropertiesL
        ( CPbk2StorePropertyArray& /*aPropertyArray*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::CreatePbk2ContactEditorExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorExtension*
    CPsu2UIExtensionPlugin::CreatePbk2ContactEditorExtensionL
        ( CVPbkContactManager& aContactManager,
          MVPbkStoreContact& aContact,
          MPbk2ContactEditorControl& aEditorControl )
    {
    return CPsu2ContactEditorExtension::NewL
        ( aContactManager, aContact, aEditorControl );
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::CreatePbk2UiControlExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactUiControlExtension*
    CPsu2UIExtensionPlugin::CreatePbk2UiControlExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPmuUIExtensionPlugin::CreatePbk2SettingsViewExtensionL
// --------------------------------------------------------------------------
//
MPbk2SettingsViewExtension* CPsu2UIExtensionPlugin::
        CreatePbk2SettingsViewExtensionL
            ( CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::CreatePbk2AppUiExtensionL
// --------------------------------------------------------------------------
//
MPbk2AppUiExtension* CPsu2UIExtensionPlugin::CreatePbk2AppUiExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::CreatePbk2CommandForIdL
// --------------------------------------------------------------------------
//
MPbk2Command* CPsu2UIExtensionPlugin::CreatePbk2CommandForIdL
        ( TInt aCommandId, MPbk2ContactUiControl& aUiControl) const
    {
    MPbk2Command* result = NULL;
    switch (aCommandId)
        {
        case EPbk2CmdCopyContacts: // Pbk2 copy query uses this
            {
            MVPbkContactStore* target =
                GetTargetForCopyOrNullL( aUiControl );
            if (target)
                {
                result = CPsu2CopyToSimCmd::NewL( aUiControl,*target );
                }
            break;
            }
        case EPsu2CmdLaunchOwnNumberView:
            {
            result = CPsu2LaunchViewCmd::NewL(*iViewManager,
                EPsu2OwnNumberViewId);
            break;
            }
        case EPsu2CmdLaunchServiceDialingView:
            {
            result = CPsu2LaunchViewCmd::NewL(*iViewManager,
                EPsu2ServiceDialingViewId);
            break;
            }
        case EPsu2CmdLaunchFixedDialingView:
            {
            result = CPsu2LaunchViewCmd::NewL(*iViewManager,
                EPsu2FixedDialingViewId);
            break;
            }
        case EPsu2CmdCopyFromContacts:
            {
            MVPbkContactStore* target = iAppServices->ContactManager().
                ContactStoresL().Find(VPbkContactStoreUris::SimGlobalFdnUri());
            result = CPsu2CopyFromPbkCmd::NewL( *target, aUiControl, const_cast<CPsu2UIExtensionPlugin*>( this ) );
            break;
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::CreatePbk2AiwInterestForIdL
// --------------------------------------------------------------------------
//
MPbk2AiwInterestItem* CPsu2UIExtensionPlugin::CreatePbk2AiwInterestForIdL
        ( TInt /*aInterestId*/,
          CAiwServiceHandler& /*aServiceHandler*/ ) const
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::GetHelpContextL
// --------------------------------------------------------------------------
//
TBool CPsu2UIExtensionPlugin::GetHelpContextL
        ( TCoeHelpContext& aContext, const CPbk2AppViewBase& aView,
          MPbk2ContactUiControl& /*aUiControl*/ )
    {

    TBool helpFound( EFalse );
    switch ( aView.ViewId().iViewUid.iUid )
        {
        case EPsu2ServiceDialingViewId:         // FALLTHROUGH
        case EPsu2ServiceDialingInfoViewId:
            {
            aContext.iContext = KSDN_HLP_APP;
            helpFound = ETrue;
            break;
            }
        case EPsu2FixedDialingViewId:
            {
            aContext.iContext = KFDN_HLP_FDN_VIEW;
            helpFound = ETrue;
            break;
            }
        case EPsu2FixedDialingInfoViewId:
            {
            aContext.iContext = KFDN_HLP_FDN_NUMBER_VIEW;
            helpFound = ETrue;
            break;
            }
        case EPbk2NamesListViewId:
            {
            if ( OnlyStoreShownL
                    ( VPbkContactStoreUris::SimGlobalAdnUri() ) )
                {
                aContext.iContext = KPHOB_HLP_SIM_LIST;
                helpFound = ETrue;
                }
            else if ( OnlyStoreShownL
                    ( VPbkContactStoreUris::SimGlobalSdnUri() ) )
                {
                aContext.iContext = KSDN_HLP_APP;
                helpFound = ETrue;
                }
            break;
            }
        case EPbk2ContactInfoViewId:
            {
            if ( OnlyStoreShownL
                    ( VPbkContactStoreUris::SimGlobalAdnUri() ) )
                {
                aContext.iContext = KHLP_CCA_DETAILS;
                helpFound = ETrue;
                }
            else if ( OnlyStoreShownL
                        ( VPbkContactStoreUris::SimGlobalSdnUri() ) )
                {
                aContext.iContext = KSDN_HLP_APP;
                helpFound = ETrue;
                }
            break;
            }
        default:
            {
            // Unknown view, no help
            break;
            }
        }

    if ( helpFound )
        {
        // set uid to phonebook uid because
        // that is used in all phonebook helps
        aContext.iMajor = aView.ApplicationUid();
        }

    return helpFound;
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::ApplyDynamicViewGraphChangesL
// --------------------------------------------------------------------------
//
void CPsu2UIExtensionPlugin::ApplyDynamicViewGraphChangesL
        ( CPbk2ViewGraph& /*aViewGraph*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::ApplyDynamicPluginInformationDataL
// --------------------------------------------------------------------------
//
void CPsu2UIExtensionPlugin::ApplyDynamicPluginInformationDataL
        ( CPbk2UIExtensionInformation& /*aUiExtensionInformation*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::GetTargetForCopyOrNullL
// --------------------------------------------------------------------------
//
MVPbkContactStore* CPsu2UIExtensionPlugin::GetTargetForCopyOrNullL(
            MPbk2ContactUiControl& aUiControl) const
    {
    MVPbkContactStore* target = NULL;
    CArrayPtr<MVPbkContactStore>* stores =
        aUiControl.SelectedContactStoresL();
    const TInt oneSelectedStore = 1;
    if (stores && stores->Count() == oneSelectedStore)
        {
        // Allow copying only to global ADN store
        CleanupStack::PushL(stores);
        MVPbkContactStore* temp = iAppServices->ContactManager().
                ContactStoresL().Find(
                    VPbkContactStoreUris::SimGlobalAdnUri() );
        if ((*stores)[0] == temp)
            {
            target = temp;
            }
        CleanupStack::PopAndDestroy(stores);
        }
    else
        {
        delete stores;
        }
    return target;
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::OnlyStoreShownL
// --------------------------------------------------------------------------
//
TBool CPsu2UIExtensionPlugin::OnlyStoreShownL( const TDesC& aStoreUri )
    {
    TBool ret( EFalse );
    CVPbkContactStoreUriArray* uriArray = iAppServices->
        StoreValidityInformer().CurrentlyValidShownStoresL();
    if ( uriArray && uriArray->Count() == 1 )
        {
        TVPbkContactStoreUriPtr shownUriPtr = (*uriArray)[0];
        TVPbkContactStoreUriPtr uriPtr( aStoreUri );
        if ( uriPtr.Compare( shownUriPtr,
                TVPbkContactStoreUriPtr::EContactStoreUriAllComponents )
                    == 0 )
            {
            ret = ETrue;
            }
        }
    delete uriArray;

    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::UIExtensionPluginExtension
// --------------------------------------------------------------------------
//
TAny* CPsu2UIExtensionPlugin::UIExtensionPluginExtension( TUid aExtensionUid )
    {
    TAny* ret = NULL;
    if (aExtensionUid == ImplementationUid())
        {
        ret = static_cast<MPbk2PluginCommandListerner*>( this );
        }    
    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::HandlePbk2Command
// --------------------------------------------------------------------------
//
void CPsu2UIExtensionPlugin::HandlePbk2Command( TInt aCommand )
    {
    switch( aCommand )
        {
        // Pbk2 goes to the backgound
        case EAknCmdHideInBackground:
            {
            iEndKeyPressed = ETrue; 
            }
            break;
   
        default:
            break;
        }	
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::IsEndKeyPressed
// --------------------------------------------------------------------------
//
TBool CPsu2UIExtensionPlugin::IsEndKeyPressed()
    {
    return iEndKeyPressed;
    }

// --------------------------------------------------------------------------
// CPsu2UIExtensionPlugin::ClearEndKeyPressedFlag
// --------------------------------------------------------------------------
//
void CPsu2UIExtensionPlugin::ClearEndKeyPressedFlag()
    {
    iEndKeyPressed = EFalse;
    }

//  End of File
