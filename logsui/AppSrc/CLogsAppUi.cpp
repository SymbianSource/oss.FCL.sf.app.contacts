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
*     Logs application UI class
*
*/
 

// INCLUDE FILES
#include <vwsdef.h>
#include <AknQueryDialog.h>
#include <aknnotedialog.h>
#include <akntabgrp.h>
#include <aknnavide.h>
#include <featmgr.h>
#include <sendui.h>
#include <apgtask.h>
#include <apgcli.h>
#include <gsplugininterface.h>  
#include <hlplch.h>
#include <LogsUiCmdStarterConsts.h>
#include <logs.rsg>

//+ __GFXTRANS__
// Transition effects
#include <gfxtranseffect/gfxtranseffect.h> 	
#include <akntranseffect.h> 				
#include "LogsUID.h"
//-- __GFXTRANS__

#include "CLogsAppUi.h"
#include "CLogsEngine.h"
#include "CLogsAppListView.h"
#include "CLogsSubAppListView.h"
#include "CLogsEventListView.h"
#include "CLogsRecentListView.h"
#include "CLogsDetailView.h"
#include "CGprsCtView.h"
#include "MLogsSharedData.h"
#include "CCtView.h"
#include "MLogsModel.h"
#include "LogsConstants.hrh"
#include "../LogsPlugin/CLogsSettingsView.h"

#include "LogsDebug.h"
#include "LogsTraces.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CLogsAppUi::~CLogsAppUi
// ----------------------------------------------------------------------------
//
CLogsAppUi::~CLogsAppUi()
    {   
    /**********************************************************************
    Not in use anymore, Phonebook icons replaced by own icons 
    if(iIconIdArray)
        {
        iIconIdArray->Reset();
        delete iIconIdArray;
        }
       
    delete iIconInfoContainer;
    **********************************************************************/    
	delete iSendUi;    
	delete iServiceHandler;	
	
    FeatureManager::UnInitializeLib();
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::CLogsAppUi
// ----------------------------------------------------------------------------
//
CLogsAppUi::CLogsAppUi( CLogsEngine* aEngine ) : 
    iEngine( aEngine ), iResetToMain(EFalse)
    {
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::Engine
// ----------------------------------------------------------------------------
//
CLogsEngine* CLogsAppUi::Engine()
    { 
    return iEngine; 
    }
    
// ----------------------------------------------------------------------------
// CLogsAppUi::SendUiL
//
// Provide singleton instance of SendUI 
// ownership remains in ClogsAppUi
// ----------------------------------------------------------------------------
//
CSendUi* CLogsAppUi::SendUiL()
    {
    TRACE_ENTRY_POINT;
    
	if(!iSendUi)
	    {
		iSendUi=CSendUi::NewL();
		LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
            ( "CLogsAppUi::SendUiL - iSendUi:0x%x"), iSendUi ); 
	    }
	    
	TRACE_EXIT_POINT;
	return iSendUi;
    }
    
// ----------------------------------------------------------------------------
// CLogsAppUi::CallUiL
//
// Provide singleton instance of iServiceHandler for Phone calls etc,
// ownership remains in ClogsAppUi. Attach AIW CallUI, PoC menus to Logs. 
// ----------------------------------------------------------------------------
//
CAiwServiceHandler* CLogsAppUi::CallUiL( TBool aReset )
    {
    TRACE_ENTRY_POINT;
    
    if( aReset )
        {
        delete iServiceHandler;
        iServiceHandler = NULL;
        }
    
	if(!iServiceHandler)
	    {
		iServiceHandler = CAiwServiceHandler::NewL();
        LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
            ( "CLogsAppUi::CallUiL - iServiceHandler:0x%x"), iServiceHandler ); 

        iServiceHandler->AttachL( R_LOGS_CALLUI_INTEREST );
        iServiceHandler->AttachL( R_LOGS_POC_INTEREST ); 

        iServiceHandler->AttachL( R_LOGS_SAVETOPBK_INTEREST );
        
        iServiceHandler->AttachMenuL( R_COMMON_SAVETOPBK_SUBMENU, R_LOGS_SAVETOPBK_INTEREST );                        

        iServiceHandler->AttachMenuL( R_COMMON_CALLUI_SUBMENU, R_LOGS_CALLUI_INTEREST );
        iServiceHandler->AttachMenuL( R_COMMON_POC_SUBMENU,    R_LOGS_POC_INTEREST );
	    }
	    
    TRACE_EXIT_POINT;   
	return iServiceHandler;
    }
    
