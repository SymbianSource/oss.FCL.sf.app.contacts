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
*     Logs application "Event list" view class implementation
*
*/


// INCLUDE FILES
#include <eikmenub.h>
#include <AknQueryDialog.h>
#include <featmgr.h>    //FeatureManger
#include <Logs.rsg>

#include "CLogsEventListView.h"
#include "CLogsAppUi.h"
#include "CLogsEngine.h"
#include "MLogsModel.h"
#include "MLogsEventGetter.h"
#include "MLogsClearLog.h"
#include "CLogsEventListControlContainer.h"
#include "CLogsReaderConfigFactory.h"
#include "MLogsStateHolder.h"
#include "MLogsSharedData.h"
#include "LogsEngConsts.h"  //KLogEngWlanEventTypeUid
#include "MLogsReader.h"
#include "LogsUID.h"

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
// CLogsEventListView::NewL
// ----------------------------------------------------------------------------
//
CLogsEventListView* CLogsEventListView::NewL()
    {
    CLogsEventListView* self = new( ELeave ) CLogsEventListView;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsEventListView::~CLogsEventListView
// ----------------------------------------------------------------------------
//
CLogsEventListView::~CLogsEventListView()
    {
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    }

// ----------------------------------------------------------------------------
// CLogsEventListView::CLogsEventListView
// ----------------------------------------------------------------------------
//
CLogsEventListView::CLogsEventListView() :
                iCurrentFilter( ELogsFilterAllComms ),
                iState( EStateUndefined ),
                iClearListInProgress(EFalse)
{
}

// ----------------------------------------------------------------------------
// CLogsEventListView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CLogsEventListView::HandleCommandL( TInt aCommandId )
    {
    iEventListCurrent = iContainer->ListBox()->CurrentItemIndex();

    const MLogsEventGetter* event = NULL;
    event = Engine()->Model( ELogsMainModel )->At( iEventListCurrent );

    switch( aCommandId )
        {
        case ELogsCmdMenuDetails:
		    //if in clearlog process, ignore the request to avoid show
			//deteiled view EJDU-7PCDKG 
            if(!iClearListInProgress)
            {
            LogsAppUi()->CmdOkL( iEventListCurrent );
            }
            break;

        case ELogsCmdMenuFilter:
            CmdFilterL();
            break;
     
        case ELogsCmdMenuClearList:
            CmdClearListL();
            break;

        default:
            {
            CLogsBaseView::HandleCommandEventL( aCommandId, event );
            }
        }
    }
    
// ----------------------------------------------------------------------------
// CLogsEventListView::ChangeTitlePaneTextToDefaultL
// ----------------------------------------------------------------------------
//
void CLogsEventListView::ChangeTitlePaneTextToDefaultL()
    {       
    iContainer->SetTitlePaneTextToDefaultL( LogsAppUi()->ActiveViewId() );
    } 

// ----------------------------------------------------------------------------
// CLogsEventListView::ProcessCommandL
//
// Called from FW when e.g. cba pressed 
// ----------------------------------------------------------------------------

void CLogsEventListView::ProcessCommandL( TInt aCommand )
    {
    if (LogsAppUi()->ActiveViewId() != ELogEventListViewId || !iContainer )
           {
           LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
                   ( "CLogsRecentListView::ProcessCommandL - view already deactivated! return " ));  
           return;
           }
    
    if (aCommand == EAknSoftkeyOptions)
        { 
        TInt currentEventIndex = iContainer->ListBox()->CurrentItemIndex();
        const MLogsEventGetter* event = NULL;
        event = Engine()->Model( ELogsMainModel )->At( currentEventIndex );
       
        // If options menu is opened call SetRefreshMenuOnUpdate which records the current event id
        // which can then be used to make a decision on wether options menu needs to be refreshed
        // when reading is finished.
        SetRefreshMenuOnUpdate( event );
        }    
    
    CLogsBaseView::ProcessCommandL( aCommand );
    }

