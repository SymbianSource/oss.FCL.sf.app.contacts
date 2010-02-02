/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Declares view for application.
*
*/






// INCLUDE FILES
#include <bldvariant.hrh>
#include <aknViewAppUi.h>
#include <avkon.hrh>
#include <akntitle.h>
#include <aknlists.h>
#include <eikmenub.h>

#include <SpeedDial.rsg>
#include <featmgr.h>
#include <AiwCommon.hrh>    //KAiwCmdCall
#include <aiwdialdataext.h>
#include <StringLoader.h>
#include <aknnotedialog.h>
#include  "speeddial.hrh"
#include "SpdiaControl.hrh"
#include  "speeddialprivate.h"

#include  "SpdiaView.h"
#include  "SpdiaContainer.h"
#include "SpdiaAppUi.h"
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <vmnumber.h>
#include <voicemailboxdomaincrkeys.h>
#include <MVPbkContactLink.h>
#include <telvmbxsettingscrkeys.h>

#include <SettingsInternalCRKeys.h>
#include <SpeeddialPrivateCRKeys.h>
#include "Spdctrl.rsg"


#include <CMessageData.h>
#include <txtrich.h>
#include <SendUiConsts.h>
#include <sendnorm.rsg>
#include <MVPbkContactFieldTextData.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSpdiaView::NewLC
//
// ---------------------------------------------------------
CSpdiaView* CSpdiaView::NewLC(CSpeedDialPrivate* aSpeedPrivate)
    {
    CSpdiaView* self = new( ELeave ) CSpdiaView;
    CleanupStack::PushL( self );
    self->ConstructL(aSpeedPrivate);
    return self;
    }

// ---------------------------------------------------------
// CSpdiaView::ConstructL
// Symbian two-phased constructor
// ---------------------------------------------------------
void CSpdiaView::ConstructL(CSpeedDialPrivate* aSpeedPrivate)
    {
    BaseConstructL( R_SPDIA_VIEW );
    CSpdiaBaseView::ConstructL();
    
		iSpeedPrivate = aSpeedPrivate;
	
    iCbaID = R_SPDIA_MENUBAR_VIEW_VOICE;

    // Attach the AIW CallUI menus with to speeddial menus.
    // This is used for CallUI menus that we want attach
    // into our owm menus.

    iServiceHandler = CAiwServiceHandler::NewL();
    iServiceHandler->AttachMenuL( R_SPDIA_VIEW_MENU, R_SPDIAL_APP_INTEREST );
    iServiceHandler->AttachMenuL( R_SPDIA_VIEW_MENU_OK_CALL, R_SPDIAL_APP_INTEREST );
    iServiceHandler->AttachMenuL( R_SPDIA_VIEW_MENU_OK_CALL_VIDEO, R_SPDIAL_APP_INTEREST );

    // Attach the AIW CallUI query menus to speeddial .
    // This is used for CallUI list query menu (i.e menu that
    // is not attached to any of speeddial own menus).

    iServiceHandler->AttachL( R_SPDIAL_APP_INTEREST );
    iReleasenumberFlag = EFalse;
    iClientRectChanged = EFalse;
    //CEikonEnv::Static()->AppUiFactory()->StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_EMPTY );

    iSendUi = CSendUi::NewL();
    iAIWRunFlag = EFalse;
    iAssignCompletedFlag = ETrue;
    iIsShowVmbxDlg = EFalse;
    }

// ---------------------------------------------------------
// CSpdiaView::~CSpdiaView
// ?implementation_description
// ---------------------------------------------------------
CSpdiaView::~CSpdiaView()
    {
    delete iContainer;
    delete iServiceHandler;
    delete iSendUi;
    }

// ---------------------------------------------------------
// CSpdiaView::LoadCbaL
// Loads relevant CBA key
// ---------------------------------------------------------
//
void CSpdiaView::LoadCba(TInt aResourceID)
    {
    if ( iCbaID != aResourceID )
        {
        MenuBar()->SetMenuTitleResourceId( aResourceID );
        iCbaID = aResourceID;
        }
    }

