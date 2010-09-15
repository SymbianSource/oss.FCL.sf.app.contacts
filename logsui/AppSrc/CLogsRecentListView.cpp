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
*     Abstract base class for recent views
*
*/


// INCLUDE FILES

// System includes
#include <eikmenub.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h> 
#include <baclipb.h>
#include <StringLoader.h> 
#include <CMessageData.h>
#include <sendui.h> 
#include <SendUiConsts.h>
#include <TSendingCapabilities.h>
#include <sendnorm.rsg>
#include <logs.rsg>
#include <AiwCommon.hrh>                //KAiwCmdCall
#include <AiwPoCParameters.h>           //TAiwPocParameterData
#include <aknViewAppUi.h>
#include <AiwServiceHandler.h>
#include <AiwGenericParam.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <AknGlobalNote.h>
#include <AknGlobalConfirmationQuery.h>
#include <MGFetch.h>
#include <AiwContactSelectionDataTypes.h>
#include <AiwContactAssignDataTypes.h>

// Virtual Phonebook
#include <RVPbkContactFieldDefaultPriorities.h>
#include <VPbkFieldType.hrh>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkContactLinkArray.h>
#include <VPbkContactStoreUris.h>
#include <VPbkContactViewFilterBuilder.h>
//------------------------

#include "CLogsRecentListView.h"
#include "CLogsRecentListControlContainer.h"
#include "CLogsEngine.h"
#include "MLogsEventGetter.h"
#include "MLogsClearLog.h"
#include "MLogsStateHolder.h"
#include "MLogsSharedData.h"
#include "CLogsAppUi.h"
#include "LogsUID.h"
#include "CLogsViewGlobals.h"
#include "MLogsExtensionFactory.h"
#include "MLogsViewExtension.h"
#include "MLogsSystemAgent.h"
#include "MLogsUiControlExtension.h"
#include "LogsConstants.hrh"
#include "CLogsPrependQuery.h"

#include "LogsDebug.h"
#include "LogsTraces.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// Panic string
_LIT( KStmRecentView, "StmRecentView" );

// MACROS


// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CLogsRecentListView::NewL
// ----------------------------------------------------------------------------
//
CLogsRecentListView* CLogsRecentListView::NewL( TLogsModel aModel )
    {
    CLogsRecentListView* self = new ( ELeave ) CLogsRecentListView( aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );  // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::CLogsRecentListView
// ----------------------------------------------------------------------------
//
CLogsRecentListView::CLogsRecentListView( TLogsModel aModel ) :
    iState( EStateUndefined ),
    iViewExtension( NULL ),
    iEventListCurrentNoChange( KErrNotFound ) //iEventListCurrentNoChange switched off
    {
    iModel = aModel;
    }
    
// ----------------------------------------------------------------------------
// CLogsRecentListView::~CLogsRecentListView
// ----------------------------------------------------------------------------
//
CLogsRecentListView::~CLogsRecentListView()
    {
    delete iExitCbaButton;
    delete iBackCbaButton;
    iCoeEnv->RemoveForegroundObserver( *this );
    
    Release( iViewExtension );
    Release( iViewGlobal );
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::ConstructL()
    {
    BaseConstructL( R_STM_COMMON_VIEW );
    iCoeEnv->AddForegroundObserverL( *this );
    iExitCbaButton = iCoeEnv->AllocReadResourceL( R_STM_EXIT_CBA_BUTTON );
    iBackCbaButton = iCoeEnv->AllocReadResourceL( R_STM_BACK_CBA_BUTTON );
    iViewGlobal = CLogsViewGlobals::InstanceL();
    
// Safwish VoIP changes  >>>
    if ( !iViewExtension )
        {
        CPbkContactEngine* nullContactEngine = NULL;
        iViewExtension = iViewGlobal->ExtensionFactoryL().
                CreateLogsViewExtensionL( *nullContactEngine, this );        
                
        iViewExtension->SetSendUi( *LogsAppUi()->SendUiL() ); 
        iViewExtension->SetSendUiText( SendUiTextL() );                 
        }
// <<<  Safwish VoIP changes
    }
    
// ----------------------------------------------------------------------------
// CLogsRecentListView::LogsCurrentRecentViewId
// ----------------------------------------------------------------------------
//
TLogsViewIds CLogsRecentListView::LogsCurrentRecentViewId() const
    {
    switch( iModel )
        {
        case ELogsReceivedModel:
            return EStmReceivedListViewId;
        case ELogsDialledModel:
            return EStmDialledListViewId;
        case ELogsMissedModel:
            return EStmMissedListViewId;
        default:
            User::Panic( KStmRecentView, KErrUnknown );
            return ELogsDummyViewId; // just to compile without error
            //break;
        }
    }


// ----------------------------------------------------------------------------
// CLogsRecentListView::Id
// ----------------------------------------------------------------------------
//
TUid CLogsRecentListView::Id() const
    {
    return TUid::Uid( LogsCurrentRecentViewId() );
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::RecentListType
// ----------------------------------------------------------------------------
//
TLogsModel CLogsRecentListView::RecentListType() const
    {
    return iModel;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::ProcessKeyEventL
//
// Called by base control container when a key press happens. 
// ----------------------------------------------------------------------------
//
TBool CLogsRecentListView::ProcessKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if( MenuBar()->ItemSpecificCommandsEnabled() && aType == EEventKey )
        {
        if( aKeyEvent.iCode == EKeyBackspace ) 
            {
            HandleCommandL( ELogsCmdMenuDelete );
            return ETrue;
            }
        }
        
    const MLogsEventGetter* event( CurrentEvent() );
    
    //FIXME (CLogsRecentListView and CLogsEventListView): 
    //The below is needed only because we get key event before the actual row
    //is changed in the listbox. If/when listbox observer for changed row focus
    //is available, remove code below and get correct event index from observer.
    SetEventListCurrent(iContainer->ListBox()->CurrentItemIndex());

    if( aType == EEventKey && EventListCurrent() != KErrNotFound ) 
        {
        TInt count( CurrentModel()->Count() );
        switch (aKeyEvent.iScanCode)
            {
            case EStdKeyUpArrow:
                //Set iEventListCurrent + jump to previous or last event depending on position in list
                iEventListCurrent > 0 ? event = CurrentModel()->At( --iEventListCurrent ) :
                                        event = CurrentModel()->At( iEventListCurrent = count - 1 );
                break;

            case EStdKeyDownArrow:
                //Set iEventListCurrent + jump to next or first event depending on position in list                
                iEventListCurrent < count - 1 ? event = CurrentModel()->At( ++iEventListCurrent ) :
                                                event = CurrentModel()->At( iEventListCurrent = 0 );
                break;                
            }
        } 
		 
        
    return CLogsBaseView::ProcessKeyEventEventL( aKeyEvent,aType, event );
    }
    
// ----------------------------------------------------------------------------
// CLogsRecentListView::ProcessPointerEventL
//
// Handler for pointer events, when the current focused item is tapped
// ----------------------------------------------------------------------------
// 
void CLogsRecentListView::ProcessPointerEventL( TInt /* aIndex */ )
	{
    // Open the context sensitive menu   
	ProcessCommandL( EAknSoftkeyContextOptions );      	
	}

// ----------------------------------------------------------------------------
// CLogsRecentListView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::HandleCommandL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;

// Safwish VoIP changes  >>>
    //1. Process first the commands that may be in range interested by PECLogs exension
    if ( iViewExtension->HandleCommandL( aCommandId ) )
        {
        return;
        }
// <<<  Safwish VoIP changes

    //2. Was not a command in range intended for extension. Continue processing.
    const MLogsEventGetter* event = CurrentEvent();
                        
    switch( aCommandId )
        {
        case ELogsCmdMenuDeleteAll:
            CmdClearRecentListL();
            break;

        case ELogsCmdMenuDelete:
            CmdDeleteEventL();
            break;

        case ELogsCmdHandleMSK:
            {
            // If single click or press MSK for a log event,
            // it will request directly by AiwSrvCmd.
            aCommandId = ELogsCmdSingleTapCall;
            } // Fall through
            
        default:
            {
            CLogsBaseView::HandleCommandEventL( aCommandId, event );
            }
        } 
   
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::ChangeTitlePaneTextToDefaultL
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::ChangeTitlePaneTextToDefaultL()
    {       
    iContainer->SetTitlePaneTextToDefaultL( LogsAppUi()->ActiveViewId() );
    } 

// ----------------------------------------------------------------------------
// CLogsRecentListView::ProcessCommandL
//
// Called from FW when e.g. cba pressed 
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::ProcessCommandL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;
           
    if (LogsAppUi()->ActiveViewId() != LogsCurrentRecentViewId() || !iContainer )
        {
        LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
                ( "CLogsRecentListView::ProcessCommandL - view already deactivated! return " ));  
        return;
        }
    
    if (aCommand == EAknSoftkeyOptions)
        {
        // If options menu is opened call SetRefreshMenuOnUpdate which records the current event id
        // which can then be used to make a decision on wether options menu needs to be refreshed
        // when reading is finished.
        SetRefreshMenuOnUpdate( CurrentEvent() );
        }
    
    CLogsBaseView::ProcessCommandL( aCommand );
        
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CLogsRecentListView::DynInitMenuPaneL
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::DynInitMenuPaneL( 
    TInt aResourceId, 
    CEikMenuPane* aMenuPane )
    {
    //Get event. Note that event may NOT yet be available if asynch request(s) are not completed. 
    const MLogsEventGetter*  event = CurrentEvent(); //returns NULL if no data yet available
    CLogsBaseView::DynInitMenuPaneEventL( aResourceId, aMenuPane, event );    
    
// Sawfish VoIP changes  >>>
    iViewExtension->DynInitMenuPaneL( aResourceId, aMenuPane, event );
// <<<  Sawfish VoIP changes    

    }


// ----------------------------------------------------------------------------
// CLogsRecentListView::HandleClientRectChange
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::HandleClientRectChange()
    {
    if (iContainer)
    	{
    	iContainer->SetRect( ClientRect() );  
    	}
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::DrawComponents
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::DrawComponents()
    {
    if (iContainer)
        {
        iContainer->DrawNow();
        }
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::ViewDeactivated
//
// This is called by framework when really losing foreground but not when losing foreground to some note etc.
// Also called by framework when switching views inside application.
// So DoDeactivate is not always called when ViewDeactivated is called.
//
// Note: there is no guarenteed order of which is called first ViewDeactivated or HandleLosingForeground
//
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::ViewDeactivated()
    {
    // When view is deactivated due to losing foreground we reset the list here in advance so there won't be 
    // flickering of the old list. Since view server introduced redraw storing the HandleGainingForeground
    // comes too late and the old list will flicker before it is reseted. 
    // We'll skip this when Phonebook update pending - the list is going to be updated later.
    //
    // If we would do this in HandleLosingForeground, then the list would be emptied when for example when
    // active applications list, global note etc. is shown.
    //
    if (CurrentModel() && iContainer && ( iFocusChangeControl != ELogsNoChange_PbkUpdPending ) )
        {
		
		// EJZO-7RJB3V
        if (IgnoreViewDeactivatedOperation())
            {
            CAknView::ViewDeactivated();
            return;
            }

        if (iFocusChangeControl != ELogsOkToChange)
            {
            StoreEvenListCurrentFocus();
            }
         
        // Reset array and set dirty which means refresh requested, 
        // will call StateChangedL with state EStateArrayReseted and update the listbox.
        // Keep db connection.
        CurrentModel()->DoDeactivate( MLogsModel::ESkipClearing,
                                      MLogsModel::EKeepDBConnection );
     
        }       
    
    CAknView::ViewDeactivated();
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::DoActivateL
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    TRACE_ENTRY_POINT;
       
    CLogsBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );    

    //To prevent first time flicker of context menu if no ctx mnu will be displayed
    Cba()->MakeCommandVisible( EAknSoftkeyContextOptions, EFalse );
    
    iFocusChangeControl = ELogsOkToChange;           
    iEventListCurrentNoChange = KErrNotFound;
    
    LogsAppUi()->SetCurrentViewId( Id() );
    
    //To avoid possible flicker, change the cba text to 'Exit' when going to background
    //but don't draw it. Sometimes CAknView::ConstructMenuAndCbaL might draw the cba
    //when we are going to background and activating dialled calls view. It is safer 
    //to have the cba thus set as 'Exit'
    if( aCustomMessageId.iUid == ELogsViewActivationBackground && 
        LogsAppUi()->ExecutionMode() == ELogsInBackground_ExitOrEndPressed )
        {
        ChangeCba2ToExitL( ELogsDontDraw );     
        }
    else if ( LogsAppUi()->ProvideOnlyRecentViews())                   
        {  
        LogsAppUi()->SetPreviousAppUid( aPrevViewId.iAppUid );
        LogsAppUi()->SetPreviousViewId( aPrevViewId.iViewUid );
        Engine()->DeleteConfig();           //Logs settings
        ChangeCba2ToExitL( ELogsDrawNow );                //We must not be able to back to other Logs views, just exit
        }
    else  
        {
        ChangeCba2ToBackL();
        }
   

    Engine()->CreateModelL( iModel );       //Creates model only if not yet exists

    //Create the control container. 
    if( !iContainer )
        {
        iContainer = CLogsRecentListControlContainer::NewL( this, ClientRect() );
        AppUi()->AddToViewStackL( *this,iContainer );  
        }
    iContainer->ListBox()->SetListBoxObserver( this );

    //External launch has been done already, so come here
    if ( ! LogsAppUi()->ProvideOnlyRecentViews() )
        {
        LogsAppUi()->SetPreviousAppUid( TUid::Uid( KLogsAppUID3 ) );
        LogsAppUi()->SetPreviousViewId( aPrevViewId.iViewUid );
        }

    //Update item selection. Set always focus to top. If previous focus is to be retained, comment below two rows out
    SetEventListCurrent(0);
    SetEventListTop(0);
    
    CurrentModel()->SetObserver( this );
    
    if( aCustomMessageId.iUid == ELogsViewActivationBackground && 
        LogsAppUi()->ExecutionMode() == ELogsInBackground_ExitOrEndPressed )
        {
        // We have activated this view because we are hiding Logs into background (faking exit) 
        // so this view is waiting when Logs is again foregrounded.    
        //
        // With MLogsModel::ECloseDBConnectionAndResetArray the connection to the database is
        // closed and event array will be deleted. Calls StateChangedL with state EStateArrayReseted 
        // which will update the listbox to empty state.
        // Keep db connection
        CurrentModel()->DoDeactivate( MLogsModel::ENormalOperation,
                                      MLogsModel::EKeepDBConnection );

        CurrentModel()->DoActivateL( MLogsModel::EResetAndRefresh );
        // Now we can enable bring-to-foreground on view activation:
        AppUi()->SetCustomControl(0); 
        AppUi()->HideInBackground(); 
        // If listbox needs to redraw,do it.
        if( CurrentModel()->Count()>0 && 
              ( iContainer->ListBox()->TopItemIndex() != EventListTop() ||
                  iContainer->ListBox()->CurrentItemIndex() != EventListCurrent())) 
             {
                iContainer->ListBox()->DrawDeferred();
             }
       
        }
    else
        {
        // Just to make sure the inputblocker is not on
        RemoveInputBlocker();
        TRAPD( err, CurrentModel()->DoActivateL( MLogsModel::EResetAndRefresh ));   
        if( err ) 
            {
            iCoeEnv->HandleError( err );
            }
        if( err == KErrDiskFull )
            {
            RWsSession& wsSession = CCoeEnv::Static()->WsSession();  
            TApaTask logsui( wsSession );
            TInt wgId = CCoeEnv::Static()->RootWin().WindowGroupId();
            logsui.SetWgId( wgId );
            logsui.KillTask();  
            return;	
            }
         
        // Avoid the flicking when transfer to foreground from background.
        if(LogsAppUi()->IsBackground())
          {
              iContainer->DrawNow();         
              iEikonEnv->AppUiFactory()->StatusPane()->DrawNow(); 
              LogsAppUi()->SetCustomControl(0);  
          
              CCoeEnv * env = CCoeEnv::Static();
              env->WsSession().SetWindowGroupOrdinalPosition(env->RootWin().Identifier(),0);
                            
           }   

        }
    
    
    if( CurrentModel()->Count() )
          {
          iContainer->ListBox()->SetTopItemIndex( EventListTop() );
          if( EventListCurrent() < 0 )
              {
              SetEventListCurrent(0);
              }
          iContainer->ListBox()->SetCurrentItemIndex( EventListCurrent() );
          }
     
    ClearMissedCallNotificationsL();     //Clear cenrep new missed calls counter + notifications (however, possible missed 
                                         //list duplicate counters need to be cleared separately).  
     
    TRACE_EXIT_POINT;    
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::ReadingFinished
// ----------------------------------------------------------------------------
//
TBool CLogsRecentListView::ReadingFinished()
    {
    TBool finished = ( iState != EStateActive       && iState != EStateUndefined  &&  
                       iState != EStateInitializing && iState != EStateSemiFinished &&
                       iState != EStateInterrupted );  
                                          
    return finished;
    }   
    
// ----------------------------------------------------------------------------
// CLogsRecentListView::IgnoreViewDeactivatedOperation
// ----------------------------------------------------------------------------
//  
TBool CLogsRecentListView::IgnoreViewDeactivatedOperation()
    {
    TBool ret = EFalse;
    if (iIgnoreViewDeactivatedHandling)
        {
	      // no guarenteed order of which is called first ViewDeactivated or HandleLosingForeground,
	      // so add the iSemiFinishViewDeactivatedOperation flag
        if (iSemiFinishViewDeactivatedOperation)
            {
            iIgnoreViewDeactivatedHandling = EFalse;
            iSemiFinishViewDeactivatedOperation = EFalse;
            }
        else
            {
            iSemiFinishViewDeactivatedOperation = ETrue;
            }
                
        ret = ETrue;
        }
        
    return ret;   
	}
	  
// ----------------------------------------------------------------------------
// CLogsRecentListView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::DoDeactivate()
    {
    TRACE_ENTRY_POINT;
        
    CLogsBaseView::DoDeactivate(); 
                         
    if( iContainer )
        {
        SetEventListCurrent(iContainer->ListBox()->CurrentItemIndex());
        SetEventListTop(iContainer->ListBox()->TopItemIndex());
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }

     if( iModel == ELogsMissedModel )
        {
        // Below we test for reading finshed to prevent unnecessary clearing of duplicate counters, 
        // new missed call icons and missed call notification and the "x missed calls" text 
        // in the Call register view. 
        //
        // This would otherwise be done when user reopens Logs by 
        // green key or from appshell after first leaving Missed calls view to background. 
        //
        // (Missed calls view will first receive HandleGainingForeground followed by an 
        // immediate DoDeactivate )
        //
        // FIXME: This fix is not 100% sure cause it might happen rarely that the reading has time to 
        // finish before framework activates the actual view that was opened (and calls DoDeactivate for 
        // missed calls view). No better solution found yet though. 
        //
        if ( ReadingFinished() )
            {
            LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
                    ( "CLogsRecentListView::DoDeactivate - ReadingFinished" ));  
            ClearMissedCallNotifications();   //Clear cenrep new missed calls counter + notifications          
            // Keep db connection
            CurrentModel()->DoDeactivate( MLogsModel::ENormalOperation,
                                          MLogsModel::EKeepDBConnection );//ETrue: disconnect from db. This helps for EMSH-6JDFBV but
                                              //reduces user's perceived performance for other views
                                              //(seems to keep clearing of missed duplicates process alive
                                              // so no need for EFalse here as it would increase probability of 
                                              // EMSH-6JDFBV occurring again   
                                    
            }
                              
        else
            {
            LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
                    ( "CLogsRecentListView::DoDeactivate - Reading interrupted" )); 
            // Keep db connection
            CurrentModel()->DoDeactivate( MLogsModel::ESkipClearing,
                                          MLogsModel::EKeepDBConnection );
            }    
        }
    else
        {
        CurrentModel()->DoDeactivate( MLogsModel::ENormalOperation,
                                      MLogsModel::EKeepDBConnection );//ETrue: disconnect from db. This helps for EMSH-6JDFBV but
                                              //reduces user's perceived performance for other views
                                              //EFalse: don't disconnect from db. This keeps read data cached in Logs.
        }
        
    //To prevent second time flicker of context menu if ctx mnu was was displayed
    //but will not later be displayed
    Cba()->MakeCommandVisible( EAknSoftkeyContextOptions, EFalse );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::CmdClearRecentListL
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::CmdClearRecentListL()
    {
    if( CurrentModel()->Count() )
        {
        TInt resId( 0 );

        CAknQueryDialog* queryDlg = CAknQueryDialog::NewL();
                
        switch( iModel )
            {
            case ELogsMissedModel:
                resId = R_CLEAR_MISSED_LIST_CONFIRMATION_QUERY;
                break;

            case ELogsReceivedModel:
                resId = R_CLEAR_RECEIVED_LIST_CONFIRMATION_QUERY;
                break;

            case ELogsDialledModel:
                resId = R_CLEAR_DIALLED_LIST_CONFIRMATION_QUERY;
                break;

            default:
                User::Panic( KStmRecentView, KErrUnknown );
                break;
            }
          
        if( queryDlg->ExecuteLD( resId ) )
            {
            Engine()->ClearLogsL()->ClearModelL( iModel );
            SetEventListTop(0);
            SetEventListCurrent(0);
                
            SetInputBlockerL();
            } 
        }
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::CmdDeleteEventL
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::CmdDeleteEventL()
    {
    if( CurrentModel()->Count() && iContainer->ListBox()->CurrentItemIndex() != KErrNotFound )    
        {
        CAknQueryDialog* queryDlg = CAknQueryDialog::NewL();
        SetEventListTop(iContainer->ListBox()->TopItemIndex());
        SetEventListCurrent(iContainer->ListBox()->CurrentItemIndex());

        TLogId id = CurrentModel()->At( EventListCurrent() )->LogId();
        
        if( queryDlg->ExecuteLD( R_DELETE_CONFIRMATION_QUERY ) )
            {
            iEventListCurrentNoChange = iContainer->ListBox()->CurrentItemIndex(); 
            Engine()->ClearLogsL()->DeleteEventL( id ); //Delete the event from LogDb. Event from view gets 
                                                        //removed when the view data is reread from db.
            iFocusChangeControl = ELogsNoChange;        //Prevent focus moving to top when list is reread 
            } 
        }
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::ChangeCba2ToExitL
// ----------------------------------------------------------------------------
//
TBool CLogsRecentListView::ChangeCba2ToExitL( TLogsDrawNow aDrawNow )
    {
    //We must attempt to change CBA only if we have correct view in the foreground
    if( LogsAppUi()->ActiveViewId() != LogsCurrentRecentViewId() )
        {
        return EFalse; //There is some other view already active, so we don't have 
        }              // our own cba's present
 
    TInt pos = Cba()->PositionById( EAknSoftkeyBack );   
         
    if( pos > 0 )
        {
        Cba()->SetCommandL( pos, 
                            ELogsCmdMenuExit, 
                            iExitCbaButton->Des() );
                            
        if (aDrawNow == ELogsDrawNow)
            {
            Cba()->DrawNow();  
            }
            
        return ETrue;
        }
        
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::ChangeCba2ToBackL
// ----------------------------------------------------------------------------
//
TBool CLogsRecentListView::ChangeCba2ToBackL()
    {
    if( LogsAppUi()->ActiveViewId() != LogsCurrentRecentViewId() )
        {
        return EFalse; //There is some other view already active, so we don't have our own cba's present
        }              // (e.g. FSW can be brought to foreground already)
        
    TInt pos = Cba()->PositionById( ELogsCmdMenuExit ); 
           
    if( pos > 0 )
        {
        Cba()->SetCommandL( pos, 
                            EAknSoftkeyBack, 
                            iBackCbaButton->Des() );
        Cba()->DrawNow();//Needed when this view already been open and cba changed in other view
        
        return ETrue;
        }
        
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::HandleGainingForeground
//
// This function is called once for each already constructed Recent View when 
// Logs is gaining foreground. No matter if a view is deactivated (i.e. is also 
// called for view(s) not actually gaining foreground but just have been constructed)
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::HandleGainingForeground()
    {   
    TRACE_ENTRY_POINT;  
         
    CLogsBaseView::HandleGainingForeground();

    //Process cba if this view is already open and then just brought to foreground
    //in ProvideOnlyRecentViews mode
    if( LogsAppUi()->ProvideOnlyRecentViews() )    
        {
        TRAPD( err, ChangeCba2ToExitL( ELogsDrawNow ) );  
        if( err ) 
            {
            iCoeEnv->HandleError( err );
            }
        
        // Always reset focus to the top of the list, when Logs is opened by a send key 
        // press from idle    
        if (LogsAppUi()->LogsOpenedWithSendKey() && LogsCurrentRecentViewId() == EStmDialledListViewId)
            {
            iFocusChangeControl = ELogsOkToChange;
            iEventListCurrentNoChange = KErrNotFound;
            LogsAppUi()->SetLogsOpenedWithSendKey(EFalse); // reset the send key checking 
            LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
                ( "CLogsRecentListView::HandleGainingForeground - LogsOpenedWithSendKey: reset focus" ));  
            }       
        }

    //Activate current view's model that we deactivated in HandleLosingForeground().
    //No need to check if already active, DoActivateL() handles this.
    if( LogsAppUi()->ActiveViewId() == LogsCurrentRecentViewId() && iContainer )  //Only if this object corresponds 
                                                                    //to current view.
        {
        MLogsModel* model = CurrentModel();
        
        //ELogsNoChange is special case here, i.e contact just added to pbk: skip activating to avoid unwanted UI-focus change.
        //We don't know in hw which is finished first, reading of entries because of gaining of foreground (EMSH-6JDFBV) here or 
        //re-reading of entries because change observed in db, or will the first db-read have enough time to finish before.
        if( model )  
            {
            if( iFocusChangeControl != ELogsNoChange_PbkUpdPending) 
                {
                 //If menu is shown, we need to close it as focus is lost
                if (MenuBar()->IsDisplayed())
                    {
                    MenuBar()->StopDisplayingMenuBar();
                    }
                 }
            else
                {
                // Change ELogsNoChange_PbkUpdPending to ELogsNoChange now so there is no need to 
                // differentiate those in StateChangedL
                iFocusChangeControl = ELogsNoChange;
                }
            }
        // Sure "ClearMissedCallNotifications" is called.
        ClearMissedCallNotifications();
        
        RemoveInputBlocker(); //just in case
        }                            
    TRACE_EXIT_POINT; 
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::StoreEvenListCurrentFocus()
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::StoreEvenListCurrentFocus()
    {
    //
    // Check that not called from deactivated recent list view that also gets 
    // a notification of losin foreground.
    if (!iContainer || LogsAppUi()->ActiveViewId() != LogsCurrentRecentViewId())
        {
        return;
        }
     
    // Store the current focused item. Sometimes foreground can be regained for only very briefly and 
    // event reading is not finished so skip those situations. 
    // Also there is no guarantees which is called fist, HandleLosingForeground
    // or ViewDeactivated so don't change when list is already deleted.
    //
    if( (iContainer->ListBox()->CurrentItemIndex() != KErrNotFound) && ReadingFinished() )
        {
        SetEventListCurrent(iContainer->ListBox()->CurrentItemIndex());
        
        // Switch on iEventListCurrentNoChange: store the current item index to 
        // keep it over effects of loss and gain of foreground.
        iEventListCurrentNoChange = EventListCurrent(); 
        SetEventListTop(iContainer->ListBox()->TopItemIndex());
        }
    //
    // If the list has no focus and iEventListCurrentNoChange is not set, let the focus go 
    // to the top of the list when events are reread.
    else if ( ( iContainer->ListBox()->CurrentItemIndex() == KErrNotFound ) &&
              ( iEventListCurrentNoChange == KErrNotFound ) )
        {
        iFocusChangeControl = ELogsOkToChange;
        }
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::HandleLosingForeground
//
// This function is called once for each type of already constructed recent view (e.g.when fast swapping 
// window is brought to foreground).
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::HandleLosingForeground()
    {
    TRACE_ENTRY_POINT;
    CLogsBaseView::HandleLosingForeground(); 
    
    TBool finished = ReadingFinished();     
    TLogsViewIds currView = LogsCurrentRecentViewId();      
    
    
    if ( iContainer && ( LogsAppUi()->ActiveViewId() == currView )) 
        {
        // EJZO-7RJB3V
        if (IgnoreViewDeactivatedOperation())
            {
            TRACE_EXIT_POINT;
            return; 
            }
           
        // If we are losing foreground in the middle of event reading, force empty text to the view.
        // This happens for example when keys are locked EYSN-6X5DYS
        if (!finished)
            {
            // Does not have effect if list is not empty
            iContainer->ForceEmptyTextListBox();
            }
        //
        // If focus is to be kept (iFocusChangeControl is ELogsNoChange or 
        // ELogsNoChange_PbkUpdPending) store the current focused item
       if ( iFocusChangeControl != ELogsOkToChange  )   
            {
            StoreEvenListCurrentFocus();
            }
        }
    
    // The below is additional code for tuning performance consumption. It can be removed when no need 
    // for this is found as the code below does not affect functionality.
    // For performance consumption reasons (EMSH-6JDFBV) we need to deactivate the 
    // current view when we lose foreground. However, this needs to be done only for 
    // missed calls view as other views should not be affected by this. However we do it for dialled calls in order 
    // to prevent old wrong entry staying a while in top of list when redialing from dialled list. 
    //
    // Update: apply the same for Received calls view. This is needed to to avoid inconsistency 
    // between the views, like in EYSN-6X5DYS  
    //
    if( ( LogsAppUi()->ActiveViewId() == currView ) &&  //Only if this object corresponds to current view.
        ( currView == EStmMissedListViewId  || 
          currView == EStmDialledListViewId ||
          currView == EStmReceivedListViewId ) ) 
        {                                               
        MLogsModel* model = CurrentModel();
        if( model )
            {
            // End key pressed in missed calls view: sent to background before the Dialled calls view 
            // is activated while in background. so need to clear duplicates now since the reader is deleted
            // and they cannot be cleared in DoDeactivate anymore
            if( LogsAppUi()->ExecutionMode() == ELogsInBackground_ExitOrEndPressed && currView == EStmMissedListViewId )
                {
                 LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
                    ( "CLogsRecentListView::HandleLosingForeground - clear duplicates" ));      
                 // Keep db connection
                model->DoDeactivate( MLogsModel::ENormalOperation, 
                                     MLogsModel::EKeepDBConnection );    //EFalse: don't disconnect from db 
                }
            else    
                {
                LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
                    ( "CLogsRecentListView::HandleLosingForeground - skip clearing" )); 
                //Other loss of foreground (call or AppKey). We'll not touch duplicate counters
                // Keep db connection
                model->DoDeactivate( MLogsModel::ESkipClearing, //Don't update db (for missed view)
                                     MLogsModel::EKeepDBConnection );    //ETrue: disconnect from db in order to immediately to stop
                                                 //EFalse: don't disconnect from db                                     
                }
            }
        }
    
    TRACE_EXIT_POINT;
    }
 
// ----------------------------------------------------------------------------
// CLogsRecentListView::StateChangedL
//
// CLogsModel informs it has changed event array, e.g. added entry to array or reseted the array etc.
// This function is called as many times as there is new/changed row in the event array. 
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::StateChangedL( MLogsStateHolder* aHolder )
    {
    TRACE_ENTRY_POINT;
    
    if( !aHolder )
         {
         return;  
         }
         
     iState = aHolder->State();
    
    // When check contact link finished, call base method to check if need to 
    // refresh menu.
     if( iState == EStateCheckContactLinkFinished )
         {
         CLogsBaseView::StateChangedL(aHolder);
         return;
         }
    
    //Call Update() when related items available to show changes immediately plus when reading is finished. 
    const TInt KNbrShownEntries = 8;
    
    iState = aHolder->State();
    LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
        ( "CLogsRecentListView::StateChangedL - iState:%d  iFocusChangeControl:%d viewid:%d"), 
          iState, iFocusChangeControl, LogsCurrentRecentViewId() );  
        
    // If there was some error reading events - most likely KErrNoMemory, 
    // display information to user and close Logs 
    if( iState == EStateError )
    	{
    	iCoeEnv->HandleError(KErrNoMemory);
	    LogsAppUi()->CmdExit();
    	}
    
    //A. If reader deleted (EMSH-6JDFBV), stop all processing.
    if( iState == EStateReaderDeletedOrStopped )
        { 
        return;
        }
    // Special case, array is reseted so update the list to empty state. This prevents flicker
    // of the old list when regaining foreground
    else if ( iState == EStateArrayReseted )
        {
        if( iContainer )
            {
            iContainer->UpdateL();
            }
        return;
        }
              
    //B. Continue to show contents quickly and without flicker when enough events available
    if( iContainer )
        {
        //1. Check are we finished and set scroll bars
        TInt count( iContainer->ListBox()->Model()->NumberOfItems() );  //Current count of items CLogsModel has read into item array
        TBool finished = ReadingFinished();  
        CEikScrollBarFrame* sbFrame = iContainer->ListBox()->ScrollBarFrame();    

        if( finished || iState == EStateSemiFinished) //To avoid scroll bar flicker don't show scroll bar until completely finished
            {
            sbFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOn );  
            }
        else if (count == 0)
            {
            sbFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff ); 
            }
        
        //2. Update listbox when sufficiently events available
        if( finished || iState == EStateSemiFinished)
            {
            iContainer->UpdateL();      //Calls HandleItemAdditionL (plus sets Empty text if needed)    
            // Fetch data for extension
            iContainer->ControlExtension()->HandleAdditionalData( 
                *CurrentModel(), 
                *iContainer->ListBox() );
            // Add condition check: To avoid Missing Call Note don't display ,do not call "ClearMissedCallNotificationsL" when logs is in background.
            if(!LogsAppUi()->IsBackground())
                ClearMissedCallNotificationsL();
            // When event reading is finished, remove inputblocker
            RemoveInputBlocker();
            }        
        // If focus is not retained, refresh the UI when a screenful of events is read
        else if( iFocusChangeControl == ELogsOkToChange && count == KNbrShownEntries ) 
            {
            iContainer->UpdateL();
            }
   
        //3. Control focus changes in the list during the read of events
        
        //Set focus once when ok to change     
        if ( iFocusChangeControl == ELogsOkToChange ) 
            {
            if( iState == EStateFinished || iState == EStateSemiFinished )
                {            
                //Reset iEventListCurrentNoChange and iFocusChangeControl
                iEventListCurrentNoChange = KErrNotFound; 
                iFocusChangeControl =  ELogsNoChange;
                }
            
            if( count &&  //at least one entry from previous read to this view
               ( iState == EStateUndefined || iState == EStateInitializing )) //we're about start reread        
                {
                SetEventListCurrent(0); 
                iContainer->ListBox()->SetCurrentItemIndex( EventListCurrent() );
                }                
            }
  
        else
            //No focus change when entry saved to phonebook, deleted or list updated twice.
            {
            if( (iState == EStateFinished && LogsCurrentRecentViewId() != EStmMissedListViewId ) || iState == EStateSemiFinished)
                {
                //If e.g. foreground regained, use iEventListCurrentNoChange if switched on
                if( iEventListCurrentNoChange != KErrNotFound &&    //iEventListCurrentNoChange is switched on.
                    ( EventListCurrent() == KErrNotFound ||
                      EventListCurrent() == 0) )     //No iEventListCurrent available.
                    {
                    SetEventListCurrent(iEventListCurrentNoChange);  //Use iEventListCurrentNoChange
                    }
             
                if( count > 0 ) 
                    {
                    //If now less rows than current item before, or if no current selection (e.g last
                    //row was deleted), try to set focus to last available item. Otherwise try to use 
                    //iEventListCurrent.
                    if( count <= EventListCurrent() || EventListCurrent() < 0 ) 
                        {
                        iContainer->ListBox()->SetCurrentItemIndex( count - 1 );
                        }
                    else if( count > EventListCurrent() )
                        {
                        iContainer->ListBox()->SetCurrentItemIndex( EventListCurrent() );
                        // if event list top is sensible, adjust the window back to what it was
                        if ((EventListTop() >= 0) && (EventListTop() < count) )
                            {
                            iContainer->ListBox()->SetTopItemIndex(EventListTop());
                            }
                        }
                    }
                
                iFocusChangeControl =  ELogsNoChange;
                SetEventListCurrent( iContainer->ListBox()->CurrentItemIndex() );                    
                }
            }
        
                //When reading twice (EMSH-6JDFBV), prevent setting focus again for second phase read. 
        if( iState == EStateSemiFinished ) 
            {
            iFocusChangeControl = ELogsNoChange; 
            }
        
        //4. Set MSK state
        if( iState == EStateFinished || iState == EStateSemiFinished )         
            {
            Cba()->MakeCommandVisible( 
                EAknSoftkeyContextOptions, 
                IsOkToShowContextMenu( CurrentEvent() ));
            }

        //5. To improve user's perceived responsiviness we probably now have a good moment to perform 
        //some time consuming operations if not yet already done
        if( count == KNbrShownEntries ) //if( iState == EStateFinished )
            {
            ConstructDelayedL( ETrue );   //EFalse: perform immediately, ETrue: perform using idle time                                
            }
        
        //6. When starting reading, call UpdateL so the informative text 
        // "Retrieving data" can be shown instead just the empty screen.    
        if (count == 0 && iState == EStateInitializing )
            {
            iContainer->UpdateL();  
            }
      
        //7. Just in case the menu was open before reading events, handle the possible menu refresh now 
        if (finished)
            {
            HandleMenuRefreshL(CurrentEvent());
            }
        
        }
        
    TRACE_EXIT_POINT;    
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::ExtStateChangedL
// ----------------------------------------------------------------------------
//
void CLogsRecentListView::ExtStateChangedL()
    {
    if( iContainer )
        {
        iContainer->UpdateL();
        }
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::State
// ----------------------------------------------------------------------------
//
TLogsState CLogsRecentListView::State() const
    {
    return iState;  
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::CurrentEvent
//
// Returns event relating current selection of listbox or NULL
// ----------------------------------------------------------------------------
//
const MLogsEventGetter* CLogsRecentListView::CurrentEvent()
    {
    TRACE_ENTRY_POINT;
    //KErrNotFound if no current selection in listbox:
    if (iContainer && iContainer->ListBox())
        {
        SetEventListCurrent(iContainer->ListBox()->CurrentItemIndex());
        }
    else
        {
        TRACE_EXIT_POINT;
        return NULL;
        }
    
    //Return null if no current selection or no suitable event available 
    if( CurrentModel() && CurrentModel()->Count() > 0 && EventListCurrent() >= 0  )    
        {
        TRACE_EXIT_POINT;
        return CurrentModel()->At( EventListCurrent() );    
        }
    else
        {
        TRACE_EXIT_POINT;
        return NULL;
        }
    }

// ----------------------------------------------------------------------------
// CLogsRecentListView::HandleNotifyL
//
// Handle AIW-notifications (from asynch ExecuteCmdL call)
// ----------------------------------------------------------------------------
//
TInt CLogsRecentListView::HandleNotifyL(
    TInt aCmdId, 
    TInt aEventId,
    CAiwGenericParamList& aEventParamList,
    const CAiwGenericParamList& aInParamList )
    {
    TInt result = CLogsBaseView::HandleNotifyL( aCmdId, aEventId, aEventParamList, aInParamList );        
    return result;    
    }

void CLogsRecentListView::ViewActivatedL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,const TDesC8& aCustomMessage)
    {
    //To avoid the flicking when transfer to foreground from background,we control the view show manually.
    if(LogsAppUi()->IsBackground()  &&     
             LogsAppUi()->ActiveViewId() != LogsCurrentRecentViewId() )
        {
            LogsAppUi()->SetCustomControl(1);
        }      
    
      CLogsBaseView::ViewActivatedL(aPrevViewId,aCustomMessageId,aCustomMessage);
      
    }

//  End of File  