// ----------------------------------------------------------------------------
// CLogsEventListView::ProcessKeyEventL
// ----------------------------------------------------------------------------
//
TBool CLogsEventListView::ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                            TEventCode aType )
    {
    iEventListCurrent = iContainer->ListBox()->CurrentItemIndex();

    const MLogsEventGetter* event = CurrentModel()->At( iEventListCurrent );
    
    //FIXME (CLogsRecentListView and CLogsEventListView): 
    //The below is needed only because we get key event before the actual row
    //is changed in the listbox. If/when listbox observer for changed row focus
    //is available, remove code below and get correct event index from observer.
    iEventListCurrent = iContainer->ListBox()->CurrentItemIndex();

    if( aType == EEventKey && iEventListCurrent != KErrNotFound ) 
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
// CLogsEventListView::Id
// ----------------------------------------------------------------------------
//
TUid CLogsEventListView::Id() const
    {
    return TUid::Uid( ELogEventListViewId );
    }

// ----------------------------------------------------------------------------
// CLogsEventListView::DynInitMenuPaneL
// ----------------------------------------------------------------------------
//
void CLogsEventListView::DynInitMenuPaneL
    (   TInt aResourceId
    ,   CEikMenuPane* aMenuPane
    )
    {
    //First check do we have data already available. Data may not yet be available if asynch request(s) are 
    //not yet completed
    iEventListCurrent = iContainer->ListBox()->CurrentItemIndex();
    const MLogsEventGetter* event = NULL;

    if( Engine()->Model( ELogsMainModel )->Count() > 0 && iEventListCurrent >= 0 )    
        {
        event = Engine()->Model( ELogsMainModel )->At( iEventListCurrent );             
        }
        
    //Process menuitems. Note that data may not yet be available.
    if( aResourceId == R_LOGS_EVENT_LIST_MENU_CLEAR_LIST )
        {
        if( !event )        
            {            
            aMenuPane->DeleteMenuItem( ELogsCmdMenuClearList );
            return; 
            }
        }
    else if( aResourceId == R_LOGS_EVENT_LIST_MENU_FILTER )
        {
        if( !event )        
            {            
            //Remove filter option if we had broadest filter in use and still got an empty list
            if( Engine()->Model( ELogsMainModel )->Count() == 0 && 
                iCurrentFilter == ELogsFilterAllComms )
                {            
                aMenuPane->DeleteMenuItem( ELogsCmdMenuFilter );            
                }
            return; 
            }
        }
    else if( aResourceId == R_LOGS_EVENT_LIST_MENU_DETAILS )
        {
        if( !event )        
            {            
            aMenuPane->DeleteMenuItem( ELogsCmdMenuDetails );                    
            return; 
            }
        else
            {
            // If MSK is enabled both in the platform and in the application, the menu text 
            // for viewing the event details is changed to conform with the MSK label
            if( Engine()->SharedDataL()->IsMSKEnabledInPlatform() && 
                AknLayoutUtils::MSKEnabled() )
                {
                aMenuPane->SetItemTextL( ELogsCmdMenuDetails, R_QTN_LOGS_CMD_VIEW);
                }
            }
        }
    else 
        {
        CLogsBaseView::DynInitMenuPaneEventL( aResourceId, aMenuPane, event );    
        }
    }


// ----------------------------------------------------------------------------
// CLogsEventListView::HandleClientRectChange
// ----------------------------------------------------------------------------
//
void CLogsEventListView::HandleClientRectChange()
    {
    if (iContainer)
        {
        iContainer->SetRect( ClientRect() );  
        }
    }

// ----------------------------------------------------------------------------
// CLogsEventListView::DrawComponents
// ----------------------------------------------------------------------------
//
void CLogsEventListView::DrawComponents()
    {
    if (iContainer)
        {
        iContainer->DrawNow();
        }
    }

// ----------------------------------------------------------------------------
// CLogsEventListView::DoActivateL
// ----------------------------------------------------------------------------
//
void CLogsEventListView::DoActivateL
        ( const TVwsViewId& aPrevViewId, 
          TUid aCustomMessageId,
          const TDesC8& aCustomMessage )
    {
    TRACE_ENTRY_POINT;
    CLogsBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );  
    
    
    Engine()->CreateModelL( ELogsMainModel );   //Creates if not already exist

    if( iContainer == NULL )
        {
        iContainer = CLogsEventListControlContainer::NewL( this, ClientRect() );
        AppUi()->AddToViewStackL( *this, iContainer );
        }
    iContainer->ListBox()->SetListBoxObserver( this );
    
    //We reset this list when activated. Only exception is when returning from Logs Detail View
    if( aPrevViewId.iViewUid != TUid::Uid( ELogDetailViewId  ))   
        {
        MLogsReaderConfig* readerConfig;
        iCurrentFilter = ELogsFilterAllComms;                                         //reset filter selection
        readerConfig = CLogsReaderConfigFactory::LogsReaderConfigLC( iCurrentFilter); //reread all events
        iContainer->ListBox()->SetCurrentItemIndex( 0 );
        Engine()->Model( ELogsMainModel )->ConfigureL( readerConfig );        
        CleanupStack::PopAndDestroy();  //readerConfig
        }

    LogsAppUi()->SetPreviousViewId( aPrevViewId.iViewUid );
    LogsAppUi()->SetCurrentViewId( Id() );
    Engine()->Model( ELogsMainModel )->SetObserver( this );
    Engine()->Model( ELogsMainModel )->DoActivateL( MLogsModel::ERefresh );
    Engine()->StartSMSEventUpdaterL( /* MLogsReader::EPbkAndSim */ );//Starts the asynch process of reading Phonebook, 
                                                                     //updating Logs event and eventually receiving 
                                                                     //notification  of changed database data to view.
    // Just to make sure the inputblocker is not on
    iClearListInProgress = EFalse;
    RemoveInputBlocker();
      
    TRACE_EXIT_POINT;
    }                                                               
