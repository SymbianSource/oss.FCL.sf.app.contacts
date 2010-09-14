/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Logs application "Settings" view class implementation
*
*/


// INCLUDE FILES
#include <aknnotedialog.h>    
#include <AknQueryDialog.h>  

#include <gsfwviewuids.h>
#include "LogsUID.h"

#include <bautils.h> //parser
#include <featmgr.h>

#include <logs.rsg>  
#include <StringLoader.h>   // Series 60 localisation stringloader
#include <hlplch.h>

#include "CLogsSettingsView.h"
#include "CLogsSettingsControlContainer.h"
#include "CLogsEngine.h"
#include "MLogsSharedData.h"
#include "MLogsConfig.h"
#include "MLogsClearLog.h"
#include "CLogsAocObjFactory.h"
#include "MLogsCallStatus.h"
#include "LogsConsts.h"
#include "LogsConstants.hrh"        //KLogsPluginImplUID
#include "MLogsStateHolder.h"

#include "Logs.hrh"
#include "LogsIcons.hrh"
#include <logs.mbg>     // Logs icons

#include "LogsDebug.h"
#include "LogsTraces.h"

const TInt KMaxEventCount = 1000;
const TInt KZeroEventCount = 0;



// =========================== From CGSPluginInterface ========================

// ----------------------------------------------------------------------------
// CLogsSettingsView::NewPluginL
//
// From CGSPluginInterface
// ----------------------------------------------------------------------------
//
CGSPluginInterface* CLogsSettingsView::NewPluginL(
    TUid  /* aImplementationUid */, 
    TAny* /* aInitParams */  ) 
    {
    TRACE_ENTRY_POINT;
    CLogsSettingsView* self = new ( ELeave ) CLogsSettingsView();  
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();  // self
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::GetCaptionL
//
// From CGSPluginInterface. Text for GS's list of applicatons
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::GetCaptionL( TDes& aCaption ) const
    {
    TRACE_ENTRY_POINT;
    // Resource file is already opened in ConstructL.    
    HBufC* text = iCoeEnv->AllocReadResourceLC( R_SET_FOLDER_LOGS );
    aCaption.Copy( *text );    
    CleanupStack::PopAndDestroy( text ); 
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::PluginProviderCategory
//
// From CGSPluginInterface
// ----------------------------------------------------------------------------
//
int CLogsSettingsView::PluginProviderCategory(void) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return CGSPluginInterface::EGSPluginProviderOEM;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::CreateIconL
//
// From CGSPluginInterface
// ----------------------------------------------------------------------------
//
CGulIcon* CLogsSettingsView::CreateIconL( const TUid aIconType )
    {
    TRACE_ENTRY_POINT;
    TFileName iconFileWithPath;
    iconFileWithPath += KLogsIconFileDrive;
    iconFileWithPath += KDC_APP_BITMAP_DIR;          
    iconFileWithPath += KLogsIconFile;  

    CGulIcon* icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnPropSetAppsLog,
        iconFileWithPath,
        EMbmLogsQgn_prop_set_apps_log, 
        EMbmLogsQgn_prop_set_apps_log_mask );
        
     if( aIconType == KGSIconTypeLbxItem )
        {
        //nop currently
        }
    else if( aIconType == KGSIconTypeTab )
        {
        //nop
        }
    
    TRACE_EXIT_POINT;
    return icon;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::CustomOperationL
//
// From CGSPluginInterface
// ----------------------------------------------------------------------------
//
TAny* CLogsSettingsView::CustomOperationL( 
    TAny* /* aParam1 */,
    TAny* /* aParam2 */ )
    {
    TRACE_ENTRY_POINT;
    //Called only from Logs, so we can use this to swich off GS specific title
    iForGs = EFalse;
    TRACE_EXIT_POINT;
    return NULL;
    }
//=============================================================================


// ----------------------------------------------------------------------------
// CLogsSettingsView::CLogsSettingsView
// ----------------------------------------------------------------------------
//
CLogsSettingsView::CLogsSettingsView() :
    iState( EStateUndefined )            
    {
    TRACE_ENTRY_POINT;
    iConfig.iMaxEventAge = ELogsConfigAge30Days;
    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CLogsSettingsView::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::ConstructL()
    {
    TRACE_ENTRY_POINT;
    iForGs = ETrue;   
    iDialog = NULL; 
    iEikEnv = CEikonEnv::Static();
    iEngine = CLogsEngine::NewL();    
    iEngine->ConfigL()->SetObserver( this );
    iEngine->SharedDataL()->SetObserver( this );
    iCoeEnv->AddForegroundObserverL( *this );

    _LIT( KLogsPluginResourceFileName, "z:\\resource\\Apps\\Logs.RSC" );
    TFileName fileName( KLogsPluginResourceFileName );

    // Get language of resource file
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );       

    // Open resource file
    iResourceFileOffset=iEikEnv->AddResourceFileL( fileName );	
    BaseConstructL( R_LOGS_SETTINGS_VIEW );    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::~CLogsSettingsView
// ----------------------------------------------------------------------------
//
CLogsSettingsView::~CLogsSettingsView()
    {
    TRACE_ENTRY_POINT;
    
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }

	iEikEnv->DeleteResourceFile(iResourceFileOffset);
    delete iEngine;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::Id
// ----------------------------------------------------------------------------
//
TUid CLogsSettingsView::Id() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return TUid::Uid( KLogsPluginImplUID );//Impl UID for CGSPluginInterface
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::DoActivateL
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::DoActivateL(
    const TVwsViewId& aPrevViewId , 
    TUid  /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    TRACE_ENTRY_POINT;
    iPrevViewId = aPrevViewId;

    // Create the control container
    if( iContainer == NULL )
        {
        iContainer = CLogsSettingsControlContainer::NewL( this, ClientRect(), iForGs );
        iContainer->SetMopParent( this );        
        AppUi()->AddToViewStackL( *this, iContainer );  
        }
    
    iContainer->ListBox()->SetListBoxObserver( this );
        
    iEngine->ConfigL()->GetConfig( iConfig );        
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::DoDeactivate()
    {
    TRACE_ENTRY_POINT;
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::HandleGainingForeground
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::HandleGainingForeground()
    {
    TRACE_ENTRY_POINT;
    
    // There can be Settings view open from general settings and Logs 
    // at the same time so we need to re-read the config for possible 
    // changes.
    //
    TRAPD(err, iEngine->ConfigL()->GetConfig( iConfig ) );
    if (err)
    	{
    	iCoeEnv->HandleError(err);
    	}   
        	
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CLogsSettingsView::HandleLosingForeground
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::HandleLosingForeground()
    {
    TRACE_ENTRY_POINT;
    // Do nothing   
    TRACE_EXIT_POINT;
    }
      
// ----------------------------------------------------------------------------
// CLogsSettingsView::ProcessCommandL
//
// Called from FW when e.g. cba pressed
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::ProcessCommandL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;
    CAknView::ProcessCommandL( aCommand );
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::ProcessKeyEventL
// ----------------------------------------------------------------------------
//
TBool CLogsSettingsView::ProcessKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    TRACE_ENTRY_POINT;

    //Copied for base view    
    if( aType == EEventKey )
        {
        switch( aKeyEvent.iCode )
            {
            case EKeyApplication0:
                iCoeEnv->Flush( KDelayMicroSeconds );
                TRACE_EXIT_POINT;
                return EFalse;
            }
        }
    
    TRACE_EXIT_POINT;
    return EFalse;
    }
     
//
// ----------------------------------------------------------------------------
// CLogsSettingsView::ProcessPointerEventL
//
// Handler for pointer events, when the current focused item is tapped
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::ProcessPointerEventL( TInt /* aIndex */ )
	{
	TRACE_ENTRY_POINT;
	HandleCommandL( ELogsCmdChange );
	TRACE_EXIT_POINT;
	}
	
// ----------------------------------------------------------------------------
// CLogsSettingsView::FocusChangedL
//
// Informs subclass that focus is changed in listbox (item is tapped)
// ----------------------------------------------------------------------------
//
// void CLogsSettingsView::FocusChangedL()
//	{
//	}
	
// ----------------------------------------------------------------------------
// CLogsSettingsView::TabObserver
// ----------------------------------------------------------------------------
//
MAknTabObserver* CLogsSettingsView::TabObserver()  
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;  //
    }
    

// ----------------------------------------------------------------------------
// CLogsSettingsView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::HandleCommandL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;
    switch( aCommandId )
        {
        //
        // Menu commands
        //
        case ELogsCmdChange: // settings item selected from options menu
            {
            switch( iContainer->ListBox()->CurrentItemIndex() )
                {
                case ELogSettingsLogLength:
                    if( iState == EStateFinished ) 
                        {
                        CmdLogLengthL();     
                        }
                    
                    break;
                /************************************************
                "Show Call Duration" moved General Settings application, not in use any more in Logs 
                case ELogSettingsShowCallDurationPhone:
                    if( !CallIsActiveL() )
                        {
                        CmdShowCallDurationPhoneL();
                        }
                    else
                        {

                        CAknNoteDialog* noteDlg = new( ELeave ) CAknNoteDialog( CAknNoteDialog::ENoTone,
                                                       CAknNoteDialog::ELongTimeout );
                        noteDlg->ExecuteLD( R_LOGS_NOTE_DURING_CALL_OPTION_NOT_ALLOWED );
                        }
                    break;
                ************************************************/
                } // switch
                break;
            } // case
            
        case EEikCmdExit:       // flow-through
        case ELogsCmdMenuExit:  // flow-through
        case EAknSoftkeyExit:
            if (iForGs)
                { // Fix for error EAWG-6UGBGV: If opened from general settings we need to change 
                  // the command id to EEikCmdExit
                aCommandId = EEikCmdExit;     
                }
            AppUi()->HandleCommandL( aCommandId  );  
            break;        
        
        case EAknSoftkeyBack:
            //Return to previous view if it is from GS or Logs
            if( iPrevViewId.iAppUid == TUid( KUidGS ) || 
                iPrevViewId.iAppUid == TUid::Uid( KLogsAppUID3 )) 
                {
                AppUi()->ActivateLocalViewL( iPrevViewId.iViewUid );
                }
            else
                //Put Logs to background as unknown application is owner of previous view (e.g. Help)
                {
                AppUi()->HandleCommandL( ELogsCmdMenuExit );  
                }

            break;

        case EAknCmdHelp:
        
            if( iContainer )
                {
                TCoeHelpContext context;
                iContainer->GetHelpContext( context );
                CArrayFixFlat<TCoeHelpContext>* array = new(ELeave)CArrayFixFlat<TCoeHelpContext>(1);
                array->AppendL( context );
                HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), array );
                }
            break;

        default:
            AppUi()->HandleCommandL( aCommandId );  
            break;
        }
    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CLogsSettingsView::DynInitMenuPaneL
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::DynInitMenuPaneL(
    TInt /* aResourceId */,
    CEikMenuPane* aMenuPane )
    {
    TRACE_ENTRY_POINT;
    TInt position = -1;

    if( aMenuPane->MenuItemExists( EAknCmdHelp, position ) )
        {
        if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }
        }
    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CLogsSettingsView::HandleClientRectChange
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::HandleClientRectChange()
    {
    TRACE_ENTRY_POINT;
    
    if (iContainer)
        {
        iContainer->SetRect( ClientRect() );  
        }
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CLogsSettingsView::HandleListBoxEventL
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    TRACE_ENTRY_POINT;
    // Handle pointer events
    if( AknLayoutUtils::PenEnabled() )  
        {
            switch ( aEventType )
            {
            case EEventItemSingleClicked:
            case EEventEnterKeyPressed:
                HandleCommandL( ELogsCmdChange ); 
                break;
            default:
                break;
            }  
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::MakeSettingLineL
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::MakeSettingLineL(
    CDesCArrayFlat* aItems,
    TInt aResourceText )
    {
    TRACE_ENTRY_POINT;
    
    HBufC* text = iCoeEnv->AllocReadResourceLC( aResourceText );
    TPtr ptr = text->Des();
    AknTextUtils::LanguageSpecificNumberConversion( ptr );
    aItems->AppendL( ptr  );
    CleanupStack::PopAndDestroy();  // textFromResourceFile

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::CmdLogLengthL
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::CmdLogLengthL()
    {
    TRACE_ENTRY_POINT;
    // If dialog already exist, do nothing. Avoids for example
    // multiple dialogs opening from multiple quick taps. 
    if (iDialog == NULL)
        {   
        TLogConfig temp;
        
        // 4 is number of options in query
        iSettingItems = new( ELeave ) CDesCArrayFlat( 4 );
        
        MakeSettingLineL( iSettingItems, R_LOGS_SV_LOG_LENGTH_0_TEXT );
        MakeSettingLineL( iSettingItems, R_LOGS_SV_LOG_LENGTH_1_TEXT );
        MakeSettingLineL( iSettingItems, R_LOGS_SV_LOG_LENGTH_10_TEXT );
        MakeSettingLineL( iSettingItems, R_LOGS_SV_LOG_LENGTH_30_TEXT );

        TInt current( 0 );

        switch( iConfig.iMaxEventAge )
            {
            case ELogsConfigAgeNoLog:
                current = 0;
                break;

            case ELogsConfigAge1Days:
                current = 1;
                break;

            case ELogsConfigAge10Days:
                current = 2;
                break;

            case ELogsConfigAge30Days:
                current = 3;
                break;

            default:
                break;
            }

        iDialog = new ( ELeave ) CAknRadioButtonSettingPage( R_LOGS_SETTINGS_LOG_LENGTH_PAGE, 
                                                             current , iSettingItems );
                                        
        if ( iDialog->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
            {
            switch( current )
                {
                case 0:
                    temp.iMaxEventAge = ELogsConfigAgeNoLog;
                    temp.iMaxLogSize = KZeroEventCount;
                    break;

                case 1:
                    temp.iMaxEventAge = ELogsConfigAge1Days;
                    temp.iMaxLogSize = KMaxEventCount;
                    break;

                case 2:
                    temp.iMaxEventAge = ELogsConfigAge10Days;
                    temp.iMaxLogSize = KMaxEventCount;
                    break;

                case 3:
                    temp.iMaxEventAge = ELogsConfigAge30Days;
                    temp.iMaxLogSize = KMaxEventCount;
                    break;

                default:
                    break;
                }
        
            if( current == 0 )
                {
				// if No Log had been before, don't need pop No Log Confirm Quary and
				// clear log again.
                if( iConfig.iMaxEventAge != ELogsConfigAgeNoLog )
                    {
                    if( NoLogQueryL() )
                        {
                        iConfig.iMaxEventAge = temp.iMaxEventAge;
                        iConfig.iMaxLogSize = temp.iMaxLogSize;
                        iEngine->ConfigL()->SetConfig( iConfig );
                        iEngine->SharedDataL()->SetLoggingEnabled( EFalse );
                        iEngine->ClearLogsL()->ClearModelL( ELogsMainModel );
                        iState = EStateActive;
                        }
                    }
                }
            else
                {
                iConfig.iMaxEventAge = temp.iMaxEventAge;
                iConfig.iMaxLogSize = temp.iMaxLogSize;
                iEngine->ConfigL()->SetConfig( iConfig );
                iEngine->SharedDataL()->SetLoggingEnabled( ETrue );
                iState = EStateActive;
                }

            iContainer->UpdateListBoxContentL();
            }
        delete iSettingItems;    
        iSettingItems = NULL;
        iDialog = NULL; //ExecuteLD handles destruction
        }
    TRACE_EXIT_POINT;
    }


/************************************************
"Show Call Duration" moved to General Settings application, not in use any more in Logs 

void CLogsSettingsView::CmdShowCallDurationPhoneL()
    {
    // 2 is number of options in query
    CDesCArrayFlat* items = new( ELeave ) CDesCArrayFlat( 2 );
    CleanupStack::PushL( items );

    TBool currentItem( iEngine->SharedDataL()->ShowCallDurationPhone() );

    // Timer OFF
    MakeSettingLineL( items, R_QTN_LOGS_LDV3_TIMER_OFF_TEXT );

    // Timer ON
    MakeSettingLineL( items, R_QTN_LOGS_LDV3_TIMER_ON_TEXT );

    CAknRadioButtonSettingPage* dlg = new ( ELeave )
        CAknRadioButtonSettingPage( R_LOGS_SETTINGS_CALL_DURATION_PAGE,
        ( TInt & )currentItem, items );

    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        MakeCallDurationChangeL( currentItem );
        }
    CleanupStack::PopAndDestroy(); // items
    }

void CLogsSettingsView::MakeCallDurationOppositeL()
    {
    MakeCallDurationChangeL( ! iEngine->SharedDataL()->ShowCallDurationPhone() );
    }
    
void CLogsSettingsView::MakeCallDurationChangeL( TBool aChange )
    {
    iEngine->SharedDataL()->SetShowCallDurationPhone( aChange );
    //iContainer->CreateListBoxL();
    iContainer->UpdateListBoxContentL();

    }
************************************************/
    

//  Sets logging enabled/disabled
// ----------------------------------------------------------------------------
// CLogsSettingsView::NoLogQueryL
// ----------------------------------------------------------------------------
//
TBool CLogsSettingsView::NoLogQueryL()
    {
    TRACE_ENTRY_POINT;
    TBool ret;
    iNoLogQueryDlg = CAknQueryDialog::NewL();

    ret = iNoLogQueryDlg->ExecuteLD( R_LL_NO_LOG_CONFIRMATION_QUERY );
    iNoLogQueryDlg = NULL;
   
    TRACE_EXIT_POINT;
    return ret;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::ShowNoteL
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::ShowNoteL(TInt aResourceID)
    {
    TRACE_ENTRY_POINT;
    CAknNoteDialog* noteDlg = new ( ELeave ) 
                CAknNoteDialog(
                    CAknNoteDialog::ENoTone, 
                    CAknNoteDialog::ELongTimeout );
    noteDlg->ExecuteLD( aResourceID );
    TRACE_EXIT_POINT;
    }


/************************************************
"Show Call Duration" moved General Settings application, not in use any more in Logs 

TBool CLogsSettingsView::CallIsActiveL()
    {
    MLogsCallStatus* callStatus = CLogsAocObjFactory::CallStatusLC();
    TBool activeCall( EFalse );
    User::LeaveIfError( callStatus->CallIsActive( activeCall ) );
    CleanupStack::PopAndDestroy(); // call status
    return activeCall;
    }
************************************************/


// ----------------------------------------------------------------------------
// CLogsSettingsView::StateChangedL
// ----------------------------------------------------------------------------
//
void CLogsSettingsView::StateChangedL( MLogsStateHolder* aHolder )
    {
    TRACE_ENTRY_POINT;
	//  void panic as the sharedata may pass the NULL to observer
    if (!aHolder )
        {
        return;
        }
    iState = aHolder->State();
    
    TVwsViewId activeViewId;
    TInt ActiveViewExists = AppUi()->GetActiveViewId(activeViewId);
       
    if (ActiveViewExists == KErrNone && 
        activeViewId == ViewId() && IsForeground() )
    	{ 
    	//-------------------------------------------------------------	
    	if( iContainer && iState == EStateFinished )
    	        {
    	        iContainer->UpdateListBoxContentL();
    	        }
    	   
	    // If log lenght has changed and there is a dialog open, update dialog to change 
	    // focus and selected item as there can be Settings view open from general settings 
	    // and Logs at the same time.
	    if  (!iNoLogQueryDlg && (iLastMaxEventAge != iConfig.iMaxEventAge) )
	        {         
	         if (iDialog && iSettingItems)
	            {    
	            TInt current( 0 );
	                switch( iConfig.iMaxEventAge )
	                {
	                case ELogsConfigAgeNoLog:
	                    current = 0;
	                    break;

	                case ELogsConfigAge1Days:
	                    current = 1;
	                    break;

	                case ELogsConfigAge10Days:
	                    current = 2;
	                    break;

	                case ELogsConfigAge30Days:
	                    current = 3;
	                    break;

	                default:
	                    break;
	                }
	            
	            // Update setting item selection and listbox focus
	            iDialog->SetItemArrayAndSelectionL(iSettingItems, current); 
	            iDialog->ListBoxControl()->SetCurrentItemIndex(current);
	            iDialog->ListBoxControl()->FocusChanged( EDrawNow );  
	            }
	        } 	
    	}
		// for Error EMHU-7PLCXX, close the No Log Quary dialog to avoid the Redundant 
		// when GS log setting and logs setting open at the same time
        else
        {
        if(iState == EStateLogEnableChanged && iNoLogQueryDlg)
            {
            if ( !iEngine->SharedDataL()->IsLoggingEnabled() )
                {
                    delete iNoLogQueryDlg;
					iNoLogQueryDlg = NULL;
                }
            }
    	} //-------------------------------------------------------------
   
    // Finally store the current config for future reference value.
    iLastMaxEventAge = iConfig.iMaxEventAge;     
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsView::LogAge
// ----------------------------------------------------------------------------
//
TLogAge CLogsSettingsView::LogAge() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iConfig.iMaxEventAge;
    }
    
// ----------------------------------------------------------------------------
// CLogsSettingsView::Engine
// ----------------------------------------------------------------------------
//
CLogsEngine* CLogsSettingsView::Engine()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iEngine;
    }



//  End of File
