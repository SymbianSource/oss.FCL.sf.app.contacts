/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Call Vmbx for softnotification.
*
*/





// INCLUDE FILES
#include <avkon.hrh>
#include <bldvariant.hrh>
#include <AiwCommon.hrh>

#include <speeddial.rsg>

#include <vmnumber.h>
#include <AknQueryDialog.h> 
#include <aknnotedialog.h> 
#include <StringLoader.h> 
// needed just to get localized error message
#include <AknGlobalNote.h>
#include <ConeResLoader.h>
#include <errorres.rsg>

#include "SpdiaAppUi.h"
#include "speeddialprivate.h"
#include "SpdiaBaseView.h"
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <e32base.h> 
#include "SpdiaCallingVmbxView_voip.h"
#include "SpdiaCallingVmbxContainer.h"

#include <featmgr.h>
#include <settingsinternalcrkeys.h>
#include <centralrepository.h>

//_LIT8( KSpdiaCustomMesg, "outside" );
const TInt KVoIPOFF = 0;

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CSpdVmbxAsync::NewLC
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
CSpdVmbxAsync* CSpdVmbxAsync::NewL(CSpdiaCallingVmbxView* aSpdiaCallingVmbxView)
	{
  	CSpdVmbxAsync* self = new(ELeave) CSpdVmbxAsync(aSpdiaCallingVmbxView);
	CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self; 
	}
	
// ---------------------------------------------------------------------------
// CSpdVmbxAsync::CSpdVmbxAsync
// C++ constructor
// ---------------------------------------------------------------------------               
CSpdVmbxAsync::CSpdVmbxAsync(CSpdiaCallingVmbxView* aSpdiaCallingVmbxView):CActive(CActive::EPriorityLow)
	{
	iSpdiaCallingVmbxView =aSpdiaCallingVmbxView;
	CActiveScheduler::Add(this);
	}
// ---------------------------------------------------------------------------
// CSpdVmbxAsync::SetVmbxActiveL
// Sets Active objects
// ---------------------------------------------------------------------------
void CSpdVmbxAsync::SetVmbxActiveL()
	{
	SetActive();
	TRequestStatus* status = &iStatus;
 	User::RequestComplete(status,KErrNone);
	}
	
// ---------------------------------------------------------------------------
// CSpdVmbxAsync::ConstructL
// Symbian two-phased constructor
// ---------------------------------------------------------------------------           
void CSpdVmbxAsync::ConstructL()
	{
	}
	
// ---------------------------------------------------------------------------
// CSpdVmbxAsync::~CSpdVmbxAsync
// Destructor
// ---------------------------------------------------------------------------
CSpdVmbxAsync::~CSpdVmbxAsync()
	{
	Cancel();
	}
	
// ---------------------------------------------------------------------------
// CSpdVmbxAsync::DoCancel()
// Cancels any outstanding requests
// ---------------------------------------------------------------------------
void CSpdVmbxAsync::DoCancel()
	{
     //Cancel();
	}
	
// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::ExitSpdApp
// Exits speed dial Application
// ---------------------------------------------------------------------------
void CSpdiaCallingVmbxView::ExitSpdApp()
	{
	iAppUi->RunAppShutter();
	}

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::DispalyVmbxDialogL
// Displays VMBX text quary dialog
// ---------------------------------------------------------------------------
void CSpdiaCallingVmbxView::DispalyVmbxDialogL()
	{

//***********Start******************  Code to Support Alternate Line Handling(ALS).

	TPhCltTelephoneNumber vmbxNumber;
	RVmbxNumber vmbx;
	TBuf< KVmbxMaxAddressLength > vmbxAddress;


    //TBool call = iContainer->iControl->VoiceMailL( vmbxNumber );
	 CleanupClosePushL(vmbx); // Close() will called if a leave happens.

    if (vmbx.Open() != KErrNone)   
        {
        User::Leave(KLeaveWithoutAlert); //No vmbx available, so nothing we can do
        }

     TInt err1 = vmbx.GetVmbxNumber( vmbxNumber );
     TInt err2 = vmbx.GetVideoMbxNumber(vmbxNumber );
     TInt type(0);
                    
    if ( ( err1 == KErrNotFound || err1 == KErrNotSupported ) &&
         ( err2 == KErrNotFound || err2 == KErrNotSupported ))
        {
        type = vmbx.SelectTypeL( EVmbxDefine );
        }
    else 
        {
        type = vmbx.SelectTypeL( EVmbxCall );
        }   
    
    if( type == EVmbxIP )
        {
        TInt err( vmbx.GetVmbxAddressL( vmbxAddress ) );
        if( err == KErrNotFound )
            {
            vmbxAddress.Zero();
            if( !vmbx.QueryAddressL( EVmbxNotDefinedQuery, vmbxAddress ) )
                {
                User::Leave(KLeaveWithoutAlert);        
                }
            }        
        }
    else if( type == EVmbxVideo )
        {
        // Check if the message corresponds to active line 
        TInt lineInUse(0);
    	User::LeaveIfError(vmbx.GetAlsLine(lineInUse));
    	if ( lineInUse != iCustomMessageId.iUid ) 
    	    {
    	    if ( DoWeCallToDifferentLineL() == EFalse )
                {
                User::Leave(KLeaveWithoutAlert); //return;
                }
            }    
    	TInt err(vmbx.GetVideoMbxNumber(vmbxNumber, (TVmbxNumberEntry)iCustomMessageId.iUid));
    	if( err == KErrNotFound )  //!= KErrNone )
            {   vmbxNumber.Zero();
            if( !vmbx.QueryVideoMbxNumberL( EVmbxNotDefinedQuery, vmbxNumber ) )  //vmbxNumber: the result value of an accepted query
                {
                User::Leave(KLeaveWithoutAlert);   //User did not provide a vmbxNumberEntry, so no number to call
                }
            }                    
        }
    else if(type ==EVmbx)
        {
        // Check if the message corresponds to active line 
        TInt lineInUse(0);
    	User::LeaveIfError(vmbx.GetAlsLine(lineInUse));   //Gets currently active line number (1 or 2)
    	// 1. If a different line is active than in soft note, check do we change line. If not, then there's
    	// no need to continue as the voice mail is available on line, we are not going to switch to.
    	if ( lineInUse != iCustomMessageId.iUid ) 
    	    {
    	    if ( DoWeCallToDifferentLineL() == EFalse )
                {
                User::Leave(KLeaveWithoutAlert); //return;
                }
            }
    	
    	TInt err(vmbx.GetVmbxNumber(vmbxNumber, (TVmbxNumberEntry)iCustomMessageId.iUid)); // Line number provided by soft note

    	if( err == KErrNotFound )  //!= KErrNone )
            {   
            vmbxNumber.Zero();
            //Let's put a query on the screen to try to request for a voice mailbox number directly from user. 
            if( !vmbx.QueryNumberL( EVmbxNotDefinedQuery, vmbxNumber ) )  //vmbxNumber: the result value of an accepted query
                {
                User::Leave(KLeaveWithoutAlert);   //User did not provide a vmbxNumberEntry, so no number to call
                }
            }        
        }

	CleanupStack::PopAndDestroy(); // vmbx.Close()

    if( type == EVmbxIP )
        {
        if ( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
            {
            AddressCallL( vmbxAddress );
            }
        }
    else if( type == EVmbxVideo )
        {
        NumberCallL( vmbxNumber, CAiwDialData::EAIWForcedVideo );
        }        
    else if(type ==EVmbx)
        {
        // The type change to EAIWForcedCS for VOIP change.
        NumberCallL( vmbxNumber, CAiwDialData::EAIWForcedCS );
        }


	//***********End******************  Code to Support Alternate Line Handling(ALS).
	
	}
	
// ---------------------------------------------------------------------------
// CSpdVmbxAsync::RunL()
// Respond to an Asynchronous event
// ---------------------------------------------------------------------------
void CSpdVmbxAsync::RunL()
	{
    if ( iStatus == KErrNone )
    	{
    	TInt err=0;
    	TRAP( err, iSpdiaCallingVmbxView->DispalyVmbxDialogL() );
    	}
    
    if ( iSpdiaCallingVmbxView->GetSureToExit() )
    	{
    	iSpdiaCallingVmbxView->ExitSpdApp();
    	}
    else
    	{
    	iSpdiaCallingVmbxView->SetSureToExit( ETrue );
    	}
	}

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::NewLC
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
CSpdiaCallingVmbxView* CSpdiaCallingVmbxView::NewLC()
    {
    CSpdiaCallingVmbxView* self = new( ELeave ) CSpdiaCallingVmbxView;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::CSpdiaCallingVmbxView
// C++ constructor
// ---------------------------------------------------------------------------
CSpdiaCallingVmbxView::CSpdiaCallingVmbxView()
    {
    iAppUi = static_cast<CSpdiaAppUi*> ( AppUi() );
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::~CSpdiaCallingVmbxView
// Destructor
// ---------------------------------------------------------------------------
CSpdiaCallingVmbxView::~CSpdiaCallingVmbxView()
    {
    if ( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
    	{	
        if ( iVoIPVariationNotifier )
            {
            iVoIPVariationNotifier->StopListening();
            delete iVoIPVariationNotifier;
            iVoIPVariationNotifier = NULL;
            }
        
        if ( iVoIPVariation )
            {
            delete iVoIPVariation;
            iVoIPVariation = NULL;
            }
    	}
    
    if ( iContainer )
        {
        iAppUi->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    if(iSpdVmbxAsync)    
    delete iSpdVmbxAsync;
    iContainer = NULL;

	delete iServiceHandler;
	FeatureManager::UnInitializeLib();
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::ConstructL
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
void CSpdiaCallingVmbxView::ConstructL()
    {
    BaseConstructL( R_SPDIA_CALLING_VMBX_VIEW );
    CSpdiaBaseView::ConstructL();
    CEikonEnv::Static()->AppUiFactory()->StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_EMPTY );
	// Create the AIWFW object and specifiy the criteria items.
	iServiceHandler =CAiwServiceHandler::NewL();
	iServiceHandler->AttachL(R_SPDIAL_APP_INTEREST);
	FeatureManager::InitializeLibL();
	  TInt VoIPSupported( 0 );

	if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
            {
            iVoIPVariation = CRepository::NewL( KCRUidTelephonySettings );
            iVoIPVariation->Get( KDynamicVoIP, VoIPSupported );
    
            iVoIPVariationNotifier = CCenRepNotifyHandler::NewL( *this, *iVoIPVariation,
                CCenRepNotifyHandler::EIntKey, KDynamicVoIP );
            iVoIPVariationNotifier->StartListeningL();
            }
   
    iVoIPSupported = KVoIPOFF != VoIPSupported && 
        FeatureManager::FeatureSupported( KFeatureIdCommonVoip );
    iSureToExit = ETrue;
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::Id
// Returns view's ID.
// ---------------------------------------------------------------------------
TUid CSpdiaCallingVmbxView::Id() const
    {
    return KCallingVmbxViewId;
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::HandleDialL
// From phoneClient
// ---------------------------------------------------------------------------
void CSpdiaCallingVmbxView::HandleDialL(const TInt /*iStatus*/ )
    {
    HandleCommandL( EEikCmdExit );
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::NumberCallL
// Call Phone client
// ---------------------------------------------------------------------------
void CSpdiaCallingVmbxView::NumberCallL( const TDesC& aNumber )
    {
	CAiwDialDataExt *dialData = CAiwDialDataExt::NewLC();
	dialData->SetPhoneNumberL(aNumber);
    // The type change to EAIWForcedCS for VOIP change.
	dialData->SetCallType( CAiwDialData::EAIWForcedCS );
	dialData->SetWindowGroup(iEikonEnv->RootWin().Identifier());
	dialData->SetShowNumber(ETrue);
	CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
	dialData->FillInParamListL(paramList);
	
    iServiceHandler->ExecuteServiceCmdL( KAiwCmdCall,
									paramList,
                                    iServiceHandler->OutParamListL(),
                                    0,
                                    NULL ); //Callback is NULL as no dial results wanted.
	CleanupStack::PopAndDestroy(dialData);
    }
// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::NumberCallL
// Call Phone client
// ---------------------------------------------------------------------------

void CSpdiaCallingVmbxView:: NumberCallL( const TDesC& aNumber, 
                          CAiwDialData::TCallType aCallType )		
    {
	CAiwDialDataExt *dialData = CAiwDialDataExt::NewLC();
	dialData->SetPhoneNumberL(aNumber);
	dialData->SetCallType(aCallType);
	dialData->SetWindowGroup(iEikonEnv->RootWin().Identifier());
	dialData->SetShowNumber(ETrue);
	CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
	dialData->FillInParamListL(paramList);
	

    iServiceHandler->ExecuteServiceCmdL( KAiwCmdCall,
									paramList,
                                    iServiceHandler->OutParamListL(),
                                    0,
                                    NULL ); //Callback is NULL as no dial results wanted.

	CleanupStack::PopAndDestroy(dialData);
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::DoActivateL
// Activates this view.
// ---------------------------------------------------------------------------
void CSpdiaCallingVmbxView::DoActivateL( const TVwsViewId& aPrevViewId,
                                         TUid aCustomMessageId,
                                         const TDesC8& aCustomMessage )
    {

    ASSERT(iContainer == NULL);
    if ( !iContainer )
        iContainer = CSpdiaCallingVmbxContainer::NewL();
    iAppUi->AddToStackL( *this, iContainer );
    CEikonEnv::Static()->AppUiFactory()->StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_EMPTY );
    // Show status pane before showing vmbx dialog
    TRect origRect = iContainer->iControl->Rect();
    TRect newRect( origRect );
    TInt statusPaneHeight ;
    TRect mainPaneRect ;
   	TRect statusPaneRect;
	if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
	{
	   if (Layout_Meta_Data::IsLandscapeOrientation())
		 {
		  AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
		  iContainer->iControl->SetRect( mainPaneRect );
		 }
		 else
		 {
		  AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane,statusPaneRect);
		  statusPaneHeight = statusPaneRect.Height();
		  newRect.iTl.iY = statusPaneHeight;  // height of the status pane
       	  iContainer->iControl->SetRect( newRect );
		 }
	  }
            	
     else
     {
        
    TAknWindowLineLayout statusPaneL = 
        AKN_LAYOUT_WINDOW_status_pane( origRect, 0 );
	TAknLayoutRect layout;
	layout.LayoutRect(origRect,statusPaneL);
	 statusPaneHeight = layout.Rect().Height();
	 newRect.iTl.iY = statusPaneHeight;  // height of the status pane
    iContainer->iControl->SetRect( newRect );
    }
    
   iPrevViewId.iAppUid =aPrevViewId.iAppUid;
   iPrevViewId.iViewUid=aPrevViewId.iViewUid;
   
	iCustomMessageId.iUid = aCustomMessageId.iUid;
	iCustomMessage.Copy(aCustomMessage);
	
	if ( !iSpdVmbxAsync )
	    {
	    iSpdVmbxAsync = CSpdVmbxAsync::NewL( this );
	    iSureToExit = ETrue;
	    }
	else
	    {
	    iSureToExit = EFalse;
	    iSpdVmbxAsync->Cancel();
	    }
    iSpdVmbxAsync->SetVmbxActiveL();
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::DoWeCallToDifferentLine
// 
// ---------------------------------------------------------------------------

TBool CSpdiaCallingVmbxView::DoWeCallToDifferentLineL()
    {
    TBool okToCall = EFalse;

    //Let's read the needed resource first
    RConeResourceLoader resourceLoader(*iCoeEnv);
    CleanupClosePushL(resourceLoader);
    CAknQueryDialog* queryDlg = CAknQueryDialog::NewL();  

    //Do we change to the other line
    
    if( queryDlg->ExecuteLD( R_SPDIA_ALS_CONFIRMATION_QUERY ) )
        {
        RSSSettings settings;
        CleanupClosePushL( settings );
        User::LeaveIfError( settings.Open() );  

        if(IsLineBlocked(settings))
            {
            //Line is blocked. Not allowed to change the active line
         
            HBufC* string = NULL;
            TPtrC ptr;
            string = StringLoader::LoadLC( R_SPDIA_LINE_BLOCKING_NOTE_TEXT );
            ptr.Set( *string );
			CAknNoteDialog* dlg = new ( ELeave ) CAknNoteDialog();
            dlg->PrepareLC( R_SPDIA_ERROR_NOTE );   
            dlg->SetTextL( ptr );//KNullDesC );
            dlg->RunDlgLD( CAknNoteDialog::ELongTimeout,CAknNoteDialog::EErrorTone );
			 CleanupStack::PopAndDestroy();  //string
             okToCall = EFalse;                  
            }
        else
            {
            okToCall = ToggleLine(settings);    //Line not blokced, let's try to change the line
            }
        CleanupStack::PopAndDestroy();          //Close settings
        }
    CleanupStack::PopAndDestroy();              //Close resourceLoader
    return okToCall;
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::ToggleLine
// 
// ---------------------------------------------------------------------------
TBool CSpdiaCallingVmbxView::ToggleLine(RSSSettings& aSettings)
    {
    TInt oldValue( ESSSettingsAlsNotSupported );
    TInt newValue( ESSSettingsAlsNotSupported );

    if ( aSettings.Get( ESSSettingsAls, oldValue ) != KErrNone )
        {
        return EFalse;
        }

    switch ( oldValue )
        {
        case ESSSettingsAlsPrimary:
            newValue = ESSSettingsAlsAlternate;
            break;

        case ESSSettingsAlsAlternate:
            newValue = ESSSettingsAlsPrimary;
            break;

        case ESSSettingsAlsNotSupported:
        default:
        {
        	return EFalse;  
        }            
         //   break;
        }

    if ( aSettings.Set( ESSSettingsAls, newValue ) != KErrNone )
        {
        return EFalse;
        }

    return ETrue;

    }


// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::IsLineBlockedL
// 
// ---------------------------------------------------------------------------
TBool CSpdiaCallingVmbxView::IsLineBlocked(RSSSettings& aSettings)
    {
    TInt value = ESSSettingsAlsBlockingNotSupported;
    TInt err = aSettings.Get( ESSSettingsAlsBlocking, value );

    if ( err == KErrNotSupported )
        {
        return EFalse; //Als not supported, so no need to care for blocking
        }
    else if ( err == KErrNone )
        {
        if ( value == ESSSettingsAlsBlockingNotSupported ||
             value == ESSSettingsAlsBlockingOff )
            {
            return EFalse; //Als not blocked
            }
        }

    return ETrue;  //Line is blocked. Not allowed to change active line
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::DoDeactivate
// Deactivates this view.
// ---------------------------------------------------------------------------
void CSpdiaCallingVmbxView::DoDeactivate()
    {
    if ( iContainer )
        {
        iAppUi->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::HandleForegroundEventL
// Foreground and background events.
// ---------------------------------------------------------------------------
void CSpdiaCallingVmbxView::HandleForegroundEventL( TBool /*aForeground*/ )
    {
    }
    
// ---------------------------------------------------------------------------
// CSpdiaCallingVmbxView::AddressCallL
// Call Phone client
// ---------------------------------------------------------------------------
void CSpdiaCallingVmbxView::AddressCallL( const TDesC& aAddress )
    {
    if ( iVoIPSupported )
        {

	CAiwDialDataExt *dialData = CAiwDialDataExt::NewLC();
	dialData->SetPhoneNumberL(aAddress);
	dialData->SetCallType(CAiwDialData::EAIWVoiP);
	dialData->SetWindowGroup(iEikonEnv->RootWin().Identifier());
	dialData->SetShowNumber(ETrue);
	CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
	dialData->FillInParamListL(paramList);
	

    iServiceHandler->ExecuteServiceCmdL( KAiwCmdCall,
									paramList,
                                    iServiceHandler->OutParamListL(),
                                    0,
                                    NULL ); //Callback is NULL as no dial results wanted.
        }
    }
    
// ----------------------------------------------------
//  CSpdiaCallingVmbxView::HandleNotifyInt
// ----------------------------------------------------
//
void CSpdiaCallingVmbxView::HandleNotifyInt(
    const TUint32 aID,
    const TInt aNewValue )
    {
    if ( KDynamicVoIP == aID )
        {
        iVoIPSupported = KVoIPOFF != aNewValue &&
        FeatureManager::FeatureSupported( KFeatureIdCommonVoip );
        }
    }

/**
* set the AO exit flag;
*/ 
void CSpdiaCallingVmbxView::SetSureToExit( TBool aValue )
	{
	iSureToExit = aValue;
	}
/**
* get the AO exit flag;
*/ 	
TBool CSpdiaCallingVmbxView::GetSureToExit()
	{
	return iSureToExit;
	}

// end of file