// ----------------------------------------------------------------------------
// CLogsEventListView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CLogsEventListView::DoDeactivate()
    {
    if( iContainer )
        {
        iEventListCurrent = iContainer->ListBox()->CurrentItemIndex();
        iEventListTop = iContainer->ListBox()->TopItemIndex();
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }

    //Deactivate model. We must keep the model connected to the database, otherwise we don't have the
    //the same data available when we return from Detail view.
    Engine()->Model( ELogsMainModel )->DoDeactivate( MLogsModel::ENormalOperation,
                                                     MLogsModel::EKeepDBConnection ); //EFalse: keep connected to db
   
    }

// ----------------------------------------------------------------------------
// CLogsEventListView::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsEventListView::ConstructL()
    {
    BaseConstructL( R_LOGS_EVENT_LIST_VIEW );
    }


// ----------------------------------------------------------------------------
// CLogsEventListView::CmdClearListL
// ----------------------------------------------------------------------------
//
void CLogsEventListView::CmdClearListL()
    {
    CAknQueryDialog* queryDlg = CAknQueryDialog::NewL();
    if( queryDlg->ExecuteLD( R_CLEAR_LIST_CONFIRMATION_QUERY ) )
        {
        Engine()->ClearLogsL()->ClearModelL( ELogsMainModel );
        Engine()->SharedDataL()->NewMissedCalls( ETrue );
        SetInputBlockerL();
        iClearListInProgress = ETrue;
        
        }
    }

// ----------------------------------------------------------------------------
// CLogsEventListView::AddOneListBoxTextLineL
// ----------------------------------------------------------------------------
//
void CLogsEventListView::AddOneListBoxTextLineL(
    CDesCArrayFlat* aItems,
    TInt aResourceText )
    {
    HBufC* text;
    text = iCoeEnv->AllocReadResourceLC( aResourceText );
    aItems->AppendL( *text );
    CleanupStack::PopAndDestroy();  // text
    }