// ---------------------------------------------------------
// CSpdiaView::Id
// ---------------------------------------------------------
TUid CSpdiaView::Id() const
    {
    return KViewId;
    }

// ---------------------------------------------------------------------------
// CSpdiaView::NumberCallL
// Call Phone client
// Function used when send key is pressed.
// ---------------------------------------------------------------------------
void CSpdiaView::NumberCallL( const TDesC& aNumber,
        TInt& aNumberType, TBool aVmbx )          
    {
    // Set type to EAIWForcedCS if calltype is Vmbx.
    CAiwDialData::TCallType callType 
        = aVmbx ? CAiwDialData::EAIWForcedCS : CAiwDialData::EAIWVoice;                      
	
	
    switch( aNumberType )
        {
		    case ESpDialPbkFieldIdPhoneNumberVideo:
				    callType = CAiwDialData::EAIWForcedVideo;	
				    break;   			

		    case ESpDialPbkFieldIdVOIP:
		    	if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
		    		{
				    callType = CAiwDialData::EAIWVoiP;	
		    		}
        		break;
        default:
				    break;
		
        }
    CmdNumberCallL( KAiwCmdCall, ETrue, aNumber, callType );

 	
#ifdef COMMENTED //Start of Comment -Existing Code
	
    if ( aNumberType == ESpDialPbkFieldIdPhoneNumberVideo ) // change
        {
        callType = EAiwForcedVideo;
        CmdNumberCallL( KAiwCmdCall, ETrue, aNumber, callType );
        }
    // If field Id is ESpDialPbkFieldIdVOIP, set call type to VoIP.
    else if ( aNumberType == ESpDialPbkFieldIdVOIP )
    	&& FeatureManager::FeatureSupported(KFeatureIdCommonVoip))
        {
        callType = EAiwVoIPCall;
        CmdNumberCallL(KAiwCmdCall, ETrue, aNumber, callType );
        }
    else
        {
        CmdNumberCallL( KAiwCmdCall, ETrue, aNumber, callType );
        }
 #endif //End of Comment - Existing code
    }

