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
*     Logs application "Detail" view
*
*/


// INCLUDE FILES
#include <baclipb.h>      
#include <eikmenub.h>   
#include <StringLoader.h> 
#include <aknnotewrappers.h> 
#include <logs.rsg>

#include "CLogsDetailView.h"
#include "CLogsDetailControlContainer.h"
#include "CLogsAppUi.h"
#include "CLogsEngine.h"
#include "MLogsModel.h"
#include "MLogsEventGetter.h"
#include "MLogsStateHolder.h"
#include "MLogsGetEvent.h"
#include "MLogsSharedData.h"
#include "LogsConstants.hrh"    //KLogsSipUriMaxLen

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
// CLogsDetailView::NewL
// ----------------------------------------------------------------------------
//
CLogsDetailView* CLogsDetailView::NewL()
    {
    CLogsDetailView* self = new( ELeave ) CLogsDetailView;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::~CLogsDetailView
// ----------------------------------------------------------------------------
//
CLogsDetailView::~CLogsDetailView()
    {
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::StateChangedL
//
// CLogsModel informs that it has changed event array, e.g. asynch get event 
// is now done (also array resets etc informed)
// ----------------------------------------------------------------------------
//
void CLogsDetailView::StateChangedL( MLogsStateHolder* aHolder )
    {
    if( !aHolder )
        {
        return;
        }
    
    iState = aHolder->State();
    
    if( iState == EStateCheckContactLinkFinished )
        {
        CLogsBaseView::StateChangedL(aHolder);
        return;
        }
    
    if( iState == EStateFinished ) 
        {
        TInt index( LogsAppUi()->EventListViewCurrent() );
        Cba()->MakeCommandVisible( 
            EAknSoftkeyContextOptions, 
            IsOkToShowContextMenu( Engine()->Model( ELogsMainModel )->At( index ) ));          
              
        //Force remaining construct operations if not yet already done
        ConstructDelayedL( EFalse );   //EFalse: perform immediately
        SetToolbarItems( Engine()->Model( ELogsMainModel )->At( index ) );                        
        }
    
    if( iContainer )
        {
        iContainer->UpdateL();
        }
    }


// ----------------------------------------------------------------------------
// CLogsDetailView::ProcessKeyEventL
// ----------------------------------------------------------------------------
//
TBool CLogsDetailView::ProcessKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {    
    if( aType == EEventKey )
        {
        switch( aKeyEvent.iCode )
            {
            case EKeyLeftArrow:
                AknLayoutUtils::LayoutMirrored() ? CmdNextEventL():
                                                   CmdPreviousEventL();
                return ETrue;

            case EKeyRightArrow:
                AknLayoutUtils::LayoutMirrored() ? CmdPreviousEventL():
                                                   CmdNextEventL();
                return ETrue;
                
            default:
                break;
            }
        }
        
    const MLogsEventGetter* event; 
    event = Engine()->Model( ELogsMainModel )->At( LogsAppUi()->EventListViewCurrent() );            
    return CLogsBaseView::ProcessKeyEventEventL( aKeyEvent,aType, event );        
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::ProcessPointerEventL
//
// Handler for pointer events, when the current focused item is tapped
// ----------------------------------------------------------------------------
//
void CLogsDetailView::ProcessPointerEventL( TInt /* aIndex */)
	{
	// Open the context sensitive menu
	ProcessCommandL( EAknSoftkeyContextOptions );    
	}
	
// ----------------------------------------------------------------------------
// CLogsDetailView::HandleListBoxEventL
//
// If touch support is on, we handle the touch related events here.
// ----------------------------------------------------------------------------
//
void CLogsDetailView::HandleListBoxEventL(
    CEikListBox* aListBox,
    TListBoxEvent aEventType)
    { 
    if( AknLayoutUtils::PenEnabled() )  
        { 
        switch ( aEventType )
            {
            case EEventItemSingleClicked:
            case EEventEnterKeyPressed:
                {
                ProcessPointerEventL(aListBox->CurrentItemIndex()); //aListBox->CurrentItemIndex() not used         
                break;
                }
                
            default:
               break;
            }
            
      	}
    }
	
// ----------------------------------------------------------------------------
// CLogsDetailView::HandleNaviDecoratorEventL
// Handler for pointer events, when the Navi Pane arrows have been tapped
// ----------------------------------------------------------------------------
//
void CLogsDetailView::HandleNaviDecoratorEventL( TInt aEventID )
    {
        switch (aEventID)
            {
            case MAknNaviDecoratorObserver::EAknNaviDecoratorEventRightTabArrow:
                AknLayoutUtils::LayoutMirrored() ? CmdPreviousEventL():
                                                   CmdNextEventL();
                break;
            
            case MAknNaviDecoratorObserver::EAknNaviDecoratorEventLeftTabArrow:
                AknLayoutUtils::LayoutMirrored() ? CmdNextEventL():
                                                   CmdPreviousEventL();
                break;
            }
    } 
  
// ----------------------------------------------------------------------------
// CLogsDetailView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CLogsDetailView::HandleCommandL( TInt aCommandId )
    {
    TInt index( LogsAppUi()->EventListViewCurrent() );    
    const MLogsEventGetter*  event = Engine()->Model( ELogsMainModel )->At( index );        
    
    switch( aCommandId )
        { 
        case ELogsCmdHandleMSK:
            //Select key pressed
            if( iContainer->ListBox()->View() )
                {
                CmdContextMenuL();
                }
            break;            
            
        default:
            CLogsBaseView::HandleCommandEventL( aCommandId, event );
        }    
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::ChangeTitlePaneTextToDefaultL
// ----------------------------------------------------------------------------
//
void CLogsDetailView::ChangeTitlePaneTextToDefaultL()
    {       
    iContainer->UpdateL();
    } 

// ----------------------------------------------------------------------------
// CLogsDetailView::Id
// ----------------------------------------------------------------------------
//
TUid CLogsDetailView::Id() const
    {
    return TUid::Uid( ELogDetailViewId );
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::DynInitMenuPaneL
// ----------------------------------------------------------------------------
//
void CLogsDetailView::DynInitMenuPaneL( 
    TInt aResourceId, 
    CEikMenuPane* aMenuPane )
    {
    //Get event, may not yet be available if asynch request(s) not completed. 
    TInt index( LogsAppUi()->EventListViewCurrent() );    
    const MLogsEventGetter*  event = Engine()->Model( ELogsMainModel )->At( index );     
    
    // If Logs settings is changed to "no log" returning back from settings view results in an
    // invalid state: details shown of a non-existent event. Prevent this by removing the "Settings"
    // option from 
    if (aResourceId == R_LOGS_SYSTEM_MENU_BOTTOM)
        {
        aMenuPane->DeleteMenuItem(ELogsCmdMenuSettings);
        }
    
    CLogsBaseView::DynInitMenuPaneEventL( aResourceId, aMenuPane, event );    
    // Detail view has no item specific commands
    switch ( aResourceId )
        {
        case ( R_COMMON_EVENT_MENU_EDIT_CALLS ):
        case ( R_COMMON_SAVETOPBK_SUBMENU ):
        case ( R_COMMON_SEND_MESSAGE_SUBMENU ):
        case ( R_COMMON_POC_SUBMENU ):
        case ( R_COMMON_CALLUI_SUBMENU ):
            {
            SetMenuItemsItemSpecificProperty( aMenuPane, EFalse );
            }
        default:
            {
            break;
            }
        }
    }


// ----------------------------------------------------------------------------
// CLogsDetailView::HandleClientRectChange
// ----------------------------------------------------------------------------
//
void CLogsDetailView::HandleClientRectChange()
    {
    if (iContainer)
        {
        iContainer->SetRect( ClientRect() );  
        }
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::DrawComponents
// ----------------------------------------------------------------------------
//
void CLogsDetailView::DrawComponents()
    {
    if (iContainer)
        {
        iContainer->DrawNow();
        }
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::ViewActivatedL
// ----------------------------------------------------------------------------
//
void CLogsDetailView::ViewActivatedL(const TVwsViewId& aPrevViewId,
                                        TUid aCustomMessageId,
                                        const TDesC8& aCustomMessage)
    { 
    SetToolbarStateL(ELogsToolbarOn, EFalse);      
    CAknView::ViewActivatedL(aPrevViewId, aCustomMessageId, aCustomMessage); 
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::ViewDeactivated
// ----------------------------------------------------------------------------
//
void CLogsDetailView::ViewDeactivated()
    {
    if (iFocusChangeControl != ELogsNoChange_PbkUpdPending )
           {
           SetToolbarState(ELogsToolbarOff, EFalse);  
           }
    CAknView::ViewDeactivated();
    }


// ----------------------------------------------------------------------------
// CLogsDetailView::DoActivateL
// ----------------------------------------------------------------------------
//
void CLogsDetailView::DoActivateL( 
    const TVwsViewId& aPrevViewId, 
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    CLogsBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );  
    
    
    if( ! iContainer )
        {
        iContainer = CLogsDetailControlContainer::NewL( this, ClientRect() );
        AppUi()->AddToViewStackL( *this, iContainer );
        } 
        
    //To prevent first time flicker of context menu if no ctx mnu will be displayed
    Cba()->MakeCommandVisible( EAknSoftkeyContextOptions, EFalse );

    Engine()->GetEventL()->SetObserver( this ); //Engine will inform when asynch get event is ok
    iContainer->ListBox()->SetListBoxObserver( this ); 
    LogsAppUi()->SetPreviousViewId( aPrevViewId.iViewUid );
    LogsAppUi()->SetCurrentViewId( Id() );

    TInt index( LogsAppUi()->EventListViewCurrent() );
	SetEventListCurrent(index  );	// update this detail view objects iEventListCurrent
    
    if( index != KErrNotFound )         
        {
        TLogId id( Engine()->Model( ELogsMainModel )->At( index )->LogId() ); //Get id from event array
        Engine()->GetEventL()->Get( id );  									  //Read details using event id (asynch read)
        }
    
    // Just to make sure the inputblocker is not on
    RemoveInputBlocker();
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CLogsDetailView::DoDeactivate()
    {
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        } 
    Engine()->DeleteGetEvent();
    
    //To prevent second time flicker of context menu if ctx mnu was was displayed
    //but will not later be displayed
    Cba()->MakeCommandVisible( EAknSoftkeyContextOptions, EFalse );
    }


// ----------------------------------------------------------------------------
// CLogsDetailView::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsDetailView::ConstructL()
    {
    BaseConstructL( R_LOGS_DETAIL_VIEW );

    //Perform some time consuming operations 
    ConstructDelayedL( ETrue );   //ETrue: perform using idle time
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::CmdContextMenuL
// ----------------------------------------------------------------------------
//
void CLogsDetailView::CmdContextMenuL()
    {
    TInt index( LogsAppUi()->EventListViewCurrent() );
    
    if( IsOkToShowContextMenu( Engine()->Model( ELogsMainModel )->At( index ) ))
        {
        LaunchPopupMenuL( R_LOGS_LOG_DETAIL_OK_MENUBAR );
        }
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::CmdNextEventL
// ----------------------------------------------------------------------------
//
void CLogsDetailView::CmdNextEventL()
    {
    TInt index( LogsAppUi()->EventListViewCurrent() );
    TInt count( Engine()->Model( ELogsMainModel )->Count() );

    if( index != KErrNotFound && ( index < count - 1 ) )
        {
        LogsAppUi()->SetEventListViewCurrent( index + 1 );  // update the event list views objects iEventListCurrent
        SetEventListCurrent(index + 1 );                    // update this deteail view objects iEventListCurrent
        index = LogsAppUi()->EventListViewCurrent();
        TLogId id = Engine()->Model( ELogsMainModel )->At( index )->LogId(); //Get id from event array
        Engine()->GetEventL()->Get( id );                                    //Read details using event id (asynch read)
        }
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::CmdPreviousEventL
// ----------------------------------------------------------------------------
//
void CLogsDetailView::CmdPreviousEventL()
    {
    TInt index( LogsAppUi()->EventListViewCurrent() );

    if( index != KErrNotFound && index != 0 )
        {
        LogsAppUi()->SetEventListViewCurrent( index - 1 ); // update the event list views objects iEventListCurrent
        SetEventListCurrent(index - 1 );                   // update this deteail view objects iEventListCurrent
        index = LogsAppUi()->EventListViewCurrent();
        TLogId id = Engine()->Model( ELogsMainModel )->At( index )->LogId(); //Get id from event array
        Engine()->GetEventL()->Get( id );                                    //Read details using event id (asynch read)
        }
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::PopUpNoteL
// ----------------------------------------------------------------------------
//
void CLogsDetailView::PopUpNoteL()
    {
    CAknNoteDialog* noteDlg = new( ELeave ) 
        CAknNoteDialog( CAknNoteDialog::ENoTone, CAknNoteDialog::ELongTimeout );
    noteDlg->ExecuteLD( R_NUMBER_COPIED_TO_CB_NOTE );
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::SetMenuItemsItemSpecificProperty
//
// Sets menu items item specific property.
// ----------------------------------------------------------------------------
//
void CLogsDetailView::SetMenuItemsItemSpecificProperty(
        CEikMenuPane* aMenuPane, TBool aItemSpecific )
    {
    if ( aMenuPane )
        {
        TInt menuItemCount( aMenuPane->NumberOfItemsInPane() );
        TInt commandId( 0 );
        for ( TInt i = 0; i < menuItemCount; i++ )
            {
            commandId = aMenuPane->MenuItemCommandId( i );
            aMenuPane->SetItemSpecific( commandId, aItemSpecific );
            }
        }
    }

// ----------------------------------------------------------------------------
// CLogsDetailView::OriginalCallMenuIsVisible
// ----------------------------------------------------------------------------
//
TBool CLogsDetailView::OriginalCallMenuIsVisible()
	{
	// Original CallMenu is visible
	return ETrue;
	}

//  End of File