// ----------------------------------------------------------------------------
// CLogsEventListView::InitFilterPopupListL
//
// Show Popuplist containing filtering options
// ----------------------------------------------------------------------------
//
TLogsFilter CLogsEventListView::InitFilterPopupListL()
    {
    CDesCArrayFlat* items = new( ELeave )CDesCArrayFlat( ELogsFilterNumberOfFilters );
    CleanupStack::PushL( items );
    
    //NOTE! In enum TLogsFilter does not necessarily contain same options as in created filter listbox., 
    //Therefore we create popUpRow[] containing only the filtering options corresponding to 
    //rows available in popuplistbox (e.g. wlan option is not present in all configurations).
    TLogsFilter popUpRow[ELogsFilterNumberOfFilters];  //(ELogsFilterNumberOfFilters is last of enums)
    TInt i = 0;
    TInt setFocusInLB = KErrNotFound;   
    TInt count( Engine()->Model( ELogsMainModel )->Count() );

    //If previous choice did not give any events, then do not add it again to listbox (hide that option)
    if( !( iCurrentFilter == ELogsFilterAllComms && count == 0 ) )  //1.Add other than current filters to list 2.Add also current filter to 
        {                                                           //  list if it found events from db, otherwise discard current filter.
        (iCurrentFilter == ELogsFilterAllComms && count > 0) ? setFocusInLB = i : 0; //Set focus to this line if current & found events from db.
        AddOneListBoxTextLineL( items, R_LOGS_MF_ALL_COMMUNICATION_TEXT );
        popUpRow[i++] = ELogsFilterAllComms;
        }
    if( !( iCurrentFilter == ELogsFilterOutgoing && count == 0 ) ) 
        {
        (iCurrentFilter == ELogsFilterOutgoing && count > 0) ? setFocusInLB = i : 0; 
        AddOneListBoxTextLineL( items, R_LOGS_MF_OUTGOING_TEXT );
        popUpRow[i++] = ELogsFilterOutgoing;
        }
    if( !( iCurrentFilter == ELogsFilterIncoming && count == 0 ) ) 
        {
        (iCurrentFilter == ELogsFilterIncoming && count > 0) ? setFocusInLB = i : 0; 
        AddOneListBoxTextLineL( items, R_LOGS_MF_INCOMING_TEXT );
        popUpRow[i++] = ELogsFilterIncoming;
        }
    if( !( iCurrentFilter == ELogsFilterVoice && count == 0 ) ) 
        {
        (iCurrentFilter == ELogsFilterVoice && count > 0) ? setFocusInLB = i : 0;         
        if( FeatureManager::FeatureSupported( KFeatureIdCsVideoTelephony ) )
            {
            AddOneListBoxTextLineL( items, R_LOGS_MF_VOICEVIDEO_TEXT );  
            }
        else
            {
            AddOneListBoxTextLineL( items, R_LOGS_MF_VOICE_TEXT );  
            }
        popUpRow[i++] = ELogsFilterVoice;
        }
    if( !( iCurrentFilter == ELogsFilterMessages && count == 0 ) ) 
        {
        (iCurrentFilter == ELogsFilterMessages && count > 0) ? setFocusInLB = i : 0;                 
        AddOneListBoxTextLineL( items, R_LOGS_MF_SMS_TEXT );
        popUpRow[i++] = ELogsFilterMessages;
        }
    if( !( iCurrentFilter == ELogsFilterPacket && count == 0 ) ) 
        {
        (iCurrentFilter == ELogsFilterPacket && count > 0) ? setFocusInLB = i : 0;                         
        AddOneListBoxTextLineL( items, R_LOGS_MF_PACKET_TEXT );
        popUpRow[i++] = ELogsFilterPacket;
        }
    if( !( iCurrentFilter == ELogsFilterData && count == 0 ) ) 
        {
        (iCurrentFilter == ELogsFilterData && count > 0) ? setFocusInLB = i : 0;                                 
        AddOneListBoxTextLineL( items, R_LOGS_MF_DATA_TEXT );
        popUpRow[i++] = ELogsFilterData;
        }
    if( !( iCurrentFilter == ELogsFilterWlan && count == 0 ) ) 
        {
        if( FeatureManager::FeatureSupported( KFeatureIdProtocolWlan ) )   
            {
            (iCurrentFilter == ELogsFilterWlan && count > 0) ? setFocusInLB = i : 0;                                             
            AddOneListBoxTextLineL( items, R_LOGS_MF_WLAN_TEXT );
            popUpRow[i++] = ELogsFilterWlan;
            }
        }   
    if( count > 0 ) 
        {
        const MLogsEventGetter* event = Engine()->Model( ELogsMainModel )->At( iEventListCurrent );
        //Seletected event supported only for events that have number or remoty party available, so e.g
        //payphones, unknowns, privates and poc group calls not supported
        if( event->Number() || event->RemoteParty() )
            {
            (iCurrentFilter == ELogsFilterPerson) ? setFocusInLB = i : 0;                                                     
            AddOneListBoxTextLineL( items, R_LOGS_MF_SELECTED_PERSON_TEXT );
            popUpRow[i++] = ELogsFilterPerson;
            }
        }

    //  Create popuplistbox
    CAknSinglePopupMenuStyleListBox* popupListBox = new( ELeave ) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( popupListBox );
    CAknPopupList* popupFrame = CAknPopupList::NewL( popupListBox, 
                                                     R_AVKON_SOFTKEYS_SELECT_CANCEL );
    CleanupStack::PushL( popupFrame );  
    popupListBox->ConstructL( popupFrame, CEikListBox::ELeftDownInViewRect );

    //  Create the model for listbox
    CTextListBoxModel* model = popupListBox->Model();
    model->SetItemTextArray( items );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray ); // items in CleanupStack 
    popupListBox->HandleItemAdditionL();

    //  Create title for the Pop-Up window
    HBufC* title;
    title = iCoeEnv->AllocReadResourceLC( R_LOGS_MF_SHOW_TEXT );
    popupFrame->SetTitleL( *title );
    CleanupStack::PopAndDestroy();  // title

    // Select previous filter (except for the first time or if no rows got using that filter)    
    if( setFocusInLB != KErrNotFound )
        {
        popupListBox->SetCurrentItemIndex( setFocusInLB ); //Same choice as on previous filtering
        }
    else
        {
        popupListBox->SetCurrentItemIndex( 0 );            
        }
    
    popupListBox->SetTopItemIndex( 0 );                 // To see as many lines as possible
    iContainer->MakeScrollArrowsL( popupListBox );      // Activate scrollbar
    CleanupStack::Pop( popupFrame );    

    TLogsFilter newFilter;
    
    if( popupFrame->ExecuteLD() )
        {
        newFilter = popUpRow[popupListBox->CurrentItemIndex()];
        }
    else
        {
        newFilter = ELogsFilterNothing;                 //ELogsFilterNothing is last enum option
        }

    CleanupStack::PopAndDestroy( popupListBox ); 
    CleanupStack::PopAndDestroy( items );                
    return newFilter;
    }
    