// ---------------------------------------------------------
// CSpdiaView::HandleCommandL
// ?implementation_description
// ---------------------------------------------------------
void CSpdiaView::HandleCommandL(TInt aCommand)
    {
    if(iSpeedPrivate->GetWait()->IsStarted())
    return;
    
    if ( !iAssignCompletedFlag )
        {
        return;
        }
    
    switch ( aCommand )
        {
        case ESpdiaCmdNumberCall:
            {
            CmdNumberCallL(KAiwCmdCall, ETrue );  //KAiwCmdCall: CallUI shows list query if necessary
            break;
            }
        case EAknSoftkeyExit:
            {
            AppUi()->Exit();
            break;
            }
        case EAknCmdExit:
            {
            AppUi()->ProcessCommandL( EAknCmdExit );
            break;
            }
        case ESpdiaCmdChange:
        case ESpdiaCmdAssign:
            {
            CmdAssignNumberL();
            break;
            }
        case ESpdiaCmdClear:
            {
            CmdReleaseNumberL();
            break;
            }
        case ESpdiaCmdView:
            {
            CmdViewNumberL();
            break;
            }

        case ESpdiaCmdMessage:
        	{
    		CmdSendMessageL();    	
    		break;
	       	}

        //for help     
        case EAknCmdHelp:
            {
            AppUi()->HandleCommandL(aCommand);
            break;
            }
        case EAknCmdTaskSwapper:
            {
            //do nothing
            break;
            } 
        default:        
            {
            CmdNumberCallL( aCommand, EFalse );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CSpdiaView::HandleClientRectChange
//
// ---------------------------------------------------------
void CSpdiaView::HandleClientRectChange()
    {
    if ( iContainer && !iClientRectChanged )
        {
        iClientRectChanged = ETrue;
        iContainer->SetRect( ClientRect() );
        iClientRectChanged = EFalse;
        }
    }
    
// ---------------------------------------------------------
// CSpdiaView::HandleStatusPaneSizeChange
//
// ---------------------------------------------------------
void CSpdiaView::HandleStatusPaneSizeChange()
    {
    HandleClientRectChange();
    }

// ---------------------------------------------------------
// CSpdiaView::CmdAssignNumberL
// Command handle : Assign
// ---------------------------------------------------------
TBool CSpdiaView::CmdAssignNumberL()
    {
    if ( ( iContainer->IsVoiceMailBox() ) || ( iContainer->IsVdoMailBox() ) )
        {
        return EFalse;
        }
    // Return EFalse if speeddial does not finish assigning.
    if ( CSpeedDialPrivate::STATE_IDLE != iSpeedPrivate->State() )
        {
        return EFalse;
        }
    TInt error( KErrNone );
    TInt result( KErrNone );
    CSpeedDialPrivate& control = iContainer->Control();
    
    // A display state will be restored supposing it is an error.
    TInt index( iContainer->CurGridPosition() );
   	result = control.CheckingIfPopUpNeededL();
    control.MakeVisible( EFalse );
    
    if ( result )
        {
        result = PopUpEnableSpDiaDlgL();
        if ( result != ESpdCmdEnableYes && 
            result != ESpdCmdEnableNo && 
            result != EEikBidOk )
            {
            return EFalse;
            }			
        }
    
    MVPbkContactLink *assignContact = NULL;
    
    // set the update flag
    iAssignNumberFlag = EFalse;
    index = index + 1;
    
    // launches the ContactFetchView of the Phonebook
    iAssignCompletedFlag = EFalse;
    result = control.ShowAssign( index, assignContact );
   	iAssignCompletedFlag = ETrue;
   	index = index - 1; 
    delete assignContact;
  	
    if ( result == KErrNone )
        {	
        // If contact is selected sucessfully
        iAssignNumberFlag = EFalse;
        }
    else
        {	
        // If ConatactFetchView is canceled
        iAssignNumberFlag = ETrue;
        }
    
    control.MakeVisible( ETrue );
    UndoLayoutAndLeaveL( error );
    // Control pane is not updated quickly after canceling "Change" from options menu
    iContainer->MiddleSoftKeyL();
    
    TRect mainPaneRect ;
    TRect statusPaneRect;
    TPoint oldPosition;
    oldPosition.iX = 0;
    oldPosition.iY = 0;
    
    // Set the layout after assingning the contact
    iContainer->SetStatusPaneL();

    // Update the whole grid 
    TRAP( error, iContainer->UpdatePhoneNumberL( -1 ) );

    UndoLayoutAndLeaveL( error );
   
    // After assigning the contact,set the state as STATE_IDLE 
    iSpeedPrivate->SetState( CSpeedDialPrivate::STATE_IDLE );
   
    // Handle Msk
    iContainer->MiddleSoftKeyL();
   
    // To exit from FSW 
    if ( result == KLeaveExit )
        {
        AppUi()->Exit();
        }
   
    // return ETrue if assignment operation success, else return EFalse.
    if ( KErrNone == result )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CSpdiaView::DoActivateL
// ?implementation_description
// ---------------------------------------------------------
void CSpdiaView::DoActivateL(
        const TVwsViewId& /*aPrevViewId*/,
		TUid /*aCustomMessageId*/,
        const TDesC8& /*aCustomMessage*/)
    {
    if ( !iContainer )
        {
        iContainer = new(ELeave) CSpdiaContainer( iSpeedPrivate );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( this );
        }
    else
        {
        iContainer->ActivateL();
        }
    AppUi()->AddToStackL( *this, iContainer );
    iContainer->MiddleSoftKeyL();
  }

// ---------------------------------------------------------
// CSpdiaView::DoDeactivate
// ?implementation_description
// ---------------------------------------------------------
void CSpdiaView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer=NULL;
        }
    }

// ---------------------------------------------------------
// CSpdiaView::HandleForegroundEventL(...)
// Foreground and background events.
// @param aForeGround  ETrue : come to the foreground
//                     EFalse: removed to the foreground
// ---------------------------------------------------------
void CSpdiaView::HandleForegroundEventL(TBool aForeground)
    {
    if ( iSpeedPrivate == NULL )
        {
        return;
        }

    if ( aForeground )
        {
        // We had gone to idle mode using active application..
        // We are coming back..Restore the GridStatus..
        if ( iSpeedPrivate->State() == CSpeedDialPrivate::STATE_IDLE && 
            iSpeedPrivate->GridStatus() == CSpeedDialPrivate::EGridNoUse )
            {
            iSpeedPrivate->SetGridStatus( CSpeedDialPrivate::EGridUse );
            }

        // The cba resource should be updated when go back to speeddial.
        iContainer->MiddleSoftKeyL();
        LoadCba( iContainer->MenuResourceID() );        
        
        iAIWRunFlag = EFalse;	 
        }
    else
        {
        // We are going out of use by active application..
        // We are no longer using the Grid..Update GridStatus..
        if ( iSpeedPrivate->State() == CSpeedDialPrivate::STATE_IDLE && 
            iSpeedPrivate->GridStatus() == CSpeedDialPrivate::EGridUse )
            {
            iSpeedPrivate->SetGridStatus( CSpeedDialPrivate::EGridNoUse );
            }
        }

    // For updation of the grid view while coming from the Phonebook view
    if ( iReleasenumberFlag || iAssignNumberFlag )
        {
        // Refresh and draw the grid
        iAssignNumberFlag = EFalse;
        }	

    // On skin change create the speeddial grid and draw the new icons.
    if ( ( aForeground && ( STATIC_CAST( CSpdiaAppUi*, AppUi() )->IsSkinChanged() ) ) )
        {			
        iContainer->HandleResourceChange( KAknsMessageSkinChange );		
        iContainer->DrawNow();
        }	
    }

// ---------------------------------------------------------
// CSpdiaView::CmdNumberCallL
// Command handle : Call
// ---------------------------------------------------------
void CSpdiaView::CmdNumberCallL ( TInt aCommand,
                                  TBool aShowQuery  //True: Let CallUI to show query to user
                                                    //False:Speed Dial menu already shown to user
                                )
    {
    CSpeedDialPrivate& control = iContainer->Control();/////////////////need 2 change
    TPhCltTelephoneNumber number;
    TBool call( EFalse );
    TInt index( iContainer->CurGridPosition() );

    number = control.PhoneNumber( index );
    int type = control.NumberType( index );
    if ( ( iContainer->IsVoiceMailBox()) || ( iContainer->IsVdoMailBox() ) ) 
        {
        TInt numberType( ESpDialPbkFieldIdNone );
        CRepository*  vmbxkey2 = CRepository::NewL( KCRUidTelVideoMailbox );
        vmbxkey2->Get( KTelVideoMbxKey, vmbxsupported );
        delete vmbxkey2;
        vmbxkey2 = NULL;
        iAIWRunFlag = ETrue;
        if ( vmbxsupported == 2 )
            {
            if ( iContainer->IsVoiceMailBox() )
                {
                call = control.ExVoiceMailL( number );
                }
            else if ( iContainer->IsVdoMailBox() )
                {
                call = control.ExVideoMailL( number );
                }
            }
        else
            {
            iStatusPane->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
            iStatusPane->MakeVisible( ETrue );
            iStatusPane->DrawNow(); 
            // When Vmbx dialog need to showing, set the flag to ETrue.
            iIsShowVmbxDlg = ETrue;
            if ( iContainer->IsVoiceMailBox() || iContainer->IsVdoMailBox() )
                {
                call = control.VoiceMailL( number );
                }
            iIsShowVmbxDlg = EFalse;
            iContainer->VoiceMailDialogLaunched( EFalse );
            iContainer->SetStatusPaneL();
            }
        iAIWRunFlag = EFalse;
        if ( call )
            {
            TInt vmbxType = control.VoiceMailType();
            if( vmbxType == EVmbxIP )
                {
                numberType = ESpDialPbkFieldIdVOIP;
                }
            else if( vmbxType == EVmbxVideo )
                {
                numberType = ESpDialPbkFieldIdPhoneNumberVideo;
                }				        
            // Vmbx call, set ETrue to set EAIWForcedCS type.
            NumberCallL( number, numberType, ETrue );
            }
        }
    else if ( control.Contact(index ) != NULL )
        {
        //Set default call type (however this may be changed by CallUI).If event or
        //call type is video number type, we provie video call option as default
        CAiwDialData::TCallType callType = CAiwDialData::EAIWVoice;
				
        if ( type == ESpDialPbkFieldIdPhoneNumberVideo )
            {
            callType = 	CAiwDialData::EAIWForcedVideo;
            }
        // If field Id is ESpDialPbkFieldIdVOIP, set call type to VoIP.
        else if ( type == ESpDialPbkFieldIdVOIP && FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
            {
            callType = 	CAiwDialData::EAIWVoiP;   
            }
        CmdNumberCallL( aCommand, aShowQuery, number, callType );
        }
    }


// ---------------------------------------------------------
// CSpdiaView::CmdNumberCallL
// Command handle : Call
// ---------------------------------------------------------

void CSpdiaView::CmdNumberCallL ( 	TInt aCommandId, 
                              		TBool aShowQuery, 
                              		TPtrC aNumber, 
							  		CAiwDialData::TCallType aCallType )		
    {
    if( !aNumber.Length() )
        {
        return;
        }
   
    // We have phone number, so let's set the dial data
    CAiwDialDataExt* dialData = CAiwDialDataExt::NewLC();
    dialData->SetCallType( aCallType );

    MVPbkStoreContact* contact = iSpeedPrivate->Contact( iContainer->CurGridPosition() );
    if ( contact )
        {
        HBufC* nameBuffer( NULL );
        nameBuffer = iSpeedPrivate->ContactTitleL( contact, ETrue );
        CleanupStack::PushL( nameBuffer );
        if ( nameBuffer->Length() > AIWDialDataExt::KMaximumNameLength )
            {
            TPtrC namePtr = nameBuffer->Left( AIWDialDataExt::KMaximumNameLength );
            dialData->SetNameL( namePtr );
            }
        else
            {
            dialData->SetNameL( *nameBuffer );
            }
        CleanupStack::PopAndDestroy( nameBuffer );
        }
        
    dialData->SetPhoneNumberL( aNumber );
    dialData->SetWindowGroup( AIWDialData::KAiwGoToIdle );
    CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
    dialData->FillInParamListL( paramList );
    CleanupStack::PopAndDestroy( dialData );

    // Set the flag to ETrue, it means the aiw will be called in a fast time.
    iAIWRunFlag = ETrue;
    if ( aShowQuery )
        {
        // Execute direct command (we use CallUI's list query)
        iServiceHandler->ExecuteServiceCmdL( aCommandId,
                            paramList,
                            iServiceHandler->OutParamListL(),
                            0,
                            this ); //Callback is NULL as no dial results wanted.
        }
    else
        {
        // Execute Menu based command (we have already shown speeddial-AIW menu to user)
        iServiceHandler->ExecuteMenuCmdL( aCommandId,
                            paramList,
                            iServiceHandler->OutParamListL(),
                            0,
                            this ); //Callback is NULL as no dial results wanted.
        }
    }

// ---------------------------------------------------------
// CSpdiaView::CmdReleaseNumberL
// Command handle: Release
// ---------------------------------------------------------
void CSpdiaView::CmdReleaseNumberL()
    {
    TInt index(iContainer->CurGridPosition());
    
    iReleasenumberFlag = ETrue;
    if ( iContainer->Control().ShowRemove( index ) )
        {
        iContainer->UpdatePhoneNumberL( index );
        iContainer->Grid()->DrawDeferred();
        }
    iReleasenumberFlag = EFalse;
    iContainer->MiddleSoftKeyL();
     
    iSpeedPrivate->SetState(CSpeedDialPrivate::STATE_IDLE);
    }

// ---------------------------------------------------------
// CSpdiaView::CmdViewNumberL
// Command handle: ViewNumber
// ---------------------------------------------------------
void CSpdiaView::CmdViewNumberL()
    {
    iContainer->ShowViewNumberL();
    }

// ---------------------------------------------------------
// CSpdiaView::UndoLayoutAndLeaveL
// ?implementation_description
// ---------------------------------------------------------
void CSpdiaView::UndoLayoutAndLeaveL(TInt aError, TBool aUpdateSp)
    {
    if ( aError != KErrNone )
        {
        iContainer->MakeVisible( ETrue );
        if (aUpdateSp)
            {
            iStatusPane->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_EMPTY );
            }
        User::Leave( aError );
        }
    }

// ---------------------------------------------------------
// CSpdiaView::MakeInvisibleCallBack
// ---------------------------------------------------------
TInt CSpdiaView::MakeInvisibleCallBack(TAny* aContainer)
    {
    STATIC_CAST( CSpdiaContainer*, aContainer )->MakeVisible( EFalse );
    return FALSE;
    }

// ---------------------------------------------------------
// CSpdiaView::DynInitMenuPaneL
// Remove menu item for help application
// when feature is not supported.
// ---------------------------------------------------------
void CSpdiaView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {

    //AIW knows its own submenu hooks, so we can return from here if AIW handled this.
    if ( iServiceHandler->HandleSubmenuL( *aMenuPane ) )
        {
        return;
        }

    TInt index;

    switch( aResourceId )
        {
        case R_SENDUI_MENU:
            index = 2;
            iSendUi->AddSendMenuItemL( *aMenuPane, index, ESpdiaCmdMessage, KCapabilitiesForAllServices );
            break;

        case R_SPDIA_VIEW_MENU:
        case R_SPDIA_VIEW_MENU_VOICE:
        case R_SPDIA_VIEW_MENU_ASSIGN:
        case R_SPDIA_VIEW_MENU_OK_CALL:
        case R_SPDIA_VIEW_MENU_OK_CALL_VIDEO:
            {
            // Check if help feature is available
            if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                if ( ( aResourceId != R_SPDIA_VIEW_MENU_OK_CALL ) &&
                	   ( aResourceId != R_SPDIA_VIEW_MENU_OK_CALL_VIDEO ) )
                    {
                		aMenuPane->DeleteMenuItem( EAknCmdHelp );	
                    }
                }
            DynInitAiwMenuPaneL( aResourceId, aMenuPane, ETrue ); 	
            break;
            }
        default:
            {
            break;
            }
        }
    }

void CSpdiaView::DynInitAiwMenuPaneL( TInt aResourceId,
                                      CEikMenuPane* aMenuPane,
                                      TBool aVideo )
    {
    CAiwGenericParamList& paramList = iServiceHandler->InParamListL();

    // Get number type for commands.
    TInt numberType = 0;
    
    if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
    	{
    	numberType = iContainer->Control().NumberType( iContainer->CurGridPosition() );//////need2 change
        if ( numberType == ESpDialPbkFieldIdVOIP )
            {
            TAiwVariant variant;
            TAiwGenericParam param( EGenericParamSIPAddress, variant );
            paramList.AppendL( param );
            }
        }
    else if ( !aVideo )
        {
        TAiwVariant variant;
        TAiwGenericParam param( EGenericParamPhoneNumber, variant );
        paramList.AppendL( param );
        }
    	
    iServiceHandler->InitializeMenuPaneL( *aMenuPane,
                                          aResourceId,
                                          ESpdiaCmdLast,
                                          paramList );
    if ( aMenuPane->NumberOfItemsInPane() == KErrNone )
        {
        User::Leave( KErrNotFound );
        }
    }


// ---------------------------------------------------------
// CSpdiaView::SetCba
// Only in 5.0, used for changing cba buttons dynamically
// ---------------------------------------------------------
void CSpdiaView::SetCba( TInt aResourceId ) 
    {
    if ( Cba() )
        {
        TRAP_IGNORE(Cba()->SetCommandSetL(aResourceId));
        }
    Cba()->DrawDeferred(); 
    }

// ---------------------------------------------------------
// CSpdiaView::CmdSendMessageL
// Command handle: Send Message
// ---------------------------------------------------------
void CSpdiaView::CmdSendMessageL()
    { 
    CArrayFixFlat<TUid>* servicesToDim = new(ELeave) CArrayFixFlat<TUid>(1);
    CleanupStack::PushL( servicesToDim );
    
    servicesToDim->AppendL( KSenduiMtmBtUid );
    servicesToDim->AppendL( KSenduiMtmIrUid );
    servicesToDim->AppendL( KSenduiMtmPostcardUid );

 
    TUid serviceUid = iSendUi->ShowTypedQueryL(CSendUi::EWriteMenu,NULL ,KCapabilitiesForAllServices,servicesToDim,KNullDesC);
    CleanupStack::PopAndDestroy(); //servicesToDim 

    if ( serviceUid.iUid == 0 )
        return;
    
    TSendingCapabilities capabilities;
    iSendUi->ServiceCapabilitiesL( serviceUid, capabilities );
    
    CMessageData *messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );

    // Gets current tile pane and saves it to operatorName
    TBuf<20> operatorName;
    CAknTitlePane* title = NULL;
    CEikStatusPane *sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
    title = STATIC_CAST( CAknTitlePane*, 
        sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    operatorName.Copy( *title->Text() );

    // send as body
    CRichText* text = CRichText::NewL(
                            iEikonEnv->SystemParaFormatLayerL(), 
                            iEikonEnv->SystemCharFormatLayerL(), CEditableText::EFlatStorage);
    CleanupStack::PushL( text );
    text->InsertL( 0, KNullDesC );
    messageData->SetBodyTextL( text );
   
    MVPbkStoreContact* contact( iSpeedPrivate->Contact(iContainer->CurGridPosition()) );
    if ( NULL == contact )
        {
        return;
        }

    // get the phone number
    const TDesC& phoneNumber = iSpeedPrivate->PhoneNumber( iContainer->CurGridPosition() );
    TBool unnamedContact( EFalse );
    // get the contact title
    HBufC* nameBuffer = iSpeedPrivate->ContactTitleL( contact, EFalse );
    if ( nameBuffer == NULL )
        {
        // For unnamed contact, use phonenumber
        unnamedContact = ETrue;
        nameBuffer = HBufC::NewL( phoneNumber.Length() );
        nameBuffer->Des() = phoneNumber;
        }
    CleanupStack::PushL( nameBuffer );  
    
    TUid techTypeUid = iSendUi->TechnologyType( serviceUid );
        
    if ( KSenduiMtmSmtpUid == serviceUid || KSenduiTechnologyMailUid == techTypeUid )
        {
        HBufC* email = NULL;
        email = iSpeedPrivate->EMail( contact );
        CleanupStack::PushL( email );   
         	
        if ( NULL == email )
            {
            CleanupStack::PopAndDestroy( email );  //email
            CleanupStack::PopAndDestroy( nameBuffer );      // nameBuffer
            CleanupStack::PopAndDestroy( 2 );               // text and messageData
            return; 
            }
            		
        // append the to address
        if ( !unnamedContact )
            {
            // not unnamed contact, use name
            messageData->AppendToAddressL( email->Des(), *nameBuffer );
            }
        else
            {
            // unnamed contact, use email
            messageData->AppendToAddressL( email->Des(), *email );
            }
	    
        // Create and Send Message
        iSendUi->CreateAndSendMessageL( serviceUid /*KSenduiMtmUniMessageUid*/, messageData );
        CleanupStack::PopAndDestroy( email ); //email
        }
    else
        {
        // append the to address
        messageData->AppendToAddressL( phoneNumber, *nameBuffer );

        // Create and Send Message
        iIsShowMsgDlg = ETrue;
        iSendUi->CreateAndSendMessageL(serviceUid /*KSenduiMtmUniMessageUid*/, messageData );
        iIsShowMsgDlg = EFalse;
        }

    title->SetTextL( operatorName ); //Sets title pane for operatorName which was save  
    CleanupStack::PopAndDestroy( nameBuffer ); //nameBuffer
    CleanupStack::PopAndDestroy( text ); //text
    CleanupStack::PopAndDestroy( messageData ); //messageData
    iContainer->SetStatusPaneL();		
    }


// ---------------------------------------------------------
// CSpdiaView::IsSettingType
// ---------------------------------------------------------
TBool CSpdiaView::IsSettingType( )
    {
    return ( ( CSpdiaAppUi* )AppUi() )->IsSettingType();
    }
    
// ---------------------------------------------------------
// CSpdiaView::SetSpeedPrivate
// Only invoked by CSpdiaAppUi::~CSpdiaAppUi()
// ---------------------------------------------------------
void CSpdiaView::SetSpeedPrivate( CSpeedDialPrivate* aSpeedPrivate )
    {
    iSpeedPrivate = aSpeedPrivate;
    }

// ---------------------------------------------------------
// CSpdiaView::PopUpEnableSpDiaDlgL()
// ---------------------------------------------------------
TInt CSpdiaView::PopUpEnableSpDiaDlgL()
    {
    TInt result( EKeyPhoneEnd );
    CRepository* repository = CRepository::NewL( KCRUidTelephonySettings );

    result = ESpdCmdEnableYes;
    if ( result == ESpdCmdEnableYes || result == EEikBidOk )
        {
        repository->Set( KSettingsOneKeyDialling, 1 );
        }

    delete repository;
    return result;
    }

// ---------------------------------------------------------
// CSpdiaView::IsShowMsgDlg
// if msg dialog is showng.
// ---------------------------------------------------------
TBool CSpdiaView::IsShowMsgDlg()
    {
    return iIsShowMsgDlg; 
    }    

// ---------------------------------------------------------
// CSpdiaView::IsShowVmbxDlg
// if voice mail box dialog is showng.
// ---------------------------------------------------------
TBool CSpdiaView::IsShowVmbxDlg()
    {
    return iIsShowVmbxDlg;
    }
   
// ---------------------------------------------------------
// CSpdiaView::ProcessCommandL
// from CAknView::ProcessCommandL .
// ---------------------------------------------------------
void CSpdiaView::ProcessCommandL(TInt aCommand)
	  {
	  if ( !iAssignCompletedFlag )
	      {
	      return;
	  	  }
	  switch( aCommand )
		    {
        case EAknSoftkeyOptions:
            {
            if ( iAIWRunFlag || iSpeedPrivate->State() != CSpeedDialPrivate::STATE_IDLE )
                {
                }
            else
                {
                CAknView::ProcessCommandL(aCommand);
                }
            break;
            }
  	    default:
  	        {
            CAknView::ProcessCommandL(aCommand);
  	    	  }
		    }
	  }

// ---------------------------------------------------------
// CSpdiaView::HandleNotifyL
// call back funtion of aiw.
// ---------------------------------------------------------
TInt CSpdiaView::HandleNotifyL(
        TInt /*aCmdId*/,
        TInt aEventId,
        CAiwGenericParamList& /*aEventParamList*/,
        const CAiwGenericParamList& /*aInParamList*/)
    {
    TInt result = 0;
    // when the aiw is started, it means the service has been used at this moment.
    if ( aEventId == KAiwEventStarted )
        {
        iAIWRunFlag = EFalse;
        }
    return result;
    }

// ---------------------------------------------------------
// CSpdiaView::IsAssignCompleted
// return the iAssignCompletedFlag.
// ---------------------------------------------------------    
TBool CSpdiaView::IsAssignCompleted()
    {
    return iAssignCompletedFlag;
    }

// ---------------------------------------------------------
// Check whether there has been a focus when clicking MSK 
// @return ETrue for focus exists, or EFalse 
// ---------------------------------------------------------
TBool CSpdiaView::IsFocused()
	{
	return MenuBar()->ItemSpecificCommandsEnabled();
	}
// End of File
