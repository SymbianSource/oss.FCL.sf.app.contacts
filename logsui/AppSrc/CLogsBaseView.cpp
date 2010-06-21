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
*     Logs Application view superclass
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logengevents.h>
#endif
#include <eikmenub.h>
#include <Pbk2UID.h>
#include <bldvariant.hrh>
#include <featmgr.h>
#include <baclipb.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h> 
#include <StringLoader.h> 
#include <AiwCommon.hrh>                //kaiwcmdcall
#include <AiwContactSelectionDataTypes.h>
#include <AiwContactAssignDataTypes.h>

#ifdef __OMA_POC
#include <pocuiintegrationservices.h>   //cpocuiintegrationservices 
#endif

#include <commonphoneparser.h>
#include <CMessageData.h>
#include <sendui.h> 
#include <SendUiConsts.h>
#include <TSendingCapabilities.h>
#include <aknbutton.h>
#include <AknsUtils.h>
#include <logs.mbg>     // logs own icons
#include <LogsApiConsts.h>      //additional event uids
#include <CPhCltEmergencyCall.h>

#include <Logs.rsg>

#include "LogsIcons.hrh"
#include "LogsConsts.h"
#include "LogsConstants.hrh"
#include "LogsUID.h"
#include "CLogsBaseView.h"
#include "CLogsAppUi.h"
#include "CLogsEngine.h"
#include "CLogsCntLinkChecker.h"
#include "CLogsReaderConfigFactory.h"
#include "MLogsReader.h"
#include "MLogsEventGetter.h"
#include "MLogsSharedData.h"
#include "MLogsSystemAgent.h"   
#include "CLogsTimer.h"
#include "aiwdialdataext.h"
#include "CPhoneNumberFormat.h"
#include "LogsDebug.h"
#include "LogsTraces.h"
#include "CLogsPrependQuery.h"    

