/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of ccappcommlauncher menuhandler
*
*/

#include "ccappcommlauncherheaders.h"

#include <mccacontactobserver.h>
#include <mccappengine.h>

#include <mnproviderfinder.h>
#include <mnprovider.h>

using namespace AiwContactAssign;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::NewL
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherMenuHandler* CCCAppCommLauncherMenuHandler::NewL(
    CCCAppCommLauncherPlugin& aPlugin )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::NewL()"));
    CCCAppCommLauncherMenuHandler* self = new ( ELeave ) CCCAppCommLauncherMenuHandler( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::NewL()"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::~CCCAppCommLauncherMenuHandler
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherMenuHandler::~CCCAppCommLauncherMenuHandler()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("~CCCAppCommLauncherMenuHandler()"));
    // iView is deleted through the alfdisplay
    delete iPbkCmd;
    delete iAiwServiceHandler;
    if ( iInputBlock )
        {
        iInputBlock->Cancel();
        }    
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::CCCAppCommLauncherMenuHandler()
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherMenuHandler::CCCAppCommLauncherMenuHandler(
    CCCAppCommLauncherPlugin& aPlugin ) :
        iPlugin( aPlugin ),iInputBlock( NULL )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherMenuHandler()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::ConstructL()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherMenuHandler::ConstructL()"));
    PrepareAiwMenusL();
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::PrepareAiwMenusL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::PrepareAiwMenusL()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::PrepareAiwMenusL()"));

    // Initialisation of AIW Servicehandler cannot be in
    // ConstructL, since the menus of plugin are given
    // later via ProvideApplicationResourcesL(..)
    iAiwServiceHandler = CAiwServiceHandler::NewL();
    iAiwServiceHandler->AttachL(
        R_CCACOMMLAUNCHER_AIW_SERVICE_INTERESTS );
    iAiwServiceHandler->AttachMenuL(
        R_CCACOMMLAUNCHER_MENUPANE,
        R_CCACOMMLAUNCHER_AIW_MENU_INTERESTS );

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::PrepareAiwMenusL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::AknInputBlockCancel
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::AknInputBlockCancel()
    {
    iInputBlock = NULL;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::StopInputBlock
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::StopInputBlock()
    {
    // stop user input blocker
    if( iInputBlock )
        {
        iInputBlock->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::SetContactStore
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::SetContactStore(
    const TCmsContactStore& aStore )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherMenuHandler::SetContactStore() - %d"), (TInt)aStore );
    iContactStore = aStore;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::DynInitMenuPaneL()"));

    if ( iAiwServiceHandler->HandleSubmenuL( *aMenuPane ))
        return;

    switch ( aResourceId )
        {
        case R_CCACOMMLAUNCHER_MENUPANE:
            {
            SelectDynInitMenuPaneL( aMenuPane );

            if ( iAiwServiceHandler->IsAiwMenu( aResourceId ))
                {
                
                iAiwServiceHandler->InitializeMenuPaneL(
                    *aMenuPane, aResourceId, ECCAppCommLauncherAiwBaseCmd,
                    iAiwServiceHandler->InParamListL() );
                }
            break;
            }
        default:
            break;
        }

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::DynInitMenuPaneL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::SelectDynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::SelectDynInitMenuPaneL(
    CEikMenuPane* aMenuPane )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::SelectDynInitMenuPaneL()"));

    switch ( iContactStore )
        {
        case ECmsContactStorePbk :
            PbkContactDynInitMenuPaneL( aMenuPane );
            break;
        case ECmsContactStoreSim :
            SimContactDynInitMenuPaneL( aMenuPane );
            break;
        case ECmsContactStoreSdn :
            SdnContactDynInitMenuPaneL( aMenuPane );
            break;
        case ECmsContactStoreXsp :
            XspContactDynInitMenuPaneL( aMenuPane );
            break;
        default:
            CCA_DP(KCommLauncherLogFile, CCA_L("::DynInitMenuPaneL() - unsupported store type"));
            break;
        }

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::SelectDynInitMenuPaneL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::PbkContactDynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::PbkContactDynInitMenuPaneL(
    CEikMenuPane* aMenuPane )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::ContactDynInitMenuPaneL()"));

    DynInitSelectMenuItemL( aMenuPane );
    DynInitSelectOtherMenuItemL( aMenuPane );

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::ContactDynInitMenuPaneL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::SimContactDynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::SimContactDynInitMenuPaneL(
    CEikMenuPane* aMenuPane )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::SimContactDynInitMenuPaneL()"));

    DynInitSelectMenuItemL( aMenuPane );
    aMenuPane->DeleteMenuItem( ECCAppCommLauncherSelectOtherNmbCmd );
    aMenuPane->DeleteMenuItem( ECCAppCommLauncherSelectOtherAddrCmd );
    aMenuPane->DeleteMenuItem( ECCAppCommLauncherDefaultsCmd );
    aMenuPane->DeleteMenuItem( ECCAppCommLauncherVoiceTagDetailsCmd );

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::SimContactDynInitMenuPaneL()"));

    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::SdnContactDynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::SdnContactDynInitMenuPaneL(
    CEikMenuPane* aMenuPane )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::SdnContactDynInitMenuPaneL()"));

    DynInitSelectMenuItemL( aMenuPane );
    aMenuPane->DeleteMenuItem( ECCAppCommLauncherSelectOtherNmbCmd );
    aMenuPane->DeleteMenuItem( ECCAppCommLauncherSelectOtherAddrCmd );
    aMenuPane->DeleteMenuItem( KAiwCmdAssign );
    aMenuPane->DeleteMenuItem( ECCAppCommLauncherDefaultsCmd );
    aMenuPane->DeleteMenuItem( ECCAppCommLauncherVoiceTagDetailsCmd );
    aMenuPane->DeleteMenuItem( ECCAppCommLauncherDeleteCmd );

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::SdnContactDynInitMenuPaneL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::DynInitSelectMenuItemL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::DynInitSelectMenuItemL(
    CEikMenuPane* aMenuPane )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::DynInitSelectMenuItemL()"));
    
    if ( !iPlugin.Container().CommMethodsAvailable() )
        {// no comm methods available
        aMenuPane->DeleteMenuItem( ECCAppCommLauncherSelectCmd );
        // ECCAppCommLauncherDefaultsCmd is deleted in the SimContactDynInitMenuPaneL() method
        if (iContactStore != ECmsContactStoreSim)
            {
            aMenuPane->DeleteMenuItem( ECCAppCommLauncherDefaultsCmd );
            }
        }
    else
    	{
        CCCAppCommLauncherContactHandler& contactHandler = iPlugin.ContactHandler();
        
        // Get the number how many addresses are defined.
        const TInt addressAmount = 
             contactHandler.AddressAmount( VPbkFieldTypeSelectorFactory::EFindOnMapSelector);
        
        // If the amount of address is not 0 and the amount of listbox in launcher view is 1.
        // That means only address is defined in the contact.
        if ( ( addressAmount > 0 ) 
        	   && ( iPlugin.Container().GetListBoxItemAmount() == 1 ) )
        	{
        	aMenuPane->DeleteMenuItem( ECCAppCommLauncherDefaultsCmd );
        	}
    	}
    
    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::DynInitSelectMenuItemL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::DynInitSelectOtherMenuItemL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::DynInitSelectOtherMenuItemL(
    CEikMenuPane* aMenuPane )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::DynInitSelectOtherMenuItemL()"));

    if ( iPlugin.Container().CommMethodsAvailable() && 
    		iPlugin.Container().IsListBoxHighlightEnabled())
        {// comm methods available, continue..
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector contactAction =
            iPlugin.Container().SelectedCommunicationMethod();
        CCCAppCommLauncherContactHandler& contactHandler = iPlugin.ContactHandler();//not owned

        if ( contactHandler.IsItNumberAddress( contactAction ))
            {
            aMenuPane->DeleteMenuItem( ECCAppCommLauncherSelectOtherAddrCmd );

            if ( contactHandler.AddressAmount( contactAction ) < 2
                 || !contactHandler.HasDefaultAttribute( contactAction ))
                {
                aMenuPane->DeleteMenuItem( ECCAppCommLauncherSelectOtherNmbCmd );
                }
            }
        else
            {
            aMenuPane->DeleteMenuItem( ECCAppCommLauncherSelectOtherNmbCmd );

            if ( contactHandler.AddressAmount( contactAction ) < 2
                 || !contactHandler.HasDefaultAttribute( contactAction ))
                {
                aMenuPane->DeleteMenuItem( ECCAppCommLauncherSelectOtherAddrCmd );
                }
            }
        }
    else
        {// no comm methods available or Listbox highlight is disabled, don't show anything
        aMenuPane->DeleteMenuItem( ECCAppCommLauncherSelectOtherNmbCmd );
        aMenuPane->DeleteMenuItem( ECCAppCommLauncherSelectOtherAddrCmd );
        }

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::DynInitSelectOtherMenuItemL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::HandleCommandL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::HandleCommandL( TInt aCommand )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::HandleCommandL()"));

    // Check and handle the Aiw commands 1st
    TInt aiwCmd = iAiwServiceHandler->ServiceCmdByMenuCmd( aCommand );
    if( aiwCmd )
        {
        DoAiwCommandL( aCommand, aiwCmd );
        return;// aCommand should not match with non-Aiw commands, but just to be sure
        }

    // Then check if command is normal application command
    switch( aCommand )
        {
        case EAknSoftkeySelect:
        case ECCAppCommLauncherSelectCmd:
            DoSelectCmdL( ETrue );// use default
            break;
        case ECCAppCommLauncherSelectOtherNmbCmd:
        case ECCAppCommLauncherSelectOtherAddrCmd:
            DoSelectCmdL( EFalse );// do not use default
            break;
        case KAiwCmdAssign:// AIW service cmd for Editing
            DoEditCmdL();
            break;
        case ECCAppCommLauncherDefaultsCmd:
            DoSetDefaultCmdL();
            break;
        case ECCAppCommLauncherDeleteCmd:
            DoDeleteCmdL();
            break;
        default:
            break;
        }

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::HandleCommandL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::DoSelectCmdL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::DoSelectCmdL( TBool aUseDefaultAddress )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::DoSelectCmdL()"));

    if ( iPlugin.Container().SelectedCommunicationMethod()
    		== VPbkFieldTypeSelectorFactory::EFindOnMapSelector )
    	{
    	DoShowOnMapCmdL();
    	}
    else
    	{
        TPtrC fullName;
        iPlugin.ContactHandler().ContactFieldItemDataL(
            CCmsContactFieldItem::ECmsFullName, fullName );
        TUint paramFlag =
            aUseDefaultAddress ? CCAContactorService::TCSParameter::EEnableDefaults : 0;
        CCAContactorService::TCSParameter param(
            iPlugin.Container().SelectedCommunicationMethod(),
            *iPlugin.ContactHandler().ContactIdentifierLC(),//contactlinkarray
            paramFlag,
            fullName );

        iPlugin.ExecuteServiceL( param );
        CleanupStack::PopAndDestroy( 1 );// contactlinkarray
    	}
    
    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::DoSelectCmdL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::DoEditCmdL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::DoEditCmdL()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::DoEditCmdL()"));

    TCCAppCommandState& cmdState( iPlugin.CommandState() );
    cmdState.SetRunningAndPushCleanupL();
    
    // Avoid user input during Edit command execution
    if( iInputBlock )
        {
        iInputBlock->Cancel();
        }

    iInputBlock = CAknInputBlock::NewCancelHandlerLC( this );
    CleanupStack::Pop( iInputBlock );
    iInputBlock->SetCancelDelete( iInputBlock );
    CAiwGenericParamList& inParamList = iAiwServiceHandler->InParamListL();
    TInt popAndDestroyUs = 0;

    inParamList.AppendL(
        TAiwGenericParam(
            EGenericParamContactAssignData,
            TAiwVariant( TAiwSingleContactAssignDataV1Pckg(
                TAiwSingleContactAssignDataV1().SetFlags( EskipInfoNotes )))));

    popAndDestroyUs += PackedContactLinkArrayToInParamListLC( inParamList );

    iAiwServiceHandler->ExecuteServiceCmdL(
        KAiwCmdAssign,
        inParamList,
        iAiwServiceHandler->OutParamListL(), NULL, this );

   CleanupStack::PopAndDestroy( popAndDestroyUs );
   // Async AIW command -> set state to not running in HandleNotifyL.
   cmdState.PopCleanup();

   CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::DoEditCmdL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::DoAiwCommandL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::DoAiwCommandL( TInt aCmdId, TInt aServiceId )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::DoAiwCommandL()"));

    // Command state is checked in CCCAppCommLauncherPlugin::HandleCommandL 
    // before calling the menuhandler.
    TCCAppCommandState& cmdState( iPlugin.CommandState() );
    cmdState.SetRunningAndPushCleanupL();
    
    CAiwGenericParamList& inParamList = iAiwServiceHandler->InParamListL();
    TInt popAndDestroyUs = 0;

    switch( aServiceId )
        {
        case KAiwCmdSindInfoView:
            popAndDestroyUs += PackedContactLinkArrayToInParamListLC( inParamList );
            break;
        default:
            break;
        }

    iAiwServiceHandler->ExecuteMenuCmdL(
        aCmdId,
        inParamList,
        iAiwServiceHandler->OutParamListL(), NULL, NULL );// not needed (so far)

   CleanupStack::PopAndDestroy( popAndDestroyUs );
   // Synchronous AIW call -> state to not running.
   cmdState.SetNotRunningAndPopCleanup();

   CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::DoAiwCommandL()"));
   }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::PackedContactLinkArrayToInParamListLC
// ---------------------------------------------------------------------------
//
TInt CCCAppCommLauncherMenuHandler::PackedContactLinkArrayToInParamListLC(
    CAiwGenericParamList& aInParamList )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::PrepareAiwParamListForVoiceTagLC()"));

    HBufC8* packedLinks = iPlugin.ContactHandler().ContactIdentifierLC();

    aInParamList.AppendL(
        TAiwGenericParam( EGenericParamContactLinkArray,
        TAiwVariant( *packedLinks )));

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::PrepareAiwParamListForVoiceTagLC()"));
    return 1;// packedLinks
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::HandleNotifyL
// ---------------------------------------------------------------------------
//
TInt CCCAppCommLauncherMenuHandler::HandleNotifyL(
    TInt aCmdId,
    TInt aEventId,
    CAiwGenericParamList& aEventParamList,
    const CAiwGenericParamList& /*aInParamList*/ )
    {
    if ( aEventId == KAiwEventCompleted || 
         aEventId == KAiwEventCanceled ||
         aEventId == KAiwEventError )
        {
        iPlugin.CommandState().SetNotRunning();
        }
    
    TInt returnValue = KErrNone;
    TInt index = 0;
    const TAiwGenericParam* param = aEventParamList.FindFirst(index,
        EGenericParamContactLinkArray);
    
    index = 0;
    const TAiwGenericParam* errParam = aEventParamList.FindFirst(index,
            EGenericParamError);

    if (param)
        {
        TPtrC8 contactLink = param->Value().AsData();

        // Update the CCA parameter contact data
        HBufC16* link16 = HBufC16::NewLC( contactLink.Length() );
        link16->Des().Copy( contactLink );
        iPlugin.AppEngine()->Parameter().SetContactDataL( link16->Des() );
        // Set contact data flag to EContactLink, because the parameters returned from 
        // phonebook AIW provider have conatct link but not contact id.
        iPlugin.AppEngine()->Parameter().SetContactDataFlag( MCCAParameter::EContactLink );
        CleanupStack::PopAndDestroy(1); // link16

        // Inform the app engine of the contact event
        iPlugin.AppEngine()->CCAppContactEventL();

        // Update the commlauncher the UI contact data
        iPlugin.ContactHandler().RefetchContactL();
        iPlugin.Container().ContactsChangedL();
        }

    if (errParam)
        {
        TInt32 err;
        errParam->Value().Get(err);
        if (err == (TInt32)KErrInUse)
            {
            CCoeEnv::Static()->HandleError(KErrInUse);
            }
        }
    
    if ( KAiwCmdAssign == aCmdId && KAiwEventQueryExit == aEventId )
        {
        // Send an exit command in order to close CCA and Phonebook2
		CAknAppUi* appUi = static_cast<CAknAppUi*>( CCoeEnv::Static()->AppUi() );

		if ( appUi )
			{
			appUi->ProcessCommandL( EAknCmdExit );
			}

		// allow pbk editor AIW service to close whole app chain
        returnValue = ETrue;
        }
    
    // stop user input blocker
    StopInputBlock();
    
    return returnValue;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::DoSetDefaultCmdL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::DoSetDefaultCmdL()
    {
    if( !iPbkCmd )
        iPbkCmd = CCCAppCommLauncherPbkCmd::NewL( iPlugin );

    iPbkCmd->ExecutePbk2CmdAssignDefaultL(
        *iPlugin.ContactHandler().ContactIdentifierLC() );

    CleanupStack::PopAndDestroy( 1 ); // ContactIdentifierLC
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::DoShowOnMapCmdL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::DoShowOnMapCmdL()
    {
    if( !iPbkCmd )
    	{
        iPbkCmd = CCCAppCommLauncherPbkCmd::NewL( iPlugin );
    	}
        
    iPbkCmd->ExecutePbk2CmdShowMapL(
            iPlugin.ContactHandler().ContactIdentifierLC(),
            (TPbk2CommandId)EPbk2ExtensionShowOnMap );

    CleanupStack::PopAndDestroy( 1 ); // ContactIdentifierLC
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::DoDeleteCmdL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::DoDeleteCmdL()
    {
    if( !iPbkCmd )
	    {
        iPbkCmd = CCCAppCommLauncherPbkCmd::NewL( iPlugin );
        }
    iPbkCmd->ExecutePbk2CmdDeleteL(
        *iPlugin.ContactHandler().ContactIdentifierLC() );

    CleanupStack::PopAndDestroy( 1 ); // ContactIdentifierLC
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherMenuHandler::XspContactDynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherMenuHandler::XspContactDynInitMenuPaneL(
    CEikMenuPane* aMenuPane )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherMenuHandler::XspContactDynInitMenuPaneL()"));

    DynInitSelectMenuItemL( aMenuPane );
    DynInitSelectOtherMenuItemL( aMenuPane );

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherMenuHandler::XspContactDynInitMenuPaneL()"));
    }


// End of File