// ----------------------------------------------------------------------------
// CLogsEventListView::CmdFilterL
// ----------------------------------------------------------------------------
//
void CLogsEventListView::CmdFilterL()
    {
    TLogsFilter newFilter( InitFilterPopupListL() );  //Show popuplist and get user's selection for filter option

    // Start filtering procedure only if new filter is selected.
    if( iCurrentFilter != newFilter && 
        newFilter != ELogsFilterNothing )
        {
        iCurrentFilter = newFilter;
        MLogsReaderConfig* readerConfig;
        
        if( iCurrentFilter == ELogsFilterPerson &&
            Engine()->Model( ELogsMainModel )->Count() )
            {
            const MLogsEventGetter* event = Engine()->
                            Model( ELogsMainModel )->At( iEventListCurrent );
            TPtrC number(KNullDesC());
            TPtrC remote(KNullDesC());
            if( event->Number() )
                {
                number.Set( *(event->Number()) );
                }
            if( event->RemoteParty() )
                {
                remote.Set( *(event->RemoteParty()) );
                }
		    
            readerConfig = CLogsReaderConfigFactory::
                            LogsReaderConfigLC( &number, 
                                                &remote );
            }
        else
            {
            readerConfig = CLogsReaderConfigFactory::
                        LogsReaderConfigLC( iCurrentFilter );            
            }
            
        iContainer->ListBox()->SetCurrentItemIndex( 0 );
        Engine()->Model( ELogsMainModel )->ConfigureL( readerConfig );        
        CleanupStack::PopAndDestroy(); //readerConfig         
        }
    }