// ----------------------------------------------------------------------------
// CLogsAppUi::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsAppUi::ConstructL()  //Called from framework (CEikDocument)
    {
    TRACE_ENTRY_POINT;
       
    iSendUi = NULL;
    iServiceHandler = NULL;    

    BaseConstructL(
        EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );

    SetProvideOnlyRecentViews( EFalse );   //By default we provide all views.
    SetLogsOpenedWithSendKey( EFalse );  
    
/**********************************************************************    
    Not in use anymore, Phonebook icons replaced by own icons 
    //Read Phonebook iconIds for each type of Phonebook field.    
    iIconInfoContainer = CPbkIconInfoContainer::NewL(); //Provides instance initialised with Phonebook's
         												//default icon info (from PbkView.rsc).    
    iIconIdArray = new(ELeave) CArrayFixFlat<TPbkIconId>( 1 );	
**********************************************************************/    

    SetExecutionMode( ELogsInForeground );
    
/*******************************************************************************
 FIXME: Toolbar is currently always on - keeping the toolbar visibility handling 
        sources in comments for now - remove later.    
    //This cannot be done in DoConstructDelayedL, happens too late
    if( AknLayoutUtils::PenEnabled() )
        {
    
        iShowToolbar = iEngine->SharedDataL()->ToolbarVisibility() == 1;  
             
        }  
*******************************************************************************/ 

    //Construct CIdle object call back to finalise time consuming construction
    iConstructDelayed = CIdle::NewL( CActive::EPriorityIdle );
    iConstructDelayed->Start( TCallBack( ConstructDelayedCallbackL, this) );            
     
    TRACE_EXIT_POINT;           
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::ConstructDelayedCallbackL
//
// Called from CIdle once to finish time consuming construction operations
// ----------------------------------------------------------------------------
//
TInt CLogsAppUi::ConstructDelayedCallbackL( TAny* aContainer )
    {
    CLogsAppUi* container = static_cast<CLogsAppUi*>( aContainer );
    container->DoConstructDelayedL();    
    return 0;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::DoConstructDelayedL
//
// Perform time consuming construction operations once
// ----------------------------------------------------------------------------
//
void CLogsAppUi::DoConstructDelayedL()
    {
    TRACE_ENTRY_POINT;
    
    if( iConstructDelayed ) 
        {
        if( iConstructDelayed->IsActive() ) //Sometimes cancel is expensive, so
            {                               //check as should be no need for Cancel()
            iConstructDelayed->Cancel();
            }
        
        delete iConstructDelayed;
        iConstructDelayed = NULL;
        }

    //Initial Logs view is created on-need in ProcessCommandParametersL. However, 
    //AppShell doesn't currently provide option to send cmdline parameter, so we need 
    //proactively be prepared to provide AppListView if activated later from AppShell.
    //This is needed if we lose foreground and later gain it from AppShell to AppListView. 
    //Because of EMSH-6JDFBV it's also better not try do the below in HandleLosingForeground()
    //so we need to accept a minor performance penalty of executing the below already here.
    if( ! View( TUid::Uid( ELogAppListViewId ) ) && !FeatureManager::FeatureSupported( KFeatureIdSimpleLogs ) )  
        {
        CAknView* logsView = CLogsAppListView::NewL();    
        CleanupStack::PushL(logsView);
        AddViewL( logsView );       //takes ownership
        CleanupStack::Pop(logsView);   
        }
    else if ( ! View( TUid::Uid( ELogSubAppListViewId ) ) && FeatureManager::FeatureSupported( KFeatureIdSimpleLogs ) )
        {
        // When simple mode enabled, provide SubAppListView here.
        CAknView* logsView = CLogsSubAppListView::NewL();    
        CleanupStack::PushL(logsView);
        AddViewL( logsView );       //takes ownership
        CleanupStack::Pop(logsView);
        }
        
    TRACE_EXIT_POINT;
    }


/**********************************************************************
Not in use anymore, Phonebook icons replaced by own icons 

CArrayFix<TPbkIconId>* CLogsAppUi::IconIdArray()
    {
    return iIconIdArray;
    }

CPbkIconInfoContainer* CLogsAppUi::IconInfoContainer()
    {
    return iIconInfoContainer;
    }
**********************************************************************/    

// ----------------------------------------------------------------------------
// CLogsAppUi::CmdExit
// ----------------------------------------------------------------------------
//
void CLogsAppUi::CmdExit()
    {
    
/*******************************************************************************
 FIXME: Toolbar is currently always on - keeping the toolbar visibility handling 
        sources in comments for now - remove later.   
    //Proceed to exit even if leave would happen
    TInt err;
    TRAP( err, iEngine->SharedDataL()->SetToolbarVisibility( iShowToolbar ? 1 : 0 ) ); //0=no toolbar  
*******************************************************************************/    

    Exit();
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::CmdBackL
// ----------------------------------------------------------------------------
//
void CLogsAppUi::CmdBackL()
    {
    switch( iCurrentViewId.iUid )
        {
        case ELogDetailViewId:
            ActivateLogsViewL( ELogEventListViewId );
            break;

        case ELogSettingsViewId:
            ActivateLogsViewL( static_cast<TLogsViewIds>( iPreviousViewId.iUid ) );
            break;

        case EStmMissedListViewId: // fall through
        case EStmReceivedListViewId:
        case EStmDialledListViewId:
            ActivateLogsViewL( ELogSubAppListViewId );
            break;

        case ELogSubAppListViewId: // fall through
        case ECtViewId:
        case EGprsCounterViewId:
            ActivateLogsViewL( ELogAppListViewId );
            break;

        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::ActivateLogsViewL
// ----------------------------------------------------------------------------
//
void CLogsAppUi::ActivateLogsViewL( TLogsViewIds aView )
    {
    ActivateLogsViewL( aView, ELogsViewActivationNoMessage, KNullDesC8()); 
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::ActivateLogsViewL
//
// Activate requested view. The view is also created if it does not already exist.
// ----------------------------------------------------------------------------
//
void CLogsAppUi::ActivateLogsViewL( 
    TLogsViewIds aView,
    TLogsViewActivationMessages aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    TRACE_ENTRY_POINT;
    
    TBool rc( ETrue );
    CAknView* logsView = View( TUid::Uid( aView ));

    //If the view does not already exist, let's try to create it first
    if( !logsView ) 
        {
        switch( aView )  //not yet exists, create the view
            {
            case ELogAppListViewId:
                logsView = CLogsAppListView::NewL();
                CleanupStack::PushL(logsView);
                break;

            case ELogSubAppListViewId:
                logsView = CLogsSubAppListView::NewL();
                CleanupStack::PushL(logsView);
                break;

            case ELogEventListViewId:
                logsView = CLogsEventListView::NewL();
                CleanupStack::PushL(logsView);
                break;

            case EStmMissedListViewId:
                logsView = CLogsRecentListView::NewL( ELogsMissedModel );
                CleanupStack::PushL(logsView);
                break;

            case EStmReceivedListViewId:
                logsView = CLogsRecentListView::NewL( ELogsReceivedModel );
                CleanupStack::PushL(logsView);
                break;

            case EStmDialledListViewId:
                logsView = CLogsRecentListView::NewL( ELogsDialledModel );
                CleanupStack::PushL(logsView);
                break;

            case ELogDetailViewId:
                logsView = CLogsDetailView::NewL();
                CleanupStack::PushL(logsView);
                break; 

            case EGprsCounterViewId:
                logsView = CGprsCtView::NewL();
                CleanupStack::PushL(logsView);
                break;

            case ELogSettingsViewId:  //viewId is implementation UID too in this case
                //Settings is a Ecom plugin (implements CGSPluginInterface). CGSPluginInterface.cpp
                //takes care of needed ecom clear up issues.
                //Note! REComSession::FinalClose() is called in CLogsDocument destructor. Calling
                //in CLogsAppUi dtor is too early as view framework destroys the view *after* appui has already 
                //been destroyed.
                {
                CGSPluginInterface* gs = CGSPluginInterface::NewL( TUid::Uid( aView ), NULL); 
                logsView = static_cast<CGSPluginInterface*>(gs);
                CleanupStack::PushL(logsView);                
                gs->CustomOperationL( NULL, NULL );
                }
                break;
                                   
            case ECtViewId:
                logsView = CCtView::NewL();
                CleanupStack::PushL(logsView);
                break;

            default:
                break;
            }

        if( logsView )
            {                       //Registers the view with the view server            
            AddViewL( logsView );   // takes ownership of logsView 
            CleanupStack::Pop(logsView);   
            }
        else
            {
            rc = EFalse;
            }
        }

    //Activate the requested view
    if( rc )
        {    
        ActivateLocalViewL( TUid::Uid( aView ), 
                            TUid::Uid( aCustomMessageId ), 
                            aCustomMessage );
         
        // If activating settings view, call SetCurrentViewId here, cause
        // settings view can't access CLogsAppUi's functions
        if (aView == ELogSettingsViewId)
        	{
        	SetCurrentViewId(TUid::Uid(aView));
        	}
        }
        
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::SetPreviousViewId
// ----------------------------------------------------------------------------
//
void CLogsAppUi::SetPreviousViewId( TUid aView )
    {
    iPreviousViewId = aView;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::SetCurrentViewId
// ----------------------------------------------------------------------------
//
void CLogsAppUi::SetCurrentViewId( TUid aView )
    {
    iCurrentViewId = aView;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::CmdOkL
// ----------------------------------------------------------------------------
//
void CLogsAppUi::CmdOkL( TInt aIndex )
    {    
    if( TUid::Uid( ELogAppListViewId ) == iCurrentViewId )
        {        
        switch( aIndex )
            {
            case EStmStmSubAppId:
                ActivateLogsViewL( ELogSubAppListViewId );
                break;

            case EStmTimersSubAppId:
                ActivateLogsViewL( ECtViewId );
                break;

            case EGprsCountSubAppId:
                ActivateLogsViewL( EGprsCounterViewId );
                break;

            default:
                break;
            }
        }
    
    else if( TUid::Uid( ELogSubAppListViewId ) == iCurrentViewId )
        {
        switch( aIndex )
            {
            case EStmMissedId:
                ActivateLogsViewL( EStmMissedListViewId );
                break;

            case EStmReceivedId:
                ActivateLogsViewL( EStmReceivedListViewId );
                break;

            case EStmDialledId:
                ActivateLogsViewL( EStmDialledListViewId );
                break;

            default:
                break;
            }
        }
    
    else if( TUid::Uid( ELogEventListViewId ) == iCurrentViewId )
        {
        if( iEngine->Model( ELogsMainModel )->Count() )
            {
            ActivateLogsViewL( ELogDetailViewId );
            }
        }
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::HandleCommandL
// ----------------------------------------------------------------------------
//
void CLogsAppUi::HandleCommandL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;
    switch( aCommand )
        {
        case EEikCmdExit:   //We receive this when e.g. close app is selected in fast swapping list.
            {
            LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
            		( "CLogsAppUi::HandleCommandL(0x%x) - EEikCmdExit: real exit"), this );
            CmdExit(); //Do real exit (close Logs).
            break;     
            }
            
        case EAknCmdHideInBackground:
            {
            TBool hideInBg( ETrue );
#ifdef _DEBUG  //Debug mode, logs exits completely
            hideInBg = EFalse;  
#endif
            if(hideInBg)
                {
                LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
                        ( "CLogsAppUi::HandleCommandL(0x%x) - EAknCmdHideInBackground: fake exit"), this );
                // Fake exit - leave app to memory but hide from user
                 HideLogsToBackgroundL();
                }
            else
                { 
                LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
                        ( "CLogsAppUi::HandleCommandL(0x%x) - EAknCmdHideInBackground: exit completely"), this );
                //logs exits completely
                CmdExit();
                }
            break;
            }
        
        case EAknSoftkeyExit:
        case ELogsCmdMenuExit:
            {
            if (ExitHidesInBackground())
            	{
            	LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
            			( "CLogsAppUi::HandleCommandL(0x%x) - EAknSoftkeyExit: fake exit"), this ); 
            	// Fake exit - leave app to memory but hide from user
            	HideLogsToBackgroundL();
            	}
            else
            	{
            	LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
            			( "CLogsAppUi::HandleCommandL(0x%x) - EAknSoftkeyExit: real exit"), this ); 
            	//Do real exit, close Logs.
            	CmdExit(); 
            	}
            break;
            }       

        case ELogsCmdMenuSettings:
            ActivateLogsViewL( ELogSettingsViewId );
            break;

        case ELogsCmdFailedDueActiveCall:
            {
            CAknNoteDialog* noteDlg = new( ELeave ) CAknNoteDialog( CAknNoteDialog::ENoTone,
                            CAknNoteDialog::ELongTimeout );
            noteDlg->ExecuteLD( R_LOGS_NOTE_DURING_CALL_OPTION_NOT_ALLOWED );
            break;
            }

        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), AppHelpContextL());
            break;
            }

        default:
            iExitOrEndPressed = ENone;      //Reset also here 
            break;          
        }
        
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::HideLogsToBackgroundL
// Leave the application in memory
// ----------------------------------------------------------------------------
//
void CLogsAppUi::HideLogsToBackgroundL()
    {  
    TRACE_ENTRY_POINT;
    
    //+ __GFXTRANS__
       
    // Always use exit effect even if this is actually application switch
    // Note: Not allowed to call GfxTransEffect::EndFullScreen() as AVKON takes care of that when
    // EApplicationExit context is used!  
    // Set effect begin point
       			
   	iExitOrEndPressed = EExitPressed; 	//Reset to obey EEikCmdExit to do real exit (e.g. in case low mem, backup etc)                           
   	SetExecutionMode( ELogsInBackground_ExitOrEndPressed );

   	//Clear info whether we were called externally (Provide complete Logs to user next time)
   	SetProvideOnlyRecentViews( EFalse ); 
   	                
   	// In case foreground is later gained without commandline parameters, switch to 
   	// Logs main view instead of the Dialled calls view.   
    SetResetViewOnFocusRegain(ETrue);
    
    //Reset the ALS view selection for counters view, so that next time when Logs is opened all lines 
   	//are again shown as default
   	if ( View( TUid::Uid( ECtViewId ))) //View() returns null if view doesn't exist
   		{
   	    CCtView* view = static_cast<CCtView*>( View( TUid::Uid( ECtViewId )));
   	    view->SetAlsLine( EAllLines );
   	    }
    
    // Delete contact link checker when hiding log to background
    Engine()->DeleteCntLinkChecker();
    
   	// With the ELogsViewActivationBackground message, the ActivateLogsViewL will send
   	// a view activation message and we can go to background before it completes.
   	// We leave dialled calls view open as that is the most important use case for Logs
   	SetCustomControl(1);
   	ActivateLogsViewL(EStmDialledListViewId, ELogsViewActivationBackground, KNullDesC8());
   	      
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::HandleWsEventL
// ----------------------------------------------------------------------------
//
void CLogsAppUi::HandleWsEventL(
    const TWsEvent& aEvent,
    CCoeControl* aDestination )
    {
    TRACE_ENTRY_POINT;
    switch (aEvent.Type())
        {
        case EEventFocusGained:
            {
            // If Logs is on background because Exit has been pressed, and we have not received
            // a commandline message, Logs main view is to be activated when regaining foreground.
            //
            // This handling is needed due to the performance optimization of dialled calls used case
            // where we leave dialled calls view on background after user selecting Exit. 
            // This handling prevents errors like MMAN-6VADLV or MMHI-6V7ABV.
            // 
            if (ExecutionMode() == ELogsInBackground_ExitOrEndPressed && ResetViewOnFocusRegain())
                {
                SetResetViewOnFocusRegain(EFalse); // prevent further view switches
                if ( FeatureManager::FeatureSupported( KFeatureIdSimpleLogs ) )
                    {
                    ActivateLogsViewL( ELogSubAppListViewId );
                    }
                else
                    {
                    ActivateLogsViewL( ELogAppListViewId );
                    }    
                }
           
            CAknAppUi::HandleWsEventL( aEvent, aDestination );                    
            break;
            }
            
        case KAknUidValueEndKeyCloseEvent:  //0x101F87F0  (Avkon.hrh)

            //We have received KAknUidValueEndKeyCloseEvent. Let it go to FW (so we dont catch it here). 
            //Next we can wait EEikCmdExit to be received from FW to our HandleCommandL.
            iExitOrEndPressed = EEndPressed;  
                   
          
            /*******************************************************************************
             FIXME: Toolbar is currently always on - keeping the toolbar visibility handling 
                    sources in comments for now - remove later.
                    
            //IsShown() happens too late in CmdExit() when End key is pressed as Toolbar
            //has already hidden itself. So get status here (i.e cannot be done when EEikCmdExit 
            //is received e.g because of backup)
            if( CurrentPopupToolbar() ) 
                {
                iShowToolbar = CurrentPopupToolbar()->IsShown(); //iToolbarRef->IsShown();     
                }
            ********************************************************************************/                 

        default:
            
            // All other events are forwarded normally to base class
            CAknAppUi::HandleWsEventL( aEvent, aDestination ); 
            break;
        }
        
    TRACE_EXIT_POINT;    
    }


// ----------------------------------------------------------------------------
// CLogsAppUi::ResetToLogsMainView
// ----------------------------------------------------------------------------
//
TBool CLogsAppUi::ResetViewOnFocusRegain() const
    {
    return iResetToMain;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::SetResetToLogsMainView
// ----------------------------------------------------------------------------
//		
void CLogsAppUi::SetResetViewOnFocusRegain(TBool aResetToMain)
    {
    iResetToMain = aResetToMain;
    }
		
		
// ----------------------------------------------------------------------------
// CLogsAppUi::ActiveViewId
// ----------------------------------------------------------------------------
//
TLogsViewIds CLogsAppUi::ActiveViewId()
    {
    return TLogsViewIds( iCurrentViewId.iUid );
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::PreviousAppUid
// ----------------------------------------------------------------------------
//
TUid CLogsAppUi::PreviousAppUid() const
    {
    return iPreviousAppUid;
    }


// ----------------------------------------------------------------------------
// CLogsAppUi::SetPreviousAppUid
// ----------------------------------------------------------------------------
//
void CLogsAppUi::SetPreviousAppUid( TUid aUid )
    {
    iPreviousAppUid = aUid;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::SetProvideOnlyRecentViews
// ----------------------------------------------------------------------------
//
void CLogsAppUi::SetProvideOnlyRecentViews( TBool aProvideOnlyRecentViews )
    {
    iProvideOnlyRecentViews = aProvideOnlyRecentViews;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::ProvideOnlyRecentViews
// ----------------------------------------------------------------------------
//
TBool CLogsAppUi::ProvideOnlyRecentViews() const
    {
    return iProvideOnlyRecentViews;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::TabChangedL
//
// Activates the Logs view that corresponds to tab id of activated tab in tab group in navi pane
// ----------------------------------------------------------------------------
//
void CLogsAppUi::TabChangedL(TInt aIndex)
    {
#if defined(_DEBUG)
    _LIT( KPanicMsg, "CLogsAppUi::TabChangedL");
    CEikStatusPane* statusPane = NULL;
    statusPane = iEikonEnv->AppUiFactory()->StatusPane();
#endif  // _DEBUG

    __ASSERT_DEBUG( statusPane, User::Panic(KPanicMsg, KErrCorrupt) );

    CAknNavigationControlContainer* naviCtrlContainer = NULL;
    naviCtrlContainer = static_cast<CAknNavigationControlContainer*>( StatusPane()->
                    ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    __ASSERT_DEBUG( naviCtrlContainer, User::Panic(KPanicMsg, KErrCorrupt) );

    CAknNavigationDecorator* decorator = naviCtrlContainer->Top();
    __ASSERT_DEBUG( decorator && decorator->ControlType() == CAknNavigationDecorator::ETabGroup,
                    User::Panic(KPanicMsg, KErrCorrupt));

    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>(decorator->DecoratedControl());
    __ASSERT_DEBUG( tabGroup && aIndex >= 0 && aIndex < tabGroup->TabCount(),
                    User::Panic(KPanicMsg, KErrCorrupt));

    //TabIds in resource file are same as viewIds, so we can use them directly to activate a view
    const TInt viewId(tabGroup->TabIdFromIndex(aIndex));
    ActivateLogsViewL(static_cast<TLogsViewIds>(viewId));
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::EventListViewCurrent
// ----------------------------------------------------------------------------
//
TInt CLogsAppUi::EventListViewCurrent() const
    {
    CAknView* view = View( TUid::Uid( ELogEventListViewId ) );

    if( view )
        {
        return ( static_cast<CLogsEventListView*>( view ) )->EventListCurrent();
        }
    else
        {
        return KErrNotFound;
        }
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::SetEventListViewCurrent
// ----------------------------------------------------------------------------
//
void CLogsAppUi::SetEventListViewCurrent( TInt aCurrent )
    {
    CAknView* view = View( TUid::Uid( ELogEventListViewId ) );

    if( view )
        {
        ( static_cast<CLogsEventListView*>( view ) )->
                                SetEventListCurrent( aCurrent );
        }
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::ExecutionMode
// ----------------------------------------------------------------------------
//
TInt CLogsAppUi::ExecutionMode() const
    {
    return iExecutionMode;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::SetExecutionMode
// ----------------------------------------------------------------------------
//
void CLogsAppUi::SetExecutionMode( TInt aMode )
    {
    if (aMode == ELogsInForeground )
        {
        // Make sure that the view gets foreground when view activated.
        SetCustomControl(0);
        }
    iExecutionMode = aMode;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::LogsOpenedWithSendKey
// ----------------------------------------------------------------------------
//    
TBool CLogsAppUi::LogsOpenedWithSendKey()
    {
    return iLogsOpenedWithSendKey;
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::SetLogsOpenedWithSendKey
// ----------------------------------------------------------------------------
//  
void CLogsAppUi::SetLogsOpenedWithSendKey(TBool aLogsOpenedWithSendKey)
    {
    iLogsOpenedWithSendKey = aLogsOpenedWithSendKey;
    }    

/*******************************************************************************
 FIXME: Toolbar is currently always on - keeping the toolbar visibility handling 
        sources in comments for now - remove later.
        
// ----------------------------------------------------------------------------
// CLogsAppUi::ToolbarVisibility()
// ----------------------------------------------------------------------------
//
TInt CLogsAppUi::ToolbarVisibility() const
    {
    return iShowToolbar; 
    }
 
// ----------------------------------------------------------------------------
// CLogsAppUi::RecordToolbarVisibility()
//
// Record current toolbar visibility 
// ----------------------------------------------------------------------------
//
void CLogsAppUi::RecordToolbarVisibility() 
    { 
    CAknView* view = View( iCurrentViewId  );

    if( view && view->IsForeground())
        {
        switch( iCurrentViewId.iUid )
            {
            case ELogEventListViewId:
            case EStmMissedListViewId:
            case EStmReceivedListViewId:
            case EStmDialledListViewId:
            case ELogDetailViewId:
                if( CurrentPopupToolbar() ) 
                    {
                    iShowToolbar = CurrentPopupToolbar()->IsShown(); 
                    }
            }
        }      
    }
 *******************************************************************************/
   
// ----------------------------------------------------------------------------
// CLogsAppUi::HandleControlEventL
//
// from MCoeControlObserver        
// ----------------------------------------------------------------------------
//
void CLogsAppUi::HandleControlEventL( 
    CCoeControl* /* aControl */,   
    TCoeEvent /* aEventType */ ) 
    {
    }

// --------------------------------------------------------------------------
// CLogsAppUi::DynInitToolbarL
// --------------------------------------------------------------------------
//
void CLogsAppUi::DynInitToolbarL( 
    TInt /* aResourceId */, 
    CAknToolbar* /* aToolbar */ )
    {
    }

// --------------------------------------------------------------------------
// CLogsAppUi::OfferToolbarEventL
// --------------------------------------------------------------------------
//
void CLogsAppUi::OfferToolbarEventL( TInt aCommand )
    {
    ProcessCommandL( aCommand );
    }    

// ----------------------------------------------------------------------------
// CLogsAppUi::HandleMessageL
//
// Handle message sent by Logs client. Called by framework, when LogsUiCmdStarter
// request invocation of a specific view.
// ----------------------------------------------------------------------------
//
MCoeMessageObserver::TMessageResponse CLogsAppUi::HandleMessageL(
    TUint32 aClientHandleOfTargetWindowGroup,
    TUid aMessageUid,
    const TDesC8& aMessageParameters )
    {
    // If aTail is set, other app starts Logs
    if (aMessageParameters.Length())
	    {
		TFileName dummy;
  		TApaCommand cmd=EApaCommandRun;
  		ProcessCommandParametersL(cmd,dummy,aMessageParameters);
  		return EMessageHandled;
	    }

    return CAknViewAppUi::HandleMessageL(
        aClientHandleOfTargetWindowGroup,
        aMessageUid,
        aMessageParameters);
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::ProcessCommandParametersL
//
// Called by framework when external application requests startup of Logs and invocation 
// of a specific view. Also called by Logs when Logs has received a message requesting 
// invocation of a specific view. Orginators of these messages are:
// - LogsUiCmdStarter::CmdStartL / LogsUiCmdStarter::CmdStart 
// - CAknSoftNotificationSubject::LaunchMissedCallsAppL()
// - Context sensitive Help application (when certain logs view can be opened from help)
//
// IMPORTANT: This command line parameter interface is PRIVATE and may be replaced
// in future with more generic solution. So do not use this unless agreed with Logs
// project.
//
// For invocation of Logs application use the LogsUiCmdStarter helper functionality 
// available in LogsUiCmdStarter.h.
//
// ----------------------------------------------------------------------------
//
TBool CLogsAppUi::ProcessCommandParametersL(
    TApaCommand aCommand,
    TFileName& aDocumentName,
    const TDesC8& aTail )
    {
    TRACE_ENTRY_POINT;
    
    //Parameter strings used to invoke Logs from external applications, e.g Phone, Missed Call
    //Notification, Help. Note that AppShell does not use this method but instead uses view 
    //based invocation (for ELogAppListViewId)
    
     //1. Officially supported Logs views fo opening.    
    _LIT8(m_old,"m");      //This can be removed when missed indicator uses the new value

     //2. Non-supported Logs views for opening (for private use).    
    _LIT8(events,"events");                 //ELogEventListViewId
    _LIT8(timers,"timers");                 //ECtViewId
    _LIT8(packetcounter,"packetcounter");   //EGprsCounterViewId
    _LIT8(settings,"settings");             //ELogSettingsViewId
    _LIT8(callregister,"callregister");     //ELogSubAppListViewId

    //For internal use only    
    _LIT8(dontActivate, "dontactivate");
    TPtrC8 dontActivateMsg = dontActivate(); 
    
    // Check first if we are going to background
    if (aCommand == EApaCommandBackground)
    	{
    	HideLogsToBackgroundL();
    	}
    //1. Officially supported Logs views.
    else if( aTail.Compare( LogsUiCmdStarterConsts::KDialledView() ) == 0 )
        {
        SetProvideOnlyRecentViews( ETrue ); 
        SetLogsOpenedWithSendKey( ETrue );           
        SetExecutionMode( ELogsInForeground );
        ActivateLogsViewL( EStmDialledListViewId );      
        }
    else if( aTail.Compare( LogsUiCmdStarterConsts::KMissedView() ) == 0 || aTail.Compare( m_old ) == 0 )
        {
        SetProvideOnlyRecentViews( ETrue );     
        SetExecutionMode( ELogsInForeground );
        ActivateLogsViewL( EStmMissedListViewId );         
        }
    else if( aTail.Compare( LogsUiCmdStarterConsts::KReceivedView() ) == 0  )
        {
        SetProvideOnlyRecentViews( ETrue );   
        SetExecutionMode( ELogsInForeground );
        ActivateLogsViewL( EStmReceivedListViewId );         
        }
    else if( aTail.Compare( LogsUiCmdStarterConsts::KCountersView() ) == 0  )
        {
        SetProvideOnlyRecentViews( EFalse );  
        SetExecutionMode( ELogsInForeground );
        ActivateLogsViewL( ELogAppListViewId );         
        }
    
    //2. The views below are supported privately *only* for Context sensitive help application. 
    //For other uses they are not visible, i.e. are not available in LogsUiCmdStarter because
    //Logs application may change in the future (e.g. some views may be dropped). 
    //However, when Logs application changes, then the corresponding helps need to be changed too 
    //in parallel so we don't end up in situation in which default view is opened instead
    //of a requested view that has been removed from Logs.
    else if( aTail.Compare( events ) == 0  )
        {
        SetProvideOnlyRecentViews( EFalse ); 
        SetExecutionMode( ELogsInForeground );
        ActivateLogsViewL( ELogEventListViewId );         
        }
    else if( aTail.Compare( timers ) == 0  )
        {
        SetProvideOnlyRecentViews( EFalse ); 
        SetExecutionMode( ELogsInForeground );
        ActivateLogsViewL( ECtViewId );         
        }
    else if( aTail.Compare( packetcounter ) == 0  )
        {
        SetProvideOnlyRecentViews( EFalse );     
        SetExecutionMode( ELogsInForeground );
        ActivateLogsViewL( EGprsCounterViewId );         
        }
    else if( aTail.Compare( settings ) == 0  )
        {
        SetProvideOnlyRecentViews( EFalse );     
        SetExecutionMode( ELogsInForeground );
        ActivateLogsViewL( ELogSettingsViewId );         
        }
    else if( aTail.Compare( callregister ) == 0  )
        {
        SetProvideOnlyRecentViews( EFalse );    
        SetExecutionMode( ELogsInForeground );
        ActivateLogsViewL( ELogSubAppListViewId );         
        }

    //3. Skip view activation attempts        
    else if( aTail.Compare( dontActivateMsg ) == 0  )
        {
        //No op
        }
        
    //3. Default view
    else 
        {
        SetProvideOnlyRecentViews( EFalse ); 
        if (aCommand != EApaCommandBackground )
            {
            SetExecutionMode( ELogsInForeground );
            }
      
        //AppShell doesn't currently provide option to send cmdline parameter, so provide 
        //AppListView (or SubAppListView) as it is probably activated later from AppShell.        
        if ( FeatureManager::FeatureSupported( KFeatureIdSimpleLogs ) )
            {
            ActivateLogsViewL( ELogSubAppListViewId, 
                               aCommand == EApaCommandBackground ? 
                                   ELogsViewActivationBackground : ELogsViewActivationNoMessage, 
                               KNullDesC8() );       
            }
        else
            {    
            ActivateLogsViewL( ELogAppListViewId, 
                               aCommand == EApaCommandBackground ? 
                                   ELogsViewActivationBackground : ELogsViewActivationNoMessage, 
                               KNullDesC8() ); 
            }
        }
        
    // We have received a commandline parameter so no need to reset to Logs main view
    // when foreground\focus regained (see HandleWsEvent).
    SetResetViewOnFocusRegain(EFalse); 
    
    TRACE_EXIT_POINT;
    return CAknViewAppUi::ProcessCommandParametersL( aCommand, aDocumentName, aTail );        
    }

// ----------------------------------------------------------------------------
// CLogsAppUi::StartApplicationL
//
// Start an external application
// ----------------------------------------------------------------------------
//
TBool CLogsAppUi::StartApplicationL(
    TUid aUid,
    const TDesC8& aTail )
    {
    TApaTaskList taskList( CCoeEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( aUid );
    
    if( task.Exists() ) 
        {
        const TUid dummyUID = { 0x0 };
        task.SendMessage( dummyUID, aTail );
        task.BringToForeground();  
        }
    else
        { 
        TApaAppInfo appInfo;
        RApaLsSession lsSession;
        User::LeaveIfError( lsSession.Connect() );        
        CleanupClosePushL( lsSession );
        
        if( lsSession.GetAppInfo( appInfo, aUid ) == KErrNone )
            {
            CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL( appInfo.iFullName );
            cmdLine->SetCommandL( EApaCommandRun );
            cmdLine->SetTailEndL( aTail );

            lsSession.StartApp( *cmdLine );
            CleanupStack::PopAndDestroy( cmdLine );
            }

        CleanupStack::PopAndDestroy();  // lsSession
        }
        
    return 0;
    }

    // If Logs is background,returns is True,otherwise False.
    TBool CLogsAppUi::IsBackground() const
      {
        CCoeEnv * env = CCoeEnv::Static();
        TInt ordinal = env->RootWin().OrdinalPosition();
        return ordinal != 0;
      }


//  End of File