// service table
#include <spsettings.h>
#include <spentry.h>

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
// CLogsBaseView::LogsAppUi
// ----------------------------------------------------------------------------
//
CLogsAppUi* CLogsBaseView::LogsAppUi()
    {
    return static_cast<CLogsAppUi*>( CAknView::AppUi() );
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::~CLogsBaseView
// ----------------------------------------------------------------------------
//
CLogsBaseView::~CLogsBaseView()
    {
    delete iSendUiText;
    delete iAddress;    
    delete iConstructDelayed;
    delete iNumberInPbkProcessing;
    delete iUriInPbkProcessing;    
    delete iLogsTimer;    
    delete iEmergencyCallAPI;
    RemoveInputBlocker();
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::CLogsBaseView
// ----------------------------------------------------------------------------
//
CLogsBaseView::CLogsBaseView() : 
    iConstructDelayedOk( EFalse ),
    iSendUiText( NULL ),
    iFocusChangeControl( ELogsOkToChange ),     
    iEventListCurrent( 0 ),
    iEventListTop( 0 ),
    iRefreshMenuOnUpdate( EFalse ),
    iMenuType(CEikMenuBar::EMenuOptions),
    iInputBlocker(NULL),
    iIsCheckedCntLinkInvaild( EFalse),  
	iResetAndRefreshOnHandleNotify(MLogsModel::ERefresh),
	iIgnoreViewDeactivatedHandling(EFalse),
	iSemiFinishViewDeactivatedOperation(EFalse)
    {
    //Construct CIdle object call back to finalise time consuming construction
    iConstructDelayed = CIdle::New( CActive::EPriorityIdle );
    
    if( iConstructDelayed )
        {
        //The below seems to slow down reading of Logs entries from db so we're
        //not calling it from here
        //iConstructDelayed->Start( TCallBack( ConstructDelayedCallbackL, this) );            
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::BaseConstructL
//
// This is just a hook to catch BaseConstructL calls from derived classes before/
// after call to super class's BaseConstructL if needed.
// ----------------------------------------------------------------------------
//
void CLogsBaseView::BaseConstructL(TInt aResId )
    {
    CAknView::BaseConstructL( aResId );
    
    if( AknLayoutUtils::PenEnabled() && Toolbar() ) 
        {
        //By default keep toolbar invisible        
        ShowToolbarOnViewActivation( EFalse ); 
        Toolbar()->SetToolbarObserver( this );
        } 
    }
    

// ----------------------------------------------------------------------------
// CLogsBaseView::Engine
// ----------------------------------------------------------------------------
//
CLogsEngine* CLogsBaseView::Engine()
    {
    return LogsAppUi()->Engine();
    }

// Safwish VoIP changes  >>>
// ----------------------------------------------------------------------------
// CLogsBaseView::SendUiTextL
// ----------------------------------------------------------------------------
//
const TDesC& CLogsBaseView::SendUiTextL()
    {
    if ( !iSendUiText )
        {
        iSendUiText =  iCoeEnv->AllocReadResourceL( R_STM_WRITE_MSG );
        }    
    return *iSendUiText;
    }
// <<< Safwish VoIP changes  

// ----------------------------------------------------------------------------
// CLogsBaseView::ConstructDelayedL
//
// Perform time consuming construction operations once. Calling this is needed 
// before e.g. pressing Send key or launching a command from ui's menu.
// ----------------------------------------------------------------------------
//
void CLogsBaseView::ConstructDelayedL( TBool aIdle )  //EFalse: immediately
    {                                                 //ETrue:  using idle time
    if( iConstructDelayed && aIdle && !iConstructDelayed->IsActive() )     
        {
        iConstructDelayed->Start( TCallBack( ConstructDelayedCallbackL, this) );            
        }
    else
        {
        DoConstructDelayedL();            
        }        
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::ConstructDelayedCallbackL
//
// Called from CIdle once to finish time consuming construction operations
// ----------------------------------------------------------------------------
//
TInt CLogsBaseView::ConstructDelayedCallbackL( TAny* aContainer )
    {
    CLogsBaseView* container = static_cast<CLogsBaseView*>( aContainer );
    container->DoConstructDelayedL();    
    return 0;
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::DoConstructDelayedL
//
// Perform time consuming construction operations once. 
// ----------------------------------------------------------------------------
//
void CLogsBaseView::DoConstructDelayedL()
    {
    TRACE_ENTRY_POINT;
    
    if( iConstructDelayed ) 
        {
        iConstructDelayed->Cancel();
        delete iConstructDelayed;
        iConstructDelayed = NULL;
        }
    
    if( iConstructDelayedOk ) 
        {
        return;    
        }
        
    iConstructDelayedOk = ETrue;
    iServHandlerRef = LogsAppUi()->CallUiL();  //ownership remains in LogsAppUi,
                                               //this takes a lot of time.
                                           
    if ( !iSendUiText )
        {
        iSendUiText = iCoeEnv->AllocReadResourceL( R_STM_WRITE_MSG );
        }

    iAddress = iCoeEnv->AllocReadResourceL(R_LOGS_ADDR_PROMPT);    
    iLogsTimer = CLogsTimer::NewL( CActive::EPriorityUserInput ); //EPriorityStandard  EPriorityHigh
    iEmergencyCallAPI = CPhCltEmergencyCall::NewL( this );
    
    TRACE_EXIT_POINT
    }


// ----------------------------------------------------------------------------
// CLogsBaseView::HandleLogsTimerL
//
// Callback function for iLogsTimer 
// ----------------------------------------------------------------------------
//
void CLogsBaseView::HandleLogsTimerL( TAny* /* aPtr */ )
    {
    HandleCommandL( ELogsCmdLongSendKeyCall );
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::LaunchPopupMenuL
//
// Launch a popup menu when ok-button has been pressed
// ----------------------------------------------------------------------------
//
void CLogsBaseView::LaunchPopupMenuL( TInt aResourceId )
    {
    SetCurrentMenuType(CEikMenuBar::EMenuContext);
    
    TRAPD( err, DoLaunchPopupL( aResourceId ) );
    if( err )
        {
        SetCurrentMenuType(CEikMenuBar::EMenuOptions);
        User::Leave( err );
        }
        
    SetCurrentMenuType(CEikMenuBar::EMenuOptions);
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::DoLaunchPopupL
//
// Performs the popup menu launching
// ----------------------------------------------------------------------------
//
void CLogsBaseView::DoLaunchPopupL( TInt aResourceId )
    {
    MenuBar()->SetContextMenuTitleResourceId( aResourceId );    
    MenuBar()->TryDisplayContextMenuBarL(); //TryDisplayContextMenuBarL() does not provide "Act 
                                            //apps" selection as TryDisplayMenuBarL() would.
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::FindXSPServiceSettingsLC
// Checks for stored settings for the logged service id. Returns a pointer to the 
// found service table entry or NULL. If a NULL pointer is returned, the object is 
// not pushed to the CleanupStack.
// ----------------------------------------------------------------------------
//
CSPEntry* CLogsBaseView::FindXSPServiceSettingsLC(const MLogsEventGetter* aEvent)
    {
    CSPEntry* entry = NULL;
    
    // check only if the service id and sip uri stored.
    if (aEvent->LogsEventData()->ServiceId() && SipUriAvailable( aEvent ) )
        {
        TInt err = KErrNotFound;
        entry = CSPEntry::NewLC();      
       
        CSPSettings* settings = CSPSettings::NewL();
        CleanupStack::PushL(settings);
        err = settings->FindEntryL( aEvent->LogsEventData()->ServiceId(), *entry );  
        CleanupStack::PopAndDestroy( settings ); 
        
        if (err != KErrNone)
            {
            CleanupStack::PopAndDestroy( entry ); 
            entry = NULL;
            }   
        }
     
    return entry;
    }
        
        
        
// ----------------------------------------------------------------------------
// CLogsBaseView::CmdSaveToVirtualPhonebookL
// ----------------------------------------------------------------------------
//
void CLogsBaseView::CmdSaveToVirtualPhonebookL( 
    TInt aCommandId, 
    TBool aCalledForMenu,
    const MLogsEventGetter* aEvent )    
    {
    TRACE_ENTRY_POINT;
    
    using namespace AiwContactAssign;        
    
    CAiwGenericParamList& inParamList = iServHandlerRef->InParamListL();    
    
    TUint assignFlags = NULL;
    
    if( !aCalledForMenu )
        {
        assignFlags = AiwContactAssign::ECreateNewContact;
        }

    inParamList.AppendL(
        TAiwGenericParam(
            EGenericParamContactAssignData,     //AiwGenericParam.hrh
            TAiwVariant(TAiwSingleContactAssignDataV1Pckg( 
                TAiwSingleContactAssignDataV1().SetFlags(assignFlags)))));                    
//           TAiwVariant(TAiwContactAttributeAssignDataV1Pckg( 
//               TAiwContactAttributeAssignDataV1().SetFlags(assignFlags)))));
// the above commented code also provides just one field to add to phonenbook so we must 
// select between msisdn and uri in code below.
    
    
    // Check first if there is a service id logged and there are settigs configured.
    // If valid settings found for the logged service id, store to contacts as an xsp id
    //   
    CSPEntry* entry = FindXSPServiceSettingsLC(aEvent);
    if (entry)
        {    
        TBuf<KLogsSipUriMaxLen> xspuserid;
        if ( Engine()->ConvertToUnicode( aEvent->LogsEventData()->Url(), xspuserid ))
            {
            return; //Converting uri to unicode failed
            }
        else 
            {     
            _LIT(KLogsColon, ":");
            
            HBufC* xspid = HBufC::NewLC(
                    xspuserid.Length() + 
                    entry->GetServiceName().Length() + 
                    KLogsColon().Length() );
            
            xspid->Des().Append( entry->GetServiceName() );        
            xspid->Des().Append( KLogsColon );
            xspid->Des().Append( xspuserid );                                   
            
            inParamList.AppendL( 
                TAiwGenericParam(
                        EGenericParamXSpId,
                        TAiwVariant(  xspid->Des() )
                ));         
            CleanupStack::PopAndDestroy( xspid ); 
            CleanupStack::PopAndDestroy( entry );
            }
        }
    //
    // If service id not found or no valid settings, store as a sip uir or phonenumber
    //
    else if( SipUriAvailable( aEvent ) )
        {
        //inParamList allocates it's own buffer, so it's safe to hand over buf 
        TBuf<KLogsSipUriMaxLen> buf; 
        if ( Engine()->ConvertToUnicode( aEvent->LogsEventData()->Url(), buf ) )
            {
            return; //Converting uri to unicode failed
            }
        else
            {
            inParamList.AppendL(
                TAiwGenericParam(
                    EGenericParamSIPAddress,
                    TAiwVariant( buf )
                    ));        
            }
        }    
    else if( PhoneNumberAvailable( aEvent ) )
        {
        inParamList.AppendL(
            TAiwGenericParam(
                EGenericParamPhoneNumber,
                TAiwVariant( aEvent->Number() )
                ));
        }
    else
        {
        return; //Nothing to store
        }

    delete iNumberInPbkProcessing;
    delete iUriInPbkProcessing;    
    iNumberInPbkProcessing = NULL;
    iUriInPbkProcessing = NULL;    
    
    //2. If sip uri available, we prefer it over phone number (PoC or VoIP)
    if( SipUriAvailable( aEvent ))    
        {
        TBuf<KLogsSipUriMaxLen> buf; 
        // Returns KErrNone if everythin ok
        if ( !(Engine()->ConvertToUnicode( aEvent->LogsEventData()->Url(), buf )) ) 
            {
            iUriInPbkProcessing = buf.AllocL();
            }
        }
    else if( PhoneNumberAvailable( aEvent ))    
        {
        iNumberInPbkProcessing = aEvent->Number()->AllocL();        
        }
        
    //Add to Phonebook    will call CPbk2AiwAssignProvider::HandleServiceCmdL    
    if ( !aCalledForMenu )    
        {
        iServHandlerRef->ExecuteServiceCmdL(
            aCommandId,
            inParamList,
            iServHandlerRef->OutParamListL(),
            0,
            this );//HandleNotifyL
        }
    else
        {
        iServHandlerRef->ExecuteMenuCmdL(
            aCommandId,
            inParamList,
            iServHandlerRef->OutParamListL(),
            0,
            this );//HandleNotifyL
        }
    
    // Store AiW call parameters so that we can cancel the call later if needed. 
    SaveAiwCallParamsL(aCommandId, aCalledForMenu, &inParamList);
    
    // Set input blocker cause we don't want to have the user open menus etc
    // while opening phonebook
    SetInputBlockerL();

    TRACE_EXIT_POINT;    
    }



// --------------------------------------------------------------------------
// CLogsBaseView::SaveAiwCallParamsL
// --------------------------------------------------------------------------
//
void CLogsBaseView::SaveAiwCallParamsL( TInt aCommandId,
                                        TBool  aCalledForMenu,
                                        CAiwGenericParamList*  aInParamList ) 
    { 
    iAiwCommandId = aCommandId;
    iAiwCalledFromMenu = aCalledForMenu;
    iAiwInParamList = aInParamList;
    
    // To detect and cancel the aiw call early enough, we will need to observe view activation
    LogsAppUi()->AddViewActivationObserverL(this);
    }

// --------------------------------------------------------------------------
// CLogsBaseView::ClearAiwCallParams
// --------------------------------------------------------------------------
//
void CLogsBaseView::ClearAiwCallParams()
    {
    iAiwCommandId = KErrNotFound;
    iAiwCalledFromMenu = EFalse;
    iAiwInParamList = NULL;
      
    // No need to observe view activation anymore. 
    LogsAppUi()->RemoveViewActivationObserver(this);
    }

// --------------------------------------------------------------------------
// CLogsBaseView::CancelSaveToPbkL
// --------------------------------------------------------------------------
//
void CLogsBaseView::CancelSaveToPbkL()
    {
    if (iAiwInParamList && iAiwCommandId != KErrNotFound )
        {
        
        iResetAndRefreshOnHandleNotify = MLogsModel::EResetAndRefresh;
        
        // Cancel the previously done AiW call by calling with the same parameters
        // + flag KAiwOptCancel
        //
        if ( !iAiwCalledFromMenu )    
            {
            iServHandlerRef->ExecuteServiceCmdL(
                    iAiwCommandId,
                    *iAiwInParamList,
                    iServHandlerRef->OutParamListL(),
                    KAiwOptCancel,
                    this );//HandleNotifyL
            }
        else
            {
            iServHandlerRef->ExecuteMenuCmdL(
                    iAiwCommandId,
                    *iAiwInParamList,
                    iServHandlerRef->OutParamListL(),
                    KAiwOptCancel,
                    this  );//HandleNotifyL
            }          
        }
  
    ClearAiwCallParams();
    } 

   
// --------------------------------------------------------------------------
// CLogsBaseView::HandleViewActivation
// --------------------------------------------------------------------------
//
void CLogsBaseView::HandleViewActivation(const TVwsViewId& aNewlyActivatedViewId,const TVwsViewId& aViewIdToBeDeactivated)
    { 
    TInt err;
    
    // Do nothing if iNumberInPbkProcessing of iUriInPbkProcessing already cancelled.
    //
    if ( iNumberInPbkProcessing || iUriInPbkProcessing  )
        {
        // If another Logs view is going to be activated, cancel the save to pbk AiW command.
        //
        if ( aNewlyActivatedViewId.iAppUid == TUid::Uid(KLogsAppUID3) &&
             aNewlyActivatedViewId.iViewUid != this->Id() )            
            {  
            TRAP(err, CancelSaveToPbkL());
            }
        //
        // Also if Logs has received a commandline parameter telling it to activate this view,
        // like when opening Logs from Idle with send key, it is best to cancel the command so that
        // the view is in "fresh" state.
        //
        else if ( aNewlyActivatedViewId.iAppUid == TUid::Uid(KLogsAppUID3) &&
                  aNewlyActivatedViewId.iViewUid == this->Id() && 
                  aViewIdToBeDeactivated.iAppUid != TUid::Uid(KLogsAppUID3) )
            {
            // Reset array, will call StateChangedL with state EStateArrayReseted and update the listbox.
            CurrentModel()->DoDeactivate( MLogsModel::ESkipClearing,
                                          MLogsModel::EResetOnlyArray ); 
            DrawComponents();
            TRAP(err, CancelSaveToPbkL());
            }
        
        // If a leave happened, clear the call parameters here.
        if (err != KErrNone)
            {
            ClearAiwCallParams();
            }
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::StateChangedL
// ----------------------------------------------------------------------------
//
void CLogsBaseView::StateChangedL( MLogsStateHolder* aHolder )
    {
    if ( !aHolder )
        {
        return;
        }
    if ( aHolder->State() == EStateCheckContactLinkFinished )
        {
        const MLogsEventGetter* event = NULL;
        MLogsEventData* eventData = NULL;
        
        if( CurrentModel() && CurrentModel()->Count() > 0 && EventListCurrent() >= 0 )
            {
            event = CurrentModel()->At( EventListCurrent() );    
            eventData = event->LogsEventData();
            
            if ( eventData )
                {
                TPtrC8 ptrContactLink( KNullDesC8 );
                if ( KErrNone == eventData->GetContactLink( ptrContactLink ) )
                    {
                    CLogsCntLinkChecker* contactCheckerPtr = Engine()->CntLinkCheckerL();
                    if ( !contactCheckerPtr->IsCntLinkValidSync( ptrContactLink ) )
                        {
                        iIsCheckedCntLinkInvaild = ETrue;
                        }
                    }
                // When log event is for VOIP, it needs to check if the contact link is valid.
                // If it's invalid, refresh menubar in order to avoid "voice" and "video" items.
                // Can't avoid flicking in this case, however, it's so rare to repro.
                if ( eventData->VoIP() && SipUriAvailable( event ) 
                       && iIsCheckedCntLinkInvaild )
                    {
                    if ( MenuBar()->IsDisplayed() )
                        {
                        MenuBar()->StopDisplayingMenuBar();
        
                        if ( CurrentMenuType() == CEikMenuBar::EMenuContext )
                            {
                            MenuBar()->TryDisplayContextMenuBarL();
                            }
                            else
                            {
                            MenuBar()->TryDisplayMenuBarL();
                            }
                        }
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CLogsBaseView::SetInputBlockerL
// --------------------------------------------------------------------------
//
void CLogsBaseView::SetInputBlockerL()
     {
     if (!iInputBlocker)
         {
         iInputBlocker = CAknInputBlock::NewCancelHandlerLC( this );
         CleanupStack::Pop( iInputBlocker );   
         iInputBlocker->SetCancelDelete( iInputBlocker );
         }
     } 

// --------------------------------------------------------------------------
// CLogsBaseView::RemoveInputBlockerL
// --------------------------------------------------------------------------
//
void CLogsBaseView::RemoveInputBlocker()
    {
    if (iInputBlocker)
        {
        iInputBlocker->Cancel();
        }
    }

// --------------------------------------------------------------------------
// CLogsBaseView::AknInputBlockCancel
// --------------------------------------------------------------------------
//
void CLogsBaseView::AknInputBlockCancel()
     {
     iInputBlocker = NULL;
     } 

// ----------------------------------------------------------------------------
// CLogsBaseView::ProcessKeyEventL
//
// Called by base control container when a key press happens.     
// ----------------------------------------------------------------------------
//
TBool CLogsBaseView::ProcessKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if( aType == EEventKey )
        {
        switch( aKeyEvent.iCode )
            {
            case EKeyApplication0:  //S60 app key pressed
                iCoeEnv->Flush( KDelayMicroSeconds );
                return EFalse;
            }
        }
    
    return EFalse;
    }   
    
// ----------------------------------------------------------------------------
// CLogsBaseView::ProcessPointerEventL
//
// Default handler for pointer events, when the current focused item is tapped
// ----------------------------------------------------------------------------
//
void CLogsBaseView::ProcessPointerEventL( TInt aIndex)
	{
    // Default action when tapping the focused item is to open another view
    LogsAppUi()->CmdOkL(aIndex);
	}
  
// ----------------------------------------------------------------------------
// CLogsBaseView::FocusChangedL
//
// Informs subclass that focus is changed in listbox (item is tapped)
// ----------------------------------------------------------------------------
//
//void CLogsBaseView::FocusChangedL()
//	{
//    HandleCommandL( ELogsCmdHandleTapFocusChange );    //Calls subclass impl if available
//	}

// --------------------------------------------------------------------------
// CLogsBaseView::DynInitToolbarL
// --------------------------------------------------------------------------
//
void CLogsBaseView::DynInitToolbarL( 
    TInt /* aResourceId */, 
    CAknToolbar* /* aToolbar */ )
    {
    }

// --------------------------------------------------------------------------
// CLogsBaseView::OfferToolbarEventL
// --------------------------------------------------------------------------
//
void CLogsBaseView::OfferToolbarEventL( TInt aCommand )
    {
    // Call ConstructDelayedL now incase it has not been done yet. 
    // One example case was ELFG-6YV99Q when there are less than a screenful of events 
    // (see magic number KNbrShownEntries in CLogsRecentListView::StateChangedL)
    ConstructDelayedL( EFalse ); 
    HandleCommandL( aCommand );    //Calls subclass impl if available
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::HandleLosingForeground
//
// This function is called once for each type of already constructed view 
// (e.g.when fast swapping window is brought to foreground).
// ----------------------------------------------------------------------------
//
void CLogsBaseView::HandleLosingForeground()
    {
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::HandleGainingForeground
//
// This function is called once for each already View when 
// Logs is gaining foreground. No matter if a view is deactivated (i.e. is also 
// called for view(s) not actually gaining foreground but just have been constructed)
// ----------------------------------------------------------------------------
//
void CLogsBaseView::HandleGainingForeground()
    {     
    }
    
// ----------------------------------------------------------------------------
// CLogsBaseView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CLogsBaseView::DoDeactivate()
    {
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::DoActivateL
// ----------------------------------------------------------------------------
//
void CLogsBaseView::DoActivateL(
    const TVwsViewId& /* aPrevViewId */,
    TUid /* aCustomMessageId */,
    const TDesC8& /* aCustomMessage */ )
    {
    // AppUi owns the service handler refrence and it might have been deleted and recreated
    // so the reference here might thus be invalid. But skip this step if delayed construction 
    // is still going to be done.
    if (iConstructDelayedOk)
        {
        iServHandlerRef = LogsAppUi()->CallUiL(EFalse);  //ownership remains in LogsAppUi,
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::HandleSendKeyMenuCommandL
//
// Handles send key press when menu is open. For call commands, send key works as 
// if the menu option would have been selected. Default behaviour is not to consume the
// key which causes send key to open dialler. 
// ----------------------------------------------------------------------------
//
TBool CLogsBaseView::ProcessSendKeyEventInMenuL(TEventCode aType, const MLogsEventGetter* aEvent)
    {
    CEikMenuPane* cascadeMenuPane = MenuBar()->MenuPane()->CascadeMenuPane();
    TInt cmdId = KErrNotFound;
    TBool ret = EFalse;
    
    if (cascadeMenuPane && cascadeMenuPane->IsFocused())
        {
        cmdId = cascadeMenuPane->MenuItemCommandId( cascadeMenuPane->SelectedItem() );
        } 
    
    TInt srvCmd = iServHandlerRef->ServiceCmdByMenuCmd( cmdId ); 
    switch( srvCmd )    //AiwCommon.hrh
        {
        case KAiwCmdCall:    
        case KAiwCmdPoC:  
            {
            if (aType == EEventKeyDown ) 
                {
                MenuBar()->StopDisplayingMenuBar();
                CmdAiwMenuCmdL(cmdId, aEvent);
                }
            ret = ETrue; // consume the send key  
            break;
            }    
          
        case KErrNotFound:      
        default:
            {
            ret = EFalse; // send key event not consumed
            break;       
            }
        }
    
    return ret;
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::ProcessKeyEventEventL
//
// For subclasses to handle key events needing LogEvent data    
// ----------------------------------------------------------------------------
//
TBool CLogsBaseView::ProcessKeyEventEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType,
    const MLogsEventGetter* aEvent )
    {
    //First try non-event related
    TBool ret = CLogsBaseView::ProcessKeyEventL( aKeyEvent, aType ); //Don't call subclass again    
    
    // If events not read yet or a query dialog is shown, consume the key so dialler is not opened
    if ( aKeyEvent.iScanCode == EStdKeyYes && ( LogsAppUi()->IsDisplayingDialog() || !aEvent ) )
        {
        return ETrue;
        } 
    else if( ret || !aEvent )
        {
        return ret;
        }
  
    //Process Send key for cellular, voip or poc call 
    if( aKeyEvent.iScanCode == EStdKeyYes )
        {
        ConstructDelayedL( EFalse );        //EFalse: perform immediately
        TBool supportLongSendPress = EFalse;//FIXME: FeatureManager::FeatureSupported( ??? ) this value should be available from platform in future
        
        // Handle first the special case when send key is pressed while options menu is open
        // default behaviour is not to consume the key and let dialler open. For some call commands
        // it is better to handle the command as if the menu option had been selected.
        if (MenuBar()->IsDisplayed() && MenuBar()->MenuPane() )
            {
            return ProcessSendKeyEventInMenuL(aType, aEvent); 
            }       
        // 
        else if( aType == EEventKeyDown ) 
            {     
            if( supportLongSendPress ) 
                {
                //Start timer to wait delay period for forced video call
                const TInt delay = 600000; //0.6sec, FIXME: this value should be available from platform in future
                iLogsTimer->Cancel();
                iLogsTimer->After( delay, this,  const_cast<MLogsEventGetter*>(aEvent) );
                }
            else
                {
                HandleCommandEventL( ELogsCmdSendKeyCall, aEvent ); 
                }
            }
        else if( aType == EEventKey )             
            {
            //nop (send key down)
            }
        else if( aType == EEventKeyUp ) 
            {
            if( supportLongSendPress && iLogsTimer->IsActive() ) 
                {
                //Not pressed long enough, make normal call
                iLogsTimer->Cancel();                
                HandleCommandEventL( ELogsCmdSendKeyCall, aEvent ); 
                }
            }
        return ETrue;                 
        }

    //Process other keys
    if( aType == EEventKey )
        {
        switch( aKeyEvent.iCode )
            {
            case EKeyApplication0:  //S60 app key pressed
                iCoeEnv->Flush( KDelayMicroSeconds );
                return EFalse;
                
//            case EKeyPhoneSend: //Send key pressed      (same as EKeyYes in enum TKeyCode)                
//                return ETrue;            
                
            //PoC key usage (Voice/side key) goes as follows:
            //When Poc app is active, it captures the key press. If Logs window is active in foreground,
            //Poc app sends EKeyPoC event to Logs. Because we don't get EKeySide event to Logs, we are dependent 
            //on PoC to be active app to provide it (so no need to check the EKeySide event). Therefore only 
            //Poc call should be provided, no voip or voice call even if the current Logs event is voice/voip 
            //when EKeyPoC was pressed.

            //case EKeySide:    //Voice key pressed     (same as EKeyDevice6 in enum TKeyCode)
            case EKeyPoC:       //Poc key pressed       (same as EKeyApplication1 in enum TKeyCode)
                if ( MenuBar()->ItemSpecificCommandsEnabled() )
                    {
                    //First check have we already done construction fully. If not, then we first need to do it. 
                    ConstructDelayedL( EFalse );   //EFalse: perform immediately
                    HandleCommandEventL( ELogsCmdDeviceCallKey, aEvent ); //Other device special key pressed e.g. 
                                                                          //for poc call
                    return ETrue;
                    }
            }
        }

    //Set MSK & toolbar state        
    if(( aKeyEvent.iScanCode == EStdKeyUpArrow || 
         aKeyEvent.iScanCode == EStdKeyDownArrow ) && 
         aType == EEventKey ) 
        {
        SetToolbarItems( aEvent );
                
        Cba()->MakeCommandVisible( 
            EAknSoftkeyContextOptions, 
            IsOkToShowContextMenu( aEvent ));
        }
   
    return EFalse;
    }
    
    
// ----------------------------------------------------------------------------
// CLogsBaseView::TabObserver
//
// ----------------------------------------------------------------------------
//
MAknTabObserver* CLogsBaseView::TabObserver()
    {
    return LogsAppUi();
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::HandleCommandL
// 
// For other commands
// ----------------------------------------------------------------------------
//
void CLogsBaseView::HandleCommandL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;
    switch( aCommandId )
        {
        case ELogsCmdStartPhonebook:
            LogsAppUi()->StartApplicationL( TUid::Uid(KPbk2UID3), KNullDesC8 );
            break;
            
        case ELogsCmdMenuSettings:
            AppUi()->HandleCommandL( aCommandId );
            break;

        case EEikCmdExit:       // flow-through
        case ELogsCmdMenuExit:  // flow-through
        case EAknSoftkeyExit:
            LogsAppUi()->HandleCommandL( ELogsCmdMenuExit );
            break;

        case EAknCmdHideInBackground:
            {
            //If phonebook update is pending, request reset of callui. Otherwise in certain 
            //cases pending phonebook updates end up in wrong state (ECJA-7JDD5, ECJA-7JDCZW) 
            if( iPreviousCommand == KAiwCmdAssign )
                {
                iServHandlerRef = LogsAppUi()->CallUiL( ETrue ); 
                }
            break;
            }

        case EAknSoftkeyBack:
            {
            // Set inputblocker to avoid problems of fast tapping during view switch. 
            // View switch will then cancel the inputblokcer.
            SetInputBlockerL();  
            LogsAppUi()->CmdBackL();
            break;
            }

        case EAknCmdHelp:
            AppUi()->HandleCommandL( aCommandId );
            break;

        default:
            break;
        }
        
    TRACE_EXIT_POINT;  
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::HandleCommandEventL
//
// For event-related commands
// ----------------------------------------------------------------------------
//
void CLogsBaseView::HandleCommandEventL( 
    TInt aCommandId,
    const MLogsEventGetter* aEvent )
    {
    //Try first non-event commands
    CLogsBaseView::HandleCommandL( aCommandId ); //Don't call subclass again    
    
    if( !aEvent )
        {
        return;
        }

    //Check event releated commands    
    switch( aCommandId )
        {
        case ELogsCmdMenuPrependNumber:
            CmdPrependCallL(ETrue, aEvent);  //ETrue: prefer phone number in case sip uri also available
            break;
        
        case ELogsCmdMenuPrependAddress:        
            CmdPrependCallL(EFalse, aEvent); //EFalse: prefer sip uri if available
            break;

        case ELogsCmdMenuCopyNumber:
            CmdCopyNumberL( aEvent );
            break;

        case ELogsCmdMenuCopyAddress:
            CmdCopyAddressL( aEvent );
            break;

        case ELogsCmdDeviceCallKey:              //Special device key pressed to make a call 
        case ELogsCmdSendKeyCall:                //or Send key pressed to make a call
        case ELogsCmdLongSendKeyCall:        
        case ELogsCmdSingleTapCall:              //or touch tapping to make a call
            {
            // Reset focus when calling from dialled calls view.       
            if( iModel == ELogsDialledModel) 
                {         
                iFocusChangeControl = ELogsOkToChange;
                }
            CmdAiwSrvCmdL( aCommandId, aEvent );            
            break;
            }
            
        case ELogsCmdShowMyAddress:
            CmdShowMyAddressL( aEvent );
            break;
            
        case ELogsCmdMenuSendUi:                 // Cmd assigned for SendUI submenu processing (SMS, MMS)
        case ELogsCmdToolBarSendUi:              // or toolbar send message button pressed 
            SendMessageCmdHandlerL( aCommandId, aEvent );
            break;

        case ELogsCmdAiwCmdAssign:
            iFocusChangeControl = ELogsNoChange_PbkUpdPending; //Prevent focus moving to top when list is reread         
            CmdSaveToVirtualPhonebookL( KAiwCmdAssign, EFalse, aEvent ); 
            break;
           
        case ELogsCmdHandleTapFocusChange:   
           SetToolbarItems( aEvent );
           Cba()->MakeCommandVisible( 
               EAknSoftkeyContextOptions, 
               IsOkToShowContextMenu( aEvent ));
           break;                      
            
        default:            
            //If commandId is in range allocated to AIW, forward it to AIW for processing.
            //commandId's allocated for AIW must not overlap with the other menu ids.
            //Note, that there are some command id's allocated on top of ELogsCmdLast in PECLogs extension.
            //However, these commands are already handled in CLogsRecentListView ReceiViewExtension->HandleCommandL(aCommandId)), 
            //no extension commands left here. Therefore ELogsCmdLast is ok  instead of EDpbCmdLas of extension.
            if( aCommandId >= ELogsCmdLast ) 
                {
                CmdAiwMenuCmdL( aCommandId, aEvent );
                }
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::SetToolbarState
// ----------------------------------------------------------------------------
//
void CLogsBaseView::SetToolbarState(TLogsShowToolbar aShowToolbar, TBool aSetRect) 
	{
	TRAPD( err, SetToolbarStateL( aShowToolbar, aSetRect ) );  
	if( err ) 
		{
		// Only possible leave cause is KErrToolbarShown if 
		// DisableToolbarL is called when toolbar is shown. 
		}
	}

// ----------------------------------------------------------------------------
// CLogsBaseView::SetToolbarStateL
// ----------------------------------------------------------------------------
//
void CLogsBaseView::SetToolbarStateL(TLogsShowToolbar aShowToolbar, TBool aSetRect)
	{
    if (AknLayoutUtils::PenEnabled() )
        {
        CAknToolbar* toolbar = Toolbar();
        if (toolbar)
            {  
            if (aShowToolbar == ELogsToolbarOn  &&  
                iToolBarState != ELogsHideItemsAndDrawOnlyBackground &&
                iToolBarState != ELogsUnHideItemsAndDrawOnlyBackground )
                {
                if ( toolbar->IsToolbarDisabled() && !toolbar->IsShown() )
                    {
                    // Enable toolbar and
                    toolbar->DisableToolbarL(EFalse);     
                    }
                // set visibility on 
                toolbar->SetToolbarVisibility( ETrue, EFalse);
                iToolBarState = aShowToolbar;
                
                if (aSetRect)
                    {
                    HandleClientRectChange();
                    DrawComponents();
                    }
                }  
            else if (aShowToolbar == ELogsToolbarOff && iToolBarState != ELogsHideItemsAndDrawOnlyBackground)
                {
                // Set toolbar visibility off and disable toolbar              
                toolbar->SetToolbarVisibility( EFalse, EFalse);
                toolbar->DisableToolbarL(ETrue);  
                iToolBarState = aShowToolbar;
                }   
            
            else if (aShowToolbar == ELogsHideItemsAndDrawOnlyBackground)
                {  
                toolbar->HideItemsAndDrawOnlyBackground(ETrue);
                iToolBarState = aShowToolbar;
                }
            else if (aShowToolbar == ELogsUnHideItemsAndDrawOnlyBackground)
                {       
                toolbar->HideItemsAndDrawOnlyBackground(EFalse);
                iToolBarState = aShowToolbar;
                if (aSetRect)
                    {
                    HandleClientRectChange();
                    DrawComponents();
                    }
                }
            }        
        }    
	}

// ----------------------------------------------------------------------------
// CLogsBaseView::HandleClientRectChange
// ----------------------------------------------------------------------------
//
void CLogsBaseView::HandleClientRectChange()
    {
    // base class has no implementation.
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::DrawComponents
// ----------------------------------------------------------------------------
//
void CLogsBaseView::DrawComponents()
    {
    // base class has no implementation.
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::ProcessCommandL
//
// Called from FW when e.g. cba pressed
// ----------------------------------------------------------------------------
//
void CLogsBaseView::ProcessCommandL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;
    // Close popup menu if open
    MenuBar()->StopDisplayingMenuBar();

    //If MSK-press, convert it ELogsCmdHandleMSK in order to be forwarded to
    //HandleCommandL for possible processing
    if ( aCommand == EAknSoftkeyContextOptions )
        {
        aCommand = ELogsCmdHandleMSK;
        }

    // Call base class calls HandleCommandL 
    CAknView::ProcessCommandL( aCommand );
    
    //Store last executed command
    if( aCommand >= ELogsCmdLast && iServHandlerRef) 
        {
        iPreviousCommand = iServHandlerRef->ServiceCmdByMenuCmd( aCommand );
        }
    else
        {
        iPreviousCommand = aCommand;
        }
      
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::HandleMenuRefreshL
//
// Checks wether the options menu needs to be refreshed. This is needed when 
// options menu was opened before events were populated. Called from 
// CLogsRecentListView::StateChangedL and CLogsEventListView::StateChangedL 
// ----------------------------------------------------------------------------
//
void CLogsBaseView::HandleMenuRefreshL(const MLogsEventGetter* aCurrentEvent) 
    {
    // Will be called when reading is finished. In some scenarios could be called twice
    // so to avoid flickering the flag RefreshMenuOnUpdate is used.
    if (MenuBar()->IsDisplayed() && RefreshMenuOnUpdate( aCurrentEvent ) ) 
        {
        MenuBar()->StopDisplayingMenuBar();
        MenuBar()->TryDisplayMenuBarL();
        // After the refresh, set the conditional switch off. 
        iRefreshMenuOnUpdate = EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::SetRefreshMenuOnUpdate
// ----------------------------------------------------------------------------
//  
void CLogsBaseView::SetRefreshMenuOnUpdate( const MLogsEventGetter* aCurrentEvent )
    {
    if (aCurrentEvent)
        {
        iEventUidWhenMenuOpened = aCurrentEvent->EventUid().iUid;
        }
    else
        {
        iEventUidWhenMenuOpened = KErrNotFound;
        }
    
    iRefreshMenuOnUpdate = ETrue;
    }
    
// ----------------------------------------------------------------------------
// CLogsBaseView::RefreshMenuOnUpdate
// ----------------------------------------------------------------------------
//  
TBool CLogsBaseView::RefreshMenuOnUpdate(const MLogsEventGetter* aCurrentEvent) 
    {
    TBool refreshMenu(EFalse);
    
    if (iRefreshMenuOnUpdate)
        { 
        if (aCurrentEvent)
            {
            if (iEventUidWhenMenuOpened != aCurrentEvent->EventUid().iUid) 
                {
                refreshMenu = ETrue;
                }
            }
        else if (iEventUidWhenMenuOpened != KErrNotFound)
            {
            refreshMenu = ETrue;
            } 
        }
    
    return refreshMenu;
    }
  

// ----------------------------------------------------------------------------
// CLogsBaseView::HandleListBoxEventL
//
// If touch support is on, we handle the touch related events here.
// ----------------------------------------------------------------------------
//
void CLogsBaseView::HandleListBoxEventL(
    CEikListBox* aListBox,
    TListBoxEvent aEventType)
    { 
    TRACE_ENTRY_POINT;
    
    LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
            ( "CLogsBaseView::HandleListBoxEventL - aEventType:%d"), aEventType);
    
    if( AknLayoutUtils::PenEnabled() )  
        {
        switch ( aEventType )
            {     
            case EEventItemSingleClicked:
                {
                ProcessPointerEventL(aListBox->CurrentItemIndex());
                break;
                }
            
            case EEventPanningStarted:                         
            case EEventPanningStopped:                       
            case EEventFlickStarted:                 
            case EEventFlickStopped:                       
            case EEventPenDownOnItem:
            case EEventItemDraggingActioned:     
                {
                HandleCommandL( ELogsCmdHandleTapFocusChange );    //Calls subclass impl if available            
                break;
                }
                
            case EEventEnterKeyPressed:
                {
                TInt commandId( Cba()->ButtonGroup()->CommandId(
                        CEikButtonGroupContainer::EMiddleSoftkeyPosition ) );
                ProcessCommandL( commandId );
                break;
                }
            default:
               break;
            }
      	}
      	
    TRACE_EXIT_POINT;  	
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::EventListCurrent
// ----------------------------------------------------------------------------
//
TInt CLogsBaseView::EventListCurrent() const
    {
    return iEventListCurrent;
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::EventListTop
// ----------------------------------------------------------------------------
//
TInt CLogsBaseView::EventListTop() const
    {
    return iEventListTop;
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::SetEventListCurrent
// ----------------------------------------------------------------------------
//
void CLogsBaseView::SetEventListCurrent( TInt aIndex )
    {
    iEventListCurrent = aIndex;
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::SetEventListTop
// ----------------------------------------------------------------------------
//
void CLogsBaseView::SetEventListTop( TInt aIndex )
    {
    iEventListTop = aIndex;
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::DynInitMenuPaneL
// ----------------------------------------------------------------------------
//
void CLogsBaseView::DynInitMenuPaneL(
    TInt /* aResourceId */,
    CEikMenuPane* aMenuPane )
    {
    //First check have we already done construction fully. If not, then we first need to do it. 
    ConstructDelayedL( EFalse );   //EFalse: perform immediately, ETrue: perform using idle time
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
    }
        
// ----------------------------------------------------------------------------
// CLogsBaseView::DynInitMenuPaneEventL
// ----------------------------------------------------------------------------
//
void CLogsBaseView::DynInitMenuPaneEventL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane,
    const MLogsEventGetter* aEvent )
    {
    TRACE_ENTRY_POINT;
    
    CLogsBaseView::DynInitMenuPaneL( aResourceId, aMenuPane );//Don't call subclass again
    
    LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
            ( "CLogsBaseView::DynInitMenuPaneEventL - iServiceHandler: 0x%x"), iServHandlerRef);

    //AIW knows its own submenu hooks, so we can return from here if AIW handled this.
    if( iServHandlerRef->HandleSubmenuL(*aMenuPane) || iServHandlerRef->HandleSubmenuL(*aMenuPane) )
        {
        return;
        }
        
    //Note that aEvent may NOT yet be available if asynch request(s) are not completed. 
    TBool phoneNumberAvailable = PhoneNumberAvailable( aEvent );
    TBool sipUriAvailable = SipUriAvailable( aEvent );
    TBool isEmergencyCall = EFalse;    

    if( aEvent && aEvent->EventType() == ETypeEmerg ) 
        {
        isEmergencyCall = ETrue;    
        }

    //Intitialise menupane resources
    if( aResourceId == R_COMMON_EVENT_MENU_DELETE_EVENTS )
        {        
        // If no event data available, remove all event dependent options and exit
        if( !aEvent )        
            {            
            aMenuPane->DeleteMenuItem( ELogsCmdMenuDelete );
            aMenuPane->DeleteMenuItem( ELogsCmdMenuDeleteAll );
            return;
            }      
        }   //R_COMMON_EVENT_MENU_DELETE_EVENTS

    else if( aResourceId == R_COMMON_EVENT_MENU_EDIT_CALLS )                //Rest of Stm main menu below CallUi and PoC parts
        {        
        // If no event data available, remove all event dependent options and exit
        if( !aEvent )        
            {            
            aMenuPane->DeleteMenuItem( ELogsCmdMenuNumberSubMenu );          //Edit and copy phone number
            aMenuPane->DeleteMenuItem( ELogsCmdMenuPocAddressSubMenu );
            aMenuPane->DeleteMenuItem( ELogsCmdMenuVoipAddressSubMenu );                        
            return;
            }      

        //We have at least one event available on the list                              

        //Menu item for voice calls
        if( !phoneNumberAvailable )
            {
            aMenuPane->DeleteMenuItem( ELogsCmdMenuNumberSubMenu );          //Edit and copy phone number
            }

        /* FIXME: The below should not be needed as when both available we prefer showing/using msisdn. However,
           if available, it's good to provide also possibility for user to see sip-uri in addition to msisdn
        if( phoneNumberAvailable && sipUriAvailable )
            {
            // If both available (Phone number and SIP uri) show use address.
            aMenuPane->DeleteMenuItem( ELogsCmdMenuNumberSubMenu );
            }
        */
        
        //Menu items for PoC and Voip calls
        if( !sipUriAvailable )
            {
            aMenuPane->DeleteMenuItem( ELogsCmdMenuPocAddressSubMenu );
            aMenuPane->DeleteMenuItem( ELogsCmdMenuVoipAddressSubMenu );  
            }
        else            
            {
            if( !aEvent->LogsEventData()->PoC() )
                {                                                                            
                aMenuPane->DeleteMenuItem( ELogsCmdMenuPocAddressSubMenu );
                }

            if( !aEvent->LogsEventData()->VoIP() )
                {
                aMenuPane->DeleteMenuItem( ELogsCmdMenuVoipAddressSubMenu );
                }                
            }
        }    //R_COMMON_EVENT_MENU_EDIT_CALLS

    else if( aResourceId == R_COMMON_CALLUI_SUBMENU )                       //1. CallUi menu 
        {
        if( !aEvent || ( !phoneNumberAvailable &&                           //If event data not (yet) read, no MSISDN or no voip-uri,
                        ( !aEvent->LogsEventData()->VoIP() ||               //provide no call option
                          !sipUriAvailable )))                 
            {
            aMenuPane->DeleteMenuItem( KAiwCmdCall );                       //This menuitem is provided by AIW/CallUi    
            }
        else 
            {
            DynInitAiwCallUiMenuPaneL( aResourceId, aMenuPane, ETrue, aEvent );//Init voice&video call option in AIW menu                
            }
        }   //R_COMMON_CALLUI_SUBMENU
        
    else if( aResourceId == R_COMMON_POC_SUBMENU )                          //2. PoC menu 
        {
        if( !aEvent ||                                                      //If event data not (yet) read or uri for voip,
            aEvent->LogsEventData()->VoIP() ||                              //provide no Poc option. However if msisdn, poc
            ( !sipUriAvailable && !phoneNumberAvailable ) ||                //is ok. 
            isEmergencyCall )                                               //No Poc for emergency calls even if msisdn.
            {
            aMenuPane->DeleteMenuItem( KAiwCmdPoC );                        //This menuitem is provided by AIW/PocUi                            
            }
        else 
            {
            DynInitAiwPocMenuPaneL( aResourceId, aMenuPane, aEvent );       //Init (or remove) PoC option in AIW menu                
            }
        }   //R_COMMON_POC_SUBMENU

    else if( aResourceId == R_COMMON_SEND_MESSAGE_SUBMENU )                 //3. Send message menu 
        {
        //Add send message item if phone number available
        if( !phoneNumberAvailable ||  
            isEmergencyCall )                                               //Not for emergency calls.
            {
            aMenuPane->DeleteMenuItem( ELogsCmdMenuSendUi );      
            }
         else
            {
            TInt pos = 0;                                                   //Add send message item to menu above position pos                 
            aMenuPane->ItemAndPos( ELogsCmdMenuSendUi, pos );                //Returns pointer to menu item 
            aMenuPane->DeleteMenuItem( ELogsCmdMenuSendUi );                 //Delete marker item from menu      
            TSendingCapabilities capabilities(0, 0, 0 );                    //No need here yet to tailor subitems in SendUi menu            
            LogsAppUi()->SendUiL()->AddSendMenuItemL(*aMenuPane, 
                                                     pos,                   //Position in menupane
                                                     ELogsCmdMenuSendUi,     //Command id to be used for "Send" menu item
                                                     capabilities );
            aMenuPane->SetItemTextL( ELogsCmdMenuSendUi, iSendUiText->Des() );
            aMenuPane->SetItemSpecific( ELogsCmdMenuSendUi, ETrue );
            }
        }

    else if( aResourceId == R_COMMON_SAVETOPBK_SUBMENU )                    //4. SaveToPbk menu 
        {
        if( (!phoneNumberAvailable && !sipUriAvailable) || 
             isEmergencyCall )                                              //Not for emergency calls.
            {
            //Remove Phonebook's 'Copy to phonebook' submenu          
            aMenuPane->DeleteMenuItem( KAiwCmdAssign );         //NEW impl            
            }
        else            
            {

            //Add Phonebook's 'Copy to phonebook' submenu             
            DynInitAiwSaveToPbkMenuPaneL( aResourceId, aMenuPane, aEvent, phoneNumberAvailable, sipUriAvailable );
            }
        }
        
    else if( aResourceId == R_COMMON_SHOW_MY_ADDRESS_MENU )                       
        {
        if( !aEvent || aEvent->LogsEventData()->MyAddress().Length() <= 0 )                                      
            {
            aMenuPane->DeleteMenuItem( ELogsCmdShowMyAddress );              //User's own voip adress
            }
        }
   
    TRACE_EXIT_POINT;        
    }
    

// ----------------------------------------------------------------------------
// CLogsBaseView::SipUriAvailable
// ----------------------------------------------------------------------------
//
TBool CLogsBaseView::SipUriAvailable( const MLogsEventGetter* aEvent )
    {  
    if( !aEvent || 
         aEvent->LogsEventData()->Url().Length() <= 0 ||
         aEvent->LogsEventData()->Url().Length() > KLogsSipUriMaxLen ) //exeeds maxlen for buffers etc
        {
        return EFalse;  //no sip uri
        }

    // Sip uri exists. However, if it equals to phonenumber, and it seems to
    // be a valid phonenumber we don't consider a sip uri
    // to exist but prefer msisdn number.        
    else if (PhoneNumberAndSipAreEqual(aEvent)   &&
    		 CommonPhoneParser::IsValidPhoneNumber( 
    				 	aEvent->Number()->Des(),
    					CommonPhoneParser::EPlainPhoneNumber ) )
    	{
    	// SIP URI and number are equal and number is a 
    	// valid phonenumber, so consider SIP URI not to exist
    	return EFalse;  	
    	}

    //We have a unique sip uri
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::PhoneNumberAndSipAreEqual
// ----------------------------------------------------------------------------
//
TBool CLogsBaseView::PhoneNumberAndSipAreEqual(const MLogsEventGetter* aEvent)
	{       
    TBuf<KLogsSipUriMaxLen> buf;
	    
	if( Engine()->ConvertToUnicode( aEvent->LogsEventData()->Url(), buf) == KErrNone && 
	    aEvent->Number())        
		{
	    return ( *aEvent->Number() == buf );  
	    }
	else 
		{
		return EFalse;
		}	    	
	}

// ----------------------------------------------------------------------------
// CLogsBaseView::PhoneNumberAvailable
// ----------------------------------------------------------------------------
//
TBool CLogsBaseView::PhoneNumberAvailable( const MLogsEventGetter* aEvent )
    {
    TBool numberAvailable = ETrue;
    
    // First check the number exists and all normal tests are passed.
    if( !aEvent || !aEvent->Number()
        || aEvent->Number()->Des().Length() > KLogsPhoneNumberMaxLen
        || aEvent->EventType() == ETypeUnknown 
        || aEvent->EventType() == ETypePrivate 
        || aEvent->EventType() == ETypePayphone
        || aEvent->EventType() == ETypeSAT                   //SAT: MO call/SMS without logged phonenumber (or URL) 
        || aEvent->EventUid()  == KLogPacketDataEventTypeUid //Just in case would wrongly contain msisdn
        || aEvent->EventUid()  == KLogsEngWlanEventTypeUid   //Just in case would wrongly contain msisdn
        || aEvent->EventUid()  == KLogDataEventTypeUid       //No msisdn operations for data call 
        || aEvent->EventUid()  == KLogFaxEventTypeUid )      //No msisdn operations for fax (this may change later
        {                                                    //if sending fax from sendui is provided)
        numberAvailable = EFalse;      
        }
 
    // Phonenumber exists. However, if it equals to SIP URI, and it is not
    // a valid phonenumber we don't consider a phonenumber to exist.
    // Instead the SIP URI is preferred.  
    else if (PhoneNumberAndSipAreEqual(aEvent)   &&
        	 !CommonPhoneParser::IsValidPhoneNumber( 
        			 aEvent->Number()->Des(),
        			 CommonPhoneParser::EPlainPhoneNumber ) )  
    	{
    	// SIP URI and number are equal and number is not a 
    	// valid phonenumber, so consider phonenumber not to exist
    	numberAvailable = EFalse;     
   	    }
    
    return numberAvailable;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////




// ----------------------------------------------------------------------------
// CLogsBaseView::DynInitAiwCallUiMenuPaneL
//
// Provide CallUI AIW functionality for making voice, video and VoIP calls
// ----------------------------------------------------------------------------
//
void CLogsBaseView::DynInitAiwCallUiMenuPaneL( 
    TInt aResourceId,
    CEikMenuPane* aMenuPane,
    TBool aVideo,
    const MLogsEventGetter* aEvent )
    {
    TRACE_ENTRY_POINT;
    CAiwGenericParamList& paramList = iServHandlerRef->InParamListL();

    //Check do we provide only voice call (i.e. skip video call option)
    if ( !aVideo ) 
        {
        TAiwVariant variant;
        TAiwGenericParam param( EGenericParamPhoneNumber, variant );
        paramList.AppendL( param );
        }
        
    //Check do we provide VoiP call option     

    //FIXME: the idea of the commented if instead of the if below was to remove voip from callui when there's
    //no voip activated in phone. However, in this case there wouldn't be any voip events either.
    //So this would unnecessarily exclude voip calls in which both sip-uri and msisdn is available.
    //if( aEvent && aEvent->LogsEventData()->VoIP() && SipUriAvailable( aEvent ) && !aEvent->Number() )
 
// Sawfish VoIP changes  >>>>
    TPtrC8 tempPtr( KNullDesC8 );
    TInt result;
    CLogsCntLinkChecker* contactCheckerPtr;
    result = aEvent->LogsEventData()->GetContactLink( tempPtr );
    
    if ( KErrNone == result )
        {   
        contactCheckerPtr = Engine()->CntLinkCheckerL();
        contactCheckerPtr->SetObserver( this );
        if ( !contactCheckerPtr->IsSameContactLink( tempPtr ) )
            {
            // If current contact link is not the last one,
            // need to reconfig CntLink checker.
            iIsCheckedCntLinkInvaild = EFalse;
            contactCheckerPtr->Stop(); 
            MLogsReaderConfig* readerConfig = CLogsReaderConfigFactory::
                    LogsReaderConfigLC( NULL, NULL, &tempPtr ); 
            contactCheckerPtr->ConfigureL( readerConfig );
            CleanupStack::PopAndDestroy();      //readerConfig
            contactCheckerPtr->StartL();
            
            //if ( !MenuBar()->ItemSpecificCommandsEnabled() )
            //    {
            //    iIsCheckedCntLinkInvaild = !contactCheckerPtr->IsCntLinkValidSync( tempPtr );
            //    }

            }
        }
    
    if ( aEvent 
         && aEvent->LogsEventData()->VoIP() 
         && SipUriAvailable( aEvent ) 
         && ( ( KErrNotFound == aEvent->LogsEventData()->GetContactLink( tempPtr ) 
              && !tempPtr.Length() ) 
              || iIsCheckedCntLinkInvaild ) )
// <<<<  Sawfish VoIP changes  
        {
        // If contact link is invalid, provide internet call only.
        TAiwVariant variant;
        TAiwGenericParam param( EGenericParamSIPAddress, variant );
        paramList.AppendL( param );
        }
        
    //Handle runtime variation of VIDEOCALL_MENU_VISIBILITY flag
    VideoCallMenuVisibilityL(*aMenuPane);  
    
    //Add requested menu options to menu pane
    iServHandlerRef->InitializeMenuPaneL(    
        *aMenuPane,    //Handle of menu pane to initialise                                                  
        aResourceId,   //MenuResourceId                                                                     
        KAiwCmdCall,   //Base ID for the handler to generate menu IDs for placeholders                      
        paramList );   //input parameter list for provider's parameters checking                  
    
    TRACE_EXIT_POINT;    
    }

// --------------------------------------------------------------------------
// CLogsBaseView::VideoCallMenuVisibilityL
// 
// Handles runtime variation of __VIDEOCALL_MENU_VISIBILITY feature flag 
// i.e. Video call and Voice call command lifted to the main menu level
// instead of a submenu. Disaples\keeps the cascade flag.
// 
// See error: JKLN-76XBYX  
// --------------------------------------------------------------------------
//
void CLogsBaseView::VideoCallMenuVisibilityL( CEikMenuPane& aMenuPane ) const
    { 
    // Set to true if context menu is opened and if VOIP support is ON.
    // Avoids more than 4 entries in the context sensitive menu 
    TBool contextMenuWithVoip = EFalse;
    
    if (FeatureManager::FeatureSupported(KFeatureIdCommonVoip) && 
          (CurrentMenuType() == CEikMenuBar::EMenuContext) )
        {
        contextMenuWithVoip = ETrue;
        }
        
    if ( FeatureManager::FeatureSupported(KFeatureIdVideocallMenuVisibility) &&
         (contextMenuWithVoip == EFalse ) )
        {          
        TInt index = KErrNotFound;
        if ( aMenuPane.MenuItemExists( KAiwCmdCall, index ) &&
             index != KErrNotFound )
            {
            CEikMenuPaneItem::SData& itemData =
                aMenuPane.ItemDataByIndexL( index );
            const TInt KDisableCascade = 0;
            itemData.iCascadeId = KDisableCascade;  // disable cascade menu
            }
                 
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::CurrentMenuType
// ----------------------------------------------------------------------------
//      
CEikMenuBar::TMenuType CLogsBaseView::CurrentMenuType() const
    {
    return iMenuType;
    }
     
// ----------------------------------------------------------------------------
// CLogsBaseView::SetCurrentMenuType
// ----------------------------------------------------------------------------
// 
void CLogsBaseView::SetCurrentMenuType(CEikMenuBar::TMenuType aMenuType)
    {
    iMenuType = aMenuType;   
    }
        
// ----------------------------------------------------------------------------
// CLogsBaseView::DynInitAiwPocMenuPaneL
//
// Let AIW initialise menu for making PoC calls
// ----------------------------------------------------------------------------
//
void CLogsBaseView::DynInitAiwPocMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane,
    const MLogsEventGetter* aEvent )
    {
    TRACE_ENTRY_POINT;
    //Create CAiwGenericParamList for menu initialisation.
    TAiwPocParameterData pocParameter;
    pocParameter.iCommandId = 0;//Default, not used with menu construction.
    pocParameter.iPreferredMenu = KAiwPoCMenuDataBased; 
    TBool isParamListForPocOk( EFalse );
    CAiwGenericParamList& paramList = GetAiwGenericParamListForPocL( aEvent, pocParameter, isParamListForPocOk );

    // Let provider add it's menu items to menu only if we got sensible paramList contents
    if( isParamListForPocOk ) 
        {
        iServHandlerRef->InitializeMenuPaneL(
            *aMenuPane,
            aResourceId,
            KAiwCmdPoC,  
            paramList );
        }
     else 
        {
        aMenuPane->DeleteMenuItem( KAiwCmdPoC );         
        }
    TRACE_EXIT_POINT;    
    }


// ----------------------------------------------------------------------------
// CLogsBaseView::GetAiwGenericParamListForPocL
//
// Initialises AiwGenericParamList for Poc menu and Poc call functions. This is
// used for DynInitMenuPane and AIW excecution preparations.
// ----------------------------------------------------------------------------
//
CAiwGenericParamList& CLogsBaseView::GetAiwGenericParamListForPocL(
    const MLogsEventGetter* aEvent,         //In:  Event containing source data
    TAiwPocParameterData& aPocParameter,    //In:  PocParameter data 
    TBool& aIsParamListForPocOk,            //Out: Retuns ETrue if parameter can be used    
	const TDesC& aOverrideUsingThis,        //In:  Use this number/uri preferred instead of what is in aEvent
	TBool aOverrideIsNumber )               //In:  Type of string in aOverrideUsingThis
    {
    TRACE_ENTRY_POINT;
    
    __ASSERT_ALWAYS( aEvent != NULL, User::Leave( KErrArgument ) );

    TInt type( aEvent->LogsEventData()->Type() ); //PoC app uses type for selecting 1-to-1, group call etc     

    if( !( type >= EPoCNoOpTypeId && type < EEndOfPoCOpTypeIdRange )  )
        {
        User::Leave( KErrArgument );
        }
    
    CAiwGenericParamList& params = iServHandlerRef->InParamListL(); //Ownership in Service Handler 
    aPocParameter.iConsumerAppUid = TUid::Uid(KLogsAppUID3);
    aPocParameter.iConsumerWindowGroup = CCoeEnv::Static()->RootWin().Identifier();
    aPocParameter.iLogId = aEvent->LogId();       //PoC app uses LogId to map Log db entry to it's own log data
    aPocParameter.iLogOperation = TPoCOperationTypeId( type );

    //PoC parameter data always included
    TAiwGenericParam param = TAiwGenericParam( EGenericParamPoCData,
                                               TAiwVariant( TAiwPocParameterDataPckg(aPocParameter)));
    params.AppendL(param);            
    TAiwGenericParam addressParam;
    TBuf<KLogsSipUriMaxLen> buf;
    aIsParamListForPocOk = ETrue;

    if( aOverrideUsingThis != KNullDesC )
        {
        //Use provided nbr or address instead what is in aEvent
        addressParam = TAiwGenericParam( 
            aOverrideIsNumber ? EGenericParamPhoneNumber : EGenericParamSIPAddress, 
            aOverrideUsingThis );
        }
    else if( SipUriAvailable( aEvent ) )
        {
        // We prefer sip-uri over msisdn if both are available (if is uri & msisdn, Poc has made
        // "best guess" for corresponding msisdn for possible subsequent cs calls.
        User::LeaveIfError( Engine()->ConvertToUnicode( aEvent->LogsEventData()->Url(), buf) );
        addressParam = TAiwGenericParam( EGenericParamSIPAddress, buf );
        }
        
    else if( PhoneNumberAvailable( aEvent ) )  
        {
        // Use msisdn only if uri not available
        addressParam = TAiwGenericParam( EGenericParamPhoneNumber, aEvent->Number() );  
        }
    else
        {
        //ParamListForPoc is not ok, so it cannot be used.
        aIsParamListForPocOk = EFalse;
        }

    params.AppendL( addressParam ); //Takes own copy of addressParam
    
    TRACE_EXIT_POINT;  
    return params;
    }


// ----------------------------------------------------------------------------
// CLogsBaseView::DynInitAiwSaveToPbkMenuPaneL
//
// Provide SaveToPbk AIW functionality 
// ----------------------------------------------------------------------------
//
void CLogsBaseView::DynInitAiwSaveToPbkMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane,
    const MLogsEventGetter* aEvent,
    TBool aPhoneNumberAvailable,
    TBool aSipUriAvailable )
    {
    TRACE_ENTRY_POINT;  
    using namespace AiwContactAssign;    

    CAiwGenericParamList& inParamList = iServHandlerRef->InParamListL();

    //Set flags to zero, otherwise provides only "Assing to" menu option.
    TUint assignFlags = 0;  //TAssignFlags in AiwContactAssignDataTypes.h 
    inParamList.AppendL(
        TAiwGenericParam(
            EGenericParamContactAssignData,     //AiwGenericParam.hrh
            TAiwVariant(TAiwSingleContactAssignDataV1Pckg( 
                TAiwSingleContactAssignDataV1().SetFlags(assignFlags)))));                    

    if( aPhoneNumberAvailable )
        {
        inParamList.AppendL(
            TAiwGenericParam(
                EGenericParamPhoneNumber,
                TAiwVariant( *(aEvent->Number()) )
                ));
        }

    if( aSipUriAvailable )
        {
        TBuf<KLogsSipUriMaxLen> buf;    
        if( Engine()->ConvertToUnicode( aEvent->LogsEventData()->Url(), buf) == KErrNone )        
            {
            inParamList.AppendL(
                TAiwGenericParam(
                    EGenericParamSIPAddress,
                    TAiwVariant( buf )
                    ));
            }
        }

   // Let provider add it's menu items to the menu
  iServHandlerRef->InitializeMenuPaneL(
      *aMenuPane,
      aResourceId,
      KAiwCmdAssign,
      inParamList);    
    TRACE_EXIT_POINT;   
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::CurrentModel
// ----------------------------------------------------------------------------
//
MLogsModel* CLogsBaseView::CurrentModel()
    {
    return Engine()->Model( iModel );
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::SendMessageCmdHandlerL
// ----------------------------------------------------------------------------
//
void CLogsBaseView::SendMessageCmdHandlerL( 
    TInt  aCommandId ,
    const MLogsEventGetter* aEvent )
    {   
    
    TBool isRead = aEvent->Event()->IsRead();
    TLogId logid = aEvent->LogId();
    TInt duplicates = aEvent->Duplicates();
    
    //Check that event and phone number is available
    TPtrC number( KNullDesC );
    if( aEvent && aEvent->Number() )
        {
        number.Set( *(aEvent->Number()) );
        }
    else
        {
        return;    
        } 
    
    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );
    
    TBuf<KLogsPhoneNumberMaxLen> nbrBuff;        
    CPhoneNumberFormat::DTMFStrip( number, nbrBuff );
    
    //Append number and remote party (if available) to message data.
    if( aEvent->RemoteParty())
        {
        messageData->AppendToAddressL( nbrBuff, *(aEvent->RemoteParty()) );   //Nbr & rem party
        }
     else
        {
        messageData->AppendToAddressL( nbrBuff );                             //Only nbr
        }


    //We're are interested only in msg type that support writing messages.
    TSendingCapabilities capabilities(0,                                      //Max body size
                                      0,                                      //Max msg size (inc. attachments)
                                      TSendingCapabilities::ESupportsEditor );//Flags

    CArrayFixFlat<TUid>* servicesToDim = new (ELeave) CArrayFixFlat<TUid>( 5 );//Five services to dim
    CleanupStack::PushL( servicesToDim );
    

//*******************************************************
// QUICK & DIRTY: 
// In case there is a need to quickly fix the SendUI error for not dimming the 
// 'Mail for Exchange' option, we can do that excplictly with the following uids.
//    
//  TInt mailForEUid = 0x102826F8;
//  servicesToDim->AppendL( TUid::Uid(mailForEUid ) );
//*******************************************************

// In normal builds we can dim all the e-mail stuff with the 
// KSenduiTechnologyMailUid flag.
//"ESupportsEditor" takes care of removing infrared and bluetooth from menu, so no need to dim them 
#ifndef __IMAIL
    servicesToDim->AppendL( KSenduiTechnologyMailUid  );  //SendUiConsts.h
    servicesToDim->AppendL( KSenduiMtmPostcardUid    );       
#else
// In DCP builds we need to enable KSenduiMtmSmtpUid but 
// other email MTM's should be dimmed
// FIXME: we might need to dim more services.
    servicesToDim->AppendL( KSenduiMtmImap4Uid       ); //SendUiConsts.h
    servicesToDim->AppendL( KSenduiMtmPop3Uid        );
    servicesToDim->AppendL( KSenduiMtmSyncMLEmailUid );
    servicesToDim->AppendL( KSenduiMtmPostcardUid    );
#endif

// Sawfish VoIP changes  >>>>
    if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip))
        {
        servicesToDim->AppendL( KSenduiTechnologyIMUid );
        }
// <<<<  Sawfish VoIP changes  

	// Calling ShowTypedQueryAndSendL to show the write query. Different menu options are 
	// be shown depending on wether the RD_UNIFIED_EDITOR flag is defined ot not, but we don't
	// need care about the flag here.
    TUid uid;
    SetToolbarState(ELogsHideItemsAndDrawOnlyBackground, EFalse); 
    TRAPD(err,     
        if (aCommandId == ELogsCmdMenuSendUi)           // with ELogsCmdMenuSendUi show query
            {
            uid = LogsAppUi()->SendUiL()->ShowTypedQueryL(CSendUi::EWriteMenu, NULL, capabilities, servicesToDim );
            } 
        else if (aCommandId == ELogsCmdToolBarSendUi)   // with ELogsCmdToolBarSendUi dont show query
            {
            uid = KSenduiMtmUniMessageUid;    
            }
        LogsAppUi()->SendUiL()->CreateAndSendMessageL(uid, messageData);
        );
    
    SetToolbarState(ELogsUnHideItemsAndDrawOnlyBackground, ETrue);    
    User::LeaveIfError(err);
    ChangeTitlePaneTextToDefaultL();
   
    if (isRead == EFalse && uid != KNullUid && iModel == ELogsMissedModel)  // If not read, the New missed icon is shown. By setting event read
        {                  // the new missed icon is not shown when control comes back to Logs 
        Engine()->ClearNewMissedL()->SetNewMissedCallRead(logid, duplicates); 
        }
    CleanupStack::PopAndDestroy( servicesToDim );       
    CleanupStack::PopAndDestroy( messageData ); 
    
    //If missed list, clear now new missed calls counter
    ClearMissedCallNotificationsL();          
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::ChangeTitlePaneTextToDefaultL
// ----------------------------------------------------------------------------
//
void CLogsBaseView::ChangeTitlePaneTextToDefaultL()
    {
    // base class has no implementation
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::ClearMissedCallNotifications
//
// Clears missed call notifications for Voice and PoC
// ----------------------------------------------------------------------------
//
void CLogsBaseView::ClearMissedCallNotifications()
    {
     if( iModel == ELogsMissedModel )
        {
        TInt err;
        TRAP( err, ClearMissedCallNotificationsL() ); //Clear new missed calls counter  
        if( err ) 
            {
            CCoeEnv::Static()->HandleError( err );
            }             
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::ClearMissedCallNotificationsL
//
// Clears missed call notifications for Voice and PoC
// ----------------------------------------------------------------------------
//
void CLogsBaseView::ClearMissedCallNotificationsL()
    {
    if( iModel != ELogsMissedModel )
        {
        return;
        }

    //1. Reset cs and voip missed calls information
    Engine()->SharedDataL()->NewMissedCalls( ETrue );  //ETrue: clears the count of missed calls.
        
    //2. Reset missed Poc call info + notification (needs currently be done separately, 
    //   the ResetSoftNotificationL does not do this fully).
    Engine()->SystemAgentL()->ResetNewMissedPocCalls();   

#ifdef __OMA_POC   
    //   Clear missed poc call soft notification (no need to check are there any missed poc calls)
    TUid provUid = { KS60PoCAiwProviderImplUid };   
    
    CPoCUiIntegrationServices* services = CPoCUiIntegrationServices::NewLC( provUid );
    services->ResetSoftNotificationL( CPoCUiIntegrationServices::EPoCUiIntegrationMissedCall );
    // services->ResetSoftNotificationL( CPoCUiIntegrationServices::EPoCUiIntegrationCallbackRequest );
    CleanupStack::PopAndDestroy( services );       
#endif
    
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::CmdPrependCallL
//
// Provide the user option to edit the phone number or sip uri and make cellular, 
// voip or poc call using this number/uri.
// ----------------------------------------------------------------------------
//
void CLogsBaseView::CmdPrependCallL( 
    TBool aPreferNumber,             //if both uri and number, prefer number
    const MLogsEventGetter* aEvent ) 
    {
    TRACE_ENTRY_POINT;
    if ( !aEvent )
        {
        return;    
        }
        
    TInt length(0);
    TBuf<KLogsSipUriMaxLen> buf;               

    CAiwDialData::TCallType callTypeForVoice( CAiwDialData::EAIWVideo );       
    
    //Select uri or phone number to edit according to availability & preference
    if( ( aEvent->LogsEventData()->VoIP() || aEvent->LogsEventData()->PoC() ) && 
          SipUriAvailable( aEvent ) && !aPreferNumber )
        {
        
        callTypeForVoice = CAiwDialData::EAIWVoiP;           
      
        TInt ret = Engine()->ConvertToUnicode( aEvent->LogsEventData()->Url(), buf);
        if( ret != KErrNone && ret != KErrOverflow )        
            {
            return;    
            }
        length = buf.Length();  //Unicode characters            
        }
    else if ( aEvent->Number() )
        {
        buf = *(aEvent->Number());        
        length = buf.Length() ;
        }

    //Show to user for editing if proper number or uri available
    if( length && 
        ( (length <= KLogsPhoneNumberMaxLen) ||             //Max length for number
          (length <= KLogsSipUriMaxLen && !aPreferNumber) ) //Max length for uri
        )  
        {
        AknTextUtils::LanguageSpecificNumberConversion( buf );
        CLogsPrependQuery* query = CLogsPrependQuery::NewL( buf );
                       
        CAiwGenericParamList* paramList = NULL;
        TBool isParamListForPocOk( EFalse ); 
        
		if( aPreferNumber)  
	        {
	        query->PrepareLC( R_STM_USE_NUMBER_QUERY ); 
	        }
	    else
    	    {
    	    query->PrepareLC( R_STM_USE_ADDRESS_QUERY );
 			
 			if( aEvent->LogsEventData()->PoC() )
 				{		
 				TAiwPocParameterData pocParameter;
 						
// 				// Initialise parameter data to execute AIW command 
 				InitPocParameterL( pocParameter, aEvent );       //Other params from aEvent except the buf                   
 				paramList = &GetAiwGenericParamListForPocL( 
 			         	aEvent, 
 			         	pocParameter, 
 			         	isParamListForPocOk,
 			         	buf,
 			         	aPreferNumber );	
 			
				}
    	    }
		
        //Show number/address to edit it	        			
        TInt queryAction;
        queryAction = query->RunLD();
        
        AknTextUtils::ConvertDigitsTo( buf, EDigitTypeWestern ); 
        
        //Refresh contents in MLogsEventGetter. Otherwise it contains wrong data of last 
        //event read from db done in background during execution of RunLD()
        aEvent = CurrentModel()->At( iEventListCurrent );
        
        switch( queryAction )
        	{
        	case EAknSoftkeyCall:
        		if( isParamListForPocOk )
        			{
        			CmdPocCallL( KAiwCmdPoC, EFalse, *paramList );   //EFalse: Direct request                        
        			}
        		else
        			{
                    // EFalse: Direct request (not made from menu).                  
                    CmdCallL( 
                        KAiwCmdCall, 
                        EFalse, 
                        buf, 
                        callTypeForVoice,            
                        aEvent
                        );
                    }
                break;

            default:
                break;
            }
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::CmdAiwSrvCmdL
//
// Request an AIW-service from an AIW provider. The commandId relates to 
// Logs own commands.
// ----------------------------------------------------------------------------
//
void CLogsBaseView::CmdAiwSrvCmdL( 
    TInt aCommandId,
    const MLogsEventGetter* aEvent )
    {
    TRACE_ENTRY_POINT;
    
    if( !aEvent )
        {
        return; 
        }

    switch( aCommandId )
        {
        case ELogsCmdSingleTapCall:					//Touch tapping to make a call 
        case ELogsCmdSendKeyCall:                   //Send key pressing to make a call
        case ELogsCmdLongSendKeyCall:            
            //For PoC events make PoC call. Exception1: If phone number available, then 
            //for long presses attempt cs video call instead. Exception2: Long taps cause
            //always Poc calls for poc events.
            if( aEvent->LogsEventData()->PoC() && 
                !( aCommandId == ELogsCmdLongSendKeyCall &&   
                   PhoneNumberAvailable( aEvent ) ))
                {
                PreparePocCallL( KAiwCmdPoC,            
                                 EFalse,            //EFalse: Direct request (not made from menu)
                                 aEvent );
                }
            else
                {
                TBool forceVideo( EFalse );

                //For long press or single tap prefer video call but not for emergency calls
                if( (aCommandId == ELogsCmdLongSendKeyCall ||
                    aEvent->LogsEventData()->VT() &&
                    aCommandId == ELogsCmdSingleTapCall ) &&
                    aEvent->EventType() != ETypeEmerg )
                    {
                    forceVideo = ETrue;
                    }
                PrepareCallL( KAiwCmdCall, 
                              EFalse,               //EFalse: Direct request (not made from menu)
                              aEvent,
                              forceVideo );         //ETrue=force video call, EFalse=show voice/video query to user:
                }
            break;        
                
        case ELogsCmdDeviceCallKey:                 //Special device key pressed to make a call,
            PreparePocCallL( KAiwCmdPoC,            //only poc call is supported for this key.
                             EFalse,                //EFalse: Direct request (not made from menu).
                             aEvent );   
                             
            break;
        }
    TRACE_EXIT_POINT;    
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::CmdAiwMenuCmdL
//
// Forward an AIW-menu based command to corresponding AIW-provider. The commandId relates to 
// AIW's menu item already shown by AIW provider to user
// ----------------------------------------------------------------------------
//
void CLogsBaseView::CmdAiwMenuCmdL( 
    TInt aCommandId, 
    const MLogsEventGetter* aEvent )
    {
    TRACE_ENTRY_POINT;
    
    if( !aEvent || !iServHandlerRef )
        {
        return; 
        }

    TInt srvCmd = iServHandlerRef->ServiceCmdByMenuCmd( aCommandId );
    
    switch( srvCmd )    //AiwCommon.hrh
        {
        case KErrNotFound:    
            break;                

        case KAiwCmdCall:    
            PrepareCallL( aCommandId, ETrue, aEvent );    //Try to make voice call.
            break;                

        case KAiwCmdPoC:  
            PreparePocCallL( aCommandId, ETrue, aEvent ); //Try to make PoC call.
            break;   

        case KAiwCmdAssign:
            // prevent the model being activated twice: 
            // from HandleGainingForeground and HandleNotifyL  
            iFocusChangeControl = ELogsNoChange_PbkUpdPending;      
            CmdSaveToVirtualPhonebookL( aCommandId, ETrue, aEvent );
            break;
            
        default:
            // do nothing
            break;       
        }
    
     // Reset focus when calling from dialled calls view.       
     if( iModel == ELogsDialledModel && 
         ( srvCmd == KAiwCmdCall || srvCmd == KAiwCmdPoC ) ) 
        {         
        iFocusChangeControl = ELogsOkToChange;
        }    
        
    TRACE_EXIT_POINT;   
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::PreparePocCallL
//
// Prepares a PoC call.
// ----------------------------------------------------------------------------
//
void CLogsBaseView::PreparePocCallL( 
    TInt aCommandId, 
    TBool aCalledForMenu,
    const MLogsEventGetter* aEvent )                  
    {
    TBool isRead = aEvent->Event()->IsRead();
    TLogId logid = aEvent->LogId();
    TInt duplicates = aEvent->Duplicates();
    
    // Initialise PocParameterData for GenericParamList to execute AIW command    
    TAiwPocParameterData pocParameter;
    if ( aCalledForMenu )
        {
        pocParameter.iCommandId = aCommandId;
        }
    else
        {
        InitPocParameterL( pocParameter, aEvent );
        }    
    
    TBool isParamListForPocOk( EFalse );
    CAiwGenericParamList& paramList = GetAiwGenericParamListForPocL( aEvent, pocParameter, isParamListForPocOk );
    
    // Let's make call only if we got sensible paramList contents    
    if( isParamListForPocOk ) 
        {
        CmdPocCallL( aCommandId, aCalledForMenu, paramList );    
        }
    
    if (isRead == EFalse && iModel == ELogsMissedModel)  // If not read, the New missed icon is shown. By setting event read
        {                  // the new missed icon is not shown when control comes back to Logs 
        Engine()->ClearNewMissedL()->SetNewMissedCallRead(logid, duplicates); 
        }    
    }
    
// ----------------------------------------------------------------------------
// CLogsBaseView::InitPocParameterL
//
// Initialise PocParameterData.
// ----------------------------------------------------------------------------
//
TAiwPocParameterData& CLogsBaseView::InitPocParameterL( 
    TAiwPocParameterData& aPocParameter, 
    const MLogsEventGetter* aEvent )                  
    {
    TInt type( aEvent->LogsEventData()->Type() ); //PoC app uses type for selecting 1-to-1, group call etc     

    if( !( type >= EPoCNoOpTypeId && type < EEndOfPoCOpTypeIdRange )  )
        {
        User::Leave( KErrArgument );
        }
    
    aPocParameter.iLogOperation = TPoCOperationTypeId( type );
    aPocParameter.iCommandId = 0;

    // Check the type of the logged PoC call to be able to send correct 
    // command to PoC AIW. This is done because the call can be made from 
    // menu which always has an associated command and by PTT-key which
    // doesn't have a command.
    switch ( type )
        {
        case EPoC1to1MO: // Initiated 1-to-1 call.
        case EPoC1to1MT: // Received 1-to-1 call.
            {
            // This must be always set also for non menu-based 
            // PoC AIW service requests.
            aPocParameter.iCommandId = EAiwPoCCmdTalk1to1;
            break;
            }
        case EPoCAdhocDialoutGroupMO:   // Initiated dial-out call.
        case EPoCPredefDialoutGroupMO:  // Initiated predefined dial-out call.
            {
            aPocParameter.iCommandId = EAiwPoCCmdTalkMany;
            break;
            }
        case EPoCNoOpTypeId:            // When only msisdn available
            {
            aPocParameter.iCommandId = EAiwPoCCmdTalk1to1;
            break;
            }
        default:
        case EPoCCallbackRequestMO:
        case EPoCCallbackRequestMT:
        case EPoCAdhocDialoutGroupMT:
        case EPoCPredefDialoutGroupMT:
        case EPoCDialinGroupMO:
            {
            // No handling at the moment since above events are not
            // stored to system log. Maybe in future in OMA PoC these 
            // will be used aswell. Meanwhile use the 1-to-1 as default.
            aPocParameter.iCommandId = EAiwPoCCmdTalk1to1;
            break;
            }
        }

    __ASSERT_ALWAYS( aPocParameter.iCommandId > 0, User::Leave( KErrNotFound ) );
    
    return aPocParameter;
    }
    
// ----------------------------------------------------------------------------
// CLogsBaseView::CmdPocCallL
//
// Perform PoC call.
// ----------------------------------------------------------------------------
//
void CLogsBaseView::CmdPocCallL( 
    TInt aCommandId, 
    TBool aCalledForMenu,
    CAiwGenericParamList& aParamList )
    {
    if ( !aCalledForMenu )    
        {
        // Execute direct command to PoC AIW. In this case no 
        // Logs PoC AIW menu is displayed; call is intiated
        // using the PTT-key.
        iServHandlerRef->ExecuteServiceCmdL(
            aCommandId, 
            aParamList,
            iServHandlerRef->OutParamListL(),
            0,
            NULL ); // Callback is NULL as no dial results wanted.
        }
    else
        {
        // Execute Menu based command to PoC AIW. The Logs PoC AIW menu
        // has already displayed to user and user has selected a command
        // from the menu.
        iServHandlerRef->ExecuteMenuCmdL(
            aCommandId,
            aParamList,
            iServHandlerRef->OutParamListL(),
            0,
            NULL ); // Callback is NULL as no dial results wanted.
        }
        
    //If missed list, clear now new missed calls counter
    ClearMissedCallNotificationsL();          
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::PrepareCallL
//
// Perform Voice, video or VoIP call.
// ----------------------------------------------------------------------------
//
void CLogsBaseView::PrepareCallL( 
    TInt aCommandId, 
    TBool aCalledForMenu,
    const MLogsEventGetter* aEvent,
    TBool aForceVideo )             //Forces video call for non-voip events
    {
    //Quit if no event, phone number or sip uri
    if( !aEvent || (!SipUriAvailable( aEvent ) && !PhoneNumberAvailable( aEvent ) ))
        {
        return;
        }

    TBool isRead = aEvent->Event()->IsRead();
    TLogId logid = aEvent->LogId();
    TInt duplicates = aEvent->Duplicates();

    CAiwDialData::TCallType callType( CAiwDialData::EAIWVoice );

    if( aEvent->LogsEventData()->VoIP() )
        {
        callType = CAiwDialData::EAIWVoiP;
        }
    else if( aForceVideo )
        {
        callType = CAiwDialData::EAIWForcedVideo;
        }
    else if( aEvent->LogsEventData()->VT() )
        {
        callType = CAiwDialData::EAIWVideo;
        }

    //Create temporary TBuf that is transferred to CmdCallL in case sip-uri. This is ok as no asynch operations in 
    //CmdCallL, so tmp is alive long enough. Also stack consumption should not be too big, so no need for heap allocation.
    TBuf<KLogsSipUriMaxLen> buf;    
    
    TPtrC tmp;    
    
    if( aEvent->LogsEventData()->VoIP() )
        {
        //In VoIP we prefer primarily try to use sip uri if available
        if( SipUriAvailable( aEvent ) )
            {
            if( Engine()->ConvertToUnicode( aEvent->LogsEventData()->Url(), buf) != KErrNone )     
                {
                return;
                }
            tmp.Set( buf ); // sip-uri
            }
        else
            {
            tmp.Set( *( aEvent->Number() ) );
            }
        }
     else
        {
        //For voice and video calls use number
        tmp.Set( *( aEvent->Number() ) );
        }
    CmdCallL( aCommandId,
              aCalledForMenu,
              tmp, // *(event->Number()) or sip-uri
              callType,
              aEvent
              );     
              
    if (isRead == EFalse && iModel == ELogsMissedModel)  // If not read, the New missed icon is shown. By setting event read
        {                  // the new missed icon is not shown when control comes back to Logs 
        Engine()->ClearNewMissedL()->SetNewMissedCallRead(logid, duplicates); 
        }    
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::CmdCallL
//
// Perform Voice, video or VoIP call.
// ----------------------------------------------------------------------------
//
void CLogsBaseView::CmdCallL( 
    TInt aCommandId, 
    TBool aCalledForMenu,         
    TPtrC aNumberOrUrl,
    CAiwDialData::TCallType aCallType,
    const MLogsEventGetter* aEvent    
    )
    {
    TRACE_ENTRY_POINT;  
    
    if( !aNumberOrUrl.Length() )
        {
        return;
        }
    
    LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
            ( "CLogsBaseView::CmdCallL - iServiceHandler: 0x%x"), iServHandlerRef);
                 
    // We have a phone number or SIP URI, so let's set the dial data

// Sawfish VoIP changes  >>>>
    CAiwDialDataExt* dialData = CAiwDialDataExt::NewLC();
    dialData->SetPhoneNumberL( aNumberOrUrl );  
    dialData->SetCallType( aCallType );
    
    if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip))
        {
        MLogsEventData* eventData = aEvent->LogsEventData();
        if ( eventData )
            {
            dialData->SetServiceId( eventData->ServiceId() );

            if ( aCalledForMenu )
                {
                // When call is initiated from log's menu
                // - User can select service from list
                dialData->SetServiceId( NULL );
                
                TPtrC8 ptrContactLink( KNullDesC8 );
                if ( KErrNone == eventData->GetContactLink( ptrContactLink ) )
                    {    
                    CLogsCntLinkChecker* contactCheckerPtr = Engine()->CntLinkCheckerL();
                
                    SetInputBlockerL();
                
                    // If contact link is valid, dialData use contack link instead of number.
                    if ( contactCheckerPtr->IsCntLinkValidSync( ptrContactLink ) )
                        {
                        dialData->SetContactLinkL(ptrContactLink  );
                        dialData->SetPhoneNumberL( KNullDesC );
                    
                        // EJZO-7RJB3V,the CallUI will invoke phonebook2 to show the call selector, that will
                        // make logs receive ViewDeactivated/Lostforeground , then ViewActivated/Foreground events
                        //that will cause the logs view flicking.So the hack will avoid this using the following flag                    
                        iIgnoreViewDeactivatedHandling = ETrue;
                        }
                
                    RemoveInputBlocker();
                    } 
                }
            }
        }
// <<<<  Sawfish VoIP changes  
    
    dialData->SetWindowGroup( AIWDialData::KAiwGoToIdle ); 
    CAiwGenericParamList& paramList = iServHandlerRef->InParamListL();
    dialData->FillInParamListL( paramList );
    CleanupStack::PopAndDestroy( dialData );  
    
    // If this is an emergency number, initiate the call through the
    // new emergency call API. We need to check the number, and not to trust 
    // the aEventType as for example CLogsPrependQuery might change the number
    ////
    TBool isEmergencyNumber = EFalse;
    if (aNumberOrUrl.Length() <= KPhCltEmergencyNumberSize)
        {
        iEmergencyCallAPI->IsEmergencyPhoneNumber(aNumberOrUrl, isEmergencyNumber);
        }
        
   if (isEmergencyNumber)
        {
        // in case there is already a pending asynch request
        // we just skip calling the api again to avoid leaves 
        if ( !iEmergencyCallAPI->IsActive())
            {    
            iEmergencyCallAPI->DialEmergencyCallL(aNumberOrUrl);  
            }
        }
    
    // If not an emergency call, initiate call through CallUI
    // CallUI should be able to handle emergency calls too, but the
    // safest and preffered way is to use Emergency Call API 
    ////    
    else if( !aCalledForMenu )    
        // Execute direct command (CallUI shows list query 
        // if needed - i.e user has pressed Send key).
        {
        iServHandlerRef->ExecuteServiceCmdL(
            aCommandId,
            paramList,
            iServHandlerRef->OutParamListL(),
            0,
            NULL ); //Callback is NULL as no dial results wanted.
        }
    else 
        // Execute Menu based command (we have already shown 
        // Logs-AIW menu to user, so no menus wanted anymore)
        {
        iServHandlerRef->ExecuteMenuCmdL(
            aCommandId,
            paramList,
            iServHandlerRef->OutParamListL(), 
            0,      
            NULL ); //Callback is NULL as no dial results wanted.
        }

    //If missed list, clear now new missed calls counter
    ClearMissedCallNotificationsL();  
    
    TRACE_EXIT_POINT;         
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::HandleEmergencyDialL
// ----------------------------------------------------------------------------
//
void CLogsBaseView::HandleEmergencyDialL( const TInt /*aStatus */)
    {
    // TODO: Show some note to user if there is error? 
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::CmdCopyNumberL
// ----------------------------------------------------------------------------
//
void CLogsBaseView::CmdCopyNumberL( const MLogsEventGetter* aEvent )
    {
    CPlainText* plainText = CPlainText::NewL();
    CleanupStack::PushL( plainText );    
    
    TInt length( 0 );
    const HBufC* number = aEvent->Number();
    
    if( number )
        {
        length = number->Length();
        }

    if( length && length <= KLogsPhoneNumberMaxLen )
        {
        TBuf<KLogsPhoneNumberMaxLen> buf = *(aEvent->Number());  
        AknTextUtils::LanguageSpecificNumberConversion( buf );
        plainText->InsertL( 0, buf );         
        }
        
    CClipboard* cb = CClipboard::NewForWritingLC( iCoeEnv->FsSession() );
    plainText->CopyToStoreL( cb->Store(), cb->StreamDictionary(), 0, 
                                        plainText->DocumentLength() );
    cb->CommitL();
    CleanupStack::PopAndDestroy( 2 ); // plainText, cb
    
    CAknNoteDialog* noteDlg = new( ELeave ) CAknNoteDialog( CAknNoteDialog::ENoTone, 
                                                            CAknNoteDialog::ELongTimeout );
    noteDlg->ExecuteLD( R_NUMBER_COPIED_TO_CB_NOTE );
    }    
    

// ----------------------------------------------------------------------------
// CLogsBaseView::CmdCopyAddressL
// ----------------------------------------------------------------------------
//
void CLogsBaseView::CmdCopyAddressL( const MLogsEventGetter* aEvent )
    {
    TBuf<KLogsSipUriMaxLen> buf;         
    TInt ret = Engine()->ConvertToUnicode( aEvent->LogsEventData()->Url(), buf ); 
    
    if( ret != KErrNone && ret != KErrOverflow )        
        {
        return; //Converting uri to unicode failed
        }
    
    CPlainText* plainText = CPlainText::NewL();
    CleanupStack::PushL( plainText );    
    TInt length( buf.Length() );

    if( length && length <= KLogsSipUriMaxLen ) 
        {
        TBuf<KLogsSipUriMaxLen> bufConv = buf; 
        AknTextUtils::LanguageSpecificNumberConversion( bufConv );
        plainText->InsertL( 0, bufConv );         
        }
        
    CClipboard* cb = CClipboard::NewForWritingLC( iCoeEnv->FsSession() );
    plainText->CopyToStoreL( cb->Store(), cb->StreamDictionary(), 0, 
                                        plainText->DocumentLength() );
    cb->CommitL();
    CleanupStack::PopAndDestroy( 2 ); // plainText, cb
    
    HBufC* textBuf = NULL;

    if( aEvent->LogsEventData()->VoIP() )
        {
        textBuf = StringLoader::LoadLC( R_VOIP_ADDRESS_COPIED );            
        }
    else
        {
        textBuf = StringLoader::LoadLC( R_POC_ADDRESS_COPIED );                        
        }

    //FIXME: The CAknInformationNote seems to overwritten by toolbar even if toolbar is requested
    //to be hidden (compared to CAknNoteDialog above that works ok. If not later fixedby toolbar 
    //itself, convert here to use CAknNoteDialogs also using note resoruces.
    CAknInformationNote* noteDlg = new( ELeave ) CAknInformationNote( );
    noteDlg->ExecuteLD( *textBuf );                                                                
    CleanupStack::PopAndDestroy( textBuf );  
    }


// ----------------------------------------------------------------------------
// CLogsBaseView::CmdShowMyAddressL
// ----------------------------------------------------------------------------
//
void CLogsBaseView::CmdShowMyAddressL( const MLogsEventGetter* aEvent )   
    {
    TBuf<KLogsSipUriMaxLen> buf;             
    TInt ret = Engine()->ConvertToUnicode( aEvent->LogsEventData()->MyAddress(), buf );

    if( ret != KErrNone && ret != KErrOverflow )        
        {
        return; //Converting uri to unicode failed
        }

    HBufC* textBuf = NULL;
    textBuf = StringLoader::LoadLC( R_STM_MY_ADDRESS, buf );    
    CAknNoteDialog* noteDlg = new ( ELeave ) CAknNoteDialog( );
    CleanupStack::PushL( noteDlg );
    noteDlg->SetTextL( *textBuf );
    noteDlg->ExecuteLD( R_MY_ADDRESS_QUERY );	
    CleanupStack::Pop( noteDlg ); 
    CleanupStack::PopAndDestroy( textBuf );
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::HandleNotifyL
//
// Handle AIW-notifications (from asynch ExecuteCmdL call)
// ----------------------------------------------------------------------------
//
TInt CLogsBaseView::HandleNotifyL(
    TInt aCmdId, 
    TInt aEventId,
    CAiwGenericParamList& aEventParamList,
    const CAiwGenericParamList& /* aInParamList */ )
    {
    //Only EGenericParamContactLinkArray available in aEventParamList so we use iNumberInPbkProcessing instead
    
    TInt retVal = 0;
    //Currently we're only interested in result of contact addition to pbk
    if( aCmdId == KAiwCmdAssign )    
        {
        switch( aEventId ) 
            {
            case KAiwEventStarted:
                break;
            
            // If selecting Options->Exit KAiwEventError will result but the contact may have 
            // still been created so start updater
            case KAiwEventError:
            	{
            	TInt aError = aEventParamList[0].Value().AsTInt32();
            	iCoeEnv->HandleError( aError );
            	}
            case KAiwEventCompleted: // Asynch req completed. Right softkey "Done" selected for contact editor 
                {
                //Enable UI for db-originated changes (however focus needs to be kept unchanged)
                CurrentModel()->DoActivateL( MLogsModel::EActivateOnly );
                
                //If number was updated to pbk, update it to Logs too (number preferred over sip uri)
                if( iNumberInPbkProcessing )
                    {  
                    Engine()->EventUpdaterL()->Stop();  //Stop previous updating process
                    MLogsReaderConfig* readerConfig = CLogsReaderConfigFactory::
                            LogsReaderConfigLC( iNumberInPbkProcessing ); 
                    Engine()->EventUpdaterL()->ConfigureL( readerConfig );
                    CleanupStack::PopAndDestroy();      //readerConfig
                    Engine()->EventUpdaterL()->StartL();//Start asynch process of reading the name from Pbk, updating Logs event 
                                                        //and eventually notification of changed db data to respective Logs view.
                    //Note: UI Data refresh is started in CLogsEngine::StateChangedL when event updater is finished                                                        
                    
                    }
                else if( iUriInPbkProcessing )
                    {
                    //We don't support updating URIs, so nothing done, just refresh the model. 
                    CurrentModel()->DoActivateL( MLogsModel::ERefresh );
                    }
                
                delete iNumberInPbkProcessing;
                delete iUriInPbkProcessing;    
                iNumberInPbkProcessing = NULL;
                iUriInPbkProcessing = NULL;  
                ClearAiwCallParams();
        
                break; 
                }
            
            case KAiwEventQueryExit: // Contact could still be created so start updater
                {
                // Fix for EPZO-6YWDXY: we must return "true" for the  
                // phonebook to know it can exit 
                retVal = 1;// "ETrue" 
                break;
                }   
                
            case KAiwEventCanceled:   //Cancel pressed in Pbk UI
                {
                delete iNumberInPbkProcessing;
                delete iUriInPbkProcessing;    
                iNumberInPbkProcessing = NULL;
                iUriInPbkProcessing = NULL;   
                ClearAiwCallParams();   
                               
                if ( LogsAppUi()->ActiveViewId() == Id().iUid && 
                     LogsAppUi()->ExecutionMode() == ELogsInForeground )
                    {       
                    //Refresh UI
                    CurrentModel()->DoActivateL( iResetAndRefreshOnHandleNotify );   
                    }      
             
                iResetAndRefreshOnHandleNotify = MLogsModel::ERefresh;
                break;  
                }
            case KAiwEventOutParamCheck:
            case KAiwEventInParamCheck:        
            case KAiwEventStopped:
            
            default:
                {
                //Refresh UI
                CurrentModel()->DoActivateL( MLogsModel::ERefresh );                      
                break;  
                }
            }
        
        // Remove inputblocker if still exists
        RemoveInputBlocker();
        MenuBar()->StopDisplayingMenuBar();
        }
    
    return retVal;    
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::IsOkToShowContextMenu
// ----------------------------------------------------------------------------
//
TBool CLogsBaseView::IsOkToShowContextMenu( const MLogsEventGetter* aEvent )
    {
    //aEvent may not yet be available
    if( !aEvent || aEvent->EventType() == ETypeEmerg ) 
        {
        return EFalse;    
        }
        
    if( PhoneNumberAvailable( aEvent ) || SipUriAvailable( aEvent ) ) 
        {
        return ETrue;    
        }
        
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::HandleControlEventL
//
// from MCoeControlObserver        
// ----------------------------------------------------------------------------
//
void CLogsBaseView::HandleControlEventL( 
    CCoeControl* /* aControl */, 
    TCoeEvent /* aEventType */ ) 
    {
    }

// ----------------------------------------------------------------------------
// CLogsBaseView::SetToolbarItems
// ----------------------------------------------------------------------------
//
void CLogsBaseView::SetToolbarItems( 
    const MLogsEventGetter* aEvent )
    {
    TRACE_ENTRY_POINT;  
    
    if (AknLayoutUtils::PenEnabled() )
        {  
        CAknToolbar* toolbar  = Toolbar();
        if( !toolbar  ) 
            {
            return;    
            }

        //aEvent may not yet be available, show only open pbk selection then
        if( !aEvent ) 
            {
            toolbar->SetItemDimmed( ELogsCmdSendKeyCall, ETrue, EFalse );
            toolbar->SetItemDimmed( ELogsCmdToolBarSendUi, ETrue, EFalse );
            }
        else 
            {
            TBool emerg( aEvent->EventType() == ETypeEmerg );
            TBool nbr( PhoneNumberAvailable( aEvent ));    
            TBool voipUri( SipUriAvailable( aEvent ) && aEvent->LogsEventData()->VoIP() );
            TBool pocUri( SipUriAvailable( aEvent ) && aEvent->LogsEventData()->PoC() );    
            toolbar->SetItemDimmed( ELogsCmdSendKeyCall,     !nbr && !voipUri && !pocUri,  EFalse );
            toolbar->SetItemDimmed( ELogsCmdToolBarSendUi,     !nbr || emerg, EFalse  );      
            }

        //aToolbar->DrawNow();
        toolbar->DrawDeferred();
        } 
    TRACE_EXIT_POINT;  
    }

//  End of File