// ----------------------------------------------------------------------------
// CLogsEventListView::CurrentFilter
// ----------------------------------------------------------------------------
//
TLogsFilter CLogsEventListView::CurrentFilter() const
    {
    return iCurrentFilter;
    }
    
// ----------------------------------------------------------------------------
// CLogsEventListView::StateChangedL
//
// CLogsModel informs that it has changed the event array (e.g. added an entry or reseted the array etc)
// ----------------------------------------------------------------------------
//
void CLogsEventListView::StateChangedL( MLogsStateHolder* aHolder )
    {
    if( !aHolder )
        {
        return;  
        }
        
    iState = aHolder->State();
    
    // When check contact link finished, call base method to check if need to 
    // refresh menu.
    if( iState == EStateCheckContactLinkFinished )
        {
        CLogsBaseView::StateChangedL( aHolder );
        return;
        }
    
#ifdef _DEBUG
_LIT(KDebugString,"Performance monitoring: Log view list item time %D");
    if( !iCreationTimeCalculated && Engine()->Model( ELogsMainModel )->Count() > 0 )
        {
        iCreationTimeCalculated = ETrue;
        TTime currentTime;
        currentTime.UniversalTime();
        
        RDebug::Print( KDebugString, currentTime.MicroSecondsFrom( iCreationTime ));
        }
#endif

    //Call Update() when related items available to show changes immediately plus when reading is finished. 
    const TInt KNbrShownEntries = 16;

    iState = aHolder->State();
    
    if( iContainer )
        {
        //Optimization: we reduce frequent calling of iContainer->UpdateL(). Although the UpdateL() can be called always when 
        //StateChangedL is called, this slows down the performance significantly
        
        TBool finished = ( iState != EStateActive && iState != EStateUndefined  &&  iState != EStateInitializing);        
        TInt count( iContainer->ListBox()->Model()->NumberOfItems() ); 
        // TInt interval = 50;        
        // TBool update = ( count / interval == (count+interval-1) / interval );//This may cause refreshes *above* the user's current line 
                                                                                //when there are changes in db that affect the view. This
                                                                                //causes unpleasant jumping in the view so no need to update.
        
        //Magic number KNbrShownEntries: Call Update() when first screenful of items available or when reading finished, stopped, 
        //interrupted etc. Also call update when about to start to read (count==0) in order to show "Retrieving data".
        if( count == 0 || count == KNbrShownEntries || finished )  // || update )  
            {   
            iContainer->UpdateL(); 
            
            //Show MSK if anything on list
            Cba()->MakeCommandVisible( 
                ELogsCmdMenuDetails, 
                count );
            
            //Perform some time consuming operations if not yet already done
            if( count == KNbrShownEntries || finished ) 
                {
                ConstructDelayedL( ETrue );   //ETrue: perform using idle time
                iEventListCurrent = iContainer->ListBox()->CurrentItemIndex();
                const MLogsEventGetter* event = Engine()->
                    Model( ELogsMainModel )->At( iEventListCurrent );
                
                if (finished)
                    {
                    //Checks wheter the options menu was opened already and should it be refreshed
                    //now that the reading is finishing                 
                    HandleMenuRefreshL( event );
                    // just in case inputblocker still alive
                    iClearListInProgress = EFalse;
                    RemoveInputBlocker();
                    }                               
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CLogsEventListView::State
// ----------------------------------------------------------------------------
//
TLogsState CLogsEventListView::State() const
    {
    return iState;  
    }

//  End of File
