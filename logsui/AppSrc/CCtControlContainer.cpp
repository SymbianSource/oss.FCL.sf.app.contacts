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
*     Logs "AoC & CT list" view container class implementation
*
*/

// INCLUDE FILES
#include <aknnotedialog.h>      // Note popup window.
#include <AknIconArray.h>    // icon array
#include <aknnavide.h>
#include <akntabgrp.h>      // tab group
#include <aknPopup.h>       // als query popup
#include <AknsConstants.h>  //for skinned icons

#include <logs.rsg>           // note structure
#include <logs.mbg>     // Logs own icons

#include <csxhelp/log.hlp.hrh>

#include "CCtControlContainer.h"
#include "CCtView.h"
#include "CLogsCtAdapter.h"
#include "CLogsEngine.h"
#include "MLogsSharedData.h"
#include "MLogsSystemAgent.h"

#include "CLogsAocObjFactory.h"
#include "MLogsCallStatus.h"
#include "MLogsAocUtil.h"
#include "CLogsNaviDecoratorWrapper.h"

#include "LogsIcons.hrh"
#include "Logs.hrh"

#include "LogsUID.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KTimeLen = 20;       //Chars needed for time string (same as in CLogsCtAdapter.cpp)
           
// Timer timeout, used to refresh view when in call timers view. Needs to be quite short 
// in order provide perception of even time refreshments to display.         
const TInt KDelay = 250000;

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCtControlContainer::NewL
// ----------------------------------------------------------------------------
//
CCtControlContainer* CCtControlContainer::NewL( 
    CLogsBaseView* aAppView,
    const TRect& aRect )
    {
    CCtControlContainer* self = new( ELeave ) CCtControlContainer( 
                                            aAppView );
    self->SetMopParent( aAppView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();  // self
    return self;
    }

// ----------------------------------------------------------------------------
// CCtControlContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CCtControlContainer::ConstructL( const TRect& aRect)
    {
    BaseConstructL();
  
	CAknNavigationDecorator* decoratedTabGroup = NavigationTabGroupL( R_LOGS_SUB_APP_PANE_TAB_GROUP,
	                                                                  CLogsNaviDecoratorWrapper::InstanceL() );

    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>( decoratedTabGroup->
                                    DecoratedControl() );
    
    MakeTitleL( R_AOCCT_TITLE_TEXT_CT );
    tabGroup->SetActiveTabById( ESubAppTimersTabId );

    // Listbox & Adapter 
    iListBox = new ( ELeave ) CAknDoubleLargeStyleListBox;
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    
    iAocUtil = CLogsAocObjFactory::AocUtilL();
    
    iCallStatus = CLogsAocObjFactory::CallStatusLC();
    CleanupStack::Pop();    

    iCallStatus->SetObserver( this );   //In order to receive status change notifications
                                        //through MLineStatusObserver
    
    CCtView* view = static_cast<CCtView*>( iAppView );
    CLogsCtAdapter* adapter = CLogsCtAdapter::NewL( iAppView->
                                    Engine()->SharedDataL(),
                                    view->AlsLine(),  //Default EAllLines
                                    *iCallStatus);
    iListBox->Model()->SetItemTextArray(adapter);

    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    MakeScrollArrowsL( iListBox );

	AddControlContainerIconsL();

    //  Update data in listbox
    UpdateListBoxContentL();

    // Activate the control and all subcontrols
    SetRect( aRect );
    ActivateL();
    }


// ----------------------------------------------------------------------------
// CCtControlContainer::CCtControlContainer
// ----------------------------------------------------------------------------
//
CCtControlContainer::CCtControlContainer( 
    CLogsBaseView* aAppView ) :
        CLogsBaseControlContainer( aAppView ),
        iAppView( aAppView )
    {
    }

// ----------------------------------------------------------------------------
// CCtControlContainer::~CCtControlContainer
// ----------------------------------------------------------------------------
//
CCtControlContainer::~CCtControlContainer()
    {
    delete iAocUtil;
    delete iCallStatus;
    delete iPeriodic;
    delete iListBox;   
    }


// ----------------------------------------------------------------------------
// CCtControlContainer::ComponentControl
// ----------------------------------------------------------------------------
//
CCoeControl* CCtControlContainer::ComponentControl( 
    TInt /*aIndex*/) const
    {
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CCtControlContainer::SizeChanged
// ----------------------------------------------------------------------------
//
void CCtControlContainer::SizeChanged()
    {
    if( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }


//Called by timer every KDelay interval to update list box content
// ----------------------------------------------------------------------------
// CCtControlContainer::TimerLaunchedL
// ----------------------------------------------------------------------------
//
TInt CCtControlContainer::TimerLaunchedL(TAny* aContainer)
    {
    CCtControlContainer* container = 
            static_cast<CCtControlContainer*>( aContainer );
    container->UpdateListBoxContentL();
    return 0; // needs to return just something.
    }


//  Returns the iListBox.
// ----------------------------------------------------------------------------
// CCtControlContainer::ListBox
// ----------------------------------------------------------------------------
//
CAknDoubleLargeStyleListBox* CCtControlContainer::ListBox()
    {
    return iListBox;
    }


//  Checks updates for listbox. Launches timer to do update every KDelay interval
// ----------------------------------------------------------------------------
// CCtControlContainer::UpdateListBoxContentL
// ----------------------------------------------------------------------------
//
void CCtControlContainer::UpdateListBoxContentL()
    {
    TBool activeCall( EFalse );
    TBool showCallDuration( EFalse );
    iCallStatus->CallIsActive( activeCall ); 
    showCallDuration = iAppView->Engine()->SharedDataL()->ShowCallDurationLogsL();
    
    if ( activeCall && showCallDuration )
        { 
        if( ! iPeriodic )
            {
            iPeriodic = CPeriodic::NewL( EPriorityMore );
            iPeriodic->Start( KDelay, KDelay, TCallBack(TimerLaunchedL, this) );
            }                
        }
    else
        {
        delete iPeriodic;
        iPeriodic = NULL;
        }
            
    CLogsCtAdapter* adapter = static_cast<CLogsCtAdapter*>(
                        iListBox->Model()->ItemTextArray() );

    if( adapter->UpdateDataL() )    //To avoid excessive flicker, update only when 
        {                           // values have changed
        iListBox->HandleItemAdditionL();
        }
    }


// ----------------------------------------------------------------------------
// CCtControlContainer::ShowDurationDetailsL
// ----------------------------------------------------------------------------
//
void CCtControlContainer::ShowDurationDetailsL( TInt aCommandId )
    {
    // Pop-up listbox construction
    CAknSingleHeadingPopupMenuStyleListBox* tmpPopUpListBox = new ( ELeave ) CAknSingleHeadingPopupMenuStyleListBox;
    CleanupStack::PushL( tmpPopUpListBox );

    // Creation of the popupList, to which the text will be inserted
    CAknPopupList* popupFrame = CAknPopupList::NewL( tmpPopUpListBox, R_AVKON_SOFTKEYS_OK_EMPTY__OK );
    CleanupStack::PushL( popupFrame );  
    tmpPopUpListBox->ConstructL( popupFrame, CEikListBox::ELeftDownInViewRect ); //tmpPopUpListBox->ConstructL( popupFrame, EAknListBoxViewerFlags );

    // Disable highlight for the popup listbox.
    tmpPopUpListBox->ItemDrawer()->SetFlags( CListItemDrawer::EDisableHighlight );
    
    // Creation of one item, to which the temporary text will be inserted
    CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( 4 );
    CleanupStack::PushL( items );

    //  Creation of the model, which handles the text items
    CTextListBoxModel* model = tmpPopUpListBox->Model();
    model->SetItemTextArray( items );
    model->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( items );     
    tmpPopUpListBox->HandleItemAdditionL();

    //  Title of the Pop-Up window
    HBufC* textFromResourceFile;
    TAlsEnum  line = EAllLines;     
    TVoipEnum voip = EVoipIncluded;   
    
    if ( aCommandId == EAocCtCmdMenuDetailsVoip)
        {
        textFromResourceFile = CCoeEnv::Static()->AllocReadResourceLC( R_CT_QTL_VOIP_DURATION );        
        line = EVoiceLinesExcluded;    //No voice data added,
        voip = EVoipIncluded;          // only voip
        }
    else
        {
        textFromResourceFile = CCoeEnv::Static()->AllocReadResourceLC( R_CT_QTL_CS_DURATION );        
        CCtView* view = static_cast<CCtView*>( iAppView );
        line = view->AlsLine();  //User's selected ALS line
        voip = EVoipExcluded;    // no voip
        }
        
    popupFrame->SetTitleL( *textFromResourceFile );
    CleanupStack::PopAndDestroy( textFromResourceFile ); 

    DurationDetailLineL( *items, R_AOCCT_LAST_CALL_TIMER_TEXT,     line, voip );
    DurationDetailLineL( *items, R_AOCCT_DIALLED_CALLS_TIMER_TEXT, line, voip );
    DurationDetailLineL( *items, R_AOCCT_RECEIVED_CALLS_TIMER_TEXT,line, voip );
    DurationDetailLineL( *items, R_AOCCT_ALL_CALLS_TIMER_TEXT,     line, voip );

    tmpPopUpListBox->SetCurrentItemIndex( 0 ); // Set focus & top index
    tmpPopUpListBox->SetTopItemIndex( 0 );     // to first line.

    //Publish the listbow for content updates too: Call counters may be updated while listbox is shown to user.   
    iDurationDetailsPopupListBox = tmpPopUpListBox; //Now listbow is ready to update it's contents 
                                                    // if needed while shown to user
    popupFrame->ExecuteLD();
        
    CleanupStack::Pop( popupFrame );     
    CleanupStack::PopAndDestroy( iDurationDetailsPopupListBox );   
    iDurationDetailsPopupListBox = 0;   //Hide listbox (no content updates anymore possible to listbox)   
    }
    
// ----------------------------------------------------------------------------
// CCtControlContainer::DurationDetailLineL
// ----------------------------------------------------------------------------
//
void CCtControlContainer::DurationDetailLineL( 
    CDesCArrayFlat& aItems, 
    TInt aResource, 
    TAlsEnum aLine, 
    TVoipEnum aVoip )
    {
    //CLogsCtAdapter provides functionality to retrieve duration details. No need to optimise the below
    CLogsCtAdapter* adapter = CLogsCtAdapter::NewL( iAppView->Engine()->SharedDataL(),
                                                    aLine,  
                                                    *iCallStatus);
    CleanupStack::PushL( adapter );                                         

    HBufC* text = iCoeEnv->AllocReadResourceLC( aResource );
    text = text->ReAllocL( text->Length() + KTimeLen );         //deletes original descriptor
    CleanupStack::Pop(  );                                      //pop original text from cleanupstack  
    CleanupStack::PushL( text );                                //push new text to cleanupstack 

    TPtr ptr = text->Des();
    ptr.Insert( 0, KTab );        

    TBuf<KTimeLen> buf;
    buf.Zero();

    switch ( aResource )
        {
        case R_AOCCT_LAST_CALL_TIMER_TEXT:
            adapter->LastCallTimeStringL( buf, aLine, aVoip );
            break;
        case R_AOCCT_DIALLED_CALLS_TIMER_TEXT:
            adapter->DialledCallsTimeStringL( buf, aLine, aVoip );
            break;
        case R_AOCCT_RECEIVED_CALLS_TIMER_TEXT:
            adapter->ReceivedCallsTimeStringL( buf, aLine, aVoip );
            break;
        case R_AOCCT_ALL_CALLS_TIMER_TEXT:
            adapter->AllCallsTimeStringL( buf, aLine, aVoip );
            break;
        }
    
    ptr.Insert( 0, buf );  
    aItems.AppendL( *text );
    
    CleanupStack::PopAndDestroy( text );  
    CleanupStack::PopAndDestroy( adapter );      
    }
    

// ----------------------------------------------------------------------------
// CCtControlContainer::AlsSettingQueryL
// ----------------------------------------------------------------------------
//
TBool CCtControlContainer::AlsSettingQueryL( TAlsEnum& aAlsSetting )
    {
    // Pop-up listbox construction
    CAknSinglePopupMenuStyleListBox* popupListBox = new ( ELeave )
            CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( popupListBox );

    // Creation of the popupList, to which the text will be inserted
    CAknPopupList* popupFrame = CAknPopupList::NewL( popupListBox, 
                                R_AVKON_SOFTKEYS_SELECT_CANCEL );
    CleanupStack::PushL( popupFrame );  
    popupListBox->ConstructL( popupFrame, CEikListBox::ELeftDownInViewRect );

    // Creation of one item, to which the temporary text will be inserted
    CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( items );

    // ALS: Both lines logging ON
    MakeOneLineL( *items, R_CT_ALS_BOTH_LINES_TEXT );
    // ALS: Line 1 logging ON
    MakeOneLineL( *items, R_CT_ALS_LINE_ONE_TEXT );
    // ALS: Line 2 logging ON
    MakeOneLineL( *items, R_CT_ALS_LINE_TWO_TEXT );

    //  Creation of the model, which handles the text items
    CTextListBoxModel* model = popupListBox->Model();
    model->SetItemTextArray( items );
    model->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( items ); 
    popupListBox->HandleItemAdditionL();

    //  Title of the Pop-Up window
    HBufC* textFromResourceFile;
    textFromResourceFile = CCoeEnv::Static()->AllocReadResourceLC( 
                                            R_CT_ALS_SELECTION_HEADER );
    popupFrame->SetTitleL( *textFromResourceFile );
    CleanupStack::PopAndDestroy( textFromResourceFile );  

    // set focus & top index
    popupListBox->SetCurrentItemIndex( aAlsSetting );  // focus to seleted als-line
    popupListBox->SetTopItemIndex( 0 );                // show list from top

    TAlsEnum selectedAls = aAlsSetting;
    // Focus comes here after anything else, but "CANCEL" softkey
    if( popupFrame->ExecuteLD() )
        {
        selectedAls = TAlsEnum(popupListBox->CurrentItemIndex()) ;
        }
    else
        // Filter was not selected.
        {
        CleanupStack::Pop( popupFrame ); 
        CleanupStack::PopAndDestroy( popupListBox ); 
        return EFalse;
        }
    CleanupStack::Pop( popupFrame );     
    CleanupStack::PopAndDestroy( popupListBox ); 

    if( selectedAls >= EAllLines && selectedAls <= ELineTwo )    //First three enums can be selected here (LogsAlsEnum.hrh)
        {
        aAlsSetting = selectedAls;
        return ETrue;
        }
    return EFalse;
    }

// Makes one listbox line
// ----------------------------------------------------------------------------
// CCtControlContainer::MakeOneLineL
// ----------------------------------------------------------------------------
//
void CCtControlContainer::MakeOneLineL( 
    CDesCArrayFlat& aItems, 
    TInt aResource )
    {
    HBufC* text = iCoeEnv->AllocReadResourceLC( aResource );    
    aItems.AppendL( *text );
    CleanupStack::PopAndDestroy();  // text
    }

//Called when call status has changed (e.g. call ended)
// ----------------------------------------------------------------------------
// CCtControlContainer::LineStatusChangedL
// ----------------------------------------------------------------------------
//
void CCtControlContainer::LineStatusChangedL()
    {
    UpdateListBoxContentL();  //This calls adapter->UpdateDataL() 
    
    //If duration details popup happens to be just now open, update it also.
    if( iDurationDetailsPopupListBox )  
        {
        
        //FIXME: Add here also updating the listbox contents 
        iDurationDetailsPopupListBox->HandleItemAdditionL();        
        }
    }

// ----------------------------------------------------------------------------
// CCtControlContainer::SimInfoNotificationL
// ----------------------------------------------------------------------------
//
void CCtControlContainer::SimInfoNotificationL()
    {
    UpdateListBoxContentL();
    }

// ----------------------------------------------------------------------------
// CCtControlContainer::CallStatus
// ----------------------------------------------------------------------------
//
MLogsCallStatus& CCtControlContainer::CallStatus() const
    {
    return *iCallStatus;
    }

//  Return the value of the iCommonControl
// ----------------------------------------------------------------------------
// CCtControlContainer::AocUtil
// ----------------------------------------------------------------------------
//
MLogsAocUtil& CCtControlContainer::AocUtil() const
    {
    return *iAocUtil;
    }

// ----------------------------------------------------------------------------
// CCtControlContainer::GetHelpContext
// ----------------------------------------------------------------------------
//
void CCtControlContainer::GetHelpContext( TCoeHelpContext& aContext ) const
	{
    aContext.iMajor = TUid::Uid( KLogsAppUID3 );
    aContext.iContext = KTIMERS_HLP_MAIN;
    }

// ----------------------------------------------------------------------------
// CCtControlContainer::AddControlContainerIconsL
// ----------------------------------------------------------------------------
//
void CCtControlContainer::AddControlContainerIconsL()
	{
	if( iListBox->ItemDrawer()->FormattedCellData()->IconArray() != NULL )
		{
		CArrayPtr<CGulIcon>* iconArray = iListBox->ItemDrawer()->FormattedCellData()->IconArray();
		delete iconArray;
		iconArray = NULL;
		iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( iconArray);  	
		}

	// Load and assign icons to the list box control; 7 is number of icons
	CAknIconArray* icons = new( ELeave ) CAknIconArray( KAocCtCCNrOfItems );

	iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( icons );

	// these values must not be changed unless adaptors are also modified!

	// 1st icon & mask. (Active Call Timer)
	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogTimerCallActive       //AknsConstants.h
			,	qgn_prop_log_timer_call_active          //LogsIcons.hrh
			,	qgn_prop_log_timer_call_active_mask     //LogsIcons.hrh
			);

	// 2nd icon & mask. (Last Call Timer)
	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogTimerCallLast  
			,	qgn_prop_log_timer_call_last
			,	qgn_prop_log_timer_call_last_mask
			);

	// 3th icon & mask. (Dialled Calls Timer)
	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogTimerCallOut  
			,	qgn_prop_log_timer_call_out
			,	qgn_prop_log_timer_call_out_mask
			);

	// 4rd icon & mask. (Received Calls Timer)
	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogTimerCallIn  
			,	qgn_prop_log_timer_call_in
			,	qgn_prop_log_timer_call_in_mask
			);

	// 5th icon & mask. (All Calls Timer)
	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogTimerCallAll 
			,	qgn_prop_log_timer_call_all
			,	qgn_prop_log_timer_call_all_mask
			);
	}
	
// ----------------------------------------------------------------------------
// CCtControlContainer::FocusChanged
//
// This is needed to hand focus changes to list. Otherwise animations are not displayed.
// ----------------------------------------------------------------------------
//
void CCtControlContainer::FocusChanged(TDrawNow /* aDrawNow */ )
    {
    if( iListBox)
        {
        iListBox->SetFocus( IsFocused() );
        }
    }	

//  End of File
